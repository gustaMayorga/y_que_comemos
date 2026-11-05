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
//  CONTROLES REMOTOS (se elige 3 o 4 botones con REMOTE_BUTTONS en config.h)
//
//    Boton 1 (CH1)  corto -> +1 punto Equipo A     largo -> saca el Equipo A
//    Boton 2 (CH2)  corto -> +1 punto Equipo B     largo -> saca el Equipo B
//    Boton 3 (CH3)  corto -> DESHACER (repetible)  largo -> ciclar brillo
//    Boton 4 (CH4)  corto -> ciclar brillo         largo -> reset del partido
//                   (solo si REMOTE_BUTTONS == 4)
//
//  El boton 3 es el CTRL+Z: cada pulsacion retrocede un punto, sin importar de
//  que equipo fue, hasta UNDO_DEPTH pasos. Restaura el estado completo, asi que
//  tambien deshace un juego, un set o el partido si ese punto los habia cerrado.
//
//  EN LA CAJA:
//    boton local  -> ciclar brillo
//    boton RESET  -> mantener 2 s. Mientras se mantiene, una barra roja se va
//                    llenando en el panel; al llenarse, resetea el partido.
// ============================================================================

#include <FastLED.h>
#include "config.h"
#include "padel_score.h"
#include "display7seg.h"
#include "inputs.h"
#include "deportes.h"
#include "almacen.h"
#include "licencia.h"
#include "net_stub.h"

// A 8 MHz (Pro Mini 3.3 V) el error de reloj a 115200 baudios es ~3.5 % y el
// monitor serie imprime basura. A 16 MHz es 2.1 % y anda bien. Se elige solo.
#if F_CPU >= 16000000L
  #define SERIAL_BAUD 115200
#else
  #define SERIAL_BAUD 57600
#endif

ScoreEngine      score;
Display7Seg      display;
ProximityCounter proxA, proxB;
Button           rfPointA, rfPointB, rfUndo, btnLocal, btnReset;
#if REMOTE_BUTTONS >= 4
Button           rfMode;
#endif

static const uint8_t kBrightSteps[3] = { BRIGHTNESS_DAY, BRIGHTNESS_SHADE, BRIGHTNESS_NIGHT };
static uint8_t   brightIdx = 0;
static bool      showServe = true;
static bool      dirty     = true;
static uint32_t  tBlink    = 0;
static bool      mostrandoReset = false;
static uint8_t   deporte   = DEPORTE_INICIAL;
static bool      bloqueado = false;
static uint32_t  tBloqueo  = 0;
static bool      faseBloq  = false;

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

// ------------------------- deporte / licencia / uso -------------------------
static void aplicarDeporte(uint8_t d, bool mostrarLetra) {
  deporte = d % DEP_COUNT;
  score.begin(REGLAS[deporte]);
  display.setModo(esContador(deporte), REGLAS[deporte].setsToWin < 90);
  Serial.print(F("[padel] deporte: ")); Serial.println(deporteNombre(deporte));
  if (mostrarLetra) {
    const uint8_t g[4] = { DEP_GLIFO[deporte], Glyph::BLANK,
                           DEP_GLIFO[deporte], Glyph::BLANK };
    display.mostrarCuatro(g, COLOR_SETS);
    delay(1500);
  }
  dirty = true;
}

static void ciclarDeporte() {
  aplicarDeporte((uint8_t)((deporte + 1) % DEP_COUNT), true);
  Almacen::datos().deporte = deporte;
  Almacen::guardarYa();
  beep(1600, 80);
}

// Horas de uso acumuladas, en los 4 digitos grandes.
static void mostrarHorasDeUso() {
  const uint32_t h = Almacen::minutosUso() / 60UL;
  const uint8_t g[4] = {
    (uint8_t)(h >= 1000 ? Glyph::DIGITS[(h / 1000) % 10] : Glyph::BLANK),
    (uint8_t)(h >= 100  ? Glyph::DIGITS[(h / 100)  % 10] : Glyph::BLANK),
    (uint8_t)(h >= 10   ? Glyph::DIGITS[(h / 10)   % 10] : Glyph::BLANK),
    Glyph::DIGITS[h % 10]
  };
  display.mostrarCuatro(g, COLOR_GAMES);
  Serial.print(F("[padel] uso: ")); Serial.print(h); Serial.println(F(" h"));
  delay(2500);
  dirty = true;
}

// Pantalla de licencia vencida. Se refresca sola para que titile.
static void pantallaBloqueo() {
  if (millis() - tBloqueo < 600) return;
  tBloqueo = millis();
  faseBloq = !faseBloq;
  if (!faseBloq) {
    const uint8_t apagado[4] = { Glyph::BLANK, Glyph::BLANK, Glyph::BLANK, Glyph::BLANK };
    display.mostrarCuatro(apagado, COLOR_POINTS);
    return;
  }
#if LIC_PANTALLA == 0
  // Visible: "Lc 00". El cliente entiende que tiene que llamar, no que se rompio.
  const uint8_t g[4] = { Glyph::L, Glyph::C_LOW, Glyph::DIGITS[0], Glyph::DIGITS[0] };
#else
  const uint8_t g[4] = { Glyph::BLANK, Glyph::DASH, Glyph::DASH, Glyph::BLANK };
#endif
  display.mostrarCuatro(g, COLOR_POINTS);
}

static void onDesbloqueo() {
  Almacen::extenderLicencia((uint32_t)LIC_EXTENSION_HORAS * 60UL);
  bloqueado = false;
  beep(1200, 120); delay(60); beep(1800, 120); delay(60); beep(2400, 200);
  const uint8_t g[4] = { Glyph::BLANK, Glyph::O_LOW, Glyph::DIGITS[0], Glyph::DIGITS[0] };
  display.mostrarCuatro(g, COLOR_WIN);
  Serial.print(F("[padel] licencia extendida. Desbloqueos: "));
  Serial.println(Almacen::datos().desbloqueos);
  delay(1800);
  dirty = true;
}

// ----------------------------------- setup ---------------------------------
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(200);
  Serial.println(F("[padel] tanteador MVP"));
  Serial.print(F("[padel] LEDs: "));       Serial.print(NUM_LEDS);
  Serial.print(F(" (por equipo: "));       Serial.print(LEDS_PER_TEAM);
  Serial.print(F(", centro: "));           Serial.print(LEDS_CENTER);
  Serial.println(F(")"));
  Serial.print(F("[padel] controles de "));
  Serial.print(REMOTE_BUTTONS); Serial.println(F(" botones"));

  beepInit();
  Almacen::begin();
  Licencia::begin();
  display.begin();
  display.animBoot();

  aplicarDeporte(Almacen::datos().deporte, true);

  if (Almacen::licenciaActiva()) {
    Serial.print(F("[padel] licencia: quedan "));
    Serial.print(Almacen::minutosRestantes() / 60UL);
    Serial.println(F(" h de uso"));
  }
  Serial.print(F("[padel] uso acumulado: "));
  Serial.print(Almacen::minutosUso() / 60UL); Serial.println(F(" h"));

  proxA.begin(PIN_TRIG_A, PIN_ECHO_A);
  proxB.begin(PIN_TRIG_B, PIN_ECHO_B);

  rfPointA.begin(PIN_RF_CH1, true, BTN_LONG_MS);
  rfPointB.begin(PIN_RF_CH2, true, BTN_LONG_MS);
  rfUndo.begin(PIN_RF_CH3,   true, BTN_LONG_MS);
#if REMOTE_BUTTONS >= 4
  rfMode.begin(PIN_RF_CH4,   true, BTN_LONG_MS);
#endif
  btnLocal.begin(PIN_BTN_LOCAL, false);
  btnReset.begin(PIN_BTN_RESET, false, RESET_HOLD_MS);

  beep(1400, 80);
  draw();
  publish();
}

// ------------------------------------ loop ---------------------------------
void loop() {
  Almacen::tick();

  // --- controles RF + botones de la caja ---
  rfPointA.poll(); rfPointB.poll(); rfUndo.poll();
  btnLocal.poll(); btnReset.poll();
#if REMOTE_BUTTONS >= 4
  rfMode.poll();
#endif

  // --- detector del combo de licencia: se alimenta con CADA soltada ---
  {
    uint16_t ms = 0;
    if (rfPointA.released(&ms)) Licencia::evento(1, ms);
    if (rfPointB.released(&ms)) Licencia::evento(2, ms);
    if (rfUndo.released(&ms))   Licencia::evento(3, ms);
#if REMOTE_BUTTONS >= 4
    if (rfMode.released(&ms))   Licencia::evento(4, ms);
#endif
  }
  if (Licencia::desbloqueoListo()) onDesbloqueo();

  // --- licencia vencida: el cartel no cuenta hasta que se desbloquee ---
  bloqueado = Almacen::licenciaVencida();
  if (bloqueado) {
    // consumir los eventos para que no se disparen despues del desbloqueo
    rfPointA.pressedShort(); rfPointA.pressedLong();
    rfPointB.pressedShort(); rfPointB.pressedLong();
    rfUndo.pressedShort();   rfUndo.pressedLong();
    btnLocal.pressedShort(); btnLocal.pressedLong();
    btnReset.pressedShort(); btnReset.pressedLong();
#if REMOTE_BUTTONS >= 4
    rfMode.pressedShort();   rfMode.pressedLong();
#endif
    pantallaBloqueo();
    return;
  }

  // Boton 1 y 2: corto suma punto, largo asigna el saque a ese equipo
  if (rfPointA.pressedShort()) onPoint(0);
  if (rfPointA.pressedLong())  { score.setServing(0); beep(1000, 60); dirty = true; }
  if (rfPointB.pressedShort()) onPoint(1);
  if (rfPointB.pressedLong())  { score.setServing(1); beep(1000, 60); dirty = true; }

  // Boton 3: CTRL+Z. Corto deshace un paso (se puede repetir), largo cambia brillo
  if (rfUndo.pressedShort()) onUndo();
  if (rfUndo.pressedLong())  cycleBrightness();

#if REMOTE_BUTTONS >= 4
  // Boton 4: la funcion extra del control de 4 botones
  if (rfMode.pressedShort()) cycleBrightness();
  if (rfMode.pressedLong())  onReset();
#endif

  if (btnLocal.pressedShort()) cycleBrightness();
  if (btnLocal.pressedLong())  ciclarDeporte();      // mantener 2 s: cambia de deporte
  if (btnReset.pressedShort()) mostrarHorasDeUso();  // pulso corto: horas de uso

  // --- boton dedicado de RESET, con barra de progreso mientras se mantiene ---
  if (btnReset.isHeld()) {
    const uint32_t held = btnReset.heldMs();
    uint8_t pct = (uint8_t)((held * 100UL) / RESET_HOLD_MS);
    if (pct > 100) pct = 100;
    display.drawResetProgress(pct);
    mostrandoReset = true;
  } else if (mostrandoReset) {
    mostrandoReset = false;        // se solto antes de tiempo: volver a dibujar
    dirty = true;
  }
  if (btnReset.pressedLong()) { mostrandoReset = false; onReset(); }

  // --- proximidad de paleta (A = izquierda, B = derecha) ---
  if (proxA.poll()) onPoint(0);
  if (proxB.poll()) onPoint(1);

#if ENABLE_AUTO_BRIGHT
  autoBrightness();
#endif

  // --- refresco ---
  if (mostrandoReset) return;             // la barra de reset tiene prioridad

  if (dirty) { draw(); dirty = false; tBlink = millis(); }
  else if (display.isCelebrating() && (millis() - tBlink) >= 25) {
    display.refresh();                    // latido del ganador
    tBlink = millis();
  }
  else if (display.isBlinking() && (millis() - tBlink) >= (ICON_BLINK_MS / 2)) {
    display.refresh();                    // titileo del aviso de punto de partido
    tBlink = millis();
  }
}
