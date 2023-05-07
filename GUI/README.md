## Re-Imagining Breath GUI
### Running instructions

- Run `gui_with_logic.py` (while the  gui.py file is what everything was before merging your playback logic in), 
- Play() function starts on line 193 (for video speed/direction)
- other files: thumbnails, videos folders, and cursor_20t.cur file. 
	- only first 9 videos and thumbnails included to save space, rest are available in Google Drive.
- To use a Joy-Con with it: connect a right joycon via Bluetooth to your computer, then you'll also need the `joycon.py` file, 

### Requirements 
- Python 3.8+ (should work probably)
- `pip install opencv-python joycon-python hidapi pyglm Pillow mouse pylab-sdk requests` (might have missed a few libraries)
- Fedora linux: `sudo dnf install python3-pillow-tk`
- Debian/Ubuntu: `sudo apt-get install python3-pil python3-pil.imagetk`
