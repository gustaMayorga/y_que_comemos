#pragma once
#include <Arduino.h>
#include "config.h"

// ============================================================================
//  Datos que sobreviven al apagado (EEPROM del ATmega328P: 1024 bytes).
//
//  Guarda: deporte elegido, minutos de uso acumulados, limite de licencia,
//  partidos jugados y cuantas veces se desbloqueo.
//
//  OJO con la vida util: la EEPROM aguanta ~100.000 escrituras por celda. Por
//  eso se usa NIVELACION DE DESGASTE: hay 16 ranuras y cada guardado va a la
//  siguiente. Al arrancar se lee la de numero de secuencia mas alto. Con un
//  guardado cada 10 minutos de uso eso da decadas.
//
//  Importante: la EEPROM NO se borra al recargar el sketch por USB (el
//  bootloader solo escribe flash). Si se borra con un programador ISP haciendo
//  chip erase completo.
// ============================================================================

struct Persistido {
  uint16_t magic;        // MAGIC_OK si la ranura es valida
  uint16_t seq;          // numero de guardado, para elegir la ranura mas nueva
  uint8_t  deporte;      // Deporte elegido
  uint8_t  desbloqueos;  // cuantas veces se extendio la licencia
  uint32_t usoMinutos;   // minutos acumulados con el cartel encendido
  uint32_t licenciaMin;  // tope de minutos; 0 = sin licencia (version libre)
  uint32_t partidos;     // partidos terminados
  uint16_t crc;
};

namespace Almacen {
  void begin();                        // lee la EEPROM o la inicializa
  Persistido& datos();
  void tick();                         // en el loop: cuenta minutos y autoguarda
  void guardarYa();
  void marcarSucio();

  uint32_t minutosUso();
  bool     licenciaActiva();           // hay un tope configurado
  bool     licenciaVencida();
  uint32_t minutosRestantes();
  void     extenderLicencia(uint32_t minutos);
  void     resetDeFabrica();
}
