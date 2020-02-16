#include <Arduino.h>
#include <EEPROM.h>
#include "SevSeg.h"

SevSeg sevseg; //Instantiate a seven segment controller object
int count;
int buttonPin = 2;
byte address = 0;
int lastButtonState = LOW;
int reset_delay = 3000 ; // Holding the button for 3 seconds will cause a reset.
unsigned long pressTime = 0;
unsigned long releaseTime = 0;
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
static unsigned long last_interrupt_time = 0;

void buttonChange();

void setup() {
  Serial.begin(19200);
  byte numDigits = 3;
  byte digitPins[] = {14, 16, 10};
  byte segmentPins[] = {3,4,5,6,7,8,9};
  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins, false, false, false, true);
  sevseg.setBrightness(150);  
  pinMode(buttonPin, INPUT);
  count = EEPROM.read(0);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonChange, CHANGE);
  Serial.println("counter initialized.");
}

void loop(){
  sevseg.setNumber(count);  
  sevseg.refreshDisplay(); // Must run repeatedly 
  // Serial.println(count);
  if (releaseTime - pressTime  > reset_delay){
    Serial.println("reset counter");
    count = 0;
    pressTime = millis();
    releaseTime = pressTime;
  }
}

void buttonChange(){
  int reading = digitalRead(buttonPin);
  unsigned long interrupt_time = millis();
  // We only care if it's a fresh interript
  if (interrupt_time - last_interrupt_time > debounceDelay && reading != lastButtonState){

    if (reading){
      count = count + 1;
      EEPROM.update(address, count);
      pressTime = interrupt_time;
      releaseTime = interrupt_time;
      Serial.println(count);
    } else {
      releaseTime = interrupt_time;
    }

    
    lastButtonState = reading;
    last_interrupt_time = interrupt_time;
  }


}
