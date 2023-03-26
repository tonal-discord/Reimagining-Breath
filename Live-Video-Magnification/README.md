## [Live Video Magnification](https://github.com/tschnz/Live-Video-Magnification)

C++ realtime(almost) camera magnification.

See link above or read orignal_README.md for details, it's pretty thorough.

## Setting it up on Windows (hasn't worked yet):
1. Install Qt Creator 8.0.1, you will have to make an account, [here](https://download.qt.io/official_releases/qtcreator/8.0/8.0.1/) 
- I used QT version 5.1538
2. Install [opencv](https://opencv.org/releases/) version 4.6.0

Three options that I've tried (none worked for me):
- Easiest option: Move the .exe from link right above to C:\ and run it. This will make a folder called opencv.
- Slowest option: Build it from source following [this tutorial](https://www.youtube.com/watch?v=_fqpYLM6SCw) (takes a long time)
- install with [vcpkg](https://vcpkg.io/en/getting-started.html) and Visual Studio
3. Now in theory you can open the program in QTCreator by selecting open project and browsing to `/src/rvm.pro`
4. Edit the paths in `rvm.pro` to match your opencv installation

Here's a way to test that OpenCV and QTCreator are working with an example project: https://www.youtube.com/watch?v=fYBdwGpHQGw 

I couldn't get this part working, I got the error `undefined reference to 'cv::Mat::Mat()'`

## Installing on Linux (Fedora Workstation - couldn't get it working on Ubuntu)
You can make a fedora virtual machine to try it out. Not sure if/how you can pass your webcam into the virtual machine.
1. Follow this: (use Fedora 37)https://www.youtube.com/watch?v=plH5iX3RzHg 
2. Once installed, run this command in the terminal: `dnf install -y opencv qtcreator`
3. Copy the program into the virtual machine, run `./build-rvm-Desktop-Debug/rvm` and the program should work
4. You can also open qtcreator in the virual machine and run the program through that.

### Running the program
1. Open Qt Creator, select open project, open`/src/rvm.pro`
- or you can open `./build-rvm-Desktop-Debug/rvm`
2. Build and run
3. Press connect/open, press OK, under options choose Laplace

## About the code:
`/src/main/magnification/Magnificator.cpp`, is where the breath detection stuff happens. 
