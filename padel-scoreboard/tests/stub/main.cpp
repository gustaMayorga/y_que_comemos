#include "Arduino.h"
#include "FastLED.h"
SerialStub Serial;
FastLEDStub FastLED;
#include "EEPROM.h"
EEPROMStub EEPROM;
void setup(); void loop();
int main(){ setup(); for(int i=0;i<3;i++) loop(); return 0; }
