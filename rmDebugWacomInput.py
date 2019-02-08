#!/usr/bin/env python3
'''
Meant to run on the reMarkable.
Shows data read from Wacom device using evdev
'''

import evdev
from select import select

wacomDev = evdev.InputDevice('/dev/input/event0')
#print(wacomDev)

# Credit to basic concept (I never used evdev): https://stackoverflow.com/a/12387122

# Source: https://github.com/canselcik/libremarkable/blob/master/src/input/wacom.rs
EV_SYNC = 0
EV_KEY = 1
EV_ABS = 3
WACOM_EVCODE_PRESSURE = 24
WACOM_EVCODE_DISTANCE = 25
WACOM_EVCODE_XTILT = 26
WACOM_EVCODE_YTILT = 27
WACOM_EVCODE_XPOS = 0
WACOM_EVCODE_YPOS = 1

lastXPos = -1
lastYPos = -1
lastXTilt = -1
lastYTilt = -1
lastDistance = -1
lastPressure = -1

while True:
	r,w,x = select([wacomDev], [], [])
	for event in wacomDev.read():
		#print(event.type, event.code, event.value, sep='\t')
		if event.type == EV_ABS:
			if event.code == WACOM_EVCODE_XPOS:
				lastXPos = event.value  # X is Y
			elif event.code == WACOM_EVCODE_YPOS:
				lastYPos = event.value  # Y is X
			elif event.code == WACOM_EVCODE_XTILT:
				lastXTilt = event.value
			elif event.code == WACOM_EVCODE_YTILT:
				lastYTilt = event.value
			elif event.code == WACOM_EVCODE_DISTANCE:
				lastDistance = event.value
			elif event.code == WACOM_EVCODE_PRESSURE:
				lastPressure = event.value

			print('XPos: %5d | YPos: %5d | XTilt: %5d | YTilt: %5d | Distance: %3d | Pressure: %4d' % (lastXPos, lastYPos, lastXTilt, lastYTilt, lastDistance, lastPressure))
