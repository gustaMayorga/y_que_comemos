// ============================================================================
//  demo_digito - Show completo sobre UN SOLO digito de 7 segmentos.
//
//  Para exprimir el primer digito armado: contador 0-9, viborita, cometa,
//  arcoiris, prueba de colores uno por uno (la mas util para mirar al sol),
//  rampa de brillo y el titileo de punto de partido.
//
//  Es autonomo: no depende de config.h ni del resto del firmware.
//  Lo unico que hay que ajustar es LEDS_POR_SEGMENTO, abajo.
//
//  CONEXION
//    tira +5V -> 5V de la fuente      (NO del pin del Arduino)
//    tira GND -> GND de la fuente Y del Arduino   (masa comun, obligatorio)
//    tira DIN -> D6                   (470 ohm en serie si lo tenes)
//    capacitor 1000 uF entre 5V y GND a la entrada de la tira
//
//  Monitor serie a 115200 (57600 si la placa es de 8 MHz). Escribi ? para el menu.
// ============================================================================

#include <FastLED.h>

// ----------------------------- A CONFIGURAR --------------------------------
#define LEDS_POR_SEGMENTO   2      // los que pusiste en cada segmento
#define DATA_PIN            6
#define LED_CHIPSET   WS2812B
#define LED_ORDER         GRB      // si pedis rojo y sale verde, probá RGB
#define MAX_LEDS           32      // 7 x LEDS_POR_SEGMENTO, con margen
#define MILIAMPERES      1200      // tope de corriente de tu fuente

// Orden en que soldaste los segmentos. Indices: 0=A 1=B 2=C 3=D 4=E 5=F 6=G
//   vuelta de reloj  : {0,1,2,3,4,5,6}
//   zigzag de lectura: {0,5,1,6,4,2,3}
const uint8_t CADENA[7] = {0, 1, 2, 3, 4, 5, 6};
// ---------------------------------------------------------------------------

#if F_CPU >= 16000000L
  #define SERIAL_BAUD 115200
#else
  #define SERIAL_BAUD  57600
#endif

CRGB leds[MAX_LEDS];
uint8_t porSeg = LEDS_POR_SEGMENTO;
uint8_t posDe[7];                 // segmento logico -> posicion fisica
char linea[16];
uint8_t largo = 0;

// Glifos: bit0=A bit1=B bit2=C bit3=D bit4=E bit5=F bit6=G
const uint8_t DIGITOS[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

struct Color { CRGB c; const char* nombre; };
const Color PALETA[] = {
  { CRGB(255,   0,   0), "ROJO        (el del tanteador)" },
  { CRGB(255,  60,   0), "AMBAR       (los juegos)"       },
  { CRGB(255, 180,   0), "AMARILLO    (los sets)"         },
  { CRGB(  0, 255,   0), "VERDE       (el saque)"         },
  { CRGB(  0, 180, 255), "CYAN        (punto de juego)"   },
  { CRGB(  0,   0, 255), "AZUL"                           },
  { CRGB(255,   0, 255), "MAGENTA"                        },
  { CRGB(255, 255, 255), "BLANCO      (consumo maximo)"   },
};
const uint8_t N_COLORES = sizeof(PALETA) / sizeof(PALETA[0]);

uint8_t nLeds() { return (uint8_t)(porSeg * 7); }

// Arcoiris sin CHSV, para no depender de nada mas
CRGB rueda(uint8_t p) {
  if (p < 85)  return CRGB(255 - p * 3, p * 3, 0);
  if (p < 170) { p -= 85;  return CRGB(0, 255 - p * 3, p * 3); }
  p -= 170;    return CRGB(p * 3, 0, 255 - p * 3);
}

void apagar() { fill_solid(leds, MAX_LEDS, CRGB::Black); }
void mostrar() { FastLED.show(); }
bool corta() { if (Serial.available()) { while (Serial.available()) Serial.read(); return true; } return false; }

// Pinta un segmento logico (0=A .. 6=G)
void segmento(uint8_t seg, const CRGB& c) {
  const uint8_t ini = posDe[seg % 7] * porSeg;
  for (uint8_t i = 0; i < porSeg; i++)
    if (ini + i < MAX_LEDS) leds[ini + i] = c;
}

void glifo(uint8_t mascara, const CRGB& c) {
  apagar();
  for (uint8_t s = 0; s < 7; s++) if (mascara & (1 << s)) segmento(s, c);
}

// ------------------------------- las demos ---------------------------------

void contar(bool multicolor) {
  Serial.println(F("contando 0-9..."));
  for (uint8_t d = 0; d < 10; d++) {
    glifo(DIGITOS[d], multicolor ? rueda((uint8_t)(d * 25)) : PALETA[0].c);
    mostrar();
    Serial.print(F("  ")); Serial.println(d);
    delay(800);
    if (corta()) break;
  }
  apagar(); mostrar();
}

void viborita() {
  Serial.println(F("viborita: un LED recorre toda la cadena"));
  for (uint8_t v = 0; v < 3; v++) {
    for (uint8_t i = 0; i < nLeds(); i++) {
      apagar();
      leds[i] = rueda((uint8_t)(i * 255 / nLeds()));
      mostrar();
      delay(70);
      if (corta()) { apagar(); mostrar(); return; }
    }
  }
  apagar(); mostrar();
}

void cometa() {
  Serial.println(F("cometa con estela"));
  for (uint8_t v = 0; v < 4; v++) {
    for (uint8_t i = 0; i < nLeds(); i++) {
      for (uint8_t k = 0; k < nLeds(); k++) {      // desvanecer
        leds[k].r = (uint8_t)((leds[k].r * 3) / 5);
        leds[k].g = (uint8_t)((leds[k].g * 3) / 5);
        leds[k].b = (uint8_t)((leds[k].b * 3) / 5);
      }
      leds[i] = CRGB(255, 0, 0);
      mostrar();
      delay(45);
      if (corta()) { apagar(); mostrar(); return; }
    }
  }
  apagar(); mostrar();
}

void arcoirisPorSegmento() {
  Serial.println(F("arcoiris: un color por segmento, girando"));
  for (uint16_t t = 0; t < 400; t += 4) {
    for (uint8_t s = 0; s < 7; s++) segmento(s, rueda((uint8_t)(t + s * 36)));
    mostrar();
    delay(25);
    if (corta()) break;
  }
  apagar(); mostrar();
}

void arcoirisContinuo() {
  Serial.println(F("arcoiris continuo LED por LED"));
  for (uint16_t t = 0; t < 512; t += 3) {
    for (uint8_t i = 0; i < nLeds(); i++) leds[i] = rueda((uint8_t)(t + i * 8));
    mostrar();
    delay(20);
    if (corta()) break;
  }
  apagar(); mostrar();
}

// La mas util: cada color del tanteador, en un "8", para mirar AL SOL.
void pruebaColores() {
  Serial.println(F("\nPRUEBA DE COLORES - mira cual se lee mejor al sol"));
  for (uint8_t i = 0; i < N_COLORES; i++) {
    glifo(0x7F, PALETA[i].c);          // un 8 completo
    mostrar();
    Serial.print(F("  ")); Serial.println(PALETA[i].nombre);
    delay(3000);
    if (corta()) break;
  }
  apagar(); mostrar();
  Serial.println(F("El rojo suele ganar al sol, sobre todo con filtro rojo oscuro."));
}

void rampaBrillo() {
  const uint8_t pasos[6] = {20, 50, 100, 150, 200, 255};
  Serial.println(F("rampa de brillo en ROJO (medi la corriente si tenes tester)"));
  glifo(0x7F, PALETA[0].c);
  for (uint8_t i = 0; i < 6; i++) {
    FastLED.setBrightness(pasos[i]);
    mostrar();
    Serial.print(F("  brillo ")); Serial.println(pasos[i]);
    delay(2000);
    if (corta()) break;
  }
  FastLED.setBrightness(100);
  apagar(); mostrar();
}

void puntoDePartido() {
  Serial.println(F("asi titila el aviso de PUNTO DE PARTIDO"));
  for (uint8_t i = 0; i < 16; i++) {
    glifo(0x7F, (i % 2) ? CRGB(255, 0, 0) : CRGB(0, 0, 0));
    mostrar();
    delay(350);
    if (corta()) break;
  }
  apagar(); mostrar();
}

void demoAutomatica() {
  Serial.println(F("\n=== DEMO AUTOMATICA (cualquier tecla corta) ==="));
  contar(false);        if (corta()) return;
  contar(true);         if (corta()) return;
  viborita();           if (corta()) return;
  cometa();             if (corta()) return;
  arcoirisPorSegmento();if (corta()) return;
  arcoirisContinuo();   if (corta()) return;
  puntoDePartido();     if (corta()) return;
  pruebaColores();
  Serial.println(F("=== fin de la demo ==="));
}

void menu() {
  Serial.println(F("\n--- demo_digito ---"));
  Serial.println(F("1  contar 0-9 en rojo"));
  Serial.println(F("2  contar 0-9 cambiando de color"));
  Serial.println(F("3  viborita (1 LED recorre la cadena)"));
  Serial.println(F("4  cometa con estela"));
  Serial.println(F("5  arcoiris por segmento"));
  Serial.println(F("6  arcoiris continuo"));
  Serial.println(F("7  PRUEBA DE COLORES uno por uno  <- la de mirar al sol"));
  Serial.println(F("8  prender todo (un 8 completo)"));
  Serial.println(F("9  rampa de brillo"));
  Serial.println(F("0  titileo de punto de partido"));
  Serial.println(F("d  demo automatica (todo seguido)"));
  Serial.println(F("x  apagar"));
  Serial.println(F("s<n>  LEDs por segmento (ej: s2)"));
  Serial.println(F("b<n>  brillo 0-255      (ej: b150)"));
  Serial.println(F("?  este menu"));
  Serial.print(F("[estado] porSegmento=")); Serial.print(porSeg);
  Serial.print(F("  LEDs del digito="));    Serial.println(nLeds());
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  for (uint8_t i = 0; i < 7; i++) posDe[CADENA[i] % 7] = i;
  FastLED.addLeds<LED_CHIPSET, DATA_PIN, LED_ORDER>(leds, MAX_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILIAMPERES);
  FastLED.setBrightness(100);
  apagar(); mostrar();
  delay(300);
  Serial.println(F("\n[demo_digito] listo."));
  menu();
  demoAutomatica();
}

void loop() {
  while (Serial.available()) {
    const char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (largo == 0) continue;
      linea[largo] = 0;
      const long n = (largo > 1) ? atol(linea + 1) : -1;
      switch (linea[0]) {
        case '1': contar(false);          break;
        case '2': contar(true);           break;
        case '3': viborita();             break;
        case '4': cometa();               break;
        case '5': arcoirisPorSegmento();  break;
        case '6': arcoirisContinuo();     break;
        case '7': pruebaColores();        break;
        case '8': glifo(0x7F, PALETA[0].c); mostrar();
                  Serial.println(F("8 completo en rojo")); break;
        case '9': rampaBrillo();          break;
        case '0': puntoDePartido();       break;
        case 'd': demoAutomatica();       break;
        case 'x': apagar(); mostrar(); Serial.println(F("apagado")); break;
        case 's': if (n > 0 && n * 7 <= MAX_LEDS) { porSeg = (uint8_t)n; menu(); } break;
        case 'b': if (n >= 0 && n <= 255) { FastLED.setBrightness((uint8_t)n); mostrar();
                    Serial.print(F("brillo=")); Serial.println(n); } break;
        default:  menu(); break;
      }
      largo = 0;
    } else if (largo < sizeof(linea) - 1) {
      linea[largo++] = c;
    }
  }
}
