#include "Arduino.h"
#include "FastLED.h"
SerialStub Serial;
FastLEDStub FastLED;
void setup(); void loop();
int main(){ setup(); for(int i=0;i<3;i++) loop(); return 0; }
