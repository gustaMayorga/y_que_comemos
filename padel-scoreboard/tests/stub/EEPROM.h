#pragma once
// Shim de EEPROM para compilar en la PC. Guarda en RAM, no persiste.
#include <stdint.h>
struct EEPROMStub {
  uint8_t mem[1024] = {0};
  uint8_t read(int a)              { return (a >= 0 && a < 1024) ? mem[a] : 0; }
  void    write(int a, uint8_t v)  { if (a >= 0 && a < 1024) mem[a] = v; }
  void    update(int a, uint8_t v) { if (read(a) != v) write(a, v); }
  int     length() const           { return 1024; }
  template<class T> T& get(int a, T& v){
    uint8_t* p=(uint8_t*)&v; for(unsigned i=0;i<sizeof(T);i++) p[i]=read(a+i); return v; }
  template<class T> const T& put(int a, const T& v){
    const uint8_t* p=(const uint8_t*)&v; for(unsigned i=0;i<sizeof(T);i++) write(a+i,p[i]); return v; }
};
extern EEPROMStub EEPROM;
