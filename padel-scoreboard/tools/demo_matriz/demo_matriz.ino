// ============================================================================
//  demo_matriz - Prueba la idea del PANEL MATRIZ con un Arduino Nano.
//
//  En vez de 7 segmentos, los LEDs van en una grilla y se dibuja cualquier
//  cosa: numeros, letras, nombres de equipo, texto corrido, animaciones.
//
//  SI, ENTRA EN UN NANO. Framebuffer = 3 bytes por pixel:
//      11 x 7  =  77 px ->  231 B   (2 digitos, 1,3 m de tira)
//      16 x 9  = 144 px ->  432 B
//      23 x 13 = 299 px ->  897 B   (el panel completo, 5 m de tira)
//  Con 23x13 quedan ~600 B libres: alcanza para ESTA demo, pero NO para la
//  demo mas el motor de tanteo completo. Esa version final pide ESP32.
//
//  EMPEZA CHICO: con 11 x 7 (1,3 m de tira) ya ves dos digitos, comprobas el
//  mapeo y lo sacas al sol. No armes los 5 metros para probar la idea.
//
//  CABLEADO EN SERPENTINA: las filas alternan de direccion, como arando.
//      fila 0:  --------------->
//      fila 1:  <---------------
//      fila 2:  --------------->
//  Asi el salto entre filas es de 2 cm en vez de cruzar el panel entero.
//
//  Monitor serie a 115200 (57600 si la placa es de 8 MHz). ? para el menu.
// ============================================================================

#include <FastLED.h>

// ----------------------------- A CONFIGURAR --------------------------------
#define ANCHO            11     // columnas
#define ALTO              7     // filas
#define SERPENTINA     true     // false si todas las filas van en el mismo sentido
#define FILA0_INVERTIDA false   // true si la fila 0 arranca por la derecha
#define ORIGEN_ARRIBA  true     // false si el primer LED quedo abajo a la izquierda

#define DATA_PIN          6
#define LED_CHIPSET WS2812B
#define LED_ORDER       GRB
#define MILIAMPERES    1200     // tope de corriente de tu fuente
// ---------------------------------------------------------------------------

#define NUM_LEDS (ANCHO * ALTO)

#if F_CPU >= 16000000L
  #define SERIAL_BAUD 115200
#else
  #define SERIAL_BAUD  57600
#endif

CRGB leds[NUM_LEDS];
CRGB tinta = CRGB(255, 0, 0);
char linea[40];
uint8_t largo = 0;

// --------------------------- fuente 5 x 7 ----------------------------------
// Cada caracter son 5 columnas; en cada byte el bit 0 es la fila de arriba.
const uint8_t FUENTE[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00, // ' '
  0x08,0x08,0x08,0x08,0x08, // '-'
  0x3E,0x51,0x49,0x45,0x3E, // 0
  0x00,0x42,0x7F,0x40,0x00, // 1
  0x42,0x61,0x51,0x49,0x46, // 2
  0x21,0x41,0x45,0x4B,0x31, // 3
  0x18,0x14,0x12,0x7F,0x10, // 4
  0x27,0x45,0x45,0x45,0x39, // 5
  0x3C,0x4A,0x49,0x49,0x30, // 6
  0x01,0x71,0x09,0x05,0x03, // 7
  0x36,0x49,0x49,0x49,0x36, // 8
  0x06,0x49,0x49,0x29,0x1E, // 9
  0x7E,0x11,0x11,0x11,0x7E, // A
  0x7F,0x49,0x49,0x49,0x36, // B
  0x3E,0x41,0x41,0x41,0x22, // C
  0x7F,0x41,0x41,0x22,0x1C, // D
  0x7F,0x49,0x49,0x49,0x41, // E
  0x7F,0x09,0x09,0x09,0x01, // F
  0x3E,0x41,0x49,0x49,0x7A, // G
  0x7F,0x08,0x08,0x08,0x7F, // H
  0x00,0x41,0x7F,0x41,0x00, // I
  0x20,0x40,0x41,0x3F,0x01, // J
  0x7F,0x08,0x14,0x22,0x41, // K
  0x7F,0x40,0x40,0x40,0x40, // L
  0x7F,0x02,0x0C,0x02,0x7F, // M
  0x7F,0x04,0x08,0x10,0x7F, // N
  0x3E,0x41,0x41,0x41,0x3E, // O
  0x7F,0x09,0x09,0x09,0x06, // P
  0x3E,0x41,0x51,0x21,0x5E, // Q
  0x7F,0x09,0x19,0x29,0x46, // R
  0x46,0x49,0x49,0x49,0x31, // S
  0x01,0x01,0x7F,0x01,0x01, // T
  0x3F,0x40,0x40,0x40,0x3F, // U
  0x1F,0x20,0x40,0x20,0x1F, // V
  0x3F,0x40,0x38,0x40,0x3F, // W
  0x63,0x14,0x08,0x14,0x63, // X
  0x07,0x08,0x70,0x08,0x07, // Y
  0x61,0x51,0x49,0x45,0x43, // Z
};

// Devuelve el indice del caracter en la fuente, o 0 (espacio) si no esta.
uint8_t idxChar(char c) {
  if (c >= 'a' && c <= 'z') c = (char)(c - 32);
  if (c == ' ') return 0;
  if (c == '-') return 1;
  if (c >= '0' && c <= '9') return (uint8_t)(2 + (c - '0'));
  if (c >= 'A' && c <= 'Z') return (uint8_t)(12 + (c - 'A'));
  return 0;
}

// ------------------------- mapeo de la grilla ------------------------------
// Convierte (columna, fila) en el numero de LED dentro de la cadena.
int16_t XY(int16_t x, int16_t y) {
  if (x < 0 || y < 0 || x >= ANCHO || y >= ALTO) return -1;
  const int16_t fila = ORIGEN_ARRIBA ? y : (ALTO - 1 - y);
  bool alReves = FILA0_INVERTIDA;
#if SERPENTINA
  if (fila & 1) alReves = !alReves;
#endif
  const int16_t col = alReves ? (ANCHO - 1 - x) : x;
  return fila * ANCHO + col;
}

void punto(int16_t x, int16_t y, const CRGB& c) {
  const int16_t i = XY(x, y);
  if (i >= 0 && i < NUM_LEDS) leds[i] = c;
}

void apagar() { fill_solid(leds, NUM_LEDS, CRGB::Black); }
void mostrar() { FastLED.show(); }
bool corta() { if (Serial.available()) { while (Serial.available()) Serial.read(); return true; } return false; }

// Dibuja un caracter con su esquina superior izquierda en (x0, y0).
void dibujarChar(char c, int16_t x0, int16_t y0, const CRGB& col) {
  const uint8_t base = idxChar(c) * 5;
  for (uint8_t cx = 0; cx < 5; cx++) {
    const uint8_t columna = pgm_read_byte(&FUENTE[base + cx]);
    for (uint8_t cy = 0; cy < 7; cy++)
      if (columna & (1 << cy)) punto(x0 + cx, y0 + cy, col);
  }
}

void dibujarTexto(const char* t, int16_t x0, int16_t y0, const CRGB& col) {
  for (int16_t i = 0; t[i]; i++) dibujarChar(t[i], x0 + i * 6, y0, col);
}

// --------------------------------- demos -----------------------------------

// Enciende LED por LED en el orden FISICO de la cadena: sirve para verificar
// que la serpentina quedo como dice la configuracion.
void mapaFisico() {
  Serial.println(F("orden FISICO de la cadena. Tiene que ir en serpentina:"));
  Serial.println(F("  fila 0 ->   fila 1 <-   fila 2 ->  ..."));
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    apagar();
    leds[i] = CRGB(255, 255, 255);
    mostrar();
    delay(60);
    if (corta()) break;
  }
  apagar(); mostrar();
}

// Marca las 4 esquinas con colores distintos: verifica la ORIENTACION.
void esquinas() {
  apagar();
  punto(0, 0, CRGB(255, 0, 0));                 // arriba izquierda: ROJO
  punto(ANCHO - 1, 0, CRGB(0, 255, 0));         // arriba derecha:   VERDE
  punto(0, ALTO - 1, CRGB(0, 0, 255));          // abajo izquierda:  AZUL
  punto(ANCHO - 1, ALTO - 1, CRGB(255,255,0));  // abajo derecha:    AMARILLO
  mostrar();
  Serial.println(F("esquinas: ROJO arr-izq | VERDE arr-der | AZUL ab-izq | AMARILLO ab-der"));
  Serial.println(F("Si no coinciden, ajusta SERPENTINA / FILA0_INVERTIDA / ORIGEN_ARRIBA."));
}

void contar() {
  Serial.println(F("contando 0-9"));
  for (uint8_t d = 0; d < 10; d++) {
    apagar();
    dibujarChar((char)('0' + d), (ANCHO - 5) / 2, (ALTO - 7) / 2, tinta);
    mostrar();
    Serial.print(F("  ")); Serial.println(d);
    delay(700);
    if (corta()) break;
  }
  apagar(); mostrar();
}

// Tanteo tipo padel: dos numeros con un separador en el medio.
void tanteo(const char* a, const char* b) {
  apagar();
  const int16_t y = (ALTO - 7) / 2;
  dibujarTexto(a, 0, y, tinta);
  const int16_t anchoB = (int16_t)strlen(b) * 6 - 1;
  dibujarTexto(b, ANCHO - anchoB, y, tinta);
  mostrar();
}

void demoTanteo() {
  const char* secuencia[][2] = {
    {"0","0"}, {"15","0"}, {"15","15"}, {"30","15"},
    {"40","15"}, {"40","40"}, {"AD","40"}, {"JU","EG"}
  };
  Serial.println(F("simulando un juego de padel"));
  for (uint8_t i = 0; i < 8; i++) {
    tanteo(secuencia[i][0], secuencia[i][1]);
    Serial.print(F("  ")); Serial.print(secuencia[i][0]);
    Serial.print(F(" - "));Serial.println(secuencia[i][1]);
    delay(1200);
    if (corta()) break;
  }
  apagar(); mostrar();
}

// Texto corrido: lo que un 7 segmentos NO puede hacer.
void correrTexto(const char* t, uint8_t vueltas) {
  const int16_t ancho = (int16_t)strlen(t) * 6;
  Serial.print(F("texto corrido: ")); Serial.println(t);
  for (uint8_t v = 0; v < vueltas; v++) {
    for (int16_t off = ANCHO; off > -ancho; off--) {
      apagar();
      dibujarTexto(t, off, (ALTO - 7) / 2, tinta);
      mostrar();
      delay(70);
      if (corta()) { apagar(); mostrar(); return; }
    }
  }
  apagar(); mostrar();
}

CRGB rueda(uint8_t p) {
  if (p < 85)  return CRGB(255 - p * 3, p * 3, 0);
  if (p < 170) { p -= 85;  return CRGB(0, 255 - p * 3, p * 3); }
  p -= 170;    return CRGB(p * 3, 0, 255 - p * 3);
}

void festejo() {
  Serial.println(F("festejo: olas de color por el panel"));
  for (uint16_t t = 0; t < 300; t += 5) {
    for (int16_t y = 0; y < ALTO; y++)
      for (int16_t x = 0; x < ANCHO; x++)
        punto(x, y, rueda((uint8_t)(t + x * 8 + y * 4)));
    mostrar();
    delay(25);
    if (corta()) break;
  }
  apagar(); mostrar();
}

void menu() {
  Serial.println(F("\n--- demo_matriz ---"));
  Serial.println(F("m        orden fisico de la cadena (verificar serpentina)"));
  Serial.println(F("e        marcar las 4 esquinas (verificar orientacion)"));
  Serial.println(F("n        contar 0-9"));
  Serial.println(F("j        simular un juego de padel"));
  Serial.println(F("v        VENTAJA en texto corrido (lo que el 7 seg no puede)"));
  Serial.println(F("f        festejo con olas de color"));
  Serial.println(F("t<texto> mostrarlo fijo        (ej: tAB)"));
  Serial.println(F("r<texto> mostrarlo corrido     (ej: rPUNTO DE PARTIDO)"));
  Serial.println(F("c<n>     color 0..5 de la tinta"));
  Serial.println(F("b<n>     brillo 0-255"));
  Serial.println(F("x        apagar      |   ?  este menu"));
  Serial.print(F("[estado] ")); Serial.print(ANCHO); Serial.print('x');
  Serial.print(ALTO); Serial.print(F(" = ")); Serial.print(NUM_LEDS);
  Serial.print(F(" px, framebuffer ")); Serial.print(NUM_LEDS * 3);
  Serial.println(F(" bytes"));
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  FastLED.addLeds<LED_CHIPSET, DATA_PIN, LED_ORDER>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILIAMPERES);
  FastLED.setBrightness(90);
  apagar(); mostrar();
  delay(300);
  Serial.println(F("\n[demo_matriz] listo."));
  menu();
  esquinas();
}

void loop() {
  while (Serial.available()) {
    const char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (largo == 0) continue;
      linea[largo] = 0;
      const long n = (largo > 1) ? atol(linea + 1) : -1;
      switch (linea[0]) {
        case 'm': mapaFisico();  break;
        case 'e': esquinas();    break;
        case 'n': contar();      break;
        case 'j': demoTanteo();  break;
        case 'v': correrTexto("VENTAJA", 2);  break;
        case 'f': festejo();     break;
        case 't': apagar(); dibujarTexto(linea + 1, 0, (ALTO - 7) / 2, tinta); mostrar(); break;
        case 'r': correrTexto(linea + 1, 1);  break;
        case 'c': if (n >= 0 && n <= 5) {
                    const CRGB pal[6] = { CRGB(255,0,0), CRGB(255,60,0), CRGB(255,180,0),
                                          CRGB(0,255,0), CRGB(0,180,255), CRGB(255,255,255) };
                    tinta = pal[n];
                    Serial.print(F("color=")); Serial.println(n);
                  } break;
        case 'b': if (n >= 0 && n <= 255) { FastLED.setBrightness((uint8_t)n); mostrar(); } break;
        case 'x': apagar(); mostrar(); Serial.println(F("apagado")); break;
        default:  menu(); break;
      }
      largo = 0;
    } else if (largo < sizeof(linea) - 1) {
      linea[largo++] = c;
    }
  }
}
