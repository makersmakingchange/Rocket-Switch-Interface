const int switchPin1 = 2;     
const int switchPin2 = 3;    

// variables will change:
int switch1State = 0; 
int switch2State = 0; 


void setup() {

  Serial.begin(115200);  
  // initialize the LED pin as an output:
  pinMode(switchPin1, INPUT_PULLUP);
  pinMode(switchPin2, INPUT_PULLUP);
}

void loop() {
  // read the state of the pushbutton value:
  switch1State = digitalRead(switchPin1);
  switch2State = digitalRead(switchPin2);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (switch1State == LOW) {
    Serial.println("Switch 1 is pressed");
  } 
  if (switch2State == LOW) {
    Serial.println("Switch 2 is pressed");
  }
  delay(100);
}
