#include "display7seg.h"

void Display7Seg::begin() {
  // Invertir CADENA_SEGMENTOS: de "que segmento ocupa cada posicion" a
  // "que posicion ocupa cada segmento", que es lo que necesita drawSegment.
  const uint8_t cadena[7] = CADENA_SEGMENTOS;
  for (uint8_t i = 0; i < 7; i++) posSeg_[cadena[i] % 7] = i;

  FastLED.addLeds<LED_CHIPSET, PIN_LED_DATA, LED_COLOR_ORDER>(leds_, NUM_LEDS);
  // Para tiras con CLOCK (APA102 / WS2801 / LPD8806) usar esta linea en su lugar
  // y definir PIN_LED_CLOCK en config.h:
  // FastLED.addLeds<LED_CHIPSET, PIN_LED_DATA, PIN_LED_CLOCK, LED_COLOR_ORDER>(leds_, NUM_LEDS);
  // Clave para alimentar con power bank: FastLED baja el brillo global solo
  // antes de cada show() para no pasar del presupuesto de corriente declarado.
  FastLED.setMaxPowerInVoltsAndMilliamps(PSU_VOLTS, PSU_MILLIAMPS);
  FastLED.setDither(0);              // sin dither: evita parpadeo en video/foto
  FastLED.setBrightness(brightness_);
  clear();
  FastLED.show();
}

void Display7Seg::setBrightness(uint8_t b) {
  brightness_ = b;
  FastLED.setBrightness(b);
}

void Display7Seg::clear() { fill_solid(leds_, NUM_LEDS, CRGB::Black); }

CRGB Display7Seg::escalar(const CRGB& c, uint8_t f) {
  return CRGB((uint8_t)(((uint16_t)c.r * f) >> 8),
              (uint8_t)(((uint16_t)c.g * f) >> 8),
              (uint8_t)(((uint16_t)c.b * f) >> 8));
}

// Xorshift chiquito: evita depender de random() de Arduino, que choca con el
// random() de la libc cuando se compila el sketch en la PC para testearlo.
static uint16_t rnd16(uint16_t max) {
  static uint16_t sem = 0xACE1;
  sem ^= (uint16_t)(sem << 7);
  sem ^= (uint16_t)(sem >> 9);
  sem ^= (uint16_t)(sem << 8);
  return max ? (uint16_t)(sem % max) : 0;
}

// Arcoiris sin usar CHSV: mas chico en flash y suficiente para el festejo.
CRGB Display7Seg::rueda(uint8_t pos) {
  if (pos < 85)  return CRGB(255 - pos * 3, pos * 3, 0);
  if (pos < 170) { pos -= 85;  return CRGB(0, 255 - pos * 3, pos * 3); }
  pos -= 170;    return CRGB(pos * 3, 0, 255 - pos * 3);
}

// Latido triangular 0-255 a partir del reloj, con piso PULSO_MIN.
uint8_t Display7Seg::pulso() const {
  const uint16_t t = (uint16_t)((millis() / 6) % 512);
  uint16_t v = (t < 256) ? t : (511 - t);
  if (v < PULSO_MIN) v = PULSO_MIN;
  return (uint8_t)v;
}

void Display7Seg::fadeTeam(uint8_t team, uint8_t amount) {
  const uint16_t start = teamOffset(team);
  for (uint16_t i = 0; i < LEDS_PER_TEAM; i++) {
    CRGB& c = leds_[start + i];
    c = escalar(c, amount);
  }
}

void Display7Seg::mostrarCuatro(const uint8_t g[4], const CRGB& c) {
  clear();
  drawGlyph(oPointsTens(0), LEDS_PER_SEG_BIG, g[0], c);
  drawGlyph(oPointsOnes(0), LEDS_PER_SEG_BIG, g[1], c);
  drawGlyph(oPointsTens(1), LEDS_PER_SEG_BIG, g[2], c);
  drawGlyph(oPointsOnes(1), LEDS_PER_SEG_BIG, g[3], c);
  FastLED.show();
  haveLast_ = false;        // lo proximo que se dibuje tiene que ser completo
}

void Display7Seg::drawResetProgress(uint8_t pct) {
  clear();
  const uint16_t n = (uint16_t)(((uint32_t)NUM_LEDS * pct) / 100);
  for (uint16_t i = 0; i < n && i < NUM_LEDS; i++) leds_[i] = CRGB(255, 0, 0);
  FastLED.show();
}

void Display7Seg::fillGroup(uint16_t start, uint8_t n, const CRGB& c) {
  for (uint8_t i = 0; i < n; i++) {
    if (start + i < NUM_LEDS) leds_[start + i] = c;
  }
}

void Display7Seg::drawSegment(uint16_t start, uint8_t ledsPerSeg, uint8_t seg, const CRGB& c) {
  // seg es el segmento LOGICO (A..G); posSeg_ dice en que lugar de la cadena
  // quedo soldado fisicamente.
  fillGroup(start + (uint16_t)posSeg_[seg % 7] * ledsPerSeg, ledsPerSeg, c);
}

void Display7Seg::drawGlyph(uint16_t start, uint8_t ledsPerSeg, uint8_t mask, const CRGB& c) {
  for (uint8_t s = 0; s < 7; s++) {
    drawSegment(start, ledsPerSeg, s, (mask & (1 << s)) ? c : (CRGB)CRGB::Black);
  }
}

// --------------------------------- equipos ----------------------------------

void Display7Seg::drawTeam(uint8_t team, const MatchState& st) {
  uint8_t tens = Glyph::BLANK, ones = Glyph::BLANK;

  if (st.phase == PHASE_TIEBREAK) {
    const uint8_t v = st.tbPoints[team];
    if (v >= 10) tens = Glyph::DIGITS[(v / 10) % 10];
    ones = Glyph::DIGITS[v % 10];
  } else if (st.phase == PHASE_MATCH_OVER) {
    if (st.winner != team) return;                 // el perdedor queda APAGADO
    tens = Glyph::P; ones = Glyph::O_LOW;          // el ganador muestra "Po"
  } else {
    switch (st.points[team]) {
      case P_0:   tens = Glyph::BLANK;     ones = Glyph::DIGITS[0]; break;
      case P_15:  tens = Glyph::DIGITS[1]; ones = Glyph::DIGITS[5]; break;
      case P_30:  tens = Glyph::DIGITS[3]; ones = Glyph::DIGITS[0]; break;
      case P_40:  tens = Glyph::DIGITS[4]; ones = Glyph::DIGITS[0]; break;
      case P_ADV: tens = Glyph::A;         ones = Glyph::D_LOW;     break;  // "Ad"
    }
  }

  const bool fin = (st.phase == PHASE_MATCH_OVER);
  const uint8_t f = fin ? pulso() : 255;         // el ganador late despacio
  const CRGB cPts = fin ? escalar(COLOR_WIN, f) : (CRGB)COLOR_POINTS;

  drawGlyph(oPointsTens(team), LEDS_PER_SEG_BIG, tens, cPts);
  drawGlyph(oPointsOnes(team), LEDS_PER_SEG_BIG, ones, cPts);

  if (!fin) {
    uint8_t gGames;
    if (contador_) {
      // En los deportes de contador el digito de juegos se reusa como CENTENA
      // del tanteo, asi el basquet puede pasar de 99 sin agregar hardware.
      const uint8_t v = st.tbPoints[team];
      gGames = (v >= 100) ? Glyph::DIGITS[(v / 100) % 10] : Glyph::BLANK;
    } else {
      gGames = Glyph::DIGITS[st.games[team] % 10];
    }
    drawGlyph(oGames(team), LEDS_PER_SEG_SMALL, gGames, COLOR_GAMES);
  }
  drawGlyph(oSets(team), LEDS_PER_SEG_TINY,
            conSets_ ? Glyph::DIGITS[st.sets[team] % 10] : Glyph::BLANK,
            fin ? escalar(COLOR_SETS, f) : (CRGB)COLOR_SETS);
}

// --------------------------- indicadores del centro -------------------------

// Color del aviso "estas a un punto de...":
//   cyan = punto de juego | amarillo = punto de set | rojo titilando = de partido
static CRGB colorAviso(Situation s, bool blinkOn) {
  switch (s) {
    case SIT_GAME_POINT:  return COLOR_GAME_POINT;
    case SIT_SET_POINT:   return COLOR_SET_POINT;
    case SIT_MATCH_POINT: return blinkOn ? (CRGB)COLOR_MATCH_POINT : (CRGB)CRGB::Black;
    default:              return CRGB::Black;
  }
}

void Display7Seg::drawCenter(const MatchState& st, const Situation sit[2], bool showServe) {
  const bool blinkOn = ((millis() / ICON_BLINK_MS) & 1) == 0;
  const bool enJuego = (st.phase != PHASE_MATCH_OVER);

#if ICONOS_POR_EQUIPO >= 2
  // Dos iconos por equipo: la flecha de saque y el triangulo de aviso,
  // independientes, se pueden ver los dos a la vez.
  for (uint8_t t = 0; t < 2; t++) {
    const bool saca = showServe && enJuego && st.serving == t;
    fillGroup(oServeIcon(t), LEDS_PER_ICON, saca ? (CRGB)COLOR_SERVE : (CRGB)CRGB::Black);
    fillGroup(oPointIcon(t), LEDS_PER_ICON, colorAviso(sit[t], blinkOn));
  }
#else
  // Un solo triangulo por equipo: manda el aviso de punto, y si no hay ninguno
  // muestra el saque en verde. El aviso importa mas que quien saca.
  for (uint8_t t = 0; t < 2; t++) {
    CRGB c = colorAviso(sit[t], blinkOn);
    if (sit[t] == SIT_NONE && showServe && enJuego && st.serving == t) c = COLOR_SERVE;
    fillGroup(oServeIcon(t), LEDS_PER_ICON, c);
  }
#endif
}

// ----------------------------------- render ---------------------------------

void Display7Seg::render(const MatchState& st, const Situation sit[2], bool showServe) {
  lastState_      = st;
  lastSit_[0]     = sit[0];
  lastSit_[1]     = sit[1];
  lastShowServe_  = showServe;
  haveLast_       = true;

  clear();
  drawTeam(0, st);
  drawTeam(1, st);
  drawCenter(st, sit, showServe);
  FastLED.show();
}

void Display7Seg::refresh() {
  if (!haveLast_) return;
  render(lastState_, lastSit_, lastShowServe_);
}

bool Display7Seg::isBlinking() const {
  return lastSit_[0] == SIT_MATCH_POINT || lastSit_[1] == SIT_MATCH_POINT;
}

// --------------------------------- animaciones ------------------------------
// No se guarda un backup del framebuffer (en un Nano son cientos de bytes de
// RAM): se pinta encima y se restaura llamando a refresh().

void Display7Seg::flashTeam(uint8_t team, const CRGB& color, uint16_t ms) {
  fillGroup(teamOffset(team), LEDS_PER_TEAM, color);
  FastLED.show();
  delay(ms);
  refresh();
}

void Display7Seg::animGameWon(uint8_t team) {
  for (uint8_t k = 0; k < 2; k++) { flashTeam(team, COLOR_GAMES, 140); delay(90); }
}

void Display7Seg::animSetWon(uint8_t team) {
  for (uint8_t k = 0; k < 4; k++) { flashTeam(team, COLOR_SETS, 120); delay(80); }
}

// ---------------------------- festejo de partido ----------------------------
// 1) se apaga el perdedor y el centro
// 2) fuegos artificiales en el bloque del ganador
// 3) cometa que lo barre
// 4) barrido arcoiris
// Despues queda el "Po" del ganador latiendo hasta el reset (ver drawTeam).
void Display7Seg::animMatchWon(uint8_t team) {
  const uint16_t gana  = teamOffset(team);
  const uint16_t pierde = teamOffset(team ^ 1);

  // 1) apagar todo menos el ganador
  for (uint8_t k = 0; k < 10; k++) {
    fadeTeam(team ^ 1, 180);
    for (uint16_t i = 0; i < LEDS_CENTER; i++)
      leds_[CENTER_OFFSET + i] = escalar(leds_[CENTER_OFFSET + i], 180);
    FastLED.show();
    delay(40);
  }
  fillGroup(pierde, LEDS_PER_TEAM, CRGB::Black);
  fillGroup(CENTER_OFFSET, LEDS_CENTER, CRGB::Black);
  FastLED.show();
  delay(150);

  // 2) fuegos artificiales
  for (uint8_t f = 0; f < CELEBRA_FUEGOS; f++) {
    for (uint8_t k = 0; k < 16; k++) {
      fillGroup(gana, LEDS_PER_TEAM, CRGB::Black);
      const CRGB chispa = (k < 8) ? CRGB(255, 255, 180) : CRGB(255, 110, 0);
      for (uint8_t s = 0; s < 10; s++)
        leds_[gana + rnd16(LEDS_PER_TEAM)] = chispa;
      FastLED.show();
      delay(45);
    }
    fillGroup(gana, LEDS_PER_TEAM, CRGB::Black);
    FastLED.show();
    delay(120);
  }

  // 3) cometa
  for (uint8_t p = 0; p < CELEBRA_COMETAS; p++) {
    for (uint16_t i = 0; i < LEDS_PER_TEAM; i++) {
      fadeTeam(team, 170);
      leds_[gana + i] = COLOR_WIN;
      FastLED.show();
      delay(5);
    }
  }

  // 4) arcoiris
#if CELEBRA_ARCOIRIS
  for (uint16_t h = 0; h < 256 * 2; h += 7) {
    for (uint16_t i = 0; i < LEDS_PER_TEAM; i++)
      leds_[gana + i] = rueda((uint8_t)(h + i * 2));
    FastLED.show();
    delay(10);
  }
#endif

  refresh();   // queda el "Po" latiendo
}

void Display7Seg::animBoot() {
  // Barrido de prueba: enciende LED por LED en orden a lo largo de toda la
  // guirnalda. Sirve para verificar que el cableado quedo en la secuencia
  // correcta ANTES de cerrar la caja.
  clear();
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds_[i] = COLOR_POINTS;
    FastLED.show();
    delay(8);
  }
  delay(250);
  clear();
  FastLED.show();
}
