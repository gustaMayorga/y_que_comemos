#include "padel_score.h"
#include <string.h>

#define HIST_CAP ((uint8_t)(sizeof(hist_) / sizeof(hist_[0])))

// ============================ transicion pura ===============================

static void awardSet(MatchState& s, const MatchRules& r, uint8_t team, uint8_t* f) {
  *f |= FLAG_SET;
  s.sets[team]++;
  s.games[0] = s.games[1] = 0;
  s.points[0] = s.points[1] = P_0;
  s.tbPoints[0] = s.tbPoints[1] = 0;
  s.phase = PHASE_GAME;

  if (s.sets[team] >= r.setsToWin) {
    s.phase  = PHASE_MATCH_OVER;
    s.winner = team;
    *f |= FLAG_MATCH;
  }
}

static void awardGame(MatchState& s, const MatchRules& r, uint8_t team, uint8_t* f) {
  *f |= FLAG_GAME;
  s.points[0] = s.points[1] = P_0;
  s.games[team]++;
  s.serving ^= 1;                               // el saque pasa al otro equipo

  const uint8_t g = s.games[team], go = s.games[team ^ 1];
  if (g >= r.gamesPerSet && (g - go) >= 2)            { awardSet(s, r, team, f); return; }
  if (g == r.gamesPerSet + 1 && go == r.gamesPerSet - 1) { awardSet(s, r, team, f); return; } // 7-5

  if (r.tieBreak && s.games[0] == r.gamesPerSet && s.games[1] == r.gamesPerSet) {
    s.phase = PHASE_TIEBREAK;                   // 6-6
    s.tbPoints[0] = s.tbPoints[1] = 0;
    s.points[0]   = s.points[1]   = P_0;
  }
}

bool applyPoint(MatchState& s, const MatchRules& r, uint8_t team, uint8_t* flagsOut) {
  uint8_t f = 0;
  if (flagsOut) *flagsOut = 0;
  if (s.phase == PHASE_MATCH_OVER) return false;

  team &= 1;
  const uint8_t other = team ^ 1;
  s.pointCount++;

  // ------------------------------- TIE BREAK -------------------------------
  if (s.phase == PHASE_TIEBREAK) {
    s.tbPoints[team]++;
    const uint8_t a = s.tbPoints[team], b = s.tbPoints[other];
    // El saque cambia al primer punto y despues cada 2 puntos.
    if (((a + b) % 2) == 1) s.serving ^= 1;
    if (a >= r.tiebreakTarget && (a - b) >= 2) {
      s.games[team]++;                          // 7-6 en games
      f |= FLAG_GAME;
      awardSet(s, r, team, &f);
    }
    if (flagsOut) *flagsOut = f;
    return true;
  }

  // ----------------------------- JUEGO NORMAL ------------------------------
  uint8_t& p  = s.points[team];
  uint8_t& po = s.points[other];

  if (p == P_ADV) {                             // tenia ventaja -> gana el juego
    awardGame(s, r, team, &f);
  } else if (po == P_ADV) {                     // el rival tenia ventaja -> 40-40
    po = P_40;
  } else if (p == P_40 && po == P_40) {         // 40-40
    if (r.goldenPoint) awardGame(s, r, team, &f);   // punto de oro
    else               p = P_ADV;                   // ventaja
  } else if (p == P_40) {                       // 40 contra <=30 -> juego
    awardGame(s, r, team, &f);
  } else {
    p++;                                        // 0 -> 15 -> 30 -> 40
  }

  if (flagsOut) *flagsOut = f;
  return true;
}

// ============================== ScoreEngine =================================

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
  lastFlags_ = 0;
}

void ScoreEngine::push() {
  hist_[histHead_] = s_;
  histHead_ = (uint8_t)((histHead_ + 1) % HIST_CAP);
  if (histCount_ < HIST_CAP) histCount_++;
}

bool ScoreEngine::undo() {
  if (histCount_ == 0) return false;
  histHead_ = (uint8_t)((histHead_ + HIST_CAP - 1) % HIST_CAP);
  s_ = hist_[histHead_];
  histCount_--;
  lastFlags_ = 0;
  return true;
}

void ScoreEngine::setServing(uint8_t team) { s_.serving = team & 1; }
void ScoreEngine::toggleServing()          { s_.serving ^= 1; }

bool ScoreEngine::addPoint(uint8_t team) {
  if (s_.phase == PHASE_MATCH_OVER) return false;
  push();
  const bool ok = applyPoint(s_, r_, team, &lastFlags_);
  if (!ok && histCount_) {                      // no deberia pasar; no ensuciar el historial
    histHead_ = (uint8_t)((histHead_ + HIST_CAP - 1) % HIST_CAP);
    histCount_--;
  }
  return ok;
}

Situation ScoreEngine::situation(uint8_t team) const {
  if (s_.phase == PHASE_MATCH_OVER) return SIT_NONE;
  MatchState tmp = s_;                          // 16 bytes en la pila
  uint8_t f = 0;
  applyPoint(tmp, r_, team, &f);
  if (f & FLAG_MATCH) return SIT_MATCH_POINT;
  if (f & FLAG_SET)   return SIT_SET_POINT;
  if (f & FLAG_GAME)  return SIT_GAME_POINT;
  return SIT_NONE;
}

bool ScoreEngine::isBreakPoint(uint8_t team) const {
  return situation(team) != SIT_NONE && s_.serving == (uint8_t)(team ^ 1);
}
