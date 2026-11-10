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
  CRGB& operator+=(const CRGB&o){
    int R=r+o.r,G=g+o.g,B=b+o.b;
    r=(uint8_t)(R>255?255:R); g=(uint8_t)(G>255?255:G); b=(uint8_t)(B>255?255:B);
    return *this; }
  CRGB operator%(uint8_t s)const{
    return CRGB((uint8_t)(r*s/255),(uint8_t)(g*s/255),(uint8_t)(b*s/255)); }
  CRGB& nscale8(uint8_t s){
    r=(uint8_t)(r*s/255); g=(uint8_t)(g*s/255); b=(uint8_t)(b*s/255); return *this; }
};
struct CHSV {
  uint8_t h=0,s=0,v=0;
  CHSV()=default; CHSV(uint8_t hh,uint8_t ss,uint8_t vv):h(hh),s(ss),v(vv){}
  operator CRGB() const { return CRGB(v,v,v); }
};
inline void fadeToBlackBy(CRGB* a,int n,uint8_t f){
  for(int i=0;i<n;i++){ a[i].r=(uint8_t)(a[i].r*(255-f)/255);
    a[i].g=(uint8_t)(a[i].g*(255-f)/255); a[i].b=(uint8_t)(a[i].b*(255-f)/255); } }
inline CRGB HeatColor(uint8_t t){ return CRGB(t,(uint8_t)(t/2),0); }
inline uint8_t  random8(){ return 7; }
inline uint8_t  random8(uint8_t m){ return (uint8_t)(m?3%m:0); }
inline uint8_t  random8(uint8_t a,uint8_t b){ return (uint8_t)(a+((b>a)?1:0)); }
inline uint16_t random16(uint16_t m){ return (uint16_t)(m?5%m:0); }
inline uint16_t sqrt16(uint16_t x){ uint16_t r=0; while((uint32_t)(r+1)*(r+1)<=x) r++; return r; }
inline uint8_t  sin8(uint8_t t){ return (uint8_t)(128+127*((t<128)?1:-1)); }
inline uint8_t  cos8(uint8_t t){ return sin8((uint8_t)(t+64)); }
inline int16_t  sin16(int16_t t){ return (int16_t)(t); }
inline uint8_t  scale8(uint8_t a,uint8_t b){ return (uint8_t)((uint16_t)a*b/255); }
inline uint8_t  qadd8(uint8_t a,uint8_t b){ uint16_t s=a+b; return (uint8_t)(s>255?255:s); }
enum EOrder { RGB, RBG, GRB, GBR, BRG, BGR };
enum EChipset { WS2812B, WS2812, WS2811, WS2813, WS2815, SK6812, APA102, WS2801, LPD8806 };
inline void fill_solid(CRGB* a,int n,const CRGB& c){for(int i=0;i<n;i++)a[i]=c;}
struct FastLEDStub {
  template<EChipset C,int PIN,EOrder O> void addLeds(CRGB*,int){}
  template<EChipset C,int PIN,int CLK,EOrder O> void addLeds(CRGB*,int){}
  void setMaxPowerInVoltsAndMilliamps(int,int){}
  void setDither(int){}
  unsigned char _bri=255;
  void setBrightness(unsigned char b){ _bri=b; }
  unsigned char getBrightness() const { return _bri; }
  void show(){}
  void clear(bool w=false){ (void)w; }
};
extern FastLEDStub FastLED;
