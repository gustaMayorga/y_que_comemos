// Test de la maquina de estados del tanteo, corre en PC (sin Arduino).
//   g++ -std=c++17 tests/test_score.cpp firmware/padel_scoreboard/padel_score.cpp
//       -o /tmp/test_score && /tmp/test_score
#include "../firmware/padel_scoreboard/padel_score.h"
#include <cstdio>
#include <cstdlib>

static int fails = 0;
#define CHECK(cond, msg) do { if (!(cond)) { printf("  FAIL: %s (linea %d)\n", msg, __LINE__); fails++; } } while (0)

static MatchRules rulesGolden() { return MatchRules{ true,  6, 2, true, 7 }; }
static MatchRules rulesAdv()    { return MatchRules{ false, 6, 2, true, 7 }; }

static void pts(ScoreEngine& e, uint8_t team, int n) { for (int i = 0; i < n; i++) e.addPoint(team); }

int main() {
  printf("== test tanteo padel ==\n");

  // --- 1) progresion simple 0/15/30/40 y juego ---
  { ScoreEngine e; e.begin(rulesGolden());
    e.addPoint(0); CHECK(e.state().points[0] == P_15, "0 -> 15");
    e.addPoint(0); CHECK(e.state().points[0] == P_30, "15 -> 30");
    e.addPoint(0); CHECK(e.state().points[0] == P_40, "30 -> 40");
    e.addPoint(0); CHECK(e.state().games[0] == 1, "40 -> juego");
    CHECK(e.state().points[0] == P_0 && e.state().points[1] == P_0, "puntos reset tras juego");
    CHECK(e.lastWonGame(), "flag de juego ganado");
    CHECK(!e.lastWonSet() && !e.lastWonMatch(), "no marca set ni partido");
  }

  // --- 2) punto de oro: 40-40 lo gana el siguiente punto ---
  { ScoreEngine e; e.begin(rulesGolden());
    pts(e, 0, 3); pts(e, 1, 3);
    CHECK(e.state().points[0] == P_40 && e.state().points[1] == P_40, "40-40");
    e.addPoint(1);
    CHECK(e.state().games[1] == 1, "punto de oro gana el juego");
  }

  // --- 3) ventajas: 40-40 -> AD -> vuelta a 40-40 -> juego ---
  { ScoreEngine e; e.begin(rulesAdv());
    pts(e, 0, 3); pts(e, 1, 3);
    e.addPoint(0); CHECK(e.state().points[0] == P_ADV, "ventaja A");
    e.addPoint(1); CHECK(e.state().points[0] == P_40 && e.state().points[1] == P_40, "vuelve a 40-40");
    e.addPoint(1); CHECK(e.state().points[1] == P_ADV, "ventaja B");
    e.addPoint(1); CHECK(e.state().games[1] == 1, "gana juego desde ventaja");
  }

  // --- 4) set 6-0 ---
  { ScoreEngine e; e.begin(rulesGolden());
    for (int g = 0; g < 6; g++) pts(e, 0, 4);
    CHECK(e.state().sets[0] == 1, "set 6-0");
    CHECK(e.state().games[0] == 0 && e.state().games[1] == 0, "games reset tras set");
  }

  // --- 5) 6-5 no cierra set, 7-5 si ---
  { ScoreEngine e; e.begin(rulesGolden());
    for (int g = 0; g < 5; g++) pts(e, 0, 4);
    for (int g = 0; g < 5; g++) pts(e, 1, 4);   // 5-5
    pts(e, 0, 4);                                // 6-5
    CHECK(e.state().sets[0] == 0, "6-5 no cierra el set");
    pts(e, 0, 4);                                // 7-5
    CHECK(e.state().sets[0] == 1, "7-5 cierra el set");
  }

  // --- 6) 6-6 -> tie break, se gana 7 con 2 de diferencia ---
  { // alternamos juegos para llegar a 6-6 (6-0 cerraria el set)
    ScoreEngine f; f.begin(rulesGolden());
    for (int g = 0; g < 6; g++) { pts(f, 0, 4); pts(f, 1, 4); }
    CHECK(f.state().phase == PHASE_TIEBREAK, "6-6 entra en tie break");
    pts(f, 0, 6); pts(f, 1, 6);                  // 6-6 en el tie break
    CHECK(f.state().phase == PHASE_TIEBREAK, "6-6 en tb sigue");
    f.addPoint(0);                               // 7-6 -> no alcanza
    CHECK(f.state().phase == PHASE_TIEBREAK, "7-6 en tb no cierra");
    f.addPoint(0);                               // 8-6 -> cierra set
    CHECK(f.state().sets[0] == 1, "tb 8-6 cierra el set");
    CHECK(f.state().phase == PHASE_GAME, "vuelve a juego normal");
  }

  // --- 7) partido al mejor de 3 ---
  { ScoreEngine e; e.begin(rulesGolden());
    for (int s = 0; s < 2; s++) for (int g = 0; g < 6; g++) pts(e, 1, 4);
    CHECK(e.state().phase == PHASE_MATCH_OVER, "partido terminado");
    CHECK(e.state().winner == 1, "gana equipo B");
    CHECK(e.addPoint(0) == false, "no acepta puntos despues del final");
  }

  // --- 8) undo ---
  { ScoreEngine e; e.begin(rulesGolden());
    pts(e, 0, 3);                                 // 40
    e.addPoint(0);                                // juego
    CHECK(e.state().games[0] == 1, "juego sumado");
    e.undo();
    CHECK(e.state().games[0] == 0 && e.state().points[0] == P_40, "undo devuelve a 40-0");
    for (int i = 0; i < 20; i++) e.undo();
    CHECK(e.undo() == false, "undo vacio devuelve false");
  }

  // --- 9) el saque cambia al ganar un juego ---
  { ScoreEngine e; e.begin(rulesGolden());
    const uint8_t s0 = e.state().serving;
    pts(e, 0, 4);
    CHECK(e.state().serving == (s0 ^ 1), "el saque cambia de equipo");
  }

  // --- 10) indicador central: punto de juego / set / partido ---
  { ScoreEngine e; e.begin(rulesGolden());
    CHECK(e.situation(0) == SIT_NONE, "0-0 no es punto de nada");
    pts(e, 0, 3);                                 // 40-0
    CHECK(e.situation(0) == SIT_GAME_POINT, "40-0 es punto de juego");
    CHECK(e.situation(1) == SIT_NONE, "el rival no tiene punto de juego");
  }
  { ScoreEngine e; e.begin(rulesGolden());
    for (int g = 0; g < 5; g++) pts(e, 0, 4);     // 5-0 en games
    pts(e, 0, 3);                                 // 40-0 en el 6to juego
    CHECK(e.situation(0) == SIT_SET_POINT, "5-0 y 40-0 es punto de set");
  }
  { ScoreEngine e; e.begin(rulesGolden());
    for (int g = 0; g < 6; g++) pts(e, 0, 4);     // gana el 1er set
    for (int g = 0; g < 5; g++) pts(e, 0, 4);     // 5-0 en el 2do
    pts(e, 0, 3);                                 // 40-0
    CHECK(e.situation(0) == SIT_MATCH_POINT, "1 set y 5-0 40-0 es punto de partido");
  }
  { // punto de oro: en 40-40 los DOS equipos tienen punto de juego
    ScoreEngine e; e.begin(rulesGolden());
    pts(e, 0, 3); pts(e, 1, 3);
    CHECK(e.situation(0) == SIT_GAME_POINT && e.situation(1) == SIT_GAME_POINT,
          "40-40 con punto de oro: ambos tienen punto de juego");
  }
  { // con ventajas, en 40-40 nadie esta a un punto del juego
    ScoreEngine e; e.begin(rulesAdv());
    pts(e, 0, 3); pts(e, 1, 3);
    CHECK(e.situation(0) == SIT_NONE && e.situation(1) == SIT_NONE,
          "40-40 con ventajas: todavia no es punto de juego");
    e.addPoint(0);                                // ventaja A
    CHECK(e.situation(0) == SIT_GAME_POINT, "con ventaja si es punto de juego");
  }
  { // punto de quiebre = punto de juego con el rival al saque
    ScoreEngine e; e.begin(rulesGolden());
    e.setServing(1);
    pts(e, 0, 3);                                 // 40-0 contra el saque de B
    CHECK(e.isBreakPoint(0), "40-0 contra el saque es punto de quiebre");
    CHECK(!e.isBreakPoint(1), "el que saca no tiene punto de quiebre");
  }

  // --- 11) la simulacion NO debe modificar el partido ---
  { ScoreEngine e; e.begin(rulesGolden());
    pts(e, 0, 3);
    const MatchState before = e.state();
    for (int i = 0; i < 5; i++) { e.situation(0); e.situation(1); e.isBreakPoint(0); }
    const MatchState after = e.state();
    CHECK(before.points[0] == after.points[0] && before.games[0] == after.games[0] &&
          before.pointCount == after.pointCount,
          "situation() no altera el estado");
  }

  // --- 12) flags de la transicion pura ---
  { MatchState s{}; s.phase = PHASE_GAME; s.winner = 0xFF;
    MatchRules r = rulesGolden();
    uint8_t f = 0;
    for (int i = 0; i < 3; i++) applyPoint(s, r, 0, &f);
    CHECK(f == 0, "sin flags antes de cerrar el juego");
    applyPoint(s, r, 0, &f);
    CHECK((f & FLAG_GAME) && !(f & FLAG_SET), "flag de juego sin flag de set");
  }

  printf(fails ? "\n%d test(s) FALLARON\n" : "\nTodos los tests OK\n", fails);
  return fails ? 1 : 0;
}
