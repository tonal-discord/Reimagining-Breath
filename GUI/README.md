## Re-Imagining Breath GUI
### Running instructions

- Run `gui_with_logic.py` (while the  gui.py file is what everything was before merging your playback logic in), 
- Play() function starts on line 193 (for video speed/direction)
- other files: thumbnails, videos folders, and cursor_20t.cur file. 
	- only first 9 videos and thumbnails included to save space, rest are available in Google Drive.
- To use a Joy-Con with it: connect a right joycon via Bluetooth to your computer, then you'll also need the `joycon.py` file, uncomment lines 34 and 56 in `gui_with_logic.py `

### Requirements (check that this is all we need)
- Python 3.8+ (mostly a guess)
- `pip install opencv-python joycon-python hidapi pyglm Pillow mouse`