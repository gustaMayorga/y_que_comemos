// ============================================================================
//  TANTEADOR DE PADEL - MVP
//  Cerebro: ESP32 DevKit v1   |   Display: 7 segmentos con tira WS2812B
//  Entradas: 2 sensores de proximidad de paleta + 2 controles RF 433 MHz
//  Alimentacion: power bank 5 V / USB-C PD (ver docs/01-hardware-bom.md)
//
//  Mapa de controles (receptor RF de 4 canales, 2 llaveros identicos):
//    CH1 (boton A del llavero del Equipo A) -> +1 punto Equipo A
//    CH2 (boton B del llavero del Equipo B) -> +1 punto Equipo B
//    CH3 corto -> deshacer ultimo punto   |  CH3 largo -> cambiar saque
//    CH4 corto -> ciclar brillo           |  CH4 2 s   -> reset del partido
//  El boton local de la caja replica CH4.
// ============================================================================

#include <FastLED.h>
#include "config.h"
#include "padel_score.h"
#include "display7seg.h"
#include "inputs.h"
#include "net_stub.h"

ScoreEngine      score;
Display7Seg      display;
ProximityCounter proxA, proxB;
Button           rfPointA, rfPointB, rfUndo, rfMode, btnLocal;

static const uint8_t kBrightSteps[3] = { BRIGHTNESS_DAY, BRIGHTNESS_SHADE, BRIGHTNESS_NIGHT };
static uint8_t  brightIdx  = 0;
static bool     showServe  = true;
static bool     dirty      = true;

// ------------------------------- buzzer ------------------------------------
#if ENABLE_BUZZER
  #if defined(ESP32)
    #define BUZZ_CH 0
    static void beepInit() { ledcSetup(BUZZ_CH, 2000, 8); ledcAttachPin(PIN_BUZZER, BUZZ_CH); }
    static void beep(uint16_t freq, uint16_t ms) {
      ledcWriteTone(BUZZ_CH, freq); delay(ms); ledcWriteTone(BUZZ_CH, 0);
    }
  #else
    static void beepInit() { pinMode(PIN_BUZZER, OUTPUT); }
    static void beep(uint16_t freq, uint16_t ms) { tone(PIN_BUZZER, freq, ms); delay(ms); }
  #endif
#else
  static void beepInit() {}
  static void beep(uint16_t, uint16_t) {}
#endif

// --------------------------- acciones de alto nivel ------------------------
static void onPoint(uint8_t team) {
  if (!score.addPoint(team)) { beep(300, 200); return; }   // partido terminado
  dirty = true;
  beep(1800, 60);
  display.render(score.state(), showServe);

  if (score.matchOver())        { beep(2400, 400); display.animMatchWon(score.state().winner); }
  else if (score.lastWonSet())  { beep(2200, 250); display.animSetWon(team); }
  else if (score.lastWonGame()) { beep(2000, 150); display.animGameWon(team); }
  else                          { display.flashTeam(team, COLOR_POINTS, FEEDBACK_FLASH_MS); }

  publishState(score.state());
  dirty = true;
}

static void onUndo() {
  if (score.undo()) { beep(900, 80); publishState(score.state()); }
  else              { beep(350, 150); }
  dirty = true;
}

static void onReset() {
  score.resetMatch();
  beep(1200, 100); delay(60); beep(1600, 100);
  publishState(score.state());
  dirty = true;
}

static void cycleBrightness() {
  brightIdx = (brightIdx + 1) % 3;
  display.setBrightness(kBrightSteps[brightIdx]);
  beep(1500, 40);
  dirty = true;
}

#if ENABLE_AUTO_BRIGHT
static void autoBrightness() {
  static uint32_t t = 0;
  if (millis() - t < 5000) return;
  t = millis();
  const int lux = analogRead(PIN_LDR);              // 0..4095
  const uint8_t b = lux > 2800 ? BRIGHTNESS_DAY
                  : lux > 1200 ? BRIGHTNESS_SHADE
                               : BRIGHTNESS_NIGHT;
  if (b != display.brightness()) { display.setBrightness(b); dirty = true; }
}
#endif

// ----------------------------------- setup ---------------------------------
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("\n[padel] tanteador MVP arrancando"));
  Serial.printf("[padel] LEDs totales: %d (%d por equipo)\n", NUM_LEDS, LEDS_PER_TEAM);

  beepInit();
  display.begin();
  display.animBoot();

  const MatchRules rules = {
    DEFAULT_GOLDEN_POINT,
    DEFAULT_GAMES_PER_SET,
    DEFAULT_SETS_TO_WIN,
    DEFAULT_TIEBREAK,
    TIEBREAK_TARGET
  };
  score.begin(rules);

  proxA.begin(PIN_TRIG_A, PIN_ECHO_A);
  proxB.begin(PIN_TRIG_B, PIN_ECHO_B);

  rfPointA.begin(PIN_RF_CH1, true);
  rfPointB.begin(PIN_RF_CH2, true);
  rfUndo.begin(PIN_RF_CH3, true);
  rfMode.begin(PIN_RF_CH4, true);
  btnLocal.begin(PIN_BTN_LOCAL, false);

  beep(1400, 80);
  publishState(score.state());
}

// ------------------------------------ loop ---------------------------------
void loop() {
  // --- controles RF + boton local ---
  rfPointA.poll(); rfPointB.poll(); rfUndo.poll(); rfMode.poll(); btnLocal.poll();

  if (rfPointA.pressedShort() || rfPointA.pressedLong()) onPoint(0);
  if (rfPointB.pressedShort() || rfPointB.pressedLong()) onPoint(1);

  if (rfUndo.pressedShort()) onUndo();
  if (rfUndo.pressedLong())  { score.toggleServing(); beep(1000, 60); dirty = true; }

  if (rfMode.pressedShort() || btnLocal.pressedShort()) cycleBrightness();
  if (rfMode.pressedLong()  || btnLocal.pressedLong())  onReset();

  // --- proximidad de paleta (A = izquierda, B = derecha) ---
  if (proxA.poll()) onPoint(0);
  if (proxB.poll()) onPoint(1);

#if ENABLE_AUTO_BRIGHT
  autoBrightness();
#endif

  if (dirty) {
    display.render(score.state(), showServe);
    dirty = false;
  }
}
