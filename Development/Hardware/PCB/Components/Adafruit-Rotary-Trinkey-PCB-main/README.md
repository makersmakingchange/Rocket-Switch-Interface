## Adafruit Rotary Trinkey PCB

<a href="http://www.adafruit.com/products/4964"><img src="assets/4964.jpg?raw=true" width="500px"><br/>
Click here to purchase one from the Adafruit shop</a>

PCB files for the Adafruit Rotary Trinkey. 

Format is EagleCAD schematic and board layout
* https://www.adafruit.com/product/4964

### Description

It's half USB Key, half Adafruit Trinket, half rotary encoder...it's Rotary Trinkey, the circuit board with a Trinket M0 heart, a NeoPixel glow, and a rotary encoder body. We were inspired by this project from TodBot where a rotary encoder was soldered onto a QT Py. So we thought, hey what if we made something like that that plugs right into your computer's USB port, with a fully programmable color NeoPixel? And this is what we came up with!

The PCB is designed to slip into any USB A port on a computer or laptop. There's an ATSAMD21 microcontroller on board with just enough circuitry to keep it happy. Three pins of the microcontroller connects to a rotary encoder with built-in push switch. Another connects to a NeoPixel LED. Finally, one pin can be used as a capacitive touch input. A reset button lets you enter bootloader mode if necessary. That's it!

Rotary encoders are soooo much fun! Twist em this way, then twist them that way. Unlike potentiometers, they go all the way around, and often have little detents for tactile feedback. But, if you just want to add one to your computer, you know its a real pain. This board is designed to make it very easy.

The SAMD21 can run CircuitPython or Arduino very nicely - with existing Rotary, NeoPixel and our FreeTouch libraries for the capacitive touch input. Over the USB connection you can have serial, MIDI or HID connectivity. The Rotary Trinkey is perfect for simple projects that use a rotating user input and underlit colorful output. Maybe you'll set it up as a volume knob, scroll wheel, or zoom macro tool - whatever you like!

Please note this board DOES NOT come with a rotary encoder soldered in - we expect folks to pick their favorite rotary encoder and knob! Five solder points and you're done.

We think it's just an adorable little board, small and durable and inexpensive enough that it could be a first microcontroller board or inspiration for advanced developers to make something simple and fun.

* ATSAMD21E18 32-bit Cortex M0+ - 48 MHz 32 bit processor with 256KB Flash and 32 KB RAM
* Native USB supported by every OS - can be used in Arduino or CircuitPython as USB serial console, MIDI, Keyboard/Mouse HID, even a little disk drive for storing Python scripts.
* Can be used with Arduino IDE or CircuitPython
* Single under-lighting RGB NeoPixel LED
* One Capacitive Touch pad
* Standard '5-pin' rotary encoder footprint can be used with any "PEC11" or "PEC12" compatible rotary encoder.
* Cute & keychain-friendly!

### License

Adafruit invests time and resources providing this open source design, please support Adafruit and open-source hardware by purchasing products from [Adafruit](https://www.adafruit.com)!

Designed by Limor Fried/Ladyada for Adafruit Industries.

Creative Commons Attribution/Share-Alike, all text above must be included in any redistribution. 
See license.txt for additional details.
