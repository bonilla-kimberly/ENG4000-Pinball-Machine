// ================================
// Main Pinball Machine Class Cotroller 
// - Handle inputs: read switches, buttons, sensors
// ================================


// -------- Pin Definitions --------
//External switch (button on playfield) = L_flipper_sw
//External switch (button on playfield) = R_flipper_sw
//Pin on Relay for right flipper solenoid = L_flipper
//Pin on Relay for ball relase solenoid = R_flipper

const int L_flipper_sw = 4; 
const int R_flipper_sw = 5; 
const int L_flipper = 6;
const int R_flipper = 7;


// -------- Setup --------
// HIGH == relay and button are off 
// LOW == relay and button are on  
void setup() {
  Serial.begin(115200);                                   

  pinMode(L_flipper_sw, INPUT_PULLUP);
  pinMode(R_flipper_sw, INPUT_PULLUP);

  pinMode(L_flipper, OUTPUT);
  pinMode(R_flipper, OUTPUT);

  // Default all should be turned off
  digitalWrite(L_flipper, HIGH); 
  digitalWrite(R_flipper, HIGH);
}

// -------- Main Loop --------
void loop() {
//Read values coming from button and relay
  int buttonState = digitalRead(L_flipper_sw);
  int buttonState2 = digitalRead(R_flipper_sw);
  Serial.print("Button: ");
  Serial.print(buttonState);
  Serial.print(buttonState2);

  // use for debugging 
  int relayState = digitalRead(L_flipper);
  Serial.print("Relay: ");
  Serial.println(relayState);

  if (buttonState == LOW && buttonState2 == LOW) {           
    activateActuatorL();
    activateActuatorR();
  } else if (buttonState == LOW) {
    activateActuatorL();
  } else if (buttonState2 == LOW) {
    activateActuatorR();
  }
   else if (buttonState == HIGH || buttonState2 == HIGH) {                        
    deactivateActuators();
  }
  delay(200); //debounce delay 
}

// ================================
// ActuatorSystem 
// - activates solenoinds via relays (flippers, bumper, ball relase solenoid)
// ================================

void activateActuatorL() {
    digitalWrite(L_flipper, LOW);  
}

void activateActuatorR() {
  digitalWrite(R_flipper, LOW); 
}

void deactivateActuators() {
    digitalWrite(L_flipper, HIGH);                         
    digitalWrite(R_flipper, HIGH); 
}













