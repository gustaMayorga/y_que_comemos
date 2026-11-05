#include "almacen.h"
#include <EEPROM.h>
#include <string.h>

#define MAGIC_OK    0x5041u   // "PA"
#define RANURAS     16
#define BASE_ADDR   0
#define TAM         ((int)sizeof(Persistido))

static Persistido g;
static uint8_t    ranura   = 0;
static bool       sucio    = false;
static uint32_t   tMinuto  = 0;
static uint32_t   tGuardado = 0;

static uint16_t crc16(const uint8_t* d, uint8_t n) {
  uint16_t c = 0xFFFF;
  for (uint8_t i = 0; i < n; i++) {
    c ^= d[i];
    for (uint8_t b = 0; b < 8; b++) c = (c & 1) ? (uint16_t)((c >> 1) ^ 0xA001) : (uint16_t)(c >> 1);
  }
  return c;
}

static uint16_t calcCrc(const Persistido& p) {
  return crc16((const uint8_t*)&p, (uint8_t)(TAM - sizeof(uint16_t)));
}

static void leerRanura(uint8_t i, Persistido& out) {
  uint8_t* d = (uint8_t*)&out;
  for (int b = 0; b < TAM; b++) d[b] = EEPROM.read(BASE_ADDR + i * TAM + b);
}

static void escribirRanura(uint8_t i, const Persistido& p) {
  const uint8_t* d = (const uint8_t*)&p;
  for (int b = 0; b < TAM; b++) EEPROM.update(BASE_ADDR + i * TAM + b, d[b]);
}

void Almacen::begin() {
  Persistido mejor; memset(&mejor, 0, sizeof(mejor));
  bool hay = false;
  uint8_t mejorIdx = 0;

  for (uint8_t i = 0; i < RANURAS; i++) {
    Persistido p;
    leerRanura(i, p);
    if (p.magic != MAGIC_OK || p.crc != calcCrc(p)) continue;
    if (!hay || (int16_t)(p.seq - mejor.seq) > 0) { mejor = p; mejorIdx = i; hay = true; }
  }

  if (hay) {
    g = mejor;
    ranura = (uint8_t)((mejorIdx + 1) % RANURAS);
  } else {
    resetDeFabrica();
  }
  tMinuto = tGuardado = millis();
}

void Almacen::resetDeFabrica() {
  memset(&g, 0, sizeof(g));
  g.magic       = MAGIC_OK;
  g.seq         = 1;
  g.deporte     = DEPORTE_INICIAL;
  g.licenciaMin = (uint32_t)LIC_HORAS * 60UL;
  ranura = 0;
  guardarYa();
}

Persistido& Almacen::datos() { return g; }
void Almacen::marcarSucio()  { sucio = true; }

void Almacen::guardarYa() {
  g.seq++;
  g.crc = calcCrc(g);
  escribirRanura(ranura, g);
  ranura = (uint8_t)((ranura + 1) % RANURAS);
  sucio = false;
  tGuardado = millis();
}

void Almacen::tick() {
  const uint32_t ahora = millis();
  // contar un minuto de uso
  if (ahora - tMinuto >= 60000UL) {
    tMinuto += 60000UL;
    g.usoMinutos++;
    sucio = true;
  }
  // autoguardado espaciado, para no desgastar la EEPROM
  if (sucio && (ahora - tGuardado >= (uint32_t)GUARDAR_CADA_MIN * 60000UL)) guardarYa();
}

uint32_t Almacen::minutosUso()     { return g.usoMinutos; }
bool     Almacen::licenciaActiva() { return g.licenciaMin > 0; }
bool     Almacen::licenciaVencida(){ return licenciaActiva() && g.usoMinutos >= g.licenciaMin; }

uint32_t Almacen::minutosRestantes() {
  if (!licenciaActiva() || licenciaVencida()) return 0;
  return g.licenciaMin - g.usoMinutos;
}

void Almacen::extenderLicencia(uint32_t minutos) {
  // El tope se corre desde el uso actual, asi cada desbloqueo da el periodo
  // completo sin importar cuanto se paso.
  g.licenciaMin = g.usoMinutos + minutos;
  g.desbloqueos++;
  guardarYa();
}
