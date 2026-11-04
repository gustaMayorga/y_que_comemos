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
//
//  Los iconos del centro (flechas de saque y avisos de punto) no tienen forma
//  propia en LEDs: son grupos de LEDs detras de una ventana recortada en la
//  mascara. La forma la da la mascara, el firmware solo enciende el grupo.
// ============================================================================

namespace Glyph {
  const uint8_t BLANK = 0x00;
  const uint8_t DASH  = 0x40;
  const uint8_t DIGITS[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
  const uint8_t A = 0x77, D_LOW = 0x5E, P = 0x73, O_LOW = 0x5C;
}

class Display7Seg {
 public:
  void begin();
  void setBrightness(uint8_t b);
  uint8_t brightness() const { return brightness_; }

  // Dibuja el estado completo y hace show().
  // sit[2] = que cierra cada equipo si gana el proximo punto (indicador central).
  void render(const MatchState& st, const Situation sit[2], bool showServe);
  // Re-dibuja con los ultimos datos (para el titileo del punto de partido).
  void refresh();
  // true si hay algo titilando y conviene refrescar periodicamente.
  bool isBlinking() const;

  void flashTeam(uint8_t team, const CRGB& color, uint16_t ms);
  // Barra roja que se va llenando mientras se mantiene el boton de reset.
  void drawResetProgress(uint8_t pct);
  // true mientras el ganador late: hay que refrescar seguido.
  bool isCelebrating() const { return haveLast_ && lastState_.phase == PHASE_MATCH_OVER; }
  void animGameWon(uint8_t team);
  void animSetWon(uint8_t team);
  void animMatchWon(uint8_t team);
  void animBoot();

 private:
  void clear();
  void drawSegment(uint16_t start, uint8_t ledsPerSeg, uint8_t seg, const CRGB& c);
  void drawGlyph(uint16_t start, uint8_t ledsPerSeg, uint8_t mask, const CRGB& c);
  void fillGroup(uint16_t start, uint8_t n, const CRGB& c);
  void drawTeam(uint8_t team, const MatchState& st);
  void drawCenter(const MatchState& st, const Situation sit[2], bool showServe);
  void fadeTeam(uint8_t team, uint8_t amount);
  uint8_t pulso() const;                       // latido 0-255 derivado de millis()
  static CRGB rueda(uint8_t pos);              // arcoiris sin depender de CHSV
  static CRGB escalar(const CRGB& c, uint8_t f);

  uint16_t teamOffset(uint8_t t) const { return t == 0 ? TEAM_A_OFFSET : TEAM_B_OFFSET; }
  uint16_t oPointsTens(uint8_t t) const { return teamOffset(t); }
  uint16_t oPointsOnes(uint8_t t) const { return teamOffset(t) + DIGIT_BIG_LEDS; }
  uint16_t oGames(uint8_t t)      const { return teamOffset(t) + 2 * DIGIT_BIG_LEDS; }
  uint16_t oSets(uint8_t t)       const { return oGames(t) + DIGIT_SMALL_LEDS; }
  // Centro: saque A, saque B, punto A, punto B
  uint16_t oServeIcon(uint8_t t)  const { return CENTER_OFFSET + (uint16_t)t * LEDS_PER_ICON; }
  uint16_t oPointIcon(uint8_t t)  const { return CENTER_OFFSET + (uint16_t)(2 + t) * LEDS_PER_ICON; }

  CRGB       leds_[NUM_LEDS];
  uint8_t    brightness_ = BRIGHTNESS_DAY;
  MatchState lastState_;
  Situation  lastSit_[2] = { SIT_NONE, SIT_NONE };
  bool       lastShowServe_ = true;
  bool       haveLast_ = false;
};
