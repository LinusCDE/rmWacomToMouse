#!/usr/bin/env python3
'''
Meant to run on the reMarkable.
Opens a server on 10.11.99.1:33333
and sends all event data to the next client.
'''

import evdev
from select import select
import socket
import struct

wacomDev = evdev.InputDevice('/dev/input/event0')
#print(wacomDev)

# Credit to basic concept (I never used evdev): https://stackoverflow.com/a/12387122

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('10.11.99.1', 33333))
server.listen()

while True:
	print('Waiting for client...')
	client, addr = server.accept()
	print('Client connected')
	try:
		while True:
			r,w,x = select([wacomDev], [], [])
			for event in wacomDev.read():
				#print(event.type, event.code, event.value)
				client.send(struct.pack('HHi', event.type, event.code, event.value))
	except BrokenPipeError:
		print('Client disconnected')
