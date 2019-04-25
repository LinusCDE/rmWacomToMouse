# rmWacomToMouse

**DISCLAIMER: PROCEED ON YOUR OWN RISK.**

[Demonstration here](https://youtu.be/R-BsDivpUQ0)

The linux tablet driver was [**moved here**](https://github.com/LinusCDE/rmTabletDriver).

---

Thanks to [Evidlos implementation](https://github.com/Evidlo/remarkable_mouse) making it a lot easier to use. **Please also check out 
his solution if you're interested in this software.**

---

## Concept

The basic concept:

- A python script / c program reads the data of the wacom input (codes taken from [libremarkable](https://github.com/canselcik/libremarkable/blob/master/src/input/wacom.rs))
- This script provides a server on port 33333 over usb and wifi
- A client script on your pc reads the data from the server and acts accordingly.


This repository contains two components:

 - A server (on the reMarkable)
   - Files that start with *rm*
   - Has both a python as well as a c implementation
- A client (on your computer)
   - Files that start wiht *pc*


## Setting it up

### Server

You can either run the python version, the precompiled c binary or [compile it yourself](https://github.com/LinusCDE/rmWacomToMouse/blob/master/c_implementation/README.MD).

The easiest one is using the precompiled c binary.


#### Using the precompiled c binary

- Download the latest binary called `rmServeWacomInput` (without any extension) [here](https://github.com/LinusCDE/rmWacomToMouse/releases).
- Copy the binary to your reMarkable using scp, FileZilla, WinSCP or similar software.
- Open a ssh session with the device (Putty can be used on windows).
- Execute the binary with `./rmServeWacomInput`
  - If that fails ensure you are in the correct directory or run `chmod +x rmServeWacomInput` and try running it again


#### Using the python script
- Copy the `rmServeWacomInput.py` file to the device using scp, FileZilla, WinSCP or similar software.
- Open a ssh session with the device (Putty can be used on windows).
- Install [the entware package manager for the reMarkable](https://github.com/Evidlo/remarkable_entware)
- To use it run `export PATH="${PATH}:/opt/bin/:/opt/sbin/"` or put that into `~/.bashrc` to be permanent
- Run `opkg install python3`
- Run the script using `python3 rmServeWacomInput.py` or just `./rmServeWacomInput.py`


### Client

- Install python 3
- Open a command prompt and run `pip3 install pynput`. If that fails try without the *3* in `pip3`.
- Clone/Download this repository
- Go into the directory of this repository using your command prompt
- Ensure the server is already running as described above
- Run one of the files beginning with `rm` using `python3` or `python` in front and
  - e.g. `python3 rmWacomToMouseTouchpad.py`


## Configure

Both scripts can be edited with most text editors (don't use notepad on windows).

Change the values in the config section to appropriate ones.
