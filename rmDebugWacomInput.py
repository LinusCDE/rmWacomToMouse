#!/usr/bin/env python3
'''
Meant to run on the reMarkable.
Shows data read from Wacom device using evdev
'''

import struct

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
# ----------


lastXPos = -1
lastYPos = -1
lastXTilt = -1
lastYTilt = -1
lastDistance = -1
lastPressure = -1

# EvDev (Event Device) basically is a struct repeadetly read from this socket/file
# Struct from https://github.com/reMarkable/linux/blob/b82cb2adc32411c98ffc0db86cdd12858c8b39df/include/uapi/linux/input.h#L24
#struct input_event {
#	struct timeval time;  # <-- 64 bits
#	__u16 type;
#	__u16 code;
#	__s32 value;
#};

wacomEvents = open('/dev/input/event0', 'rb')
try:
	while True:
		# Basically what the evdev-lib does (only more dynamic):
		_, evType, evCode, evValue = struct.unpack('QHHi', wacomEvents.read(16))
		
		if evType != EV_ABS:
			continue
		
		if evCode == WACOM_EVCODE_XPOS:
			lastXPos = evValue
		elif evCode == WACOM_EVCODE_YPOS:
			lastYPos = evValue
		elif evCode == WACOM_EVCODE_XTILT:
			lastXTilt = evValue
		elif evCode == WACOM_EVCODE_YTILT:
			lastYTilt = evValue
		elif evCode == WACOM_EVCODE_DISTANCE:
			lastDistance = evValue
		elif evCode == WACOM_EVCODE_PRESSURE:
			lastPressure = evValue

		print('XPos: %5d | YPos: %5d | XTilt: %5d | YTilt: %5d | Distance: %3d | Pressure: %4d' % (lastXPos, lastYPos, lastXTilt, lastYTilt, lastDistance, lastPressure))
except KeyboardInterrupt:
	print('Exiting...')
	wacomEvents.close()