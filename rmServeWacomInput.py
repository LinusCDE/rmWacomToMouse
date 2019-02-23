#!/usr/bin/env python3
'''
Meant to run on the reMarkable.
Opens a server on 10.11.99.1:33333
and sends all event data to the next client.
'''

import socket

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('10.11.99.1', 33333))
server.listen()

while True:
	print('Waiting for client...')
	client, addr = server.accept()
	print('Client connected')

	# Start reading input from wacom digitizer:
	wacomEvents = open('/dev/input/event0', 'rb')  # See rm.DebugWacomInput.py for more details
	try:

		# Send data while possible:
		while True:
			wacomEvents.read(8)  # Discard timestamp
			client.send(wacomEvents.read(8))  # Format happens to be the exact same I used with struct			

	except BrokenPipeError:
		print('Client disconnected')
		wacomEvents.close()  # Stop reading input from wacom digitizer