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

// ---- LED Pin Definitions (single LEDs) ----
const int TARGET_LED  = 16; //on BL, BR, GR, GL LED 
const int TW_LED    = 17; //off TW1,2,3,4,Y,R LED
const int RED_LED = 18;  //off TR, ML LED
const int YELLOW_LED = 19; //off TL, MR LED
const int P_LED = 20; //off Special
const int PATH_LED = 21; //on BR, BL, ML, MR, TL, TR path LED
// const int BALL_RELEASE_LED = 35; //off ball release LED
// const int Y_LED1 = 36; //off 
// const int Y_LED2 = 37;
// const int Y_LED3 = 38;
// const int Y_LED4 = 39;
// const int Y_LED5 = 40;
// const int R_LED1 = 41; //off
// const int R_LED2 = 42;
// const int R_LED3 = 43;
// const int R_LED4 = 44;
// const int R_LED5 = 45;

// -------- Score logic --------
int score = 0; // initial set to 0
const unsigned long HIT_COOLDOWN_MS = 500;
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

  //const int NUM_LEDS[] = {TW_LED, RED_LED, YELLOW_LED, P_LED, BALL_RELEASE_LED, Y_LED1, Y_LED2, Y_LED3, Y_LED4, Y_LED5, R_LED1, R_LED2, R_LED3, R_LED4, R_LED5};

  for(int i = 0; i < sizeof(NUM_SWITCHES) / sizeof(NUM_SWITCHES[0]); i++){
      pinMode(NUM_SWITCHES[i], INPUT_PULLUP);
  }

  for(int i = 0; i < sizeof(NUM_ACTUATORS) / sizeof(NUM_ACTUATORS[0]); i++){
    pinMode(NUM_ACTUATORS[i], OUTPUT);
  }

  for(int i = 0; i < sizeof(NUM_RELAYS) / sizeof(NUM_RELAYS[0]); i++){
    digitalWrite(NUM_RELAYS[i], RELAY_OFF);
  }

  // for(int i = 0; i < sizeof(NUM_LEDS) / sizeof(NUM_LEDS[0]); i++){
  //   pinMode(NUM_LEDS[i], OUTPUT);
  //   digitalWrite(NUM_LEDS[i], LOW); // turn off all LEDs at start
  // }

  pinMode(PATH_LED, OUTPUT);
  digitalWrite(PATH_LED, HIGH); // turn on path LED at start

  pinMode(TARGET_LED, OUTPUT);
  digitalWrite(TARGET_LED, HIGH); // turn off target LED at start
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

  // if(pathState != previousPathState) {
  //   lastHitTime = millis();
  //   currentEvent = PATH_HIT;
  // } if(topState != previousTopState) {
  //   lastHitTime = millis();
  //   currentEvent = TOP_GROUP_HIT;
  // } if(targetState != previousTargetState) {
  //   lastHitTime = millis();
  //   currentEvent = BOTTOM_TARGET_HIT;
  // } if(redState != previousRedState) {
  //   lastHitTime = millis();
  //   currentEvent = RED_GROUP_HIT;
  // } if(yellowState != previousYellowState) {
  //   lastHitTime = millis();
  //   currentEvent = YELLOW_GROUP_HIT;
  // } if(onOffState != previousOnOffState) {
  //   lastHitTime = millis();
  //   currentEvent = ON_OFF_HIT;
  // } 


  unsigned long now = millis();
  bool cooldownActive = (now - lastHitTime) > HIT_COOLDOWN_MS;

  if (cooldownActive) {
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
unsigned long pathLedStartTime = 0;
bool pathLedBlinking = false;

unsigned long targetLedStartTime = 0;
bool targetLedBlinking = false;

unsigned long topGroupMillis = 0;
bool topGroupHit = false;

unsigned long redGroupMillis = 0;
bool redGroupAdd = false;

unsigned long yellowGroupMillis = 0;
bool yellowGroupAdd = false;

// ================================
// Handle Events for switches (points)
// ================================
void handleEvent() {
  switch (currentEvent) {

    case PATH_HIT:
      addPoints(50);
      pathLedStartTime = millis(); // Start timer for path LED
      pathLedBlinking = true; // Start blinking the path LED
      break;

    case TOP_GROUP_HIT:
      addPoints(5);
      break;

    case BOTTOM_TARGET_HIT:
      addPoints(10);
      targetLedStartTime = millis(); // Start timer for target LED
      targetLedBlinking = true; // Start blinking the target LED
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
// LEDs Logic 
// ================================
void updateLEDs() {
  // Blinking LEDs
  if (pathLedBlinking) {
    if(millis() - pathLedStartTime < 3000) { // Blink for 3 seconds
      digitalWrite(PATH_LED, (millis() % 500 > 200) ? LOW : HIGH); // Blink every 250ms
    } else {
      digitalWrite(PATH_LED, HIGH); // Keep the path LED on
      pathLedBlinking = false;
    }
  }

  if (targetLedBlinking) {
    if(millis() - targetLedStartTime < 3000) { // Blink for 3 seconds
     digitalWrite(TARGET_LED, (millis() % 500 > 200) ? LOW : HIGH); // Blink every 250ms
    } else {
      digitalWrite(TARGET_LED, HIGH); // Keep the target LED on
      targetLedBlinking = false;
    }
  }

//   // Turn on LEDs based on EVENT
//   if (currentEvent == TOP_GROUP_HIT) {
//     digitalWrite(TW_LED, HIGH);
//     topGroupMillis = millis(); // Start timer for top group LED
//   }
//   if (currentEvent == RED_GROUP_HIT) {
//     digitalWrite(RED_LED, HIGH);
//     redGroupMillis = millis(); // Start timer for red group LED
//   }
//   if (currentEvent == YELLOW_GROUP_HIT) {
//     digitalWrite(YELLOW_LED, HIGH);
//     yellowGroupMillis = millis(); // Start timer for yellow group LED
//   }

//   // Turn off LEDs
//   if (millis() - topGroupMillis > 5000) { // Keep the top group LED on for 3 seconds
//     digitalWrite(TW_LED, LOW);
//   }
//   if (millis() - redGroupMillis > 5000) { // Keep the red group LED on for 3 seconds
//     digitalWrite(RED_LED, LOW);
//   }
//   if (millis() - yellowGroupMillis > 5000) { // Keep the yellow group LED on for 3 seconds
//     digitalWrite(YELLOW_LED, LOW);
//   }

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
    handleEvent();
    updateLEDs();
    readInputsPoints();
    delay(1);

    //Serial.println(digitalRead(path)); //use for debugging to check wiring (111 = correct, 101 = wiring not done correctly)
    // No blocking delay: relay outputs stay responsive while score hits are rate-limited above.

}






































