/*
This sourcecode was hacked together from those two sources:
https://www.geeksforgeeks.org/socket-programming-cc/
https://stackoverflow.com/q/22209267

The Server works like the python script but stops working after one connection.
I don't have any experience with os-sockets and evdev, so if somebody wants fix this
or just clean this mess up, he's welcome to submit push requests.
*/

// Server:
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

// EvDev:
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>

// Own packet creation:
#include <stdint.h>


#define SERVER_PORT 33333

int main(int argc, char const *argv[])
{
    // ------------------------------
    // ServerSocket setup:
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port SERVER_PORT
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( SERVER_PORT );

    // Forcefully attaching socket to the port SERVER_PORT
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // ------------------------------


    // ------------------------------
    // EV-Dev setup:
    struct input_event ev;
    int fevdev = -1;
    int result = 0;
    int size = sizeof(struct input_event);
    int rd;
    int value;
    char name[256] = "Unknown";
    char *device = "/dev/input/event0";

    // ------------------------------

    // Getting new client:
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Example transmission:
    /*
    int valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    char *hello = "Hello from server";
    send(new_socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");*/

    // Open evdev:
    fevdev = open(device, O_RDONLY);
    if (fevdev == -1) {
        printf("Failed to open event device.\n");
        exit(1);
    }
    printf("Opened evdev.\n");

    while(1) {
        // Read evdev:
        if ((rd = read(fevdev, &ev, size)) < size) {
            printf("Lost input device. Exiting.\n");
            close(fevdev);
            return 1;
        }
        
        // Debug:
        //printf("Type: %d, Code: %d, Value: %d\n", ev.type, ev.code, ev.value);

        // May not be the native values but are needed in this format anyway:
        // Create packet as in python struct fmt 'HHi'
        uint16_t evType = (uint16_t) ev.type;
        uint16_t evCode = (uint16_t) ev.code;
        int32_t evValue = (int32_t) ev.value;
        
        uint8_t packet[8];
        memcpy(packet, &evType, 2);
        memcpy(packet + 2, &evCode, 2);
        memcpy(packet + 4, &evValue, 4);
        
        // Send packet:
        send(new_socket, packet, 8, MSG_DONTWAIT);
    }

    printf("Exiting...");
    close(fevdev);
    return 0;
}
