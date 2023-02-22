## [Live Video Magnification](https://github.com/tschnz/Live-Video-Magnification)

C++ realtime(almost) camera magnification.

See link above or read orignal_README.md for details, it's pretty thorough.

### Setting it up (not thorough):
(I ran it on Linux, not sure if anything else works)
1. Install Qt Creator, I think you have to make an account, [here](https://www.qt.io/download-open-source?hsCtaTracking=e9c17691-91a0-4616-9bc2-1a6a6c318914%7C963686f8-2c68-442a-b17b-3d73ce95b819) (scroll down to "Download the QT Online Installer")
2. Install [opencv](https://opencv.org/releases/)
3. Open Qt Creator, select open project, open`/src/rvm.pro`
4. Build and run
4. Press connect/open, press OK, under options choose Laplace
5. Set frequency range to something like 6-30%


## Current Approaches/goals
Main goal: Get some sort of numerical output (to a file or something) that determines breathing in/out and the intensity.

editing `/src/main/magnification/Magnificator.cpp`, trying to detect movement in output image
- Currently trying to detect the amount of white levels/brightness in image. Breathing in is more white, out is more dark.

Ideas:
- Look into the magnificaion math more and see if there's any useful output
	+ Maybe check out [this](https://github.com/wzpan/QtEVM) C++ EVM program that this is baesd on.
- Track chest/collar area and detect motion
