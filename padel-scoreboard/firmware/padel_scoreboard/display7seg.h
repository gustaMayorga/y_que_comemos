#pragma once
#include <FastLED.h>
#include "config.h"
#include "padel_score.h"

// ============================================================================
//  Render del tanteo sobre digitos de 7 segmentos hechos con tira WS2812B.
//
//  Mapa de bits de un glifo: bit0=A bit1=B bit2=C bit3=D bit4=E bit5=F bit6=G
//
//        AAAA
//       F    B
//       F    B
//        GGGG
//       E    C
//       E    C
//        DDDD
// ============================================================================

namespace Glyph {
  constexpr uint8_t BLANK = 0x00;
  constexpr uint8_t DASH  = 0x40;
  constexpr uint8_t DIGITS[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
  constexpr uint8_t A = 0x77, D_LOW = 0x5E, P = 0x73, O_LOW = 0x5C, T = 0x78;
}

class Display7Seg {
 public:
  void begin();
  void setBrightness(uint8_t b);
  uint8_t brightness() const { return brightness_; }

  // Dibuja el estado completo y hace show().
  void render(const MatchState& st, bool showServe);

  // Animaciones bloqueantes cortas (se usan como feedback, no en el loop rapido)
  void flashTeam(uint8_t team, const CRGB& color, uint16_t ms);
  void animGameWon(uint8_t team);
  void animSetWon(uint8_t team);
  void animMatchWon(uint8_t team);
  void animBoot();

 private:
  void clear();
  void drawSegment(uint16_t start, uint8_t ledsPerSeg, uint8_t seg, const CRGB& c);
  void drawGlyph(uint16_t start, uint8_t ledsPerSeg, uint8_t mask, const CRGB& c);
  void drawTeam(uint8_t team, const MatchState& st, bool showServe);

  uint16_t teamOffset(uint8_t team) const {
    return team == 0 ? TEAM_A_OFFSET : TEAM_B_OFFSET;
  }
  // Posiciones dentro del bloque de un equipo
  uint16_t oPointsTens(uint8_t t) const { return teamOffset(t); }
  uint16_t oPointsOnes(uint8_t t) const { return teamOffset(t) + DIGIT_BIG_LEDS; }
  uint16_t oGames(uint8_t t)      const { return teamOffset(t) + 2 * DIGIT_BIG_LEDS; }
  uint16_t oSetDots(uint8_t t)    const { return oGames(t) + DIGIT_SMALL_LEDS; }

  CRGB    leds_[NUM_LEDS];
  uint8_t brightness_ = BRIGHTNESS_DAY;
};
