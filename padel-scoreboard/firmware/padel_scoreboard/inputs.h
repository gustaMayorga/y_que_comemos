#pragma once
#include <Arduino.h>
#include "config.h"

// ============================================================================
//  Entradas: sensor de proximidad de paleta + pulsadores (RF 433 y boton local)
// ============================================================================

// --------------------------------------------------------------------------
// Sensor de proximidad HC-SR04 con maquina de estados anti-rebote:
//   ARMED -> (paleta cerca PROX_HOLD_MS) -> cuenta punto -> LOCKOUT ->
//   espera que la paleta se aleje mas de PROX_REARM_CM -> ARMED
// Esto evita que una paleta apoyada o alguien pasando sume 20 puntos.
// --------------------------------------------------------------------------
class ProximityCounter {
 public:
  void begin(uint8_t trigPin, uint8_t echoPin);
  // Llamar seguido en el loop. Devuelve true UNA vez por gesto valido.
  bool poll();
  uint16_t lastDistanceCm() const { return lastCm_; }
  bool armed() const { return st_ == ARMED; }

 private:
  uint16_t measureCm();

  enum St : uint8_t { ARMED, HOLDING, LOCKOUT, WAIT_CLEAR };
  uint8_t  trig_ = 0, echo_ = 0;
  St       st_ = ARMED;
  uint32_t tHold_ = 0, tLock_ = 0, tPoll_ = 0;
  uint16_t lastCm_ = PROX_MAX_CM;
};

// --------------------------------------------------------------------------
// Pulsador generico (receptor RF de 4 canales o boton local).
// Detecta flanco corto y pulsacion larga.
// --------------------------------------------------------------------------
class Button {
 public:
  // activeHigh: true para salidas del PT2272 (van a alto al apretar el control)
  //             false para boton a GND con INPUT_PULLUP
  void begin(uint8_t pin, bool activeHigh);
  void poll();
  bool pressedShort();   // se consume al leer
  bool pressedLong();    // se consume al leer

 private:
  uint8_t  pin_ = 0;
  bool     activeHigh_ = true;
  bool     stable_ = false, lastRaw_ = false;
  uint32_t tChange_ = 0, tDown_ = 0;
  bool     shortEvt_ = false, longEvt_ = false, longFired_ = false;
};
