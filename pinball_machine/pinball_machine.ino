// ================================
// Main Pinball Machine Class Cotroller 
// - Handle inputs: read switches, buttons, sensors
// ================================


// -------- Pin Definitions --------
//External switch (button on playfield) = buttonPin
//Pin on Relay for right flipper solenoid = right_flipper_relay
//Pin on Relay for ball relase solenoid = ball_release_relay

const int buttonPin = 8; 
const int right_flipper_relay = 4;
const int ball_release_relay = 2;


// -------- Setup --------
// HIGH == relay and button are off 
// LOW == relay and button are on  
void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(right_flipper_relay, OUTPUT);
  pinMode(ball_release_relay, OUTPUT);

  // Default all should be turned off
  digitalWrite(right_flipper_relay, HIGH); 
  digitalWrite(ball_release_relay, HIGH);
}

// -------- Main Loop --------
void loop() {
//Read values coming from button and relay
  int buttonState = digitalRead(buttonPin);
  Serial.print("Button: ");
  Serial.print(buttonState);

  // use for debugging 
  int relayState = digitalRead(right_flipper_relay);
  Serial.print("Relay: ");
  Serial.println(relayState);

  if (buttonState == LOW) {           
    activateActuators();
  } else if (buttonState == HIGH) {                        
    deactivateActuators();
  }
  delay(200); //debounce delay 
}

// ================================
// ActuatorSystem 
// - activates solenoinds via relays (flippers, bumper, ball relase solenoid)
// ================================

void activateActuators() {
    digitalWrite(right_flipper_relay, LOW);      
    digitalWrite(ball_release_relay, LOW); 
}

void deactivateActuators() {
    digitalWrite(right_flipper_relay, HIGH);    
    digitalWrite(ball_release_relay, HIGH); 
}













