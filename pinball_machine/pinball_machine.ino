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

const int points10 = 9;

const int RED_sw_bumper = 10; //TR, ML
const int RED_bump = 11;
const int YELLOW_sw_bumper = 12; //TL, MR
const int YELLOW_bump = 13;

const int target_sw = 14; //BL, BR, Y, R target switch
const int target_bump = 15; //BL,BR,Y,R target bump

const int TW_switch = 18; //TW1,2,3,4, switch


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
  SW_10PTS,
  SW_R_BUMPER,
  R_BUMP,
  SW_Y_BUMPER,
  Y_BUMP,
  SW_TARGET,
  TARGET_BUMP,
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

  {"10_points",    (uint8_t)points10,     SW_10PTS,     HIGH, HIGH, 0},

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

    case SW_10PTS:
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

  // No delay -> responsive
}













