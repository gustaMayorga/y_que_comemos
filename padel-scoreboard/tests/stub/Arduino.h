#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define HIGH 1
#define LOW 0
#define A0 14
#define F(x) (x)
inline void pinMode(uint8_t,uint8_t){}
inline void digitalWrite(uint8_t,uint8_t){}
inline int  digitalRead(uint8_t){return 0;}
inline int  analogRead(uint8_t){return 0;}
inline void delay(unsigned long){}
inline void delayMicroseconds(unsigned int){}
inline unsigned long millis(){return 0;}
inline unsigned long pulseIn(uint8_t,uint8_t,unsigned long){return 0;}
inline void tone(uint8_t,unsigned int,unsigned long){}
struct SerialStub {
  void begin(long){}
  void print(const char*){} void print(char){} void print(int){} void print(unsigned){}
  void print(long){} void print(unsigned long){}
  void println(const char*){} void println(char){} void println(){}
};
extern SerialStub Serial;
