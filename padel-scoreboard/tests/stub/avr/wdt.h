#pragma once
// stub de host para avr/wdt.h
inline void wdt_disable(){}
inline void wdt_reset(){}
inline void wdt_enable(int){}
#define WDTO_8S 9
#define WDTO_4S 8
