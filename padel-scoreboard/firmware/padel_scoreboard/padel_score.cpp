#include "padel_score.h"
#include <string.h>

#define HIST_CAP (sizeof(hist_) / sizeof(hist_[0]))

void ScoreEngine::begin(const MatchRules& rules) {
  r_ = rules;
  resetMatch();
}

void ScoreEngine::resetMatch() {
  memset(&s_, 0, sizeof(s_));
  s_.phase   = PHASE_GAME;
  s_.winner  = 0xFF;
  s_.serving = 0;
  histCount_ = 0;
  histHead_  = 0;
  flagGame_  = false;
  flagSet_   = false;
}

void ScoreEngine::push() {
  hist_[histHead_] = s_;
  histHead_ = (histHead_ + 1) % HIST_CAP;
  if (histCount_ < HIST_CAP) histCount_++;
}

bool ScoreEngine::undo() {
  if (histCount_ == 0) return false;
  histHead_ = (histHead_ + HIST_CAP - 1) % HIST_CAP;
  s_ = hist_[histHead_];
  histCount_--;
  flagGame_ = false;
  flagSet_  = false;
  return true;
}

void ScoreEngine::setServing(uint8_t team) { s_.serving = team & 1; }
void ScoreEngine::toggleServing()          { s_.serving ^= 1; }

bool ScoreEngine::addPoint(uint8_t team) {
  if (s_.phase == PHASE_MATCH_OVER) return false;
  team &= 1;
  const uint8_t other = team ^ 1;

  push();
  flagGame_ = false;
  flagSet_  = false;
  s_.pointCount++;

  // ----------------------------- TIE BREAK --------------------------------
  if (s_.phase == PHASE_TIEBREAK) {
    s_.tbPoints[team]++;
    const uint8_t a = s_.tbPoints[team], b = s_.tbPoints[other];
    // El saque cambia al primer punto y despues cada 2 puntos.
    if (((a + b) % 2) == 1) s_.serving ^= 1;
    if (a >= r_.tiebreakTarget && (a - b) >= 2) {
      s_.games[team]++;          // 7-6 en games
      awardSet(team);
    }
    return true;
  }

  // ----------------------------- JUEGO NORMAL -----------------------------
  uint8_t& p  = s_.points[team];
  uint8_t& po = s_.points[other];

  if (p == P_ADV) {                       // tenia ventaja -> gana el juego
    awardGame(team);
    return true;
  }
  if (po == P_ADV) {                      // el rival tenia ventaja -> vuelta a 40-40
    po = P_40;
    return true;
  }
  if (p == P_40 && po == P_40) {          // 40-40
    if (r_.goldenPoint) awardGame(team);  // punto de oro
    else                p = P_ADV;        // ventaja
    return true;
  }
  if (p == P_40) {                        // 40 contra <=30 -> juego
    awardGame(team);
    return true;
  }
  p++;                                    // 0 -> 15 -> 30 -> 40
  return true;
}

void ScoreEngine::awardGame(uint8_t team) {
  flagGame_ = true;
  s_.points[0] = s_.points[1] = P_0;
  s_.games[team]++;
  s_.serving ^= 1;                        // el saque pasa al otro equipo

  const uint8_t g = s_.games[team], go = s_.games[team ^ 1];
  if (g >= r_.gamesPerSet && (g - go) >= 2) { awardSet(team); return; }
  if (g == r_.gamesPerSet + 1 && go == r_.gamesPerSet - 1) { awardSet(team); return; } // 7-5
  startTiebreakIfNeeded();
}

void ScoreEngine::startTiebreakIfNeeded() {
  if (!r_.tieBreak) return;
  if (s_.games[0] == r_.gamesPerSet && s_.games[1] == r_.gamesPerSet) {
    s_.phase = PHASE_TIEBREAK;
    s_.tbPoints[0] = s_.tbPoints[1] = 0;
    s_.points[0]   = s_.points[1]   = P_0;
  }
}

void ScoreEngine::awardSet(uint8_t team) {
  flagSet_ = true;
  s_.sets[team]++;
  s_.games[0] = s_.games[1] = 0;
  s_.points[0] = s_.points[1] = P_0;
  s_.tbPoints[0] = s_.tbPoints[1] = 0;
  s_.phase = PHASE_GAME;

  if (s_.sets[team] >= r_.setsToWin) {
    s_.phase  = PHASE_MATCH_OVER;
    s_.winner = team;
  }
}
