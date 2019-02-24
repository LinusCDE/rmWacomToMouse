This is basically the same as `rmServeWacomInput.py` but implemented in C.

Basic instructions without compiling are [here](https://github.com/LinusCDE/rmWacomToMouse/blob/master/README.MD).


# Compiling

There are several options including:

- Using [a cross-platform compiler](https://remarkablewiki.com/devel/qt_creator)
- Compiling on a device using the same platform like a RaspberryPi
- Compiling it directly on the reMarkable

## Compiling it on a RaspberryPi

- Clone the Repository
- Install the packages `gcc`, `make` and maybe `linux-headers`
- Go into the `c_implementation` directory of this repo and run `make`
- Copy the resulting file onto the reMarkable
- Eventually stop the UI to prevent autosleep using `systemctl stop xochitl`

## Compiling it on the reMarkable

- Install [the entware package manager for the reMarkable](https://github.com/Evidlo/remarkable_entware)
- To use it run `export PATH="${PATH}:/opt/bin/:/opt/sbin/"` or put that into `~/.bashrc` to be permanent
- Run `opkg install gcc make`
- Clone/Copy this repo onto the device (e.g. with scp, FileZilla or WinSCP)
- Go into the `c_implementation` directory of this repo and run `make`
- Eventually stop the UI to prevent autosleep using `systemctl stop xochitl`
- Run the binary with `./rmServeWacomInput`


## Running

The binary can be executed by running `./rmServeWacomInput`.

You also may consider stopping the UI (`systemctl stop xochitl`) to prevent autosleep or accidental touches.
To restart it hold the power button or run `systemctl start xochitl`.