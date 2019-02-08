# rmWacomToMouse

This is currently **a proof of concept**, sending wacom input to your pc which enables using the reMarkable as a basic drawing tablet.

*Since I currently don't have a lot of time, don't expect a lot of development here. I achived what I wanted and don't have a lot of use for it. If someone 
want's to take on this project (e.g. port the concept for easyier deployment on the rM, make it user friendly, enhance it, etc.), I welcome you.*


**DISCLAIMER:** This is currently not user friendly. In order to use this, you need to do things to your device, that could easily screw something up. 
**PROCEED 
ON YOUR OWN RISK.**

## [Demonstration here](https://youtu.be/R-BsDivpUQ0)

## Concept

The basic concept:

- A python script reads the data of the wacom input (codes taken from [libremarkable](https://github.com/canselcik/libremarkable/blob/master/src/input/wacom.rs))
- This script provides a server on port 33333 over usb (ip needs to be changed for using over wifi)
- A client script on your pc reads the data from the server and acts accordingly.


## Requirements / Challenges

Your device needs to run the `rmServeWacomInput.py`-Script. That requires you to get python 3 to run on the device.
One possibility is [installing a package manager](https://www.reddit.com/r/RemarkableTablet/comments/ag2zen/package_manager_for_remarkable/). I copied the ARM 
port of ArchLinux for the RaspberryPi to the device which gave me the ability to install python 3, too (takes 3GB, so not really recommended).

The script on the remarkable requires you to install evdev for python. This can be done by either running `pip(3) install evdev` or a package like 
`python(3)-evdev` using a package manager (the 3 may be not needed, depending on installation`).

You then simply copy the script (`rmServeWacomInput.py`) to your device, make it executable (`chmod +x <file>`) and run it as a regular executable or using 
python 3 (again, depending on installation of python 3).


On your pc, you install python 3, install `pynput` (using `pip3 install pynput`).
Open the script `pcWacomToMouse.py` and change your area to draw in the config-section.
The script is fairly basic, so you need to make sure, that your area has the same ratio as the display of the reMarkable (otherwise your input will get 
distorted to fit your defined rectangle).
Then simply execute the script on your pc (the one on the reMarkable should be already running).
