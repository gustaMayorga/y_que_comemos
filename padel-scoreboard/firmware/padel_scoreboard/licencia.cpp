#include "licencia.h"

static const uint8_t COMBO[LIC_COMBO_PASOS] = LIC_COMBO_DEF;

static uint8_t  paso = 0;
static uint32_t tUltimo = 0;
static bool     listo = false;

void Licencia::begin()      { reiniciar(); }
void Licencia::reiniciar()  { paso = 0; listo = false; }
uint8_t Licencia::pasosHechos() { return paso; }

bool Licencia::desbloqueoListo() {
  const bool e = listo;
  listo = false;
  return e;
}

void Licencia::evento(uint8_t boton, uint16_t ms) {
  const uint32_t ahora = millis();

  // Si paso demasiado tiempo desde el paso anterior, se descarta lo avanzado.
  if (paso > 0 && (ahora - tUltimo) > COMBO_TIMEOUT_MS) paso = 0;
  tUltimo = ahora;

  const uint8_t tipo = (ms >= COMBO_MUYLARGO_MS) ? 2
                     : (ms >= COMBO_LARGO_MS)    ? 1
                                                 : 0;
  const uint8_t esperado = COMBO[paso];

  if (((esperado >> 4) & 0x0F) == boton && (esperado & 0x0F) == tipo) {
    paso++;
    if (paso >= LIC_COMBO_PASOS) { listo = true; paso = 0; }
  } else {
    // Puede ser el primer paso de un intento nuevo.
    paso = (COMBO[0] == (uint8_t)((boton << 4) | tipo)) ? 1 : 0;
  }
}
