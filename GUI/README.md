## Re-Imagining Breath GUI
### Running instructions
- Run `gui_with_logic.py` 

Note: cursor doesn't work on Linux. Use linux branch or edit line 89 to `self.window.configure(bg="black")`.

### About
- Play() function starts on line 252 (for video speed/direction)
- other files: thumbnails, videos folders, and cursor_20t.cur file. 
- To use a Joy-Con with it: connect a right joycon via Bluetooth to your computer, then you'll also need the `joycon.py` file, 



### Requirements 
- Python 3.8+ (should work probably)
- `pip install opencv-python joycon-python hidapi pyglm Pillow mouse pylab-sdk requests` (might have missed a few libraries)
- Fedora linux: `sudo dnf install python3-pillow-tk`
- Debian/Ubuntu: `sudo apt-get install python3-pil python3-pil.imagetk`
