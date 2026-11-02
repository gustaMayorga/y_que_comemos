#include "display7seg.h"

void Display7Seg::begin() {
  FastLED.addLeds<WS2812B, PIN_LED_DATA, GRB>(leds_, NUM_LEDS);
  // Clave para alimentar con power bank: FastLED baja el brillo solo
  // antes de pasarse del presupuesto de corriente declarado.
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

void Display7Seg::drawSegment(uint16_t start, uint8_t ledsPerSeg, uint8_t seg, const CRGB& c) {
  const uint16_t base = start + (uint16_t)seg * ledsPerSeg;
  for (uint8_t i = 0; i < ledsPerSeg; i++) {
    const uint16_t idx = base + i;
    if (idx < NUM_LEDS) leds_[idx] = c;
  }
}

void Display7Seg::drawGlyph(uint16_t start, uint8_t ledsPerSeg, uint8_t mask, const CRGB& c) {
  for (uint8_t s = 0; s < 7; s++) {
    drawSegment(start, ledsPerSeg, s, (mask & (1 << s)) ? c : CRGB::Black);
  }
}

void Display7Seg::drawTeam(uint8_t team, const MatchState& st, bool showServe) {
  uint8_t tens = Glyph::BLANK, ones = Glyph::BLANK;

  if (st.phase == PHASE_TIEBREAK) {
    const uint8_t v = st.tbPoints[team];
    if (v >= 10) tens = Glyph::DIGITS[(v / 10) % 10];
    ones = Glyph::DIGITS[v % 10];
  } else if (st.phase == PHASE_MATCH_OVER) {
    // Ganador: "PO" (partido) fijo / perdedor: guiones
    if (st.winner == team) { tens = Glyph::P; ones = Glyph::O_LOW; }
    else                   { tens = Glyph::DASH; ones = Glyph::DASH; }
  } else {
    switch (st.points[team]) {
      case P_0:   tens = Glyph::BLANK;      ones = Glyph::DIGITS[0]; break;
      case P_15:  tens = Glyph::DIGITS[1];  ones = Glyph::DIGITS[5]; break;
      case P_30:  tens = Glyph::DIGITS[3];  ones = Glyph::DIGITS[0]; break;
      case P_40:  tens = Glyph::DIGITS[4];  ones = Glyph::DIGITS[0]; break;
      case P_ADV: tens = Glyph::A;          ones = Glyph::D_LOW;     break;
    }
  }

  const CRGB cPts = (st.phase == PHASE_MATCH_OVER && st.winner == team) ? COLOR_WIN : COLOR_POINTS;
  drawGlyph(oPointsTens(team), LEDS_PER_SEG_BIG, tens, cPts);
  drawGlyph(oPointsOnes(team), LEDS_PER_SEG_BIG, ones, cPts);

  // Games (un digito)
  const uint8_t g = st.games[team];
  drawGlyph(oGames(team), LEDS_PER_SEG_SMALL, Glyph::DIGITS[g % 10], COLOR_GAMES);

  // Dots de sets ganados; el dot libre marca el saque si showServe
  for (uint8_t d = 0; d < SET_DOTS_PER_TEAM; d++) {
    CRGB c = CRGB::Black;
    if (d < st.sets[team])                                c = COLOR_SETS;
    else if (showServe && d == st.sets[team] &&
             st.serving == team && st.phase != PHASE_MATCH_OVER) c = COLOR_SERVE;
    const uint16_t base = oSetDots(team) + (uint16_t)d * LEDS_PER_SET_DOT;
    for (uint8_t i = 0; i < LEDS_PER_SET_DOT; i++) {
      if (base + i < NUM_LEDS) leds_[base + i] = c;
    }
  }
}

void Display7Seg::render(const MatchState& st, bool showServe) {
  clear();
  drawTeam(0, st, showServe);
  drawTeam(1, st, showServe);
  FastLED.show();
}

void Display7Seg::flashTeam(uint8_t team, const CRGB& color, uint16_t ms) {
  const uint16_t start = teamOffset(team);
  CRGB backup[LEDS_PER_TEAM];
  for (uint16_t i = 0; i < LEDS_PER_TEAM; i++) backup[i] = leds_[start + i];
  for (uint16_t i = 0; i < LEDS_PER_TEAM; i++) leds_[start + i] = color;
  FastLED.show();
  delay(ms);
  for (uint16_t i = 0; i < LEDS_PER_TEAM; i++) leds_[start + i] = backup[i];
  FastLED.show();
}

void Display7Seg::animGameWon(uint8_t team) {
  for (uint8_t k = 0; k < 2; k++) flashTeam(team, COLOR_GAMES, 140), delay(90);
}

void Display7Seg::animSetWon(uint8_t team) {
  for (uint8_t k = 0; k < 4; k++) flashTeam(team, COLOR_SETS, 120), delay(80);
}

void Display7Seg::animMatchWon(uint8_t team) {
  for (uint8_t k = 0; k < 8; k++) flashTeam(team, COLOR_WIN, 150), delay(100);
}

void Display7Seg::animBoot() {
  // Barrido de prueba: enciende cada segmento en orden. Sirve para verificar
  // que el cableado A..G de cada digito quedo en la secuencia correcta.
  clear();
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds_[i] = COLOR_POINTS;
    if ((i % 4) == 0) FastLED.show();
    delay(4);
  }
  FastLED.show();
  delay(250);
  clear();
  FastLED.show();
}
