#pragma once
#include <stdint.h>
#include "config.h"

// ============================================================================
//  Maquina de estados del tanteo de padel.
//  Sin dependencias de hardware: se compila y se testea en la PC.
// ============================================================================

enum GamePhase : uint8_t {
  PHASE_GAME = 0,     // juego normal (0/15/30/40/Ventaja)
  PHASE_TIEBREAK,     // 6-6 -> tie break
  PHASE_MATCH_OVER
};

// Indice de puntos dentro de un juego normal
enum PointIdx : uint8_t { P_0 = 0, P_15, P_30, P_40, P_ADV };

// Que se cierra si el equipo gana el PROXIMO punto. Jerarquia de mayor a menor.
enum Situation : uint8_t {
  SIT_NONE = 0,
  SIT_GAME_POINT,     // punto de juego  (y si saca el rival, punto de quiebre)
  SIT_SET_POINT,      // punto de set
  SIT_MATCH_POINT     // punto de partido
};

// Flags que devuelve una transicion
#define FLAG_GAME   0x01
#define FLAG_SET    0x02
#define FLAG_MATCH  0x04

struct MatchRules {
  bool    goldenPoint;    // true: en 40-40 el siguiente punto gana el juego
  uint8_t gamesPerSet;    // normalmente 6
  uint8_t setsToWin;      // normalmente 2 (al mejor de 3)
  bool    tieBreak;       // 6-6 -> tie break
  uint8_t tiebreakTarget; // normalmente 7

  // --- lo que permite cubrir los otros deportes con el mismo motor ---
  // tiebreakOnly: no hay escalera 15/30/40 ni juegos. Cada punto suma 1 y el
  //   set se gana al llegar a tiebreakTarget con 2 de diferencia. Asi funcionan
  //   el voley (25, y 15 en el set decisivo) y, con target 0, los tanteadores
  //   libres como futbol y basquet, que suman sin limite.
  bool    tiebreakOnly;
  // Objetivo distinto para el ultimo set posible (voley: 15 en el 5to).
  // 0 = usar siempre tiebreakTarget.
  uint8_t finalSetTarget;
};

// Snapshot completo del partido: es lo que se guarda para el UNDO y lo que se
// serializa a JSON cuando se agregue WiFi. 12 bytes + contador.
struct MatchState {
  uint8_t   points[2];    // PointIdx cuando phase == PHASE_GAME
  uint8_t   tbPoints[2];  // puntos del tie break
  uint8_t   games[2];
  uint8_t   sets[2];
  uint8_t   phase;        // GamePhase
  uint8_t   winner;       // 0xFF = sin ganador, 0/1 = equipo
  uint8_t   serving;      // 0/1 equipo al saque
  uint8_t   setScore[3][2]; // juegos con que termino cada set (para el historial)
  uint32_t  pointCount;   // puntos jugados en total
};

// ----------------------------------------------------------------------------
// Transicion pura: aplica un punto sobre un estado. No guarda historial.
// Se usa para avanzar el partido y tambien para SIMULAR el proximo punto
// (de ahi salen el punto de juego / set / partido sin duplicar las reglas).
// Devuelve false si el partido ya habia terminado.
// ----------------------------------------------------------------------------
bool applyPoint(MatchState& s, const MatchRules& r, uint8_t team, uint8_t* flagsOut);

class ScoreEngine {
 public:
  void begin(const MatchRules& rules);
  void resetMatch();

  bool addPoint(uint8_t team);   // false si el partido ya termino
  bool undo();

  void setServing(uint8_t team);
  void toggleServing();

  const MatchState& state() const { return s_; }
  const MatchRules& rules() const { return r_; }
  MatchRules&       mutableRules() { return r_; }

  bool matchOver()   const { return s_.phase == PHASE_MATCH_OVER; }
  bool lastWonGame() const { return (lastFlags_ & FLAG_GAME)  != 0; }
  bool lastWonSet()  const { return (lastFlags_ & FLAG_SET)   != 0; }
  bool lastWonMatch()const { return (lastFlags_ & FLAG_MATCH) != 0; }

  // Que cierra el equipo si gana el proximo punto (simula la transicion).
  Situation situation(uint8_t team) const;
  // Punto de juego con el rival al saque.
  bool isBreakPoint(uint8_t team) const;

 private:
  void push();

  MatchState s_;
  MatchRules r_;
  MatchState hist_[UNDO_DEPTH];
  uint8_t    histCount_ = 0;
  uint8_t    histHead_  = 0;
  uint8_t    lastFlags_ = 0;
};
