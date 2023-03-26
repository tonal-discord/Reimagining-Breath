## [Live Video Magnification](https://github.com/tschnz/Live-Video-Magnification)

C++ realtime(almost) camera magnification.

See link above or read orignal_README.md for details, it's pretty thorough.

## About the branches:
- Main branch is the mostly stock program.
- Windows branch is my attempt at running it on Windows including instructions
- The progress branch is the latest version, only works on Fedora Linux somehow.

## Installing on Linux (Fedora Workstation 37- couldn't get it working on Ubuntu)
You can make a fedora virtual machine to try it out. Not sure if/how you can pass your webcam into the virtual machine. [VM tutorial](https://www.youtube.com/watch?v=plH5iX3RzHg)
1. Run this command in the terminal: `dnf install -y opencv qtcreator`
3. Copy the program into the Fedora machine, run `./build-rvm-Desktop-Debug/rvm` and the program should work
4. You can also open qtcreator in the virual machine and run the program through that.

##  Using the program
4. Press connect/open, press OK, under options choose Laplace

## About the code:
`/src/main/magnification/Magnificator.cpp`, is where the breath detection stuff happens. 
