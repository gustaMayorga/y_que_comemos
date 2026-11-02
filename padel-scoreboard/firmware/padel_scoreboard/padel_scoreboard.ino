// ============================================================================
//  TANTEADOR DE PADEL - MVP
//
//  Cerebro:   Arduino Nano (demo actual)  /  ESP32 DevKit v1 (fase 2, WiFi)
//             -> se elige en config.h con BOARD_NANO / BOARD_ESP32
//  Display:   7 segmentos con tira WS2812B
//               4 digitos GRANDES  -> puntos (2 por equipo)
//               2 digitos chicos   -> juegos (1 por equipo)
//               2 digitos mini     -> sets   (1 por equipo)
//               4 iconos al centro -> flecha de saque A/B + aviso de punto A/B
//  Entradas:  2 sensores de proximidad de paleta + 2 controles RF 433 MHz
//  Energia:   power bank 5 V / USB-C (ver docs/01-hardware-bom.md)
//
//  Controles (receptor RF de 4 canales, 2 llaveros identicos):
//    CH1 (boton A, llavero Equipo A) -> +1 punto Equipo A
//    CH2 (boton B, llavero Equipo B) -> +1 punto Equipo B
//    CH3 corto -> deshacer ultimo punto   |  CH3 largo -> cambiar saque
//    CH4 corto -> ciclar brillo           |  CH4 2 s   -> reset del partido
//  El boton de la caja replica CH4.
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
static uint8_t   brightIdx = 0;
static bool      showServe = true;
static bool      dirty     = true;
static uint32_t  tBlink    = 0;

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

// --------------------------- helpers de presentacion -----------------------
static void currentSituations(Situation out[2]) {
  out[0] = score.situation(0);
  out[1] = score.situation(1);
}

static void draw() {
  Situation sit[2];
  currentSituations(sit);
  display.render(score.state(), sit, showServe);
}

static void publish() {
  Situation sit[2];
  currentSituations(sit);
  publishState(score.state(), sit);
}

// --------------------------- acciones de alto nivel ------------------------
static void onPoint(uint8_t team) {
  if (!score.addPoint(team)) { beep(300, 200); return; }   // partido ya terminado
  beep(1800, 60);
  draw();                                                  // estado nuevo a la vista

  if (score.lastWonMatch())     { beep(2400, 400); display.animMatchWon(score.state().winner); }
  else if (score.lastWonSet())  { beep(2200, 250); display.animSetWon(team); }
  else if (score.lastWonGame()) { beep(2000, 150); display.animGameWon(team); }
  else                          { display.flashTeam(team, COLOR_POINTS, FEEDBACK_FLASH_MS); }

  publish();
  dirty = true;
}

static void onUndo() {
  if (score.undo()) { beep(900, 80); publish(); }
  else              { beep(350, 150); }
  dirty = true;
}

static void onReset() {
  score.resetMatch();
  beep(1200, 100); delay(60); beep(1600, 100);
  publish();
  dirty = true;
}

static void cycleBrightness() {
  brightIdx = (uint8_t)((brightIdx + 1) % 3);
  display.setBrightness(kBrightSteps[brightIdx]);
  beep(1500, 40);
  dirty = true;
}

#if ENABLE_AUTO_BRIGHT
static void autoBrightness() {
  static uint32_t t = 0;
  if (millis() - t < 5000) return;
  t = millis();
  const long lux = analogRead(PIN_LDR);                     // 0..ADC_MAX
  const uint8_t b = lux > (ADC_MAX * 7L / 10) ? BRIGHTNESS_DAY
                  : lux > (ADC_MAX * 3L / 10) ? BRIGHTNESS_SHADE
                                              : BRIGHTNESS_NIGHT;
  if (b != display.brightness()) { display.setBrightness(b); dirty = true; }
}
#endif

// ----------------------------------- setup ---------------------------------
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("[padel] tanteador MVP"));
  Serial.print(F("[padel] LEDs: "));       Serial.print(NUM_LEDS);
  Serial.print(F(" (por equipo: "));       Serial.print(LEDS_PER_TEAM);
  Serial.print(F(", centro: "));           Serial.print(LEDS_CENTER);
  Serial.println(F(")"));

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
  draw();
  publish();
}

// ------------------------------------ loop ---------------------------------
void loop() {
  // --- controles RF + boton de la caja ---
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

  // --- refresco ---
  if (dirty) { draw(); dirty = false; tBlink = millis(); }
  else if (display.isBlinking() && (millis() - tBlink) >= (ICON_BLINK_MS / 2)) {
    display.refresh();                    // titileo del aviso de punto de partido
    tBlink = millis();
  }
}
