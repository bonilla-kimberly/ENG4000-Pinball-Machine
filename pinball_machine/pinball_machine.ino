// Actuators activation (solenoinds / relays / flippers)

//External button = buttonPin
//Pin on Relay = relayPin
const int buttonPin = 8; 
const int relayPin = 4;
const int relayPin2 = 2;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  pinMode(relayPin2, OUTPUT);

  digitalWrite(relayPin, HIGH); // should be off default 
  digitalWrite(relayPin2, HIGH);
}

//LOW == relay on
//HIGH == relay off
void loop() {
//Read values coming from button and relay
  int buttonState = digitalRead(buttonPin);
  int relayState = digitalRead(relayPin);
  int relayState2 = digitalRead(relayPin2);

  Serial.print("Button: ");
  Serial.print(buttonState);
  Serial.print("Relay: ");
  Serial.println(relayState);
   Serial.print("Relay 2: ");
  Serial.println(relayState2);

  if (buttonState == LOW) {           // Button pressed
    digitalWrite(relayPin, LOW);      // Turn relay ON
    digitalWrite(relayPin2, LOW); 
  } else if (buttonState == HIGH) {   // Button not press                       
    digitalWrite(relayPin, HIGH);     // Turn relay OFF
    digitalWrite(relayPin2, HIGH); 
  }
  delay(200);
}
