#include "display7seg.h"

void Display7Seg::begin() {
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

void Display7Seg::fillGroup(uint16_t start, uint8_t n, const CRGB& c) {
  for (uint8_t i = 0; i < n; i++) {
    if (start + i < NUM_LEDS) leds_[start + i] = c;
  }
}

void Display7Seg::drawSegment(uint16_t start, uint8_t ledsPerSeg, uint8_t seg, const CRGB& c) {
  fillGroup(start + (uint16_t)seg * ledsPerSeg, ledsPerSeg, c);
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
    if (st.winner == team) { tens = Glyph::P;    ones = Glyph::O_LOW; }  // "Po"
    else                   { tens = Glyph::DASH; ones = Glyph::DASH;  }  // "--"
  } else {
    switch (st.points[team]) {
      case P_0:   tens = Glyph::BLANK;     ones = Glyph::DIGITS[0]; break;
      case P_15:  tens = Glyph::DIGITS[1]; ones = Glyph::DIGITS[5]; break;
      case P_30:  tens = Glyph::DIGITS[3]; ones = Glyph::DIGITS[0]; break;
      case P_40:  tens = Glyph::DIGITS[4]; ones = Glyph::DIGITS[0]; break;
      case P_ADV: tens = Glyph::A;         ones = Glyph::D_LOW;     break;  // "Ad"
    }
  }

  const CRGB cPts = (st.phase == PHASE_MATCH_OVER && st.winner == team)
                      ? (CRGB)COLOR_WIN : (CRGB)COLOR_POINTS;
  drawGlyph(oPointsTens(team), LEDS_PER_SEG_BIG, tens, cPts);
  drawGlyph(oPointsOnes(team), LEDS_PER_SEG_BIG, ones, cPts);

  drawGlyph(oGames(team), LEDS_PER_SEG_SMALL, Glyph::DIGITS[st.games[team] % 10], COLOR_GAMES);
  drawGlyph(oSets(team),  LEDS_PER_SEG_TINY,  Glyph::DIGITS[st.sets[team]  % 10], COLOR_SETS);
}

// --------------------------- indicadores del centro -------------------------

void Display7Seg::drawCenter(const MatchState& st, const Situation sit[2], bool showServe) {
  // Flechas de saque: se enciende la del equipo que saca.
  for (uint8_t t = 0; t < 2; t++) {
    const bool on = showServe && st.phase != PHASE_MATCH_OVER && st.serving == t;
    fillGroup(oServeIcon(t), LEDS_PER_ICON, on ? (CRGB)COLOR_SERVE : (CRGB)CRGB::Black);
  }

  // Aviso "estas a un punto de...": el color dice de que.
  //   cyan     = punto de juego
  //   amarillo = punto de set
  //   rojo titilando = punto de partido
  const bool blinkOn = ((millis() / ICON_BLINK_MS) & 1) == 0;
  for (uint8_t t = 0; t < 2; t++) {
    CRGB c = CRGB::Black;
    switch (sit[t]) {
      case SIT_GAME_POINT:  c = COLOR_GAME_POINT; break;
      case SIT_SET_POINT:   c = COLOR_SET_POINT;  break;
      case SIT_MATCH_POINT: c = blinkOn ? (CRGB)COLOR_MATCH_POINT : (CRGB)CRGB::Black; break;
      default: break;
    }
    fillGroup(oPointIcon(t), LEDS_PER_ICON, c);
  }
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

void Display7Seg::animMatchWon(uint8_t team) {
  for (uint8_t k = 0; k < 8; k++) { flashTeam(team, COLOR_WIN, 150); delay(100); }
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
