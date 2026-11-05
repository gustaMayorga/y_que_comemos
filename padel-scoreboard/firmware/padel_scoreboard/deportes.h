#pragma once
#include "padel_score.h"

// ============================================================================
//  Deportes soportados.
//
//  Todos usan EL MISMO motor de tanteo. La diferencia esta en las reglas:
//
//   - Padel y tenis usan la escalera 0/15/30/40 con juegos y sets. Se
//     diferencian en el punto de oro (padel si, tenis con ventajas).
//   - Voley es un "tie break" permanente: cada punto suma 1, el set se gana a
//     25 con 2 de diferencia, al mejor de 5 sets y el 5to a 15. Eso ya lo sabia
//     hacer el motor, asi que no hubo que escribir reglas nuevas.
//   - Futbol, basquet y libre son el mismo tie break pero con objetivo 0, o sea
//     sin limite: suman y suman hasta que alguien resetea.
//
//  Agregar un deporte nuevo = agregar una fila en esta tabla.
// ============================================================================

enum Deporte : uint8_t {
  DEP_PADEL = 0,
  DEP_TENIS,
  DEP_VOLEY,
  DEP_FUTBOL,
  DEP_BASQUET,
  DEP_LIBRE,
  DEP_COUNT
};

//                   oro   juegos  sets  tieB  objetivo  soloTB  setFinal
static const MatchRules REGLAS[DEP_COUNT] = {
  /* PADEL   */ { true,     6,      2,   true,     7,    false,     0 },
  /* TENIS   */ { false,    6,      2,   true,     7,    false,     0 },
  /* VOLEY   */ { false,    0,      3,   true,    25,    true,     15 },
  /* FUTBOL  */ { false,    0,     99,  false,     0,    true,      0 },
  /* BASQUET */ { false,    0,     99,  false,     0,    true,      0 },
  /* LIBRE   */ { false,    0,     99,  false,     0,    true,      0 },
};

// Letra que muestra el cartel al entrar en cada modo.
// Ojo: la V no se puede dibujar en 7 segmentos, se usa U (es lo estandar).
//   P = Padel    t = Tenis     U = Voley
//   F = Futbol   b = Basquet   L = Libre
static const uint8_t DEP_GLIFO[DEP_COUNT] = {
  0x73,  // P
  0x78,  // t
  0x3E,  // U  (voley)
  0x71,  // F
  0x7C,  // b
  0x38   // L
};

inline const char* deporteNombre(uint8_t d) {
  switch (d) {
    case DEP_PADEL:   return "padel";
    case DEP_TENIS:   return "tenis";
    case DEP_VOLEY:   return "voley";
    case DEP_FUTBOL:  return "futbol";
    case DEP_BASQUET: return "basquet";
    case DEP_LIBRE:   return "libre";
  }
  return "?";
}

// true si el marcador es un contador simple (se muestra el numero crudo)
inline bool esContador(uint8_t d) { return REGLAS[d].tiebreakOnly; }
