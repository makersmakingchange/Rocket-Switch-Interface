#include <Adafruit_NeoPixel.h>
#define PIN_NEOPIXEL 0

const int switchPin1 = 2;     
const int switchPin2 = 3;    

// variables will change:
int switch1State = 0; 
int switch2State = 0; 

// Create the neopixel strip with the built in definitions NUM_NEOPIXEL and PIN_NEOPIXEL
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
int16_t neo_brightness = 50; // initialize with 50 brightness (out of 255)

void setup() {

  Serial.begin(115200);  
  // initialize the LED pin as an output:
  pinMode(switchPin1, INPUT_PULLUP);
  pinMode(switchPin2, INPUT_PULLUP);

  strip.begin();
  strip.setBrightness(neo_brightness);
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // read the state of the pushbutton value:
  switch1State = digitalRead(switchPin1);
  switch2State = digitalRead(switchPin2);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (switch1State == LOW) {
    Serial.println("Switch 1 is pressed");
    strip.setBrightness(neo_brightness);
    strip.setPixelColor(0, 255, 0, 255);
    strip.show();
  } 
  if (switch2State == LOW) {
    Serial.println("Switch 2 is pressed");
    strip.setBrightness(neo_brightness);
    strip.setPixelColor(0, 255, 255, 0);
    strip.show();
  }
  delay(100);
  strip.setBrightness(neo_brightness);
  strip.setPixelColor(0, 0, 0, 0);
  strip.show();
  strip.clear();
}
