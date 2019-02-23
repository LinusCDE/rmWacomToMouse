Here is the basic c implementation of the rmServer

# Compile

- Either use a transpiler
- Or just take a RaspberryPi

I'm not experienced with [the transpiler for the reMarkable](https://remarkablewiki.com/devel/qt_creator). I've installed it, but can't compile anything due to missing includes.

## Using the RaspberryPi

- Get the c-file onto your RaspberryPi
- Ensure you have gcc and linux-headers installed
- Compile with the binary using `make`
- Copy the binary (rmServeWacomInput) to the reMarkable and execute it
- Run the client on your computer
