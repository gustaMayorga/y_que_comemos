// ============================================================================
//  test_segmentos - El firmware mas simple posible para probar UN digito.
//
//  Enciende los segmentos UNO POR UNO en el orden fisico de la cadena, para
//  que anotes que segmento prendio en cada posicion. Con esa tabla despues
//  configuras CADENA_SEGMENTOS en el firmware grande y listo.
//
//  No sabe nada de padel, ni de reglas, ni de equipos. Solo prende LEDs.
//
//  CONEXION
//    tira +5V -> 5V de la fuente   (NO del pin del Arduino)
//    tira GND -> GND de la fuente Y del Arduino  (masa comun, obligatorio)
//    tira DIN -> D6 del Arduino    (con 470 ohm en serie si tenes)
//    capacitor 1000 uF entre 5V y GND a la entrada de la tira
//
//  Monitor serie a 115200 (57600 si la placa es de 8 MHz). Escribi ? para ayuda.
// ============================================================================

#include <FastLED.h>

#define DATA_PIN             6
#define LED_CHIPSET    WS2812B
#define LED_ORDER          GRB
#define MAX_LEDS            64     // alcanza para 2 digitos de 4 LEDs/segmento

#if F_CPU >= 16000000L
  #define SERIAL_BAUD 115200
#else
  #define SERIAL_BAUD  57600
#endif

CRGB leds[MAX_LEDS];

uint8_t porSegmento = 3;    // LEDs que pusiste en cada segmento
uint8_t digito      = 0;    // 0 = primer digito, 1 = segundo, ...
int8_t  actual      = 0;    // posicion de la cadena que se esta mostrando (0-6)
char    linea[16];
uint8_t largo = 0;

// Glifos 7 segmentos: bit0=A bit1=B bit2=C bit3=D bit4=E bit5=F bit6=G
const uint8_t DIGITOS[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
// Orden en que el firmware grande espera los segmentos (vuelta de reloj)
const char* NOMBRE[7] = {"A arriba", "B arriba-der", "C abajo-der", "D abajo",
                         "E abajo-izq", "F arriba-izq", "G el del medio"};

uint16_t baseDigito() { return (uint16_t)digito * porSegmento * 7; }

void apagar() { fill_solid(leds, MAX_LEDS, CRGB::Black); }

void prenderPosicion(int8_t pos) {
  apagar();
  const uint16_t ini = baseDigito() + (uint16_t)pos * porSegmento;
  for (uint8_t i = 0; i < porSegmento; i++)
    if (ini + i < MAX_LEDS) leds[ini + i] = CRGB(255, 0, 0);
  FastLED.show();
  Serial.print(F("\n>>> POSICION "));  Serial.print(pos + 1);
  Serial.print(F(" de la cadena  (LEDs "));  Serial.print(ini);
  Serial.print(F(" a "));  Serial.print(ini + porSegmento - 1);
  Serial.println(F(")"));
  Serial.println(F("    Anota que segmento se prendio en el panel."));
}

void ayuda() {
  Serial.println(F("\n--- test_segmentos ---"));
  Serial.println(F("n      siguiente posicion de la cadena"));
  Serial.println(F("p      posicion anterior"));
  Serial.println(F("1..7   ir directo a esa posicion"));
  Serial.println(F("a      recorrido automatico de las 7"));
  Serial.println(F("8      prender TODAS (tiene que verse un 8 completo)"));
  Serial.println(F("c      contar 0-9 (solo sale bien si CADENA es vuelta de reloj)"));
  Serial.println(F("x      apagar"));
  Serial.println(F("s<n>   LEDs por segmento   (ej: s3)"));
  Serial.println(F("d<n>   que digito probar   (ej: d1 = el segundo)"));
  Serial.println(F("?      esta ayuda"));
  Serial.print(F("[estado] porSegmento=")); Serial.print(porSegmento);
  Serial.print(F("  digito="));             Serial.print(digito);
  Serial.print(F("  LEDs del digito="));    Serial.println(porSegmento * 7);
}

void planilla() {
  Serial.println(F("\n=== PLANILLA PARA ANOTAR ==="));
  Serial.println(F("Prende cada posicion y escribi que segmento es:"));
  Serial.println(F("        AAAA"));
  Serial.println(F("       F    B      posicion 1 -> ____"));
  Serial.println(F("       F    B      posicion 2 -> ____"));
  Serial.println(F("        GGGG       posicion 3 -> ____"));
  Serial.println(F("       E    C      posicion 4 -> ____"));
  Serial.println(F("       E    C      posicion 5 -> ____"));
  Serial.println(F("        DDDD       posicion 6 -> ____"));
  Serial.println(F("                   posicion 7 -> ____"));
  Serial.println(F("Despues copia ese orden a CADENA_SEGMENTOS en config.h:"));
  Serial.println(F("  #define CADENA_SEGMENTOS { SEG_?, SEG_?, SEG_?, SEG_?, SEG_?, SEG_?, SEG_? }"));
}

void automatico() {
  for (uint8_t i = 0; i < 7; i++) {
    prenderPosicion((int8_t)i);
    Serial.print(F("    (si tu cadena es vuelta de reloj, aca deberia ser: "));
    Serial.print(NOMBRE[i]); Serial.println(F(")"));
    delay(2000);
    if (Serial.available()) { while (Serial.available()) Serial.read(); break; }
  }
  apagar(); FastLED.show();
}

void contar() {
  for (uint8_t d = 0; d < 10; d++) {
    apagar();
    for (uint8_t s = 0; s < 7; s++) {
      if (DIGITOS[d] & (1 << s)) {
        const uint16_t ini = baseDigito() + (uint16_t)s * porSegmento;
        for (uint8_t i = 0; i < porSegmento; i++)
          if (ini + i < MAX_LEDS) leds[ini + i] = CRGB(255, 0, 0);
      }
    }
    FastLED.show();
    Serial.print(F("digito ")); Serial.println(d);
    delay(900);
    if (Serial.available()) { while (Serial.available()) Serial.read(); break; }
  }
  apagar(); FastLED.show();
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  FastLED.addLeds<LED_CHIPSET, DATA_PIN, LED_ORDER>(leds, MAX_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
  FastLED.setBrightness(90);
  apagar(); FastLED.show();
  delay(300);
  Serial.println(F("\n[test_segmentos] listo."));
  ayuda();
  planilla();
  prenderPosicion(0);
}

void loop() {
  while (Serial.available()) {
    const char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (largo == 0) continue;
      linea[largo] = 0;
      const long n = (largo > 1) ? atol(linea + 1) : -1;
      switch (linea[0]) {
        case 'n': actual = (int8_t)((actual + 1) % 7);     prenderPosicion(actual); break;
        case 'p': actual = (int8_t)((actual + 6) % 7);     prenderPosicion(actual); break;
        case 'a': automatico(); break;
        case 'c': contar();     break;
        case 'x': apagar(); FastLED.show(); Serial.println(F("apagado")); break;
        case '8': {
          apagar();
          const uint16_t ini = baseDigito();
          for (uint16_t i = 0; i < (uint16_t)porSegmento * 7 && ini + i < MAX_LEDS; i++)
            leds[ini + i] = CRGB(255, 0, 0);
          FastLED.show();
          Serial.println(F("todas prendidas: tiene que verse un 8 completo y parejo"));
          break;
        }
        case 's': if (n > 0 && n <= 8) { porSegmento = (uint8_t)n; ayuda(); prenderPosicion(actual); } break;
        case 'd': if (n >= 0 && n < 4)  { digito = (uint8_t)n;     ayuda(); prenderPosicion(actual); } break;
        default:
          if (linea[0] >= '1' && linea[0] <= '7') {
            actual = (int8_t)(linea[0] - '1');
            prenderPosicion(actual);
          } else { ayuda(); planilla(); }
          break;
      }
      largo = 0;
    } else if (largo < sizeof(linea) - 1) {
      linea[largo++] = c;
    }
  }
}
