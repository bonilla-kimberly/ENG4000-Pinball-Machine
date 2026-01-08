// Actuators activation (solenoinds / relays / flippers)

//External button = buttonPin
//Pin on Relay = relayPin
const int buttonPin = 8; 
const int relayPin = 4;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // should be off default 
}

//LOW == relay on
//HIGH == relay off
void loop() {
//Read values coming from button and relay
  int buttonState = digitalRead(buttonPin);
  int relayState = digitalRead(relayPin);

  Serial.print("Button: ");
  Serial.print(buttonState);
  Serial.print("Relay: ");
  Serial.println(relayState);

  if (buttonState == LOW) {           // Button pressed
    digitalWrite(relayPin, LOW);      // Turn relay ON
  } else if (buttonState == HIGH) {   // Button not press                       
    digitalWrite(relayPin, HIGH);     // Turn relay OFF
  }
  delay(200);
}
