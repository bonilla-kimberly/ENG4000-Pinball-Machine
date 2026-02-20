// ================================
// Main Pinball Machine Class Controller
// - Handle inputs: read switches, buttons, sensors
// - Uses a switch table + switch/case dispatcher
// - NO delay(): uses per-switch debounce with millis()
// ================================


// -------- Pin Definitions --------
const int L_flipper_sw = 4;
const int R_flipper_sw = 5;
const int L_flipper    = 6;
const int R_flipper    = 7;

const int path = 9; //BL,BR,ML,MR,TL,TR path
const int w_bumper = 10; //LW, RW bumper

const int RED_sw_bumper = 11; //TR, ML
const int RED_bump = 12;
const int YELLOW_sw_bumper = 13; //TL, MR
const int YELLOW_bump = 14;

const int target_sw = 15; //BL, BR target switch
const int target_bump = 16; //BL,BR target bump

const int TW_switch = 17; //TW1,2,3,4,Y,R switch
const int T_bump = 18; //Y,R target bump



// Pressed = LOW because INPUT_PULLUP
const int PRESSED = LOW;
const int RELEASED = HIGH;

// Relay logic
const int RELAY_ON  = LOW;   // LOW = relay ON
const int RELAY_OFF = HIGH;

// Debounce time (pinball switches usually want small debounce)
const uint16_t DEBOUNCE_MS = 15;

//Score counter initial 0
int score = 0;


// ================================
// Switch Table
// ================================
enum SwitchRole {
  SW_L_FLIPPER,
  SW_R_FLIPPER,
  SW_PATH,
  SW_WHITE,
  SW_R_BUMPER,
  SW_Y_BUMPER,
  SW_TARGET,
  SW_TW,
};

struct SwitchDef {
  const char* name;
  uint8_t pin;
  SwitchRole role;

  // --- state for debounce + edge detect ---
  bool stableState;             // last accepted stable state
  bool lastRead;                // last raw read
  uint32_t lastChangeMs;        // time when raw last changed
};

SwitchDef switches[] = {
  {"L_flipper_sw", (uint8_t)L_flipper_sw, SW_L_FLIPPER, HIGH, HIGH, 0},
  {"R_flipper_sw", (uint8_t)R_flipper_sw, SW_R_FLIPPER, HIGH, HIGH, 0},

  {"paths",        (uint8_t)path,     SW_PATH,     HIGH, HIGH, 0},
  {"white_bumper", (uint8_t)w_bumper,     SW_WHITE,    HIGH, HIGH, 0},

  {"Red_sw_bumpers",    (uint8_t)RED_sw_bumper,     SW_R_BUMPER,  HIGH, HIGH, 0},
  {"Yellow_bumpers",    (uint8_t)YELLOW_sw_bumper,  SW_Y_BUMPER,  HIGH, HIGH, 0},

  {"target_sw",         (uint8_t)target_sw,       SW_TARGET,    HIGH, HIGH, 0},

  {"TW_switch",    (uint8_t)TW_switch,    SW_TW,        HIGH, HIGH, 0},
};

const int NUM_SWITCHES = sizeof(switches) / sizeof(switches[0]);

// ================================
// ActuatorSystem
// - activates solenoinds via relays (flippers, etc.)
// ================================
void activateActuatorL() { digitalWrite(L_flipper, RELAY_ON); }
void activateActuatorR() { digitalWrite(R_flipper, RELAY_ON); }

void deactivateActuatorL() { digitalWrite(L_flipper, RELAY_OFF); }
void deactivateActuatorR() { digitalWrite(R_flipper, RELAY_OFF); }


// ================================
// ScoringSystem
// - Stores score
// - counts score and combos
// ================================
void addPoints(int points) {
  score += points;
  Serial.print("Score +");
  Serial.print(points);
  Serial.print(" => ");
  Serial.println(score);
}

// ================================
// LightingSystem
// - Activates specific LED groups when events happen
// - Non-blocking (millis-based), NO delay()
// ================================

// ---- LED Pin Definitions (single LEDs) ----
const int TARGET_LED  = 19;  // example pin (change)
const int TW_LED    = 20;  // example pin (change)
const int RED_LED = 21;  // example pin (change)
const int YELLOW_LED = 35;  // example pin (change)
const int P_LED = 36;  // example pin (change)
const int PATH_LED = 37;
const int BALL_RELEASE_LED = 38;
const uint8_t Y_LED[] = {39, 40, 41, 42, 43};
const uint8_t R_LED[] = {44, 45, 46, 47, 48};
const int NUM_Y_LED = sizeof(Y_LED) / sizeof(Y_LED[0]);
const int NUM_R_LED = sizeof(R_LED) / sizeof(R_LED[0]);


enum LedGroupId {
  LEDG_TARGET,
  LEDG_TW,
  LEDG_RED,
  LEDG_YELLOW,
  LED_PURPLE,
  LEDG_PATH,
  LEDG_BALL_RELEASE,
  LED_Y0,
  LED_Y1,
  LED_Y2,
  LED_Y3,
  LED_Y4,
  LED_R0,
  LED_R1,
  LED_R2,
  LED_R3,
  LED_R4,

  LEDG_COUNT
};

struct LedGroup {
  const char* name;
  uint8_t pin;            // for single LED groups
  bool active;
  uint32_t untilMs;       // when to turn off
};

LedGroup ledGroups[LEDG_COUNT] = {
  {"BOTTOM_TARGET", (uint8_t)TARGET_LED,  false, 0},
  {"TOP_SW_TARGET", (uint8_t)TW_LED,      false, 0},
  {"RED",           (uint8_t)RED_LED,     false, 0},
  {"YELLOW",        (uint8_t)YELLOW_LED,  false, 0},
  {"PURPLE",        (uint8_t)P_LED,       false, 0},
  {"PATH",          (uint8_t)PATH_LED,    false, 0},
  {"BALL_RELEASE",  (uint8_t)BALL_RELEASE_LED, false, 0},
  {"Y_LED1", (uint8_t)Y_LED[0], false, 0},
  {"Y_LED2", (uint8_t)Y_LED[1], false, 0},
  {"Y_LED3", (uint8_t)Y_LED[2], false, 0},
  {"Y_LED4", (uint8_t)Y_LED[3], false, 0},
  {"Y_LED5", (uint8_t)Y_LED[4], false, 0},
  {"R_LED1", (uint8_t)R_LED[0], false, 0},
  {"R_LED2", (uint8_t)R_LED[1], false, 0},
  {"R_LED3", (uint8_t)R_LED[2], false, 0},
  {"R_LED4", (uint8_t)R_LED[3], false, 0},
  {"R_LED5", (uint8_t)R_LED[4], false, 0}
};

void lightingBegin() {
  for (int i = 0; i < LEDG_COUNT; i++) {
    pinMode(ledGroups[i].pin, OUTPUT);
    digitalWrite(ledGroups[i].pin, LOW);
    ledGroups[i].active = false;
    ledGroups[i].untilMs = 0;
  }
}
/* FIX
// Flash a group for ms milliseconds (non-blocking)
void flashGroup(LedGroupId id, uint16_t ms, uint32_t now) {
  ledGroups[id].active = true;
  ledGroups[id].untilMs = now + ms;
  digitalWrite(ledGroups[id].pin, HIGH);
}*/

// Update system: turn off groups whose flash time ended
void lightingUpdate(uint32_t now) {
  for (int i = 0; i < LEDG_COUNT; i++) {
    if (ledGroups[i].active && (int32_t)(now - ledGroups[i].untilMs) >= 0) {
      ledGroups[i].active = false;
      digitalWrite(ledGroups[i].pin, LOW);
    }
  }
}


// ================================
// Handlers (Java-style switch/case)
// ================================
void handleSwitchPressed(const SwitchDef& sw) {

  switch (sw.role) {
    case SW_L_FLIPPER:
      // Flipper hold: turn on while pressed, off on release (release handler)
      activateActuatorL();
      break;

    case SW_R_FLIPPER:
      activateActuatorR();
      break;

    case SW_PATH:
      //if(LEDon){}
      addPoints(50);
      break;
    case SW_WHITE:
      addPoints(10);
      break;
    case SW_R_BUMPER:
      // TODO: flash bumper LEDs / combo logic
      break;

    case SW_Y_BUMPER:
      break;
    case SW_TARGET:
      // TODO: LED switch combo logic
      break;

    default:
      break;
  }
}

void handleSwitchReleased(const SwitchDef& sw) {
  switch (sw.role) {
    case SW_L_FLIPPER:
      deactivateActuatorL();
      break;

    case SW_R_FLIPPER:
      deactivateActuatorR();
      break;

    default:
      break;
  }
}


// -------- Setup --------
void setup() {
  Serial.begin(115200);

  // Set all switch pins as INPUT_PULLUP + init debounce state
  uint32_t now = millis();
  for (int i = 0; i < NUM_SWITCHES; i++) {
    pinMode(switches[i].pin, INPUT_PULLUP);

    bool r = digitalRead(switches[i].pin);
    switches[i].stableState = r;
    switches[i].lastRead = r;
    switches[i].lastChangeMs = now;
  }

  pinMode(L_flipper, OUTPUT);
  pinMode(R_flipper, OUTPUT);

  digitalWrite(L_flipper, RELAY_OFF);
  digitalWrite(R_flipper, RELAY_OFF);

  lightingBegin();

}


// -------- Main Loop --------
void loop() {
  uint32_t now = millis();

  // Read ALL switches in one loop (can catch multiple hits in same loop)
  for (int i = 0; i < NUM_SWITCHES; i++) {
    SwitchDef& sw = switches[i];

    // ---- Debounce (non-blocking) ----
    bool raw = digitalRead(sw.pin);

    if (raw != sw.lastRead) {
      sw.lastRead = raw;
      sw.lastChangeMs = now;
    }

    // If raw stable long enough and differs from stableState -> accept edge
    if ((now - sw.lastChangeMs) >= DEBOUNCE_MS && sw.stableState != raw) {
      bool oldStable = sw.stableState;
      sw.stableState = raw;

      // Edge detect on stable transition
      if (oldStable == HIGH && sw.stableState == PRESSED) {
        handleSwitchPressed(sw);
      }
      if (oldStable == PRESSED && sw.stableState == HIGH) {
        handleSwitchReleased(sw);
      }
    }
  }

  lightingUpdate(now);

  // No delay -> responsive
}













