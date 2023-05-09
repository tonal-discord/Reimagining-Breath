## [Live Video Magnification](https://github.com/tschnz/Live-Video-Magnification)
See link above or read orignal_README.md for details, it's pretty thorough.

## Using the program
TLDR:
1. Download the [release](https://github.com/kujaw077/senior-design-project/releases/download/v1.0.0/Re-Imagining.Breath.v1.0.0.zip)(also shown on right side under "Releases")
2. Extract
3. Open rvm.exe
4. Press "OK" or change options if you desire
5. Switch magnification type box to "Laplace"
6. The "CSV output" checkbox outputs the breath values to a CSV file called "out.csv" in the same directory as the executable when checked. The file will not delete or clear itself.
7. The "Toggle magnified view" button switches the preview from magnified motion to the contours which are used to track (motion) breathing


The following is taken from the program's original source found at link above.
### Connect
- Camera
    - Device Number: Type in the device number of your camera connected to your computer. Indexing starts with 0 which is usually your built-in webcam.
    - (Ubuntu/Linux) PointGrey Device on USB:  Having a DC1394 Camera connected to your computer, OpenCV redirects the camera over the v4l-API to device number 0. If you wish to also connect to your built-in camera, enable this option to set built-in to 0, DC1394 to 1
    - Image Buffer: Select the length of an image buffer before processing those images. If dropping frames if buffer is full is disabled, your capture rate will be same the same as your processing rate.
- Video
    - Choose a video. Compatibility is given if your computer supports the codec. Valid file endings are .avi .mp4 .m4v .mkv .mov .wmv
- Resolution: This does not work for videos on Ubuntu/Linux yet (Windows not tested). For cameras check the supported modes from camera manufacturer and type in the resolution specified for a mode.
- Frames per Second: Some cameras support multiple modes with different resolution/fps/etc. . Setting the framerate will change into a mode with a framerate near the one you typed in. For videos, some mp4-files have a bad header where OpenCV can't read out the framerate, which will normally be set to 30FPS. Anyway here you can set it manually.

![Connect Dialog](pictures/connect_dialog.png)

### Main Window
When succesfully connected to a camera or opened a window, you can **draw a box in the video by clicking and dragging, to scale and only amplify this Region Of Interest** in a video source. Setting the video back to normal can be done via menu that opens with a right click in the video. There is also the option to show the unmagnified image besides the processed one.

![Right-click Menu in Frame Label](pictures/frameLabel_menu.png)

### Magnify
Try experimenting with different option values. Furthermore tooltips are provided when hovering the cursor above a text label in the options tab. If you're using an older machine and processing is too slow, try enabling the Grayscale checkbox.

|                        |  Low *Level* value |  High *Level* value|
| :---------------------- | :-----------------: | :---------------: |
|**Motion Magnification**| More noise, less movements by big objects  |   Less noise, less movements by little objects |

#### Motion Magnification
- Amplification: The higher the value, the more movements and noise are amplified.
- Cutoff Wavelength: Reduces fast movements and noise.
- Frequency Range: Reducing the handler values leads to magnifying slow movements more than fast movements and vice versa.
- Chrom Attenuation: The higher the value, the more the chromaticity channels are getting amplified too, e.g. the more colorful the movements are.

![Panel for Motion Magnification Options](pictures/lmag_options.png)

### Save
For saving videos or recording from camera you have to specify the file extension by your own. .avi is well supported. If you should encounter problems, please try a differenct saving codec in the toolbar under File->Set Saving Codec. Note: this doesn't seem to work at the moment.


## About the code:
`/src/main/magnification/Magnificator.cpp`, is where the magnification and breath detection happens. 

`src/main/threads/ProcessingThread.cpp` or `PlayerThread.cpp` handle Magnified camera output or magnified video output respectively. CSV writing done here.

See link at the top or read orignal_README.md for more details.


### Low-hanging fruit to optimize: 
1. Magnify only the selected ROI rather than whole input video
 - Find optimal resolution to use
2. Try grayscale - is that sufficient? Does it process faster?
3. Remove overhead of other magnification types?


### Future improvements
1. Make it detect breathing better 
 	- Calibration?
	- AI/ML?
	- Low-pass filter?
	- Blur (or something else) input video more?

2. Automatically crop/zoom webcam 
	- Currently, you can drag mouse on input video to select a region. Can do this manually worst case scenario.


## Developing on Windows (for development):
1. Install Qt Creator (you will have to make an account) [here](https://www.qt.io/download-qt-installer-oss) 
   - You can use Visual Studio but if so you're on your own
2. Unzip [opencv.zip](https://drive.google.com/file/d/1sh63BbIwR6FJ2E_qEA8cqg3bwMOptkE1/view?usp=share_link)(from Google drive) in C:\, so that you have C:\opencv\opencv-4.6.0 etc...
  - [Add it to path](https://www.architectryan.com/2018/03/17/add-to-the-path-on-windows-10/) (`C:\opencv\opencv-build\install\x64\mingw\bin`)
  - This was opencv built from source using -WITHQT option. You can build it from source if you wish (see below), takes ~20 minutes and some more programs.
3. In Qt Creator, open a new project and select `./src/rvm.pro`


<details open>
<summary> Building OpenCV from source (open for detail)</summary>
<br>
Follow [this tutorial](https://wiki.qt.io/How_to_setup_Qt_and_openCV_on_Windows/)

[Video](https://www.youtube.com/watch?v=_fqpYLM6SCw) for more details- not an exact tutorial but close. Be sure to also select the parameters mentioned in the qt.io tutorial above. 

In Cmake, if some sections aren't showing up press configure again. Keep changing what it says and pressing configure until there are no red rows.  
If you installed Python through the windows store, you will probably be missing Python.h. If so download and install Python from Python's website and add that to your path.

</details>



## Developing on Linux (Fedora Workstation 37)
- see linux branch. 

### Dependencies:
These versions were used during development:
- OpenCV 4.6.0
- Qt 6.4.2


### Troubleshooting
Edit the paths in `rvm.pro` to match your opencv installation (shouldn't be an issue if instructions were followed)

Check that QTCreator is configured for version 6.4.2 or above

Here's a way to test that OpenCV and QTCreator are working with an example project: https://www.youtube.com/watch?v=fYBdwGpHQGw 

Restart your computer



-----------------------

