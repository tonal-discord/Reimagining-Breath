# Re-Imagining Breath Spring 2023
### By: Tyler Dronen, Fiorela Esquivel-Martinez, John Kujawa, Amanda Li, George Thao

### Running the program
1. Download the [release](https://github.com/kujaw077/senior-design-project/releases/download/v1.0.0/Re-Imagining.Breath.v1.0.0.zip)(also shown on right side under "Releases")
2. Extract
3. Open realtime-video-magnification folder, run `rvm.exe` 
    - Click "Connect/Open"
    - May need to change camera device number if you aren't using default webcam
    - Change other options if desired
    - Press "OK"
    - Switch magnification type box to "Laplace"
    - You should see your webcam magnified now in the preview
    - Drag mouse over the preview to set a region of interest to be magnified. Right click to clear.
4. Open GUI folder, run `gui_with_logic.exe` 
5. Should be good to go!

### In this repository:
[`./Live-Video-Magnification`](https://github.com/tschnz/Live-Video-Magnification) (**see README in this directory for details**): 
An OpenCV/Qt C++ based realtime application for Eulerian Video Magnification. Works with multiple videos and cameras at the same time and let's you export the magnified videos.

`./GUI` (**see README in this directory for details**): 
A Python based graphical user interface that displays a grid of videos that a user can select using a mouse or Nintendo Joy-Con to be played based on the output of the `./Live-Video-Magnification` application. 

### References
[Original EVM paper](http://people.csail.mit.edu/mrub/vidmag/) (has Matlab code too)
