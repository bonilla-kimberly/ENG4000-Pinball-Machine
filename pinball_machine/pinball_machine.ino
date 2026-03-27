#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>


//Hardware definition
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4


//SPI Connection
#define DATA_PIN 13 //DIN
#define CLK_PIN  11 //CLK
#define CS_PIN   10 //CS


MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


char text1[] = "Hello";
char text2[] = "World";
bool isFirstText = true;


void setup() {
 myDisplay.begin();
 displayText(text1);


}


void loop() {
 // loop through text1 and text2
 if(myDisplay.displayAnimate()){
   myDisplay.displayReset();


   if(isFirstText){
     clearDisplay;
     delay(1000);
     displayText(text2);
     isFirstText = false;
   } else {
     clearDisplay;
     delay(1000);
     displayText(text1);
     isFirstText = true;
   }
 }


}
void displayText(char * text){
 myDisplay.displayText(text, PA_LEFT, 200, 0, PA_SCROLL_LEFT);
}


void clearDisplay() {
 myDisplay.displayClear();
}



































