// ================================
// Main Pinball Machine Class Controller
// ================================

// -------- Pin Definitions --------

//Group 1 (top section) 5 points - Only switches 
const int TW_switch = 1; //TW1,2,3,4,Y,R switch

//Group 2 + 3 (Yellow and Red Middle traffic light) 10 points for now - Switches and bumpers 
const int RED_sw_bumper = 2; //TR, ML
const int RED_bump = 3;
const int YELLOW_sw_bumper = 4; //TL, MR
const int YELLOW_bump = 5;

//Group 4 + 5 (paths) 50 points - Only switches
const int path = 6; //BR, BL, ML, MR, TL, TR path, W_bumpers

//Group 6 (bottom target) 10 points - Switches and bumpers 
const int target_sw = 7; //BL, BR target switch
const int target_bump = 8; //BL,BR target bump

//Group 7 + 8 (right and left flippers) - Switches and solenoid 
const int L_flipper_sw = 9;
const int R_flipper_sw = 10;
const int L_flipper = 11;
const int R_flipper = 12;

//Group 9 (Ball release) - Switches and bumpers 
const int ball_release_sw = 13;
const int ball_release = 14;

//Group 10 (ON/OFF) - Only switches
const int ON_OFF_sw = 15;

// -------- Switch logic --------
const int PRESSED = LOW;
const int RELEASED = HIGH;

// -------- Relay logic --------
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;


// -------- Score logic --------
int score = 0; // initial set to 0
const unsigned long HIT_COOLDOWN_MS = 1000;
unsigned long lastHitTime = 0;

int previousPathState = RELEASED;
int previousTopState = RELEASED;
int previousTargetState = RELEASED;
int previousRedState = RELEASED;
int previousYellowState = RELEASED;
int previousOnOffState = RELEASED;

// ================================
// Switch Table (Different Events)
// ================================
enum Event {
  NO_EVENT,
  PATH_HIT,
  TOP_GROUP_HIT,
  BOTTOM_TARGET_HIT,
  RED_GROUP_HIT,
  YELLOW_GROUP_HIT,
  ON_OFF_HIT
};

Event currentEvent = NO_EVENT;

// -------- Setup ------------
void setup() {
  Serial.begin(115200);
  const int NUM_SWITCHES[] = {path, L_flipper_sw, R_flipper_sw, TW_switch, target_sw, RED_sw_bumper, YELLOW_sw_bumper, ball_release_sw, ON_OFF_sw};

  const int NUM_ACTUATORS[] = {L_flipper, R_flipper, target_bump, RED_bump, YELLOW_bump, ball_release};

  const int NUM_RELAYS[] = {L_flipper, R_flipper, target_bump, RED_bump, YELLOW_bump, ball_release};

  for(int i = 0; i < sizeof(NUM_SWITCHES) / sizeof(NUM_SWITCHES[0]); i++){
      pinMode(NUM_SWITCHES[i], INPUT_PULLUP);
  }

  for(int i = 0; i < sizeof(NUM_ACTUATORS) / sizeof(NUM_ACTUATORS[0]); i++){
    pinMode(NUM_ACTUATORS[i], OUTPUT);
  }

  for(int i = 0; i < sizeof(NUM_RELAYS) / sizeof(NUM_RELAYS[0]); i++){
    digitalWrite(NUM_RELAYS[i], RELAY_OFF);
  }
}

// ================================
// Read Input switches for points (switch based (instant event))
// ================================
void readInputsPoints(){
  currentEvent = NO_EVENT;

  int pathState = digitalRead(path);
  int topState = digitalRead(TW_switch);
  int targetState = digitalRead(target_sw);
  int redState = digitalRead(RED_sw_bumper);
  int yellowState = digitalRead(YELLOW_sw_bumper);
  int onOffState = digitalRead(ON_OFF_sw);

  if(pathState != previousPathState) {
    lastHitTime = millis();
    currentEvent = PATH_HIT;
  } if(topState != previousTopState) {
    lastHitTime = millis();
    currentEvent = TOP_GROUP_HIT;
  } if(targetState != previousTargetState) {
    lastHitTime = millis();
    currentEvent = BOTTOM_TARGET_HIT;
  } if(redState != previousRedState) {
    lastHitTime = millis();
    currentEvent = RED_GROUP_HIT;
  } if(yellowState != previousYellowState) {
    lastHitTime = millis();
    currentEvent = YELLOW_GROUP_HIT;
  } if(onOffState != previousOnOffState) {
    lastHitTime = millis();
    currentEvent = ON_OFF_HIT;
  } 


  unsigned long now = millis();
  bool cooldownActive = (now - lastHitTime) < HIT_COOLDOWN_MS;

  if (!cooldownActive) {
    if (pathState == PRESSED && previousPathState == RELEASED) {
      currentEvent = PATH_HIT;
      lastHitTime = now;
    } else if (topState == PRESSED && previousTopState == RELEASED) {
      currentEvent = TOP_GROUP_HIT;
      lastHitTime = now;
    } else if (targetState == PRESSED && previousTargetState == RELEASED) {
      currentEvent = BOTTOM_TARGET_HIT;
      lastHitTime = now;
    } else if (redState == PRESSED && previousRedState == RELEASED) {
      currentEvent = RED_GROUP_HIT;
      lastHitTime = now;
    } else if (yellowState == PRESSED && previousYellowState == RELEASED) {
      currentEvent = YELLOW_GROUP_HIT;
      lastHitTime = now;
    } else if (onOffState == PRESSED && previousOnOffState == RELEASED) {
      currentEvent = ON_OFF_HIT;
      lastHitTime = now;
    }
  }

  previousPathState = pathState;
  previousTopState = topState;
  previousTargetState = targetState;
  previousRedState = redState;
  previousYellowState = yellowState;
  previousOnOffState = onOffState;
}

// ================================
// Read Input switches to activate relay (continous control)
// ================================
void handleFLippers() {
if(digitalRead(L_flipper_sw) == PRESSED){
    digitalWrite(L_flipper, RELAY_ON);
  } else {
     digitalWrite(L_flipper, RELAY_OFF);
  }

if(digitalRead(R_flipper_sw) == PRESSED){
    digitalWrite(R_flipper, RELAY_ON);
  } else {
     digitalWrite(R_flipper, RELAY_OFF);
  }
}

void handleBottomTargets() {
if(digitalRead(target_sw) == PRESSED){
    digitalWrite(target_bump, RELAY_ON);
  } else {
     digitalWrite(target_bump, RELAY_OFF);
  }

}

void handleMiddleRedYellow() {
if(digitalRead(RED_sw_bumper) == PRESSED){
    digitalWrite(RED_bump, RELAY_ON);
  } else {
     digitalWrite(RED_bump, RELAY_OFF);
  }

  if(digitalRead(YELLOW_sw_bumper) == PRESSED){
    digitalWrite(YELLOW_bump, RELAY_ON);
  } else {
     digitalWrite(YELLOW_bump, RELAY_OFF);
  }
}

void handleBallRelease() {
  if(digitalRead(ball_release_sw) == PRESSED){
    digitalWrite(ball_release, RELAY_ON);
  } else {
     digitalWrite(ball_release, RELAY_OFF);
  }

}

// ================================
// Handle Events for switches (points)
// ================================
void handleEvent() {
  switch (currentEvent) {

    case PATH_HIT:
      addPoints(50);
      break;

    case TOP_GROUP_HIT:
      addPoints(5);
      break;

    case BOTTOM_TARGET_HIT:
      addPoints(10);
      break;

    case RED_GROUP_HIT:
      addPoints(10);
      break;

    case YELLOW_GROUP_HIT:
      addPoints(10);
      break;

    case ON_OFF_HIT:
      resetPoints();
      break;
      
    case NO_EVENT:
      break;
  }
}

// ================================
// Score Logic 
// ================================
void addPoints(int points){
  score += points;
  Serial.print("Score +");
  Serial.print(points);
  Serial.print(" => ");
  Serial.println(score);
}

void resetPoints(){
  score = 0;
  Serial.print("Score reset => ");
  Serial.println(score);
}


// -------- Main Loop to read inputs --------
void loop() {

    handleFLippers();
    handleBottomTargets();
    handleMiddleRedYellow();
    handleBallRelease();
    readInputsPoints();
    handleEvent();

    delay(1);
    //Serial.println(digitalRead(path)); //use for debugging to check wiring (111 = correct, 101 = wiring not done correctly)
    // No blocking delay: relay outputs stay responsive while score hits are rate-limited above.

}






































