# SPDX-FileCopyrightText: 2018 Kattni Rembor for Adafruit Industries
#
# SPDX-License-Identifier: MIT
# Modified by: Loreto Dumitrescu
# CircuitPython code for the Rocket Switch Interface designed by Milad from Makers Making Change. 
# Project Details: https://makersmakingchange.com/project/rocket-switch-interface/

"""CircuitPython Rocket Switch Interface- Keyboard"""
import time

import board
import digitalio
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS
from adafruit_hid.keycode import Keycode

# The pins we'll use, each will have an internal pullup
keypress_pins = [board.ROTA, board.SWITCH]
# Our array of key objects
key_pin_array = []
# The Keycode sent for each button
# To view a complete list of available keycodes: 
# https://docs.circuitpython.org/projects/hid/en/latest/_modules/adafruit_hid/keycode.html
keys_pressed = [Keycode.TAB, Keycode.SPACE]
control_key = Keycode.SHIFT

# The keyboard object!
time.sleep(1)  # Sleep for a bit to avoid a race condition on some systems
keyboard = Keyboard(usb_hid.devices)
keyboard_layout = KeyboardLayoutUS(keyboard)  # We're in the US :)

# Make all pin objects inputs with pullups
for pin in keypress_pins:
    key_pin = digitalio.DigitalInOut(pin)
    key_pin.direction = digitalio.Direction.INPUT
    key_pin.pull = digitalio.Pull.UP
    key_pin_array.append(key_pin)

# Creates an instance of the on board light and sets it as an output
led = digitalio.DigitalInOut(board.NEOPIXEL)
led.direction = digitalio.Direction.OUTPUT

print("Waiting for key pin...")

while True:
    # Check each pin
    for key_pin in key_pin_array:
        if not key_pin.value:  # Is it grounded?
            i = key_pin_array.index(key_pin)
            print("Pin #%d is grounded." % i)

            # Turn on the red LED
            led.value = True

            while not key_pin.value:
                pass  # Wait for it to be ungrounded!            
            key = keys_pressed[i] # Get the keycode to press
            keyboard.press(key)  # "Press"...
            keyboard.release_all()  # ..."Release"!

            # Turn off the red LED
            led.value = False

    time.sleep(0.01)