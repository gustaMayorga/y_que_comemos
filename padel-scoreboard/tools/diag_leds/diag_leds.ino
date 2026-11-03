// ============================================================================
//  diag_leds - Identifica QUE tira LED tenes y si sirve para el tanteador.
//
//  Responde estas preguntas sin tener que saber nada de antemano:
//    1. ¿Es direccionable? (si enciende un solo LED, SI; si no enciende nada o
//       enciende toda junta, NO)
//    2. ¿Cuantos LEDs tiene? (comando 'w': camina de a uno y los vas contando)
//    3. ¿Que orden de color? (comando 'o': si pedis rojo y sale verde, es RGB)
//    4. ¿Se puede armar un digito de 7 segmentos? (comando 'd': cuenta 0-9)
//    5. ¿Cuanta corriente consume? (comando 't', con un multimetro en serie)
//
//  WS2812B, WS2811, WS2812, WS2813, WS2815 y SK6812 usan EL MISMO protocolo de
//  un hilo a 800 kHz, asi que esta prueba cubre todas. Si tu tira es de esas,
//  anda sin cambiar nada. Si es APA102 / WS2801 / LPD8806 (tienen un cable de
//  CLOCK aparte, 4 hilos), cambia LED_CHIPSET y descomenta la linea con CLOCK.
//
//  CONEXION:
//    tira 5V  -> 5V  (fuente aparte si son mas de ~20 LEDs, NO del pin del Nano)
//    tira GND -> GND del Nano Y de la fuente (masa comun, siempre)
//    tira DIN -> D6 del Nano (con 470 ohm en serie si tenes)
//    tira de 12 V (WS2811 / WS2815): alimentar con 12 V, el dato igual va a D6.
//
//  Monitor serie a 115200. Escribir ? para el menu.
// ============================================================================

#include <FastLED.h>

// A 8 MHz (Pro Mini 3.3 V) el error de reloj a 115200 baudios es ~3.5 % y el
// monitor serie imprime basura. A 16 MHz es 2.1 % y anda bien. Se elige solo.
#if F_CPU >= 16000000L
  #define SERIAL_BAUD 115200
#else
  #define SERIAL_BAUD 57600
#endif

#define DATA_PIN       6
#define CLOCK_PIN      7      // solo para tiras tipo APA102/WS2801 (4 hilos)
#define LED_CHIPSET    WS2812B
#define LED_ORDER      GRB    // el mas comun; el comando 'o' te dice si es otro
#define MAX_LEDS     150      // subilo si tenes mas (cada LED = 3 bytes de RAM)

CRGB leds[MAX_LEDS];
uint16_t count   = MAX_LEDS;  // cuantos LEDs se usan
uint8_t  perSeg  = 2;         // LEDs por segmento para la demo de digito
char     line[16];
uint8_t  lineLen = 0;

// Glifos de 7 segmentos: bit0=A bit1=B bit2=C bit3=D bit4=E bit5=F bit6=G
const uint8_t DIGITS[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

void help() {
  Serial.println(F("\n--- diag_leds ---"));
  Serial.println(F("w       camina 1 LED a la vez (CONTALOS: ese es el total real)"));
  Serial.println(F("o       prueba de orden de color: LED0 rojo, LED1 verde, LED2 azul"));
  Serial.println(F("f       todo ROJO (el color del tanteador: miralo al sol)"));
  Serial.println(F("a       todo BLANCO (ojo: es el consumo maximo)"));
  Serial.println(F("d       demo de un digito de 7 segmentos contando 0-9"));
  Serial.println(F("t       rampa de corriente: brillo 25/50/100/150/200/255"));
  Serial.println(F("x       apagar todo"));
  Serial.println(F("c<n>    cantidad de LEDs a usar     (ej: c60)"));
  Serial.println(F("s<n>    LEDs por segmento del digito (ej: s3)"));
  Serial.println(F("b<n>    brillo 0-255                 (ej: b80)"));
  Serial.println(F("?       este menu"));
  Serial.print(F("[estado] count=")); Serial.print(count);
  Serial.print(F("  perSeg="));       Serial.print(perSeg);
  Serial.print(F("  MAX_LEDS="));     Serial.println(MAX_LEDS);
}

void off() { fill_solid(leds, MAX_LEDS, CRGB::Black); FastLED.show(); }

void walk() {
  Serial.println(F("caminando de a 1 LED. Conta cuantos se encienden en total."));
  Serial.println(F("Si NO se enciende ninguno -> la tira NO es direccionable (o falta masa comun)."));
  Serial.println(F("Si se encienden TODOS juntos -> NO es direccionable."));
  for (uint16_t i = 0; i < count; i++) {
    off();
    leds[i] = CRGB::White;
    FastLED.show();
    Serial.print(F("LED ")); Serial.println(i);
    delay(250);
    if (Serial.available()) { while (Serial.available()) Serial.read(); break; }
  }
  off();
  Serial.println(F("fin del barrido."));
}

void orderTest() {
  off();
  if (count > 0) leds[0] = CRGB(255, 0, 0);   // pedido: ROJO
  if (count > 1) leds[1] = CRGB(0, 255, 0);   // pedido: VERDE
  if (count > 2) leds[2] = CRGB(0, 0, 255);   // pedido: AZUL
  FastLED.show();
  Serial.println(F("Pedi: LED0=ROJO  LED1=VERDE  LED2=AZUL"));
  Serial.println(F("Que ves realmente?"));
  Serial.println(F("  rojo-verde-azul  -> LED_ORDER = GRB  (lo que ya esta puesto: no cambies nada)"));
  Serial.println(F("  verde-rojo-azul  -> LED_ORDER = RGB"));
  Serial.println(F("  rojo-azul-verde  -> LED_ORDER = GBR"));
  Serial.println(F("  azul-verde-rojo  -> LED_ORDER = BRG"));
  Serial.println(F("Cambia LED_ORDER arriba de este sketch y en config.h del firmware."));
}

void digitDemo() {
  const uint16_t need = (uint16_t)perSeg * 7;
  if (need > count) {
    Serial.print(F("hacen falta ")); Serial.print(need);
    Serial.println(F(" LEDs para un digito. Baja perSeg con s<n> o subi count con c<n>."));
    return;
  }
  Serial.println(F("contando 0-9 en UN digito. Orden de segmentos: A,B,C,D,E,F,G"));
  Serial.println(F("        AAAA"));
  Serial.println(F("       F    B      Asi hay que pegar los tramos de tira:"));
  Serial.println(F("        GGGG       primero A, despues B, C, D, E, F y G."));
  Serial.println(F("       E    C"));
  Serial.println(F("        DDDD"));
  for (uint8_t d = 0; d < 10; d++) {
    off();
    for (uint8_t s = 0; s < 7; s++) {
      if (DIGITS[d] & (1 << s)) {
        for (uint8_t i = 0; i < perSeg; i++) leds[(uint16_t)s * perSeg + i] = CRGB(255, 0, 0);
      }
    }
    FastLED.show();
    Serial.print(F("digito ")); Serial.println(d);
    delay(800);
    if (Serial.available()) { while (Serial.available()) Serial.read(); break; }
  }
  off();
}

void currentRamp() {
  const uint8_t steps[6] = {25, 50, 100, 150, 200, 255};
  Serial.println(F("rampa de brillo en BLANCO. Medi la corriente con el multimetro en serie."));
  Serial.println(F("Anota el valor de 255: es el peor caso de tu tira."));
  for (uint8_t i = 0; i < 6; i++) {
    FastLED.setBrightness(steps[i]);
    fill_solid(leds, count, CRGB::White);
    FastLED.show();
    Serial.print(F("brillo ")); Serial.print(steps[i]);
    Serial.print(F("  (")); Serial.print(count); Serial.println(F(" LEDs en blanco)"));
    delay(2500);
    if (Serial.available()) { while (Serial.available()) Serial.read(); break; }
  }
  FastLED.setBrightness(60);
  off();
  Serial.println(F("fin. Brillo devuelto a 60."));
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  FastLED.addLeds<LED_CHIPSET, DATA_PIN, LED_ORDER>(leds, MAX_LEDS);
  // Para tiras con CLOCK (APA102 / WS2801 / LPD8806), usar esta linea en su lugar:
  // FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, LED_ORDER>(leds, MAX_LEDS);
  FastLED.setBrightness(60);         // arrancamos bajo: no encandila ni pide mucha corriente
  off();
  delay(300);
  Serial.println(F("\n[diag_leds] listo."));
  help();
}

void loop() {
  while (Serial.available()) {
    const char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (lineLen == 0) continue;
      line[lineLen] = 0;
      const long n = (lineLen > 1) ? atol(line + 1) : -1;
      switch (line[0]) {
        case 'w': walk();        break;
        case 'o': orderTest();   break;
        case 'd': digitDemo();   break;
        case 't': currentRamp(); break;
        case 'x': off(); Serial.println(F("apagado")); break;
        case 'f': fill_solid(leds, count, CRGB(255,0,0)); FastLED.show();
                  Serial.println(F("todo rojo")); break;
        case 'a': fill_solid(leds, count, CRGB::White);  FastLED.show();
                  Serial.println(F("todo blanco (consumo maximo)")); break;
        case 'c': if (n > 0 && n <= MAX_LEDS) { count = (uint16_t)n;
                    Serial.print(F("count=")); Serial.println(count); }
                  else Serial.println(F("fuera de rango (subi MAX_LEDS y recompila)"));
                  break;
        case 's': if (n > 0 && n < 32) { perSeg = (uint8_t)n;
                    Serial.print(F("perSeg=")); Serial.println(perSeg); }
                  break;
        case 'b': if (n >= 0 && n <= 255) { FastLED.setBrightness((uint8_t)n); FastLED.show();
                    Serial.print(F("brillo=")); Serial.println(n); }
                  break;
        default:  help(); break;
      }
      lineLen = 0;
    } else if (lineLen < sizeof(line) - 1) {
      line[lineLen++] = c;
    }
  }
}
