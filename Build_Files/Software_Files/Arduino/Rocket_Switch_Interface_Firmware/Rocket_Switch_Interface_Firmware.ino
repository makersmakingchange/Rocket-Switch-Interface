/** ************************************************************************
 * File Name: Rocket_Switch_Interface_Firmware.ino 
 * Title: Rocket Switch Interface Firmware
 * Developed by: MakersMakingChange
 * Version Number: 1.2 (26-02-2024)
 * Github Link: https://github.com/makersmakingchange/Rocket-Switch-Interface
 ***************************************************************************/
 
#include "MouseKeyboard.h"
#include <StopWatch.h>
#include <math.h>
#include <Adafruit_NeoPixel.h>
#include <FlashStorage.h>


//Can be changed based on the needs of the users
#define MOUSE_MOVE_MULTI         2                                        //Morse mouse move multiplier 
#define SWITCH_REACTION_TIME     50                                       //Minimum time for each switch action ( level 10 : 1x50 =50ms , level 1 : 10x50=500ms )
#define SWITCH_MODE_CHANGE_TIME  4000                                     //How long to hold switch 4 to change mode 

#define LED_BRIGHTNESS           150                                      //The mode led color brightness which is always on ( Use a low value to decrease power usage )
#define LED_ACTION_BRIGHTNESS    150                                      //The action led color brightness which can be a higher value than LED_BRIGHTNESS


//Define Switch pins
#define LED_PIN                  0
#define SWITCH1_PIN              2
#define SWITCH2_PIN              3

// Variable Declaration

//Declare switch state variables for each switch
int switch1State;
int switch2State;


//Stopwatches array used to time switch presses
StopWatch timeWatcher[3];
StopWatch switch1TimeWatcher[1];

//Declare Switch variables for settings 
int switchConfigured;
int switchReactionTime;
int switchReactionLevel;
int switchMouseMove;
int switchMode;

int morseReactionTime;

FlashStorage(switchConfiguredFlash, int);
FlashStorage(switchReactionLevelFlash, int);
FlashStorage(switchModeFlash, int);
FlashStorage(switchswitchMouseMoveFlash, int);

char inputCommand[7];

//RGB LED Color code structure 

struct rgbColorCode {
    int r;    // red value 0 to 255
    int g;   // green value
    int b;   // blue value
 };

//Color structure 
typedef struct { 
  uint8_t colorNumber;
  String colorName;
  rgbColorCode colorCode;
} colorStruct;

 //Mode structure 
typedef struct { 
  uint8_t modeNumber;
  String modeName;
  uint8_t modeColorNumber;
} modeStruct;


 //Switch structure 
typedef struct { 
  uint8_t switchNumber;
  uint8_t switchChar;
  uint8_t switchMacChar;
  uint8_t switchMouseAction;
  uint8_t switchColorNumber;
} switchStruct;


 //Settings Action structure 
typedef struct { 
  uint8_t settingsActionNumber;
  String settingsActionName;
  uint8_t settingsActionColorNumber;
} settingsActionStruct;

//Color properties 
const colorStruct colorProperty[] {
    {1,"Green",{60,0,0}},
    {2,"Pink",{0,50,40}},
    {3,"Yellow",{60,50,0}},    
    {4,"Orange",{20,60,0}},
    {5,"Blue",{0,0,60}},
    {6,"Red",{0,60,0}},
    {7,"Purple",{0,50,128}},
    {8,"Teal",{128,0,128}}       
};



//Switch properties 
const switchStruct switchProperty[] {
    {1,'a', '1', 5, 6},                             //{1=dot,'a', '1', mouse action 5, 6=red}
    {2,'b', '2', 6, 5}                              //{2=dash,'b','2', mouse action 6, 5=blue}
};



//Settings Action properties 
const settingsActionStruct settingsProperty[] {
    {1,"Decrease Reaction",6},                             //{1=Increase Reaction,5=blue}
    {2,"Increase Reaction",5},                             //{2=Decrease Reaction,6=red}
    {3,"Max Reaction",3},                                  //{3=Max Reaction,1=yellow}
    {4,"Min Reaction",3},                                  //{4=Min Reaction,1=yellow}
    {5,"Web Update",1}                                     //{5,"Web Update",2=green}
};

//Mode properties 
const modeStruct modeProperty[] {
    {1,"Switch",8},
    {2,"Switch Mac",2},
    {3,"Mouse Switch",7},
    {4,"Settings",4}
};

USBMouse mouse;                                    //Starts an instance of the USB mouse object
USBKeyboard keyboard;                                    //Starts an instance of the USB keyboard object

//Setup NeoPixel LED
Adafruit_NeoPixel ledPixels = Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {

  ledPixels.begin();                                                           //Start NeoPixel
  Serial.begin(115200);                                                        //Start Serial
  mouse.begin();
  keyboard.begin();
  delay(1000);
  switchSetup();                                                               //Setup switch
  delay(5); 
  displayFeatureList();
  delay(5);
  initLedFeedback();                                                          //Led will blink in a color to show the current mode 


  //Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);                 
  //Initialize the switch pins as an input                                     
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
};

void loop() {

  static int ctr;                          //Control variable to set previous status of switches 
  unsigned long timePressed;               //Time that switch one or two are pressed
  unsigned long timeNotPressed;            //Time that switch one or two are not pressed
  static int previousSwitch1State;         //Previous status of switch 1
  
  //Update status of switch inputs
  switch1State = digitalRead(SWITCH1_PIN);
  switch2State = digitalRead(SWITCH2_PIN);

  timePressed = timeNotPressed  = 0;       //reset time counters
  if (!ctr) {                              //Set previous status of switch two 
    previousSwitch1State = HIGH;  
    ctr++;
  }
  //Check if switch 1 is pressed to change switch mode
  if (switch1State == LOW && previousSwitch1State == HIGH) {
     if (switch1State == LOW) { 
      previousSwitch1State = LOW; 
     }
     switch1TimeWatcher[0].stop();                                //Reset and start the timer         
     switch1TimeWatcher[0].reset();                                                                        
     switch1TimeWatcher[0].start(); 
  }
  // Switch 1 was released
  if (switch1State == HIGH && previousSwitch1State == LOW) {
    previousSwitch1State = HIGH;
    timePressed = switch1TimeWatcher[0].elapsed();                //Calculate the time that switch one was pressed 
    switch1TimeWatcher[0].stop();                                 //Stop the single action (dot/dash) timer and reset
    switch1TimeWatcher[0].reset();
    //Perform action if the switch has been hold active for specified time
    if (timePressed >= SWITCH_MODE_CHANGE_TIME){
      releaseAllInputs();
      incrementSwitchMode();                                                                
    } else if(switchMode==4) {
      settingsAction(LOW,switch2State); 
    }
  }
  //Perform actions based on the mode
  switch (switchMode) {
      case 1:
        keyboardAction(false,switch1State,switch2State);                                    //Keyboard Switch mode
        break;
      case 2:
        keyboardAction(true,switch1State,switch2State);                                     //Mac Keyboard Switch mode
        break;
      case 3:
        mouseAction(switch1State,switch2State);                                             //Mouse Switch mode
        break;
      case 4:
        settingsAction(HIGH,switch2State);                                                  //Settings mode
        break;
  };
  ledPixels.show(); 
  delay(5);
}

//***DISPLAY FEATURE LIST FUNCTION***//

void displayFeatureList(void) {

  Serial.println(" ");
  Serial.println(" --- ");
  Serial.println("Rocket Switch Interface");
  Serial.println(" ");
  Serial.println("VERSION: 1.0 (November 2022)");
  Serial.println(" --- ");
  Serial.println(" ");

}

//***MODIFY SETTINGS FUNCTION***//

void modifySetting(void) {
  if (Serial.available() > 0) {
    String serialString = Serial.readString();
    if(serialString.length()==6){
      String outputString = "";
      serialString.toCharArray(inputCommand, 7);
      if(inputCommand[0] == 'S' && inputCommand[1] == 'S' && inputCommand[2] == ',' && inputCommand[3] == '0' && inputCommand[4] == ':' && inputCommand[5] == '0'){
        inputCommand[5] = switchReactionLevel;
        outputString = String(inputCommand);
        Serial.println(outputString);
      }
      else if(inputCommand[0] == 'S' && inputCommand[1] == 'S' && inputCommand[2] == ',' && inputCommand[3] == '1' && inputCommand[4] == ':'){
        updateReactionLevel(inputCommand[5]-'0');
        inputCommand[5] = switchReactionLevel;
        outputString = String(inputCommand);
        Serial.println(outputString);
      }
      else if(inputCommand[0] == 'M' && inputCommand[1] == 'D' && inputCommand[2] == ',' && inputCommand[3] == '0' && inputCommand[4] == ':' && inputCommand[5] == '0'){
        inputCommand[5] = switchMode;
        outputString = String(inputCommand);
        Serial.println(outputString);
      }
      else if(inputCommand[0] == 'M' && inputCommand[1] == 'D' && inputCommand[2] == ',' && inputCommand[3] == '1' && inputCommand[4] == ':'){
        updateSwitchMode(inputCommand[5]-'0');
        inputCommand[5] = switchMode;
        outputString = String(inputCommand);
        Serial.println(outputString);
      }
    }

  }
}



//***RGB LED FUNCTION***//

void setLedBlink(int numBlinks, int delayBlinks, int ledColor,uint8_t ledBrightness) {
  if (numBlinks < 0) numBlinks *= -1;

      for (int i = 0; i < numBlinks; i++) {
        ledPixels.setPixelColor(0, ledPixels.Color(colorProperty[ledColor-1].colorCode.g,colorProperty[ledColor-1].colorCode.r,colorProperty[ledColor-1].colorCode.b));
        ledPixels.setBrightness(ledBrightness);
        ledPixels.show(); 
        delay(delayBlinks);
        ledPixels.setPixelColor(0, ledPixels.Color(0,0,0));
        ledPixels.setBrightness(ledBrightness);
        ledPixels.show(); 
        delay(delayBlinks);
      }
}

//***UPDATE RGB LED COLOR FUNCTION***//

void updateLedColor(int ledColor, uint8_t ledBrightness) {
    ledPixels.setPixelColor(0, ledPixels.Color(colorProperty[ledColor-1].colorCode.g,colorProperty[ledColor-1].colorCode.r,colorProperty[ledColor-1].colorCode.b));
    ledPixels.setBrightness(ledBrightness);
    ledPixels.show();
}

//***GET RGB LED COLOR FUNCTION***//

uint32_t getLedColor(int ledModeNumber) {

  int colorNumber= modeProperty[ledModeNumber-1].modeColorNumber-1;
  
  return (ledPixels.Color(colorProperty[colorNumber].colorCode.g,colorProperty[colorNumber].colorCode.r,colorProperty[colorNumber].colorCode.b));
}

//***GET RGB LED BRIGHTNESS FUNCTION***//

uint8_t getLedBrightness() {
  return (ledPixels.getBrightness());
}

//***SET RGB LED COLOR FUNCTION***//

void setLedColor (uint32_t ledColor, uint8_t ledBrightness){
  ledPixels.setPixelColor(0, ledColor);
  ledPixels.setBrightness(ledBrightness);
  ledPixels.show(); 
}

//***SET RGB LED BRIGHTNESS FUNCTION***//

void setLedBrightness(uint8_t ledBrightness) {
  ledPixels.setBrightness(ledBrightness);
  ledPixels.show();
}

//***CLEAR RGB LED FUNCTION***//

void ledClear() {
  ledPixels.setPixelColor(0, ledPixels.Color(0,0,0));
  ledPixels.show(); 
}

void switchFeedback(int switchNumber,int modeNumber,int delayTime, int blinkNumber =1)
{
  //Get previous led color and brightness 
  uint32_t previousColor = getLedColor(modeNumber);
  uint8_t previousBrightness = getLedBrightness();
 
  //updateLedColor(switchProperty[switchNumber-1].switchColorNumber,LED_ACTION_BRIGHTNESS);
  //delay(MORSE_REACTION);
  setLedBlink(blinkNumber,delayTime,switchProperty[switchNumber-1].switchColorNumber,LED_ACTION_BRIGHTNESS);
  delay(5);

  //Set previous led color and brightness 
  setLedColor(previousColor,previousBrightness);
  
}

void settingsFeedback(int settingsNumber,int modeNumber,int delayTime, int blinkNumber =1)
{
  //Get previous led color and brightness 
  uint32_t previousColor = getLedColor(modeNumber);
  uint8_t previousBrightness = getLedBrightness();
 
  setLedBlink(blinkNumber,delayTime,settingsProperty[settingsNumber-1].settingsActionColorNumber,LED_ACTION_BRIGHTNESS);
  delay(5);

  //Set previous led color and brightness 
  setLedColor(previousColor,previousBrightness);
  
}

void modeFeedback(int modeNumber,int delayTime, int blinkNumber =1)
{

   //Get new led color and brightness 
  uint32_t newColor = getLedColor(modeNumber);
  uint8_t newBrightness = getLedBrightness();
  
  setLedBlink(blinkNumber,delayTime,modeProperty[modeNumber-1].modeColorNumber,LED_ACTION_BRIGHTNESS);
  delay(5);

  //Set new led color and brightness 
  setLedColor(newColor,newBrightness);
  
}

//***SETUP SWITCH MODE FUNCTION***//

void switchSetup() {
  //Check if it's first time running the code
  switchConfigured = switchConfiguredFlash.read();
  delay(5);
  
  if (switchConfigured==0) {
    //Define default settings if it's first time running the code
    switchReactionLevel=10;
    switchMode=1;
    switchConfigured=1;
    switchMouseMove=2;

    //Write default settings to flash storage 
    switchReactionLevelFlash.write(switchReactionLevel);
    switchModeFlash.write(switchMode);
    switchswitchMouseMoveFlash.write(switchMouseMove);
    switchConfiguredFlash.write(switchConfigured);
    delay(5);
      
  } else {
    //Load settings from flash storage if it's not the first time running the code
    switchReactionLevel=switchReactionLevelFlash.read();
    switchMode=switchModeFlash.read();
    switchMouseMove=switchswitchMouseMoveFlash.read();
    delay(5);
  }  

    //Serial print settings 
    Serial.print("Switch Mode: ");
    Serial.println(switchMode);

    Serial.print("Switch Reaction Level: ");
    Serial.println(switchReactionLevel);
    Serial.print("Reaction Time(ms): ");
    Serial.println(switchReactionTime);
    Serial.print("Mouse Move (0-127): ");
    Serial.println(switchMouseMove);
    //Calculate switch delay based on switchReactionLevel
    switchReactionTime = ((11-switchReactionLevel)*SWITCH_REACTION_TIME);
}


void initLedFeedback(){
  setLedBlink(2,500,modeProperty[switchMode-1].modeColorNumber,LED_ACTION_BRIGHTNESS);
  delay(5);
  updateLedColor(modeProperty[switchMode-1].modeColorNumber,LED_BRIGHTNESS);
  delay(5);
}

//***ADAPTIVE SWITCH KEYBOARD FUNCTION***//

void keyboardAction(bool macMode, int switch1,int switch2) {
    if(!switch1) {
      switchFeedback(1,switchMode,switchReactionTime,1);
      //Serial.println("a");
      (macMode) ? keyboard.press(switchProperty[0].switchMacChar) : keyboard.press(switchProperty[0].switchChar);
    } else if(!switch2) {
      switchFeedback(2,switchMode,switchReactionTime,1);
      //Serial.println("b");
      (macMode) ? keyboard.press(switchProperty[1].switchMacChar) : keyboard.press(switchProperty[1].switchChar);
    } else
    {
      keyboard.releaseAll();
    }
    delay(5);

}

//***MOUSE ACTIONS FUNCTION***//

void mouseAction(int switch1,int switch2) {
    if(!switch1) {
      switchFeedback(1,switchMode,switchReactionTime,1);
      //Serial.println("a");
      mousePerform(switchProperty[0].switchMouseAction,switchMouseMove,switchMouseMove);
    } else if(!switch2) {
      switchFeedback(2,switchMode,switchReactionTime,1);
      //Serial.println("b");
      mousePerform(switchProperty[1].switchMouseAction,switchMouseMove,switchMouseMove);
    } 
    delay(5);  
}

//***PERFORM MOUSE ACTIONS FUNCTION***//

void mousePerform(int actionNumber,int xValue,int yValue) {

    switch (actionNumber) {
      case 0:
        break;
      case 1:
        mouse.move(0, -yValue*MOUSE_MOVE_MULTI);
        break;
      case 2:
        mouse.move(0, yValue*MOUSE_MOVE_MULTI);
        break;
      case 3:
        mouse.move(-xValue*MOUSE_MOVE_MULTI, 0);
        break;
      case 4:
        mouse.move(xValue*MOUSE_MOVE_MULTI, 0);
        break;
      case 5:
        if (!mouse.isPressed(MOUSE_LEFT)) {
          mouse.press(MOUSE_LEFT);
          delay(50);
          mouse.release(MOUSE_LEFT);
        } 
        else if (mouse.isPressed(MOUSE_LEFT)) {
          mouse.release(MOUSE_LEFT);
        }    
        break;
      case 6:
        if (!mouse.isPressed(MOUSE_RIGHT)) {
          mouse.press(MOUSE_RIGHT);
          delay(50);
          mouse.release(MOUSE_RIGHT);
        }
        else if (mouse.isPressed(MOUSE_RIGHT)) {
          mouse.release(MOUSE_RIGHT);
        }   
        break;                         
  };
}


//***INCREMENT SWITCH MODE FUNCTION***//

void incrementSwitchMode(){
    //Update switch mode varia
    switchMode++;
    if (switchMode == (sizeof (modeProperty) / sizeof (modeProperty[0]))+1) {
      switchMode=1;
    } 
    else {
    }

    
    //Blink 2 times in modes color 
    //setLedBlink(2,500,modeProperty[switchMode].modeColorNumber,LED_ACTION_BRIGHTNESS);
    modeFeedback(switchMode,500,2);

    //Serial print switch mode
    Serial.print("Switch Mode: ");
    Serial.println(switchMode);
    
    //Save switch mode in flash storage
    switchModeFlash.write(switchMode);
    delay(25);
}

//***UPDATE SWITCH MODE FUNCTION***//

void updateSwitchMode(int inputSwitchMode){
    if ((inputSwitchMode < (sizeof (modeProperty) / sizeof (modeProperty[0]))+1) && (inputSwitchMode > 0)) {
      switchMode=inputSwitchMode;
    } 
    modeFeedback(switchMode,500,2);

    //Serial print switch mode
    Serial.print("Switch Mode: ");
    Serial.println(switchMode);
    
    //Save switch mode in flash storage
    switchModeFlash.write(switchMode);
    delay(25);
}

//***CONFIGURATION MODE ACTIONS FUNCTION***//

void settingsAction(int switch1,int switch2) {
  if(switch1==LOW) {
    decreaseReactionLevel();
  }
  if(switch2==LOW) {
    increaseReactionLevel();
  }
}

//***Update SWITCH REACTION LEVEL FUNCTION***//

void updateReactionLevel(int inputReactionLevel) {
  if (inputReactionLevel >= 0 && inputReactionLevel <= 10) {
    switchReactionLevel = inputReactionLevel;
    settingsFeedback(5,switchMode,100,switchReactionLevel);
    switchReactionTime = ((11-switchReactionLevel)*SWITCH_REACTION_TIME);
    Serial.print("Reaction level: ");
    Serial.println(switchReactionLevel);
    Serial.print("Reaction Time(ms): ");
    Serial.println(switchReactionTime);
    switchReactionLevelFlash.write(switchReactionLevel);
    delay(25);
  } else {
    delay(25);
  }
}

//***INCREASE SWITCH REACTION LEVEL FUNCTION***//

void increaseReactionLevel(void) {
  switchReactionLevel++;
  if (switchReactionLevel == 11) {
    //setLedBlink(6,100,3,LED_ACTION_BRIGHTNESS);
    settingsFeedback(3,switchMode,100,6);
    switchReactionLevel = 10;
  } else {
    //setLedBlink(switchReactionLevel,100,4,LED_ACTION_BRIGHTNESS);
    settingsFeedback(1,switchMode,100,switchReactionLevel);
    switchReactionTime = ((11-switchReactionLevel)*SWITCH_REACTION_TIME);
    delay(25);
  }
  Serial.print("Reaction level: ");
  Serial.println(switchReactionLevel);
  Serial.print("Reaction Time(ms): ");
  Serial.println(switchReactionTime);
  switchReactionLevelFlash.write(switchReactionLevel);
  delay(25);
}

//***DECREASE SWITCH REACTION LEVEL FUNCTION***//

void decreaseReactionLevel(void) {
  switchReactionLevel--;
  if (switchReactionLevel == 0) {
    //setLedBlink(6,100,3,LED_ACTION_BRIGHTNESS);
    settingsFeedback(4,switchMode,100,switchReactionLevel);
    switchReactionLevel = 1; 
  } else {
    //setLedBlink(switchReactionLevel,100,5,LED_ACTION_BRIGHTNESS);
    settingsFeedback(2,switchMode,100,6);
    switchReactionTime = ((11-switchReactionLevel)*SWITCH_REACTION_TIME);
    delay(25);
  } 
  Serial.print("Reaction level: ");
  Serial.println(switchReactionLevel);
  Serial.print("Reaction Time(ms): ");
  Serial.println(switchReactionTime);
  
  switchReactionLevelFlash.write(switchReactionLevel);
  delay(25);
}

//*** RELEASE ALL INPUTS FUNCTION **//
void releaseAllInputs(void){
  keyboard.releaseAll();
  mouse.release(MOUSE_LEFT);
  mouse.release(MOUSE_RIGHT);
}
