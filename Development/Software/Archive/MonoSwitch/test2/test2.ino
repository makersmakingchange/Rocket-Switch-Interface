#include <Adafruit_NeoPixel.h>

// How many internal neopixels do we have? some boards have more than one!
#define NUMPIXELS        1

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);

  pinMode(PIN_SWITCH, INPUT_PULLDOWN);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20); // not so bright
  delay(4000);

}

// the loop routine runs over and over again forever:
void loop() {
  // say hi
   bool curr_switch = digitalRead(PIN_SWITCH);
  if (curr_switch == HIGH) {
    pixels.fill(0x000000);
    pixels.show();
  } else {
    pixels.fill(0x0000FF);
    pixels.show();    
  }

  delay(500); // wait half a second
}
