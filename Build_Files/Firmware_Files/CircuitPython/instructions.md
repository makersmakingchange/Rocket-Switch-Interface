# CircuitPython Installation

To install the CircuitPython Code:

1. Download the following files and folders:

- code.py
- adafruit_hid folder
- adafruit-circuitpython...uf2

2. If you haven't installed CircuitPython to the board previously, double-click the reset button on the board to enter bootloader mode. Drag the .uf2 files into the BOOT drive that appears.

3. Drag and drop the code.py file into the drive.

4. Drag the adafruit_hid folder into the lib folder in the drive.

Note: If you are on a Mac and receive a memory error, you may need to copy the folder using command line instead of dragging and dropping. You will need to:

- Open the Terminal app 
- Navigate to the location of the folder most likely you'll need to enter: </br>
```cd Downloads```

- To copy the entire folder and internal files type: </br>
```cp -rX adafruit-hid /Volumes/CIRCUITPY/lib```

You should now be able to see the adafruit-hid folder inside the lib folder.

That's it! Your Rocket Switch Interface will now send a Tab and Space when your connected switches are pressed.

If you'd like to change the keys being sent: 
1. visit the [Adafruit HID Documentation](https://docs.circuitpython.org/projects/hid/en/latest/_modules/adafruit_hid/keycode.html) for the complete list of keycodes. 

2. Open the ```code.py``` file in a simple text editor or better yet a code editor if you have one installed.
3. Type the desired keycode on line number 25 replacing the TAB or SPACE.
4. Save your file and the updates will automatically be applied.
