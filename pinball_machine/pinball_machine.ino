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

//Pin on 10 point scoring BL, BR, ML, MR, TL, TR path and LW,RW bumper = 10_points
const int points10 = 9;

//Pin on bumpers with relation to LED combo scoring
const int TL_bumper = 10;
const int TR_bumper = 11;
const int ML_bumper = 12;
const int MR_bumper = 13;

//Pin on targets
const int BR_target = 14;
const int BL_target = 15;
const int Y_target = 16;
const int R_target = 17;

//pin on LED switches
const int R1_switch = 18;
const int Y2_switch = 19;
const int R3_switch= 20;
const int Y4_switch = 21;

//Score counter initial 0
int score = 0;

// -------- Setup --------
// HIGH == relay and button are off 
// LOW == relay and button are on  
void setup() {
  Serial.begin(115200);            
                       

  pinMode(L_flipper_sw, INPUT_PULLUP);
  pinMode(R_flipper_sw, INPUT_PULLUP);

  pinMode(L_flipper, OUTPUT);
  pinMode(R_flipper, OUTPUT);

//bumpers
  pinMode(points10, INPUT_PULLUP);
  pinMode(TL_bumper, INPUT_PULLUP);
  pinMode(TR_bumper, INPUT_PULLUP);
  pinMode(ML_bumper, INPUT_PULLUP);
  pinMode(MR_bumper, INPUT_PULLUP);

  // Default all should be turned off
  digitalWrite(L_flipper, HIGH); 
  digitalWrite(R_flipper, HIGH);
}

// -------- Main Loop --------
void loop() {
//Read values coming from button and relay
  int buttonState = digitalRead(L_flipper_sw);
  int buttonState2 = digitalRead(R_flipper_sw);

  int point10s = digitalRead(points10);

  Serial.print("Button: ");
  Serial.print(buttonState);
  Serial.print(buttonState2);

  // use for debugging 
  int relayState = digitalRead(L_flipper);
  Serial.print("Relay: ");
  Serial.println(relayState);

  // display score
  Serial.print("Score: ");
  Serial.println(score);

  //flippers
  if (buttonState == LOW) {
    activateActuatorL();
  } if (buttonState2 == LOW) {
    activateActuatorR();
  }

  //score
  //if (){}

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

// ================================
// ScoringSystem
// - Stores score
// - counts score and combos
// ================================

void addPoints(int points) {
  score += points;
}












