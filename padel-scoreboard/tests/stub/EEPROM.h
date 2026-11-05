#pragma once
// Shim de EEPROM para compilar en la PC. Guarda en RAM, no persiste.
#include <stdint.h>
struct EEPROMStub {
  uint8_t mem[1024] = {0};
  uint8_t read(int a)              { return (a >= 0 && a < 1024) ? mem[a] : 0; }
  void    write(int a, uint8_t v)  { if (a >= 0 && a < 1024) mem[a] = v; }
  void    update(int a, uint8_t v) { if (read(a) != v) write(a, v); }
  int     length() const           { return 1024; }
};
extern EEPROMStub EEPROM;
