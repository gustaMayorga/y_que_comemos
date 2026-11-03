#pragma once
// Shim minimo de FastLED para compilar los sketches en la PC. No emula hardware.
#include "Arduino.h"
struct CRGB {
  uint8_t r=0,g=0,b=0;
  CRGB()=default;
  CRGB(uint8_t rr,uint8_t gg,uint8_t bb):r(rr),g(gg),b(bb){}
  enum HTMLColorCode { Black=0, White=1 };
  CRGB(HTMLColorCode){}
  bool operator!=(const CRGB&o)const{return r!=o.r||g!=o.g||b!=o.b;}
};
enum EOrder { RGB, RBG, GRB, GBR, BRG, BGR };
enum EChipset { WS2812B, WS2812, WS2811, WS2813, WS2815, SK6812, APA102, WS2801, LPD8806 };
inline void fill_solid(CRGB* a,int n,const CRGB& c){for(int i=0;i<n;i++)a[i]=c;}
struct FastLEDStub {
  template<EChipset C,int PIN,EOrder O> void addLeds(CRGB*,int){}
  template<EChipset C,int PIN,int CLK,EOrder O> void addLeds(CRGB*,int){}
  void setMaxPowerInVoltsAndMilliamps(int,int){}
  void setDither(int){}
  void setBrightness(uint8_t){}
  void show(){}
};
extern FastLEDStub FastLED;
