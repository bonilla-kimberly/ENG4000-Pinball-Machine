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

const int special_sw = 9;

//Group 7 + 8 (right and left flippers) - Switches and solenoid 
const int L_flipper_sw = 10;
const int R_flipper_sw = 11;
const int L_flipper = 12;
const int R_flipper = 13;

//Group 9 (Ball release) - Switches and bumpers 
const int ball_release_sw = 14;
const int ball_release = 15;

//Group 10 (ON/OFF) - Only switches
const int ON_OFF_sw = 16;

// -------- Switch logic --------
const int PRESSED = LOW;
const int RELEASED = HIGH;

// -------- Relay logic --------
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

// ---- LED Pin Definitions (single LEDs) ----
const int TARGET_LED  = 17; //on BL, BR, GR, GL LED 
const int TW_LED    = 18; //off TW1,2,3,4,Y,R LED
const int RED_LED = 19;  //off TR, ML LED
const int YELLOW_LED = 20; //off TL, MR LED
const int P_LED = 21; //off Special
const int PATH_LED = 22; //on BR, BL, ML, MR, TL, TR path LED
const int BALL_RELEASE_LED = 36; //off ball release LED
const int Y_LED1 = 37; //off 
const int Y_LED2 = 38;
const int Y_LED3 = 39;
const int Y_LED4 = 40;
const int Y_LED5 = 41;
const int R_LED1 = 42; //off
const int R_LED2 = 43;
const int R_LED3 = 44;
const int R_LED4 = 45;
const int R_LED5 = 46;

// -------- Score logic --------
int score = 0; // initial set to 0
const unsigned long HIT_COOLDOWN_MS = 500;
unsigned long lastHitTime = 0;

bool specialMode = false;
const unsigned long SPECIAL_MODE_DURATION_MS = 5000; // 10 seconds
unsigned long specialModeStartTime = 0;

// -------- Ball Release logic --------
const int MAX_BALLS = 5;
bool gameOver = false;
int ballcount = 0; 

int previousPathState = RELEASED;
int previousTopState = RELEASED;
int previousTargetState = RELEASED;
int previousRedState = RELEASED;
int previousYellowState = RELEASED;
int previousOnOffState = RELEASED;
int previousBallRelease = RELEASED;

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
  SPECIAL_HIT,
  ON_OFF_HIT,
  BALL_RELEASE_HIT,
};

Event currentEvent = NO_EVENT;

// -------- Setup ------------
void setup() {
  Serial.begin(115200);
  const int NUM_SWITCHES[] = {path, L_flipper_sw, R_flipper_sw, TW_switch, target_sw, RED_sw_bumper, YELLOW_sw_bumper, ball_release_sw, ON_OFF_sw};

  const int NUM_ACTUATORS[] = {L_flipper, R_flipper, target_bump, RED_bump, YELLOW_bump, ball_release};

  const int NUM_RELAYS[] = {L_flipper, R_flipper, target_bump, RED_bump, YELLOW_bump, ball_release};

  const int NUM_LEDS[] = {TW_LED, PATH_LED, TARGET_LED, P_LED, BALL_RELEASE_LED, Y_LED1, Y_LED2, Y_LED3, Y_LED4, Y_LED5, R_LED1, R_LED2, R_LED3, R_LED4, R_LED5};

  for(int i = 0; i < sizeof(NUM_SWITCHES) / sizeof(NUM_SWITCHES[0]); i++){
      pinMode(NUM_SWITCHES[i], INPUT_PULLUP);
  }

  for(int i = 0; i < sizeof(NUM_ACTUATORS) / sizeof(NUM_ACTUATORS[0]); i++){
    pinMode(NUM_ACTUATORS[i], OUTPUT);
  }

  for(int i = 0; i < sizeof(NUM_RELAYS) / sizeof(NUM_RELAYS[0]); i++){
    digitalWrite(NUM_RELAYS[i], RELAY_OFF);
  }

  for(int i = 0; i < sizeof(NUM_LEDS) / sizeof(NUM_LEDS[0]); i++){
    pinMode(NUM_LEDS[i], OUTPUT);
    digitalWrite(NUM_LEDS[i], RELAY_OFF); // turn off all LEDs at start
  }

  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, RELAY_ON); // turn on red LED at start

  pinMode(YELLOW_LED, OUTPUT);
  digitalWrite(YELLOW_LED, RELAY_ON); // turn on yellow LED at start
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
  int ballReleaseState = digitalRead(ball_release_sw);

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
    } else if (ballReleaseState == PRESSED && previousBallRelease == RELEASED){
      currentEvent = BALL_RELEASE_HIT;
      lastHitTime = now;

    }
  }

  previousPathState = pathState;
  previousTopState = topState;
  previousTargetState = targetState;
  previousRedState = redState;
  previousYellowState = yellowState;
  previousOnOffState = onOffState;
  previousBallRelease = ballReleaseState;
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
  //detect one press not button hold. delay it for a bit to avoid balls being continously relased
  static int prev = RELEASED;
  int curr = digitalRead(ball_release_sw);
  //one button press (will ignore hold)
  if(prev == RELEASED && curr == PRESSED && !gameOver){
    digitalWrite(ball_release, RELAY_ON);
    delay(50); //pulse to avoid reading a hold 
    digitalWrite(ball_release, RELAY_OFF);
  } 
    prev = curr;
  }

unsigned long pathLedStartTime = 0;
bool pathLedBlinking = false;

unsigned long targetLedStartTime = 0;
bool targetLedBlinking = false;

unsigned long topGroupStartTime = 0;
bool topGroupBlinking = false;

unsigned long redGroupStartTime = 0;
bool redGroupBlinking = false;

unsigned long yellowGroupStartTime = 0;
bool yellowGroupBlinking = false;

// ================================
// Handle Events for switches (points)
// ================================
void handleEvent() {
  switch (currentEvent) {

    case PATH_HIT:
      if(specialMode) { // Only add 5 points if the last red or yellow LED is off
        addPoints(50);
      }else{
        addPoints(5);
      }
      
      pathLedStartTime = millis(); // Start timer for path LED
      pathLedBlinking = true; // Start blinking the path LED
      break;

    case TOP_GROUP_HIT:
      if(specialMode) { // Only add points if the special LED is off
        addPoints(50);
      }else{
        addPoints(5);
      }

      topGroupStartTime = millis(); // Start timer for top group LED
      topGroupBlinking = true;
      break;

    case BOTTOM_TARGET_HIT:
      if(specialMode) {
        addPoints(10);
      } else{
        addPoints(5);
      }
      
      targetLedStartTime = millis(); // Start timer for target LED
      targetLedBlinking = true; // Start blinking the target LED
      break;

    case RED_GROUP_HIT:
      if(digitalRead(Y_LED5) == RELAY_ON && digitalRead(R_LED5) == RELAY_ON) { // Only add points if the last red or yellow LED is off
        addPoints(500);
      } else if(digitalRead(Y_LED4) == RELAY_ON && digitalRead(R_LED4) == RELAY_ON) { // If both are off, add 10 points
         addPoints(300);
      } else if(digitalRead(Y_LED3) == RELAY_ON && digitalRead(R_LED3) == RELAY_ON) {
         addPoints(200);
      } else if(digitalRead(Y_LED2) == RELAY_ON && digitalRead(R_LED2) == RELAY_ON) {
         addPoints(100);
      } else if(digitalRead(Y_LED1) == RELAY_ON && digitalRead(R_LED1) == RELAY_ON) {
         addPoints(50);
      } else {
         addPoints(10);
      }
      redGroupStartTime = millis(); // Start timer for red group LED
      redGroupBlinking = true; // Start blinking the red group LED
      break;

    case YELLOW_GROUP_HIT:
      if(digitalRead(Y_LED5) == RELAY_ON && digitalRead(R_LED5) == RELAY_ON) { // Only add points if the last red or yellow LED is off
        addPoints(500);
      } else if(digitalRead(Y_LED4) == RELAY_ON && digitalRead(R_LED4) == RELAY_ON) { // If both are off, add 10 points
         addPoints(300);
      } else if(digitalRead(Y_LED3) == RELAY_ON && digitalRead(R_LED3) == RELAY_ON) {
         addPoints(200);
      } else if(digitalRead(Y_LED2) == RELAY_ON && digitalRead(R_LED2) == RELAY_ON) {
         addPoints(100);
      } else if(digitalRead(Y_LED1) == RELAY_ON && digitalRead(R_LED1) == RELAY_ON) {
         addPoints(50);
      } else {
         addPoints(10);
      }
      yellowGroupStartTime = millis(); // Start timer for yellow group LED
      yellowGroupBlinking = true; // Start blinking the yellow group LED
      break;

    case SPECIAL_HIT:
      if (digitalRead(R_LED5) == RELAY_ON && digitalRead(Y_LED5) == RELAY_ON && specialModeStartTime == 0) { // Special mode if both last red and yellow LEDs are on
        addPoints(10);
        specialMode = true;
        specialModeStartTime = millis(); // Start timer for special mode
      } else {
      addPoints(5);
      }
      break;

    case ON_OFF_HIT:
      resetPoints();
      ballcount = 0;
      gameOver = false;
      Serial.println("New Game Started");
      break;

    case BALL_RELEASE_HIT:
      releaseBall();
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

  // if (pathLedBlinking) {
  //   if(millis() - pathLedStartTime < 3000) { // Blink for 3 seconds
  //     digitalWrite(PATH_LED, PATH_LED, millis()%500>200); // Blink every 250ms
  //   } else {
  //     digitalWrite(PATH_LED, HIGH); // Keep the path LED on
  //     pathLedBlinking = false;
  //   }
  // }

  // // Turn on LEDs based on EVENT
  // if (currentEvent == TOP_GROUP_HIT) {
  //   digitalWrite(TW_LED, HIGH);
  //   topGroupMillis = millis(); // Start timer for top group LED
  // }
  //   // Turn off LEDs
  // if (millis() - topGroupMillis > 5000) { // Keep the top group LED on for 3 seconds
  //   digitalWrite(TW_LED, LOW);
  // }

// ================================
// LEDs Logic 
// ================================

const int redLeds[] = {R_LED1, R_LED2, R_LED3, R_LED4, R_LED5};
int redLedIndex = 0;
const int yellowLeds[] = {Y_LED1, Y_LED2, Y_LED3, Y_LED4, Y_LED5};
int yellowLedIndex = 0;

void updateLEDs() {
  if(specialMode){
    if(millis() - specialModeStartTime < SPECIAL_MODE_DURATION_MS) {
      digitalWrite(P_LED, RELAY_ON); // Keep the special LED on during special mode
      digitalWrite(TW_LED, RELAY_ON); // Turn off TW LED during special mode
      digitalWrite(PATH_LED, RELAY_ON); // Turn off red LED during special mode
      digitalWrite(TARGET_LED, RELAY_ON); // Turn off target LED during special mode
    } else {
      digitalWrite(P_LED, RELAY_OFF); // Turn off the special LED after special mode ends
      digitalWrite(TW_LED, RELAY_OFF); // Turn TW LED back on after special mode ends
      digitalWrite(PATH_LED, RELAY_OFF); // Turn red LED back on after special mode ends
      digitalWrite(TARGET_LED, RELAY_OFF); // Turn target LED back on after special mode ends
      specialMode = false;
      specialModeStartTime = 0;
    }
  }

  if(specialMode) {
    if(pathLedBlinking) {
      blinkLed(PATH_LED, pathLedStartTime, 1000, pathLedBlinking); // Blink for the duration of special mode
    }
    if(topGroupBlinking) {
      blinkLed(TW_LED, topGroupStartTime, 1000, topGroupBlinking); // Blink for the duration of special mode
    }
    if(targetLedBlinking) {
      blinkLed(TARGET_LED, targetLedStartTime, 1000, targetLedBlinking); // Blink for the duration of special mode
    }
  }

  if(redGroupBlinking) {
    blinkLed(RED_LED, redGroupStartTime, 3000, redGroupBlinking); // Blink for 3 seconds
  }

  if(yellowGroupBlinking) {
    blinkLed(YELLOW_LED, yellowGroupStartTime, 3000, yellowGroupBlinking); // Blink for 3 seconds
  }

  switch(currentEvent){
    case RED_GROUP_HIT:
      digitalWrite(redLeds[redLedIndex], RELAY_ON); // Turn off red LEDs based on red group hit
      redLedIndex = (redLedIndex + 1) % (sizeof(redLeds)/sizeof(redLeds[0])); // Move to the next red LED index
      break;
    case YELLOW_GROUP_HIT:
      digitalWrite(yellowLeds[yellowLedIndex], RELAY_ON); // Turn off yellow LEDs based on yellow group hit
      yellowLedIndex = (yellowLedIndex + 1) % (sizeof(yellowLeds)/sizeof(yellowLeds[0])); // Move to the next yellow LED index
      break;
  }

}

bool blinkLed(int ledPin, long startTime, int duration, bool &blinking) {
  if (millis() - startTime < duration) {
    digitalWrite(ledPin, millis()%500>200 ? LOW : HIGH);
  } else{
    digitalWrite(ledPin, RELAY_ON);
    return blinking = false;
  }
}


void releaseBall(){
    if(!gameOver){
      ballcount++;
      Serial.print("BallCOunt: ");
       Serial.println(ballcount);

    if(ballcount >= MAX_BALLS){
      gameOver = true; 
      Serial.print("Game Over");
      }
    }
}



// -------- Main Loop to read inputs --------
void loop() {

    readInputsPoints();
    handleFLippers();
    handleBottomTargets();
    handleMiddleRedYellow();
    handleBallRelease();
    handleEvent();
    updateLEDs();
    delay(1);

    //Serial.println(digitalRead(path)); //use for debugging to check wiring (111 = correct, 101 = wiring not done correctly)
    // No blocking delay: relay outputs stay responsive while score hits are rate-limited above.

}






































