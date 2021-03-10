#!/usr/bin/env python3
'''
Meant to run on the reMarkable.
Opens a server on 10.11.99.1:33333 (or any wifi ip)
and sends all event data to the next client.
'''

import socket
from sys import argv, stderr

# Select what event file to use
if len(argv) == 2:
	event_file_name = '/dev/input/event%s' % argv[1]
	print('Event file number was given. Using event file "%s" for pen input." % event_file_name');
else:
	with open('/sys/devices/soc0/machine') as machineFp:
		machineContent = machineFp.read().strip()
	if machineContent == 'reMarkable 1.0' or machineContent == 'reMarkable Prototype 1':
		event_file_name = '/dev/input/event0'
		print('Device seems to be a reMarkable 1. Using event file "%s" for pen input.' % event_file_name)
	elif machineContent == 'reMarkable 2.0':
		event_file_name = '/dev/input/event1'
		print('Device seems to be a reMarkable 2. Using event file "%s" for pen input.' % event_file_name)
	else:
		print(stderr, 'Machine name was not recognized: "%s"' % machineContent, file=stderr)
		print(stderr, 'Please report the above machine name to the developer.', file=stderr)
		print(stderr, 'You can supply the correct event file number to bypass the problem for now.', file=stderr)
		exit(1)

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('0.0.0.0', 33333))
server.listen()

while True:
	print('Waiting for client...')
	client, addr = server.accept()
	print('Client connected')

	# Start reading input from wacom digitizer:
	wacomEvents = open(event_file_name, 'rb')  # See rm.DebugWacomInput.py for more details
	try:

		# Send data while possible:
		while True:
			wacomEvents.read(8)  # Discard timestamp
			client.send(wacomEvents.read(8))  # Format happens to be the exact same I used with struct			

	except BrokenPipeError:
		print('Client disconnected')
		wacomEvents.close()  # Stop reading input from wacom digitizer
