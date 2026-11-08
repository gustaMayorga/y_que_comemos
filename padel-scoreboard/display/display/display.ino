// ============================================================================
//  TANTEADOR DE PADEL - VERSION PANEL MATRIZ  (Arduino Nano)
//
//  Panel de 23 x 12 = 276 pixeles WS2812B en serpentina (ALTO 13 = 299).
//  Tanteo completo de padel, 2 sensores IR de paleta, control remoto de 3 o 4
//  botones, y festejos de verdad aprovechando que es una matriz.
//
//  Es AUTONOMO: no depende de ../firmware. La version con WiFi, app, mas
//  deportes y licencia va en ESP32 y se arma aparte.
//
//  ---------------------------------------------------------------------------
//  EL PANEL
//
//     col: 0...4 5...9 |10 11 12| 13..17 18..22
//          +-----+-----+--------+-----+-----+
//     f0   |     |     |   /|   |     |     |
//     ..   |  4  |  0  |  / |   |  3  |  0  |   <- PUNTOS (fuente 5x7)
//     f6   |     |     |   \|   |     |     |
//     f7   [3] [1]      |  []  |      [2] [0]   <- JUEGOS y SETS (fuente 3x5)
//     f11
//
//     centro: arriba  flecha de SAQUE (apunta al equipo que saca)
//             abajo   bloque de AVISO: cyan punto de juego,
//                     amarillo de set, rojo titilando de partido
//
//  ---------------------------------------------------------------------------
//  CABLEADO
//    tira +5V -> bornera 5V de la fuente   (NO del pin del Nano)
//    tira GND -> bornera GND, junto con el GND del Nano  (masa comun)
//    tira DIN -> D6  (470 ohm en serie)
//    capacitor 1000 uF entre 5V y GND a la entrada de la tira
//
//    sensor IR A -> D2        sensor IR B -> D3        (salida digital OUT)
//    receptor RF: D0->D7  D1->D8  D2->D9  D3->D10      antena de 17 cm
//    buzzer D11 | boton brillo D12 a GND | boton reset A1 a GND
//
//  !! Los sensores IR de obstaculo (FC-51 y similares) se enceguecen con el
//     SOL DIRECTO. Para probar y para cancha techada van bien; al aire libre
//     conviene montarlos bajo una visera, o pasar a HC-SR04 ultrasonicos, que
//     son inmunes al sol. Ver docs/01.
//
//  Monitor serie a 115200.
// ============================================================================

#include <FastLED.h>

// ============================ CONFIGURACION =================================
#define ANCHO            23
#define ALTO             12     // 12 = 276 LEDs (12 tiras) | 13 = 299 (5 m justos)
#define SERPENTINA     true
#define FILA0_INVERTIDA false
#define ORIGEN_ARRIBA  true

#define PIN_LED           6
#define PIN_IR_A          2      // sensor de paleta del equipo A (izquierda)
#define PIN_IR_B          3      // sensor de paleta del equipo B (derecha)
#define IR_ACTIVO_BAJO true      // los FC-51 dan LOW al detectar
#define PIN_RF1           7      // +1 punto equipo A
#define PIN_RF2           8      // +1 punto equipo B
#define PIN_RF3           9      // deshacer
#define PIN_RF4          10      // brillo / reset  (solo con 4 botones)
#define BOTONES_CONTROL   3      // 3 o 4
#define PIN_BUZZER       11
#define PIN_BRILLO       12
#define PIN_RESET        A1

#define MILIAMPERES    2600      // tope real de tu fuente
#define BRILLO_INICIAL  120

#define PUNTO_DE_ORO   true      // false = con ventajas
#define SETS_PARA_GANAR   2

// Sensor de paleta: las 3 barreras que evitan contar de mas
#define IR_SOSTENER_MS  250      // hay que mantener la paleta este tiempo
#define IR_BLOQUEO_MS  2500      // despues no cuenta por este rato
#define BTN_LARGO_MS   1000
#define RESET_LARGO_MS 2000

#define COL_PUNTOS   CRGB(255,   0,   0)
#define COL_JUEGOS   CRGB(255,  60,   0)
#define COL_SETS     CRGB(255, 180,   0)
#define COL_SAQUE    CRGB(  0, 255,  60)
#define COL_JUEGO_PT CRGB(  0, 180, 255)
#define COL_SET_PT   CRGB(255, 180,   0)
#define COL_PART_PT  CRGB(255,   0,   0)
// ============================================================================

#define NUM_LEDS (ANCHO * ALTO)
#if F_CPU >= 16000000L
  #define BAUD 115200
#else
  #define BAUD  57600
#endif

// Posiciones en el panel
#define COL_A1  0
#define COL_A2  5
#define COL_CEN 10
#define COL_B1 13
#define COL_B2 18
#define FILA_ALTA 0
// La fila de juegos/sets se apoya abajo: con ALTO 13 queda una fila de
// separacion, con ALTO 12 van pegadas (se distinguen igual por el color).
#define FILA_BAJA (ALTO - 5)

CRGB leds[NUM_LEDS];

// ------------------------------- fuentes ------------------------------------
// 5x7, columna por byte, bit0 = fila de arriba. Indices: 0=' ' 1='-' 2..11='0'..'9'
// 12..37 = 'A'..'Z'
const uint8_t F57[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00, 0x08,0x08,0x08,0x08,0x08,
  0x3E,0x51,0x49,0x45,0x3E, 0x00,0x42,0x7F,0x40,0x00,
  0x42,0x61,0x51,0x49,0x46, 0x21,0x41,0x45,0x4B,0x31,
  0x18,0x14,0x12,0x7F,0x10, 0x27,0x45,0x45,0x45,0x39,
  0x3C,0x4A,0x49,0x49,0x30, 0x01,0x71,0x09,0x05,0x03,
  0x36,0x49,0x49,0x49,0x36, 0x06,0x49,0x49,0x29,0x1E,
  0x7E,0x11,0x11,0x11,0x7E, 0x7F,0x49,0x49,0x49,0x36,
  0x3E,0x41,0x41,0x41,0x22, 0x7F,0x41,0x41,0x22,0x1C,
  0x7F,0x49,0x49,0x49,0x41, 0x7F,0x09,0x09,0x09,0x01,
  0x3E,0x41,0x49,0x49,0x7A, 0x7F,0x08,0x08,0x08,0x7F,
  0x00,0x41,0x7F,0x41,0x00, 0x20,0x40,0x41,0x3F,0x01,
  0x7F,0x08,0x14,0x22,0x41, 0x7F,0x40,0x40,0x40,0x40,
  0x7F,0x02,0x0C,0x02,0x7F, 0x7F,0x04,0x08,0x10,0x7F,
  0x3E,0x41,0x41,0x41,0x3E, 0x7F,0x09,0x09,0x09,0x06,
  0x3E,0x41,0x51,0x21,0x5E, 0x7F,0x09,0x19,0x29,0x46,
  0x46,0x49,0x49,0x49,0x31, 0x01,0x01,0x7F,0x01,0x01,
  0x3F,0x40,0x40,0x40,0x3F, 0x1F,0x20,0x40,0x20,0x1F,
  0x3F,0x40,0x38,0x40,0x3F, 0x63,0x14,0x08,0x14,0x63,
  0x07,0x08,0x70,0x08,0x07, 0x61,0x51,0x49,0x45,0x43,
};
// 3x5 para juegos y sets, digitos 0-9
const uint8_t F35[] PROGMEM = {
  0x1F,0x11,0x1F, 0x00,0x1F,0x00, 0x1D,0x15,0x17, 0x15,0x15,0x1F,
  0x07,0x04,0x1F, 0x17,0x15,0x1D, 0x1F,0x15,0x1D, 0x01,0x01,0x1F,
  0x1F,0x15,0x1F, 0x17,0x15,0x1F,
};

uint8_t idx57(char c) {
  if (c >= 'a' && c <= 'z') c = (char)(c - 32);
  if (c == '-') return 1;
  if (c >= '0' && c <= '9') return (uint8_t)(2 + c - '0');
  if (c >= 'A' && c <= 'Z') return (uint8_t)(12 + c - 'A');
  return 0;
}

// ------------------------------ la grilla -----------------------------------
int16_t XY(int16_t x, int16_t y) {
  if (x < 0 || y < 0 || x >= ANCHO || y >= ALTO) return -1;
  const int16_t fila = ORIGEN_ARRIBA ? y : (ALTO - 1 - y);
  bool rev = FILA0_INVERTIDA;
#if SERPENTINA
  if (fila & 1) rev = !rev;
#endif
  return fila * ANCHO + (rev ? (ANCHO - 1 - x) : x);
}
void px(int16_t x, int16_t y, const CRGB& c) {
  const int16_t i = XY(x, y);
  if (i >= 0) leds[i] = c;
}
void limpiar() { fill_solid(leds, NUM_LEDS, CRGB::Black); }
void mostrar() { FastLED.show(); }

void char57(char c, int16_t x0, int16_t y0, const CRGB& col) {
  const uint8_t b = idx57(c) * 5;
  for (uint8_t cx = 0; cx < 5; cx++) {
    const uint8_t v = pgm_read_byte(&F57[b + cx]);
    for (uint8_t cy = 0; cy < 7; cy++) if (v & (1 << cy)) px(x0 + cx, y0 + cy, col);
  }
}
void texto57(const char* t, int16_t x0, int16_t y0, const CRGB& col) {
  for (int16_t i = 0; t[i]; i++) char57(t[i], x0 + i * 6, y0, col);
}
void digito35(uint8_t d, int16_t x0, int16_t y0, const CRGB& col) {
  const uint8_t b = (d % 10) * 3;
  for (uint8_t cx = 0; cx < 3; cx++) {
    const uint8_t v = pgm_read_byte(&F35[b + cx]);
    for (uint8_t cy = 0; cy < 5; cy++) if (v & (1 << cy)) px(x0 + cx, y0 + cy, col);
  }
}

// ------------------------------- el tanteo ----------------------------------
enum { P0 = 0, P15, P30, P40, PAD };
struct Estado {
  uint8_t pts[2], juegos[2], sets[2];
  uint8_t saque;
  uint8_t ganador;     // 0xFF = sin ganador
  uint8_t tb;          // 1 = estamos en tie break
  uint8_t tbp[2];
};
Estado E;
Estado hist[8];
uint8_t nHist = 0, cabeza = 0;
uint8_t flags;                           // 1 juego, 2 set, 4 partido

void reset() {
  memset(&E, 0, sizeof(E));
  E.ganador = 0xFF;
  nHist = cabeza = flags = 0;
}

void ganaSet(uint8_t t) {
  flags |= 2;
  E.sets[t]++;
  E.juegos[0] = E.juegos[1] = 0;
  E.pts[0] = E.pts[1] = P0;
  E.tbp[0] = E.tbp[1] = 0;
  E.tb = 0;
  if (E.sets[t] >= SETS_PARA_GANAR) { E.ganador = t; flags |= 4; }
}

void ganaJuego(uint8_t t) {
  flags |= 1;
  E.pts[0] = E.pts[1] = P0;
  E.juegos[t]++;
  E.saque ^= 1;
  const uint8_t g = E.juegos[t], o = E.juegos[t ^ 1];
  if ((g >= 6 && g - o >= 2) || (g == 7 && o == 5)) { ganaSet(t); return; }
  if (E.juegos[0] == 6 && E.juegos[1] == 6) { E.tb = 1; E.tbp[0] = E.tbp[1] = 0; }
}

// Aplica un punto sobre un estado cualquiera. Se usa para avanzar el partido y
// tambien para SIMULAR el proximo punto (de ahi salen los avisos).
void aplicar(Estado& s, uint8_t t) {
  if (s.ganador != 0xFF) return;
  Estado guardar = E; uint8_t gf = flags;   // aplicar() trabaja sobre E
  E = s; flags = 0;
  const uint8_t o = t ^ 1;
  if (E.tb) {
    if (E.tbp[t] < 254) E.tbp[t]++;
    if (((E.tbp[0] + E.tbp[1]) % 2) == 1) E.saque ^= 1;
    if (E.tbp[t] >= 7 && E.tbp[t] - E.tbp[o] >= 2) { E.juegos[t]++; flags |= 1; ganaSet(t); }
  } else if (E.pts[t] == PAD)                      ganaJuego(t);
  else if (E.pts[o] == PAD)                        E.pts[o] = P40;
  else if (E.pts[t] == P40 && E.pts[o] == P40) {
    if (PUNTO_DE_ORO) ganaJuego(t); else E.pts[t] = PAD;
  }
  else if (E.pts[t] == P40)                        ganaJuego(t);
  else                                             E.pts[t]++;
  s = E; const uint8_t nf = flags;
  E = guardar; flags = gf;
  flags = nf;
}

void sumarPunto(uint8_t t) {
  if (E.ganador != 0xFF) return;
  hist[cabeza] = E;
  cabeza = (uint8_t)((cabeza + 1) % 8);
  if (nHist < 8) nHist++;
  Estado s = E;
  aplicar(s, t);
  E = s;
}

bool deshacer() {
  if (!nHist) return false;
  cabeza = (uint8_t)((cabeza + 7) % 8);
  E = hist[cabeza];
  nHist--; flags = 0;
  return true;
}

// 0 nada, 1 punto de juego, 2 de set, 3 de partido
uint8_t aviso(uint8_t t) {
  if (E.ganador != 0xFF) return 0;
  Estado s = E; const uint8_t gf = flags;
  aplicar(s, t);
  const uint8_t f = flags; flags = gf;
  if (f & 4) return 3;
  if (f & 2) return 2;
  if (f & 1) return 1;
  return 0;
}

// --------------------------------- dibujo -----------------------------------
void textoPuntos(uint8_t t, char* out) {
  if (E.tb) {                       // tie break: numero crudo
    const uint8_t v = E.tbp[t];
    uint8_t i = 0;
    if (v >= 10) out[i++] = (char)('0' + (v / 10) % 10);
    out[i++] = (char)('0' + v % 10);
    out[i] = 0;
    return;
  }
  switch (E.pts[t]) {
    case P0:  strcpy(out, "0");  break;
    case P15: strcpy(out, "15"); break;
    case P30: strcpy(out, "30"); break;
    case P40: strcpy(out, "40"); break;
    default:  strcpy(out, "AD"); break;
  }
}

void flechaSaque(uint8_t t) {               // 3 cols x 5 filas, filas 1..5
  const int8_t izq[5] = {4, 6, 7, 6, 4};    // mapa de bits por fila (3 bits)
  const int8_t der[5] = {1, 3, 7, 3, 1};
  for (uint8_t f = 0; f < 5; f++) {
    const int8_t m = (t == 0) ? izq[f] : der[f];
    for (uint8_t c = 0; c < 3; c++) if (m & (1 << c)) px(COL_CEN + c, 1 + f, COL_SAQUE);
  }
}

void bloqueAviso(const CRGB& c) {
  const uint8_t m[5] = {2, 7, 7, 7, 2};
  for (uint8_t f = 0; f < 5; f++)
    for (uint8_t x = 0; x < 3; x++) if (m[f] & (1 << x)) px(COL_CEN + x, FILA_BAJA + f, c);
}

void dibujar() {
  limpiar();
  char buf[4];
  const int16_t colTxt[2][2] = { {COL_A1, COL_A2}, {COL_B1, COL_B2} };

  for (uint8_t t = 0; t < 2; t++) {
    if (E.ganador != 0xFF && E.ganador != t) continue;   // perdedor apagado
    textoPuntos(t, buf);
    // alinear a la derecha del bloque del equipo
    const int16_t x = (strlen(buf) == 1) ? colTxt[t][1] : colTxt[t][0];
    texto57(buf, x, FILA_ALTA, COL_PUNTOS);
    if (!E.tb) digito35(E.juegos[t] % 10, colTxt[t][0] + 1, FILA_BAJA, COL_JUEGOS);
    digito35(E.sets[t] % 10, colTxt[t][0] + 6, FILA_BAJA, COL_SETS);
  }

  if (E.ganador == 0xFF) {
    flechaSaque(E.saque);
    const uint8_t a0 = aviso(0), a1 = aviso(1);
    const uint8_t a = (a0 > a1) ? a0 : a1;
    const bool on = ((millis() / 350) & 1) == 0;
    if (a == 1) bloqueAviso(COL_JUEGO_PT);
    else if (a == 2) bloqueAviso(COL_SET_PT);
    else if (a == 3 && on) bloqueAviso(COL_PART_PT);
  }
  mostrar();
}

// ------------------------------- festejos -----------------------------------
CRGB rueda(uint8_t p) {
  if (p < 85)  return CRGB(255 - p * 3, p * 3, 0);
  if (p < 170) { p -= 85; return CRGB(0, 255 - p * 3, p * 3); }
  p -= 170;    return CRGB(p * 3, 0, 255 - p * 3);
}
uint16_t rnd(uint16_t m) {
  static uint16_t s = 0xACE1;
  s ^= (uint16_t)(s << 7); s ^= (uint16_t)(s >> 9); s ^= (uint16_t)(s << 8);
  return m ? (uint16_t)(s % m) : 0;
}
void atenuar(uint8_t f) {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].r = (uint8_t)(((uint16_t)leds[i].r * f) >> 8);
    leds[i].g = (uint8_t)(((uint16_t)leds[i].g * f) >> 8);
    leds[i].b = (uint8_t)(((uint16_t)leds[i].b * f) >> 8);
  }
}

void bip(uint16_t f, uint16_t ms) { tone(PIN_BUZZER, f, ms); delay(ms); }

void flashEquipo(uint8_t t, const CRGB& c) {
  const int16_t x0 = (t == 0) ? 0 : COL_B1;
  for (int16_t y = 0; y < ALTO; y++)
    for (int16_t x = x0; x < x0 + 10; x++) px(x, y, c);
  mostrar(); delay(110);
  dibujar();
}

void olaDeColor(uint8_t vueltas) {
  for (uint16_t t = 0; t < (uint16_t)vueltas * 120; t += 6) {
    for (int16_t y = 0; y < ALTO; y++)
      for (int16_t x = 0; x < ANCHO; x++) px(x, y, rueda((uint8_t)(t + x * 8 + y * 5)));
    mostrar(); delay(18);
  }
}

void fuegos(uint8_t rondas) {
  for (uint8_t r = 0; r < rondas; r++) {
    for (uint8_t k = 0; k < 14; k++) {
      atenuar(120);
      const CRGB c = (k < 7) ? CRGB(255, 255, 180) : CRGB(255, 110, 0);
      for (uint8_t s = 0; s < 12; s++) leds[rnd(NUM_LEDS)] = c;
      mostrar(); delay(45);
    }
  }
}

void correr(const char* t, const CRGB& c, uint8_t vueltas) {
  const int16_t w = (int16_t)strlen(t) * 6;
  for (uint8_t v = 0; v < vueltas; v++)
    for (int16_t off = ANCHO; off > -w; off--) {
      limpiar(); texto57(t, off, (ALTO - 7) / 2, c); mostrar(); delay(55);
    }
}

void festejoPartido(uint8_t t) {
  bip(2400, 300);
  fuegos(3);
  olaDeColor(2);
  correr((t == 0) ? "GANA A" : "GANA B", CRGB(0, 255, 0), 2);
  dibujar();
}

// ------------------------- entradas: IR y botones ---------------------------
struct SensorIR {
  uint8_t pin; uint8_t st; uint32_t t0;
  void begin(uint8_t p) { pin = p; pinMode(p, INPUT); st = 0; t0 = 0; }
  bool detecta() { const bool v = digitalRead(pin); return IR_ACTIVO_BAJO ? !v : v; }
  bool poll() {
    const uint32_t ahora = millis();
    switch (st) {
      case 0: if (detecta()) { st = 1; t0 = ahora; } break;              // armado
      case 1: if (!detecta()) st = 0;                                    // sosteniendo
              else if (ahora - t0 >= IR_SOSTENER_MS) { st = 2; t0 = ahora; return true; }
              break;
      case 2: if (ahora - t0 >= IR_BLOQUEO_MS) st = 3; break;            // bloqueado
      case 3: if (!detecta()) st = 0; break;                             // esperar que se retire
    }
    return false;
  }
};

struct Boton {
  uint8_t pin; bool alto; bool est, crudo; uint32_t tc, td;
  bool evCorto, evLargo, yaLargo; uint16_t largoMs;
  void begin(uint8_t p, bool activoAlto, uint16_t lm) {
    pin = p; alto = activoAlto; largoMs = lm;
    pinMode(p, activoAlto ? INPUT : INPUT_PULLUP);
    est = crudo = evCorto = evLargo = yaLargo = false; tc = td = 0;
  }
  void poll() {
    const uint32_t n = millis();
    const bool v = alto ? (digitalRead(pin) == HIGH) : (digitalRead(pin) == LOW);
    if (v != crudo) { crudo = v; tc = n; return; }
    if (n - tc < 30) return;
    if (v && !est) { est = true; td = n; yaLargo = false; }
    else if (!v && est) { est = false; if (!yaLargo) evCorto = true; }
    else if (v && est && !yaLargo && (n - td >= largoMs)) { yaLargo = true; evLargo = true; }
  }
  bool corto() { const bool e = evCorto; evCorto = false; return e; }
  bool largo() { const bool e = evLargo; evLargo = false; return e; }
  bool sostenido() const { return est; }
  uint32_t desde() const { return est ? (millis() - td) : 0; }
};

SensorIR irA, irB;
Boton rf1, rf2, rf3, bBrillo, bReset;
#if BOTONES_CONTROL >= 4
Boton rf4;
#endif

const uint8_t BRILLOS[3] = { 255, BRILLO_INICIAL, 50 };
uint8_t iBrillo = 1;
bool refrescar = true;
bool barraReset = false;

int ramLibre() {
#if defined(__AVR__)
  extern int __heap_start, *__brkval;
  int v; return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
#else
  return -1;
#endif
}

void punto(uint8_t t) {
  if (E.ganador != 0xFF) { bip(300, 180); return; }
  sumarPunto(t);
  bip(1800, 50);
  dibujar();
  if (flags & 4)      festejoPartido(E.ganador);
  else if (flags & 2) { bip(2200, 200); olaDeColor(1); dibujar(); }
  else if (flags & 1) { bip(2000, 130); flashEquipo(t, COL_JUEGOS); }
  else                flashEquipo(t, COL_PUNTOS);
  Serial.print(F("punto equipo ")); Serial.print(t == 0 ? 'A' : 'B');
  Serial.print(F("  sets ")); Serial.print(E.sets[0]); Serial.print('-'); Serial.print(E.sets[1]);
  Serial.print(F("  juegos ")); Serial.print(E.juegos[0]); Serial.print('-'); Serial.println(E.juegos[1]);
  refrescar = true;
}

void setup() {
  Serial.begin(BAUD);
  pinMode(PIN_BUZZER, OUTPUT);
  FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILIAMPERES);
  FastLED.setBrightness(BRILLOS[iBrillo]);
  limpiar(); mostrar();

  irA.begin(PIN_IR_A); irB.begin(PIN_IR_B);
  rf1.begin(PIN_RF1, true, BTN_LARGO_MS);
  rf2.begin(PIN_RF2, true, BTN_LARGO_MS);
  rf3.begin(PIN_RF3, true, BTN_LARGO_MS);
#if BOTONES_CONTROL >= 4
  rf4.begin(PIN_RF4, true, BTN_LARGO_MS);
#endif
  bBrillo.begin(PIN_BRILLO, false, 2000);
  bReset.begin(PIN_RESET, false, RESET_LARGO_MS);

  reset();
  delay(200);
  Serial.println(F("\n[display] tanteador matriz"));
  Serial.print(F("  panel ")); Serial.print(ANCHO); Serial.print('x');
  Serial.print(ALTO); Serial.print(F(" = ")); Serial.print(NUM_LEDS); Serial.println(F(" px"));
  Serial.print(F("  RAM libre: ")); Serial.print(ramLibre()); Serial.println(F(" bytes"));
  Serial.print(F("  control de ")); Serial.print(BOTONES_CONTROL); Serial.println(F(" botones"));

  correr("PADEL", COL_PUNTOS, 1);
  dibujar();
}

void loop() {
  rf1.poll(); rf2.poll(); rf3.poll(); bBrillo.poll(); bReset.poll();
#if BOTONES_CONTROL >= 4
  rf4.poll();
#endif

  // boton de reset: barra que se va llenando mientras se mantiene
  if (bReset.sostenido()) {
    const uint16_t n = (uint16_t)((bReset.desde() * NUM_LEDS) / RESET_LARGO_MS);
    limpiar();
    for (uint16_t i = 0; i < n && i < NUM_LEDS; i++) leds[i] = CRGB(255, 0, 0);
    mostrar();
    barraReset = true;
    return;
  }
  if (barraReset) { barraReset = false; refrescar = true; }
  if (bReset.largo()) { reset(); bip(1200, 90); bip(1700, 120); refrescar = true; }

  if (rf1.corto()) punto(0);
  if (rf1.largo()) { E.saque = 0; bip(1000, 60); refrescar = true; }
  if (rf2.corto()) punto(1);
  if (rf2.largo()) { E.saque = 1; bip(1000, 60); refrescar = true; }
  if (rf3.corto()) { if (deshacer()) bip(900, 70); else bip(350, 140); refrescar = true; }

  if (rf3.largo() || bBrillo.corto()
#if BOTONES_CONTROL >= 4
      || rf4.corto()
#endif
     ) { iBrillo = (uint8_t)((iBrillo + 1) % 3); FastLED.setBrightness(BRILLOS[iBrillo]);
         bip(1500, 40); refrescar = true; }
#if BOTONES_CONTROL >= 4
  if (rf4.largo()) { reset(); bip(1200, 90); bip(1700, 120); refrescar = true; }
#endif

  if (irA.poll()) punto(0);
  if (irB.poll()) punto(1);

  // redibujar: al cambiar algo, o seguido si el aviso de partido esta titilando
  static uint32_t tDib = 0;
  const bool titila = (E.ganador == 0xFF) && (aviso(0) == 3 || aviso(1) == 3);
  if (refrescar || (titila && millis() - tDib > 150)) {
    dibujar(); refrescar = false; tDib = millis();
  }
}
