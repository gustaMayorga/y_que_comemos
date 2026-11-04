#pragma once
#include <Arduino.h>
#include "config.h"
#include "padel_score.h"

// ============================================================================
//  Capa de salida de datos - PREPARADA, NO ACTIVADA (ver docs/04)
//
//  Hoy: serializa el estado a JSON y lo imprime por Serial (sirve para probar
//  toda la logica sin un solo LED soldado).
//  Manana (con ESP32): el mismo string se publica por WebSocket a la app.
//  El contrato de datos ya esta congelado para no reescribir la app despues.
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
    case P_0:   return "0";
    case P_15:  return "15";
    case P_30:  return "30";
    case P_40:  return "40";
    case P_ADV: return "AD";
  }
  return "?";
}

inline const char* situationName(uint8_t sit) {
  switch (sit) {
    case SIT_NONE:        return "none";
    case SIT_GAME_POINT:  return "game";
    case SIT_SET_POINT:   return "set";
    case SIT_MATCH_POINT: return "match";
  }
  return "?";
}

// ----------------------------------------------------------------------------
// En AVR (Nano) el estado se IMPRIME campo por campo: un buffer de 200 bytes
// en la pila es el consumo evitable mas grande con 2 KB de RAM.
// En ESP32 se arma el string, porque es el que se va a mandar por WebSocket.
// Las dos ramas producen exactamente el mismo JSON.
// ----------------------------------------------------------------------------
#if defined(ESP32)

inline size_t scoreToJson(const MatchState& s, const Situation sit[2],
                          char* out, size_t cap) {
  return snprintf(out, cap,
    "{\"phase\":\"%s\",\"points\":[\"%s\",\"%s\"],\"tb\":[%u,%u],"
    "\"games\":[%u,%u],\"sets\":[%u,%u],"
    "\"setScore\":[[%u,%u],[%u,%u],[%u,%u]],\"serving\":%u,"
    "\"sit\":[\"%s\",\"%s\"],\"winner\":%d,\"n\":%lu}",
    phaseName(s.phase),
    s.phase == PHASE_TIEBREAK ? "" : pointLabel(s.points[0]),
    s.phase == PHASE_TIEBREAK ? "" : pointLabel(s.points[1]),
    (unsigned)s.tbPoints[0], (unsigned)s.tbPoints[1],
    (unsigned)s.games[0],    (unsigned)s.games[1],
    (unsigned)s.sets[0],     (unsigned)s.sets[1],
    (unsigned)s.setScore[0][0], (unsigned)s.setScore[0][1],
    (unsigned)s.setScore[1][0], (unsigned)s.setScore[1][1],
    (unsigned)s.setScore[2][0], (unsigned)s.setScore[2][1],
    (unsigned)s.serving,
    situationName(sit[0]), situationName(sit[1]),
    s.winner == 0xFF ? -1 : (int)s.winner,
    (unsigned long)s.pointCount);
}

inline void publishState(const MatchState& s, const Situation sit[2]) {
#if ENABLE_SERIAL_JSON
  char buf[280];
  scoreToJson(s, sit, buf, sizeof(buf));
  Serial.println(buf);
#endif
#if ENABLE_WIFI
  // Fase 2: ws.textAll(buf);   // AsyncWebSocket
#endif
}

#else   // ---------------------------- AVR / Nano ----------------------------

inline void publishState(const MatchState& s, const Situation sit[2]) {
#if ENABLE_SERIAL_JSON
  Serial.print(F("{\"phase\":\""));   Serial.print(phaseName(s.phase));
  Serial.print(F("\",\"points\":[\""));
  if (s.phase != PHASE_TIEBREAK) Serial.print(pointLabel(s.points[0]));
  Serial.print(F("\",\""));
  if (s.phase != PHASE_TIEBREAK) Serial.print(pointLabel(s.points[1]));
  Serial.print(F("\"],\"tb\":["));    Serial.print(s.tbPoints[0]);
  Serial.print(',');                   Serial.print(s.tbPoints[1]);
  Serial.print(F("],\"games\":["));    Serial.print(s.games[0]);
  Serial.print(',');                   Serial.print(s.games[1]);
  Serial.print(F("],\"sets\":["));     Serial.print(s.sets[0]);
  Serial.print(',');                   Serial.print(s.sets[1]);
  Serial.print(F("],\"setScore\":["));
  for (uint8_t i = 0; i < 3; i++) {
    Serial.print('['); Serial.print(s.setScore[i][0]);
    Serial.print(','); Serial.print(s.setScore[i][1]); Serial.print(']');
    if (i < 2) Serial.print(',');
  }
  Serial.print(F("],\"serving\":"));   Serial.print(s.serving);
  Serial.print(F(",\"sit\":[\""));     Serial.print(situationName(sit[0]));
  Serial.print(F("\",\""));            Serial.print(situationName(sit[1]));
  Serial.print(F("\"],\"winner\":"));  Serial.print(s.winner == 0xFF ? -1 : (int)s.winner);
  Serial.print(F(",\"n\":"));          Serial.print(s.pointCount);
  Serial.println('}');
#endif
}

#endif
