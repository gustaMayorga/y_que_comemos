#pragma once
// Shim minimo de Arduino para compilar los sketches en la PC. No emula hardware.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define HIGH 1
#define LOW 0
#define A0 14
#define A1 15
#define A2 16
#define A3 17
#define A4 18
#define A5 19
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define F(x) (x)
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
inline long random(long m){ return m? (long)(1103515245u % (unsigned long)m) : 0; }
#define PROGMEM
#define pgm_read_byte(p) (*(const uint8_t*)(p))
inline void pinMode(uint8_t,uint8_t){}
inline void digitalWrite(uint8_t,uint8_t){}
inline int  digitalRead(uint8_t){return 1;}
inline int  analogRead(uint8_t){return 0;}
inline void delay(unsigned long){}
inline void delayMicroseconds(unsigned int){}
inline unsigned long millis(){return 0;}
inline unsigned long micros(){return 0;}
#define CHANGE 1
#define RISING 2
#define FALLING 3
inline int  digitalPinToInterrupt(int p){return p;}
inline void attachInterrupt(int,void(*)(void),int){}
inline void noInterrupts(){}
inline void interrupts(){}
inline unsigned long pulseIn(uint8_t,uint8_t,unsigned long){return 0;}
typedef char __FlashStringHelper;
inline void* memcpy_P(void*d,const void*s,unsigned n){return memcpy(d,s,n);}
inline char* strncpy_P(char*d,const char*s,unsigned n){return strncpy(d,s,n);}
inline void tone(uint8_t,unsigned int,unsigned long){}
struct SerialStub {
  void begin(long){}
  int  available(){return 0;}
  int  read(){return -1;}
  void print(const char*){} void print(char){} void print(int){} void print(unsigned){}
  void print(long){} void print(unsigned long){} void print(uint8_t){} void print(uint16_t){}
  void println(const char*){} void println(char){} void println(int){} void println(unsigned){}
  void println(long){} void println(unsigned long){} void println(uint8_t){}
  void println(uint16_t){} void println(){}
};
extern SerialStub Serial;

// --- registro de causa de reset (stub) ---
__attribute__((unused)) static unsigned char MCUSR = 0x01;   // en el host simulamos power-on
#define PORF  0
#define EXTRF 1
#define BORF  2
#define WDRF  3
