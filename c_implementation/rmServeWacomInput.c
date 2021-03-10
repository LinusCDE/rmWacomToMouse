/*
 * This should act almost excatly like the python script rmServeWacomInput.py
 * This can be compiled and run on the reMarkable itself (using the toolchain or gcc on the device from entware)
 * A recent binary can be found here: https://github.com/LinusCDE/rmWacomToMouse/releases
 */

#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h> // (Only needed for TCP_NODELAY)
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h> // (Only needed for timeval)
#include <unistd.h>


// Which port to start the server on:
#define SERVER_PORT 33333


// Renamed copy of input_event from linux-kernel of the reMarkable
// Source: https://github.com/reMarkable/linux/blob/b82cb2adc32411c98ffc0db86cdd12858c8b39df/include/uapi/linux/input.h#L24
struct rm_input_event {
	struct timeval time;
	uint16_t type;
	uint16_t code;
	int32_t value;
};


int main(int argc, char const *argv[]) {
    char event_file_name[32];
    if (argc > 1) {
        snprintf(event_file_name, sizeof(event_file_name), "/dev/input/event%s", argv[1]);
        printf("Event file number was given. Using event file \"%s\" for pen input.\n", event_file_name);
    } else {
        // Read "/sys/class/devices/soc0/machine" to determine version and what file to use
        // A better way would be to detect it through the evdev capabilities, but this is fewer
        // lines and faster.
        char machineContent[32]; // Store content of file here
        // Ensure the string still will terminate with a null byte
        memset((void*) machineContent, 0, sizeof(machineContent));
        FILE* machineFp = fopen("/sys/devices/soc0/machine", "r");
        size_t readBytes = fread((void *) machineContent, 1, sizeof(machineContent), machineFp);
        if (readBytes < 0) {
            perror("Failed to read \"/sys/devices/soc0/machine\"");
            return 1;
        }else if (readBytes == 32) {
            fprintf(stderr, "Content of \"/sys/devices/soc0/machine\" is longer than expected!\n");
            fprintf(stderr, "Please report the content of the above file to the developer.\n");
            fprintf(stderr, "You can supply the correct event file number to bypass the problem for now.\n");
            return 1;
        }
        if (strcmp(machineContent, "reMarkable 1.0\n") == 0 || strcmp(machineContent, "reMarkable Prototype 1\n") == 0) {
            strcpy(event_file_name, "/dev/input/event0");
            printf("Device seems to be a reMarkable 1. Using event file \"%s\" for pen input.\n", event_file_name);
        }else if (strcmp(machineContent, "reMarkable 2.0\n") == 0) {
            strcpy(event_file_name, "/dev/input/event1");
            printf("Device seems to be a reMarkable 2. Using event file \"%s\" for pen input.\n", event_file_name);
        }else {
            fprintf(stderr, "Machine name was not recognized: \"%s\"\n", machineContent);
            fprintf(stderr, "Please report the above machine name to the developer.\n");
            fprintf(stderr, "You can supply the correct event file number to bypass the problem for now.\n");
            return 1;
        }
    }

    // Creating socket file descriptor
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);

    if (serverFd == 0) {
        perror("Failed to create a IPv4 TCP socket"); // Appends string for set variable »errno« (see manpage)
        return 1;
    }

    // Forcefully attaching socket to the port SERVER_PORT
    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Failed to setup a server");
        return 1;
    }

    // Construct server address:
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET; // AF_INET == IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Same as "0.0.0.0" (as ipv4)
    serverAddress.sin_port = htons(SERVER_PORT); // htons ensures that uint16_t is network bitorder
    size_t serverAddressSize = sizeof(serverAddress);

    // Bind socket to address:
    if (bind(serverFd, (struct sockaddr*) &serverAddress, serverAddressSize) < 0) {
        fprintf(stderr, "Failed to bind 0.0.0.0:%d!\n", SERVER_PORT);
        fprintf(stderr, "Is the port already in use?");
        perror("The error was");
        return 1;
    }

    // Disable Nagle's algorithm for better latency:
    int tcpDelayDisabled = 1;
    setsockopt(serverFd, IPPROTO_TCP, TCP_NODELAY, &tcpDelayDisabled, sizeof(tcpDelayDisabled));

    if (listen(serverFd, 3) < 0) { // Listen and queue up to 3 clients at once
        perror("Failed to start listening for clients");
        return 1;
    }

    int performServerShutdown() {
        fprintf(stderr, "Shutting down server...\n");

        int shutdownResult = shutdown(serverFd, SHUT_RDWR); // Signal closing of read (RD) and write (WR) of server
        if(shutdownResult != 0) {
            perror("Call shutdown() on server failed");
            return 2;
        }

        int closeResult = close(serverFd); // Close server
        if(closeResult != 0) {
            perror("Failed call close() on server");
            return 2;
        }

        return 1;
    }

    // Signore SIGPIPE Errors: (e.g. a client disconnected)
    signal(SIGPIPE, SIG_IGN);

    printf("Server started on port %d.\n", SERVER_PORT);
    // Handle clients (only one at a time):
    while(1) {
        // Accpet new client:
        int clientFd = accept(serverFd, (struct sockaddr *)&serverAddress, (socklen_t*)&serverAddressSize);
        if (clientFd < 0) {
            fprintf(stderr, "Failed to connect to new client.\n");
            return performServerShutdown();
        }
        printf("New client connected.\n");


        // Open file where input events for the wacom digitizer get received
        FILE* wacomInputFp = fopen(event_file_name, "r");

        if(wacomInputFp == NULL) {
            fprintf(stderr, "Failed to open wacom event file!\n");
            return performServerShutdown();
        }


        struct rm_input_event inputEvent; // Will contain the read data
        size_t inputEventSize = sizeof(inputEvent);
        size_t timeSize = sizeof(inputEvent.time);
        size_t packetSize = inputEventSize - timeSize; // Size of data that will get sent in packets
        size_t readBytes;
        ssize_t writeResult; // Either written bytes or error (0 or -1)

        while(1) {
            readBytes = fread((void*) &inputEvent, 1, inputEventSize, wacomInputFp);

            if(readBytes != inputEventSize) {
                // Failed to read required data. Should never happen.
                perror("Failed to read wacom event file");

                if(close(clientFd) != 0) // Disconnect current client
                    perror("Failed to close connection to client");
                performServerShutdown();

                if(fclose(wacomInputFp) != 0)
                    perror("Faild to close wacom event file");

                return 1;
            }

            // Write all data of struct rm_input_event beginning right after time-field:
            writeResult = write(clientFd, ((uint8_t*) &inputEvent) + timeSize, packetSize);

            if(writeResult != packetSize) {
                perror("Connection to client lost");
                if(writeResult < 1) {
                    // 0 == (Temporary) Network problems
                    // -1 == Connection lost / closed
                    // Both will suffice us to kick/ditch the client for the next one.

                    if(writeResult == 0 && close(clientFd) != 0)
                        perror("Failed to close connection to client");

                    if(fclose(wacomInputFp) != 0)
                        perror("Faild to close wacom event file");
                    break; // Wait for next client
                }
            }
        }


    } // End of handling clients
    return 0;
}
