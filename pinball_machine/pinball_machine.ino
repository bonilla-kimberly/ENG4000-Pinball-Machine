#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// ---- CONFIG ----
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4   // change depending on how many modules you have

#define DATA_PIN 47   // DIN
#define CLK_PIN  44   // CLK
#define CS_PIN   43   // CS / LOAD

// ---- OBJECT ----
MD_Parola matrix = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup() {
  matrix.begin();
  matrix.setIntensity(5);   // brightness (0–15)
  matrix.displayClear();

  matrix.displayText(
    "HELLO WORLD!",   // text
    PA_CENTER,        // alignment
    50,               // speed (lower = faster)
    1000,             // pause at end (ms)
    PA_SCROLL_LEFT,   // entry effect
    PA_SCROLL_LEFT    // exit effect
  );
}

void loop() {
  // this must be called continuously
  if (matrix.displayAnimate()) {
    matrix.displayReset();  // repeat animation
  }
}






























