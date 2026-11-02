#pragma once
#include <Arduino.h>
#include "config.h"
#include "padel_score.h"

// ============================================================================
//  Capa de red - PREPARADA, NO ACTIVADA (ver docs/04-roadmap-wifi-app.md)
//
//  Hoy: serializa el estado a JSON y lo escupe por Serial. Ese mismo string
//  es el payload que en la v2 se publica por WebSocket a la app.
//  Contrato de datos congelado desde ahora para no reescribir la app despues.
// ============================================================================

inline const char* phaseName(uint8_t phase) {
  switch (phase) {
    case PHASE_GAME:       return "game";
    case PHASE_TIEBREAK:   return "tiebreak";
    case PHASE_MATCH_OVER: return "over";
  }
  return "?";
}

inline const char* pointLabel(uint8_t p) {
  switch (p) {
    case P_0:  return "0";
    case P_15: return "15";
    case P_30: return "30";
    case P_40: return "40";
    case P_ADV:return "AD";
  }
  return "?";
}

// JSON plano, sin libreria, para no sumar dependencias al MVP.
inline size_t scoreToJson(const MatchState& s, char* out, size_t cap) {
  return snprintf(out, cap,
    "{\"phase\":\"%s\",\"points\":[\"%s\",\"%s\"],\"tb\":[%u,%u],"
    "\"games\":[%u,%u],\"sets\":[%u,%u],\"serving\":%u,\"winner\":%d,\"n\":%lu}",
    phaseName(s.phase),
    s.phase == PHASE_TIEBREAK ? "" : pointLabel(s.points[0]),
    s.phase == PHASE_TIEBREAK ? "" : pointLabel(s.points[1]),
    s.tbPoints[0], s.tbPoints[1],
    s.games[0], s.games[1],
    s.sets[0], s.sets[1],
    s.serving,
    s.winner == 0xFF ? -1 : (int)s.winner,
    (unsigned long)s.pointCount);
}

inline void publishState(const MatchState& s) {
  char buf[256];
  scoreToJson(s, buf, sizeof(buf));
  Serial.println(buf);
#if ENABLE_WIFI
  // v2: ws.textAll(buf);   // AsyncWebSocket
#endif
}
