## [Live Video Magnification](https://github.com/tschnz/Live-Video-Magnification)
See README in main branch for more details



## Developing on Linux (Fedora Workstation 37)
1. Install dependencies: `sudo dnf install opencv opencv-devel gcc-c++ glew-devel SDL2-devel SDL2_image-devel glm-devel freetype-devel`
2. Install Qt Creator (you will have to make an account) [here](https://www.qt.io/download-qt-installer-oss) 
3. You may have to edit the kits, manually set the C++ compiler to your GCC path.
4. In Qt Creator, open a new project and select `./src/rvm.pro`

## Other distributions
Was not able to get the necessary opencv modules installed and linked properly on Ubuntu 20.04.6 (yet). If  you wish to attempt this, [here](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html) is a tutorial. You will need to build with opencv_contrib.
### Future improvements:
- Check out [linuxdeployqt](https://github.com/probonopd/linuxdeployqt) to create an AppImage that in theory should work easily on all distributions
	- Wasn't able to build on Fedora due to glibc being too new.

### Dependencies:
These versions were used during Linux development:
- OpenCV 4.6.0
- Qt 6.4.3


### Troubleshooting
Edit the paths in `rvm.pro` to match your opencv installation (shouldn't be an issue if instructions were followed)

Check that QTCreator is configured for version 6.4.2 or above

Here's a way to test that OpenCV and QTCreator are working with an example project: https://www.youtube.com/watch?v=fYBdwGpHQGw 

Restart your computer



-----------------------

