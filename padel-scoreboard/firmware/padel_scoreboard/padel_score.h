#pragma once
#include <stdint.h>

// ============================================================================
//  Maquina de estados del tanteo de padel.
//  Sin dependencias de hardware: se puede compilar y testear en PC.
// ============================================================================

enum GamePhase : uint8_t {
  PHASE_GAME = 0,     // juego normal (0/15/30/40/Ventaja)
  PHASE_TIEBREAK,     // 6-6 -> tie break
  PHASE_MATCH_OVER
};

// Indice de puntos en un juego normal
enum PointIdx : uint8_t { P_0 = 0, P_15, P_30, P_40, P_ADV };

struct MatchRules {
  bool    goldenPoint;    // true: en 40-40 el siguiente punto gana el juego
  uint8_t gamesPerSet;    // normalmente 6
  uint8_t setsToWin;      // normalmente 2 (al mejor de 3)
  bool    tieBreak;       // 6-6 -> tie break
  uint8_t tiebreakTarget; // normalmente 7
};

// Snapshot completo: es lo que se guarda para el UNDO y lo que se serializa
// a JSON cuando se agregue WiFi.
struct MatchState {
  uint8_t   points[2];    // PointIdx cuando phase == PHASE_GAME
  uint8_t   tbPoints[2];  // puntos del tie break
  uint8_t   games[2];
  uint8_t   sets[2];
  uint8_t   phase;        // GamePhase
  uint8_t   winner;       // 0xFF = sin ganador, 0/1 = equipo
  uint8_t   serving;      // 0/1 equipo al saque (informativo)
  uint32_t  pointCount;   // puntos jugados en total
};

class ScoreEngine {
 public:
  void begin(const MatchRules& rules);
  void resetMatch();

  // Devuelve true si el punto cambio algo (false si el partido ya termino).
  bool addPoint(uint8_t team);
  bool undo();

  void setServing(uint8_t team);
  void toggleServing();

  const MatchState& state() const { return s_; }
  const MatchRules& rules() const { return r_; }
  MatchRules&       mutableRules() { return r_; }

  bool matchOver()   const { return s_.phase == PHASE_MATCH_OVER; }
  // true si el ultimo addPoint() cerro un juego / set / partido
  bool lastWonGame() const { return flagGame_; }
  bool lastWonSet()  const { return flagSet_; }

 private:
  void push();                 // guarda snapshot para undo
  void awardGame(uint8_t team);
  void awardSet(uint8_t team);
  void startTiebreakIfNeeded();

  MatchState s_;
  MatchRules r_;
  MatchState hist_[16];
  uint8_t    histCount_ = 0;
  uint8_t    histHead_  = 0;
  bool       flagGame_  = false;
  bool       flagSet_   = false;
};
