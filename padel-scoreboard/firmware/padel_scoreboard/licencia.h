#pragma once
#include <Arduino.h>
#include "config.h"

// ============================================================================
//  Combo secreto para extender la licencia.
//
//  Se le pasa cada SOLTADA de boton con su duracion y va avanzando por la
//  secuencia definida en LIC_COMBO_DEF (config.h). Si se tarda mas de
//  COMBO_TIMEOUT_MS entre pasos, o se aprieta un boton que no corresponde,
//  vuelve a cero sin avisar nada.
//
//  Como no da ninguna pista visual de cuantos pasos van, no se puede sacar por
//  prueba y error: hay que conocer la secuencia completa.
// ============================================================================

namespace Licencia {
  void begin();
  // boton: 1..4 segun el control. ms: cuanto estuvo apretado.
  void evento(uint8_t boton, uint16_t ms);
  // true UNA vez cuando se completo la secuencia entera.
  bool desbloqueoListo();
  uint8_t pasosHechos();
  void reiniciar();
}
