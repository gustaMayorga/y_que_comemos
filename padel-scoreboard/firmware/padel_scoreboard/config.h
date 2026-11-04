#pragma once
// ============================================================================
//  Tanteador de Padel - Configuracion central
//  Todo lo que se toca para adaptar el armado esta en este archivo.
// ============================================================================

// ---------------------------------------------------------------------------
// 0) PLACA
//    BOARD_NANO  -> Arduino Nano / Uno / Pro Mini (ATmega328P). Demo actual.
//                   Las tres placas comparten el mismo chip y la misma
//                   numeracion de pines D2..D13 / A0..A5.
//                   OJO con el Pro Mini: hay version 5 V/16 MHz y 3.3 V/8 MHz.
//                   Ver docs/01-hardware-bom.md seccion 1b.
//    BOARD_ESP32 -> ESP32 DevKit v1. Fase 2, habilita WiFi.
//    Cambiar de placa = cambiar esta linea. El resto del firmware se adapta.
// ---------------------------------------------------------------------------
#define BOARD_NANO
//#define BOARD_ESP32

// ---------------------------------------------------------------------------
// 1) PERFIL DE ARMADO
//    LAYOUT_BENCH -> banco de pruebas: 60 LEDs exactos (1 tira de 1 m de 60/m).
//                    Toda la logica se prueba con esto. Dígitos ilegibles pero
//                    el cableado y el firmware son los definitivos.
//    LAYOUT_SPARSE-> 144 LEDs. Pensado para tira de 30 LED/m: con esa densidad
//                    da digitos del MISMO tamano que LAYOUT_FULL usando la mitad
//                    de los LEDs y la mitad de la corriente. Con tira de
//                    60 LED/m da digitos chicos (115 mm).
//    LAYOUT_DEMO  -> demo jugable: 190 LEDs (~3.2 m a 60/m). Digitos de 15 cm.
//    LAYOUT_FULL  -> cartel final: 278 LEDs (~4.7 m a 60/m). Digitos de 23 cm.
//
//    OJO: lo que importa no son los metros sino la CANTIDAD de LEDs y la
//    densidad. 5 m de tira pueden ser 300 LEDs (a 60/m) o 150 (a 30/m).
// ---------------------------------------------------------------------------
//#define LAYOUT_BENCH
//#define LAYOUT_SPARSE
#define LAYOUT_DEMO
//#define LAYOUT_FULL

// ---------------------------------------------------------------------------
// 2) PINES
// ---------------------------------------------------------------------------
#ifdef BOARD_NANO
  // Nano es 5 V: NO hace falta level shifter para la tira
  // ni divisor en los ECHO de los HC-SR04. Cableado mas simple que el ESP32.
  #define PIN_LED_DATA     6
  #define PIN_TRIG_A       2
  #define PIN_ECHO_A       3
  #define PIN_TRIG_B       4
  #define PIN_ECHO_B       5
  #define PIN_RF_CH1       7    // +1 punto Equipo A
  #define PIN_RF_CH2       8    // +1 punto Equipo B
  #define PIN_RF_CH3       9    // corto: deshacer / largo: cambiar saque
  #define PIN_RF_CH4      10    // corto: brillo / 2 s: reset del partido
  #define PIN_BUZZER      11
  #define PIN_BTN_LOCAL   12    // a GND, usa INPUT_PULLUP
  #define PIN_BTN_RESET   A1    // boton dedicado de RESET del panel, a GND
  #define PIN_LDR         A0
  #define ADC_MAX       1023
  // D13 queda libre a proposito (tiene el LED de la placa en paralelo).
#endif

#ifdef BOARD_ESP32
  #define PIN_LED_DATA    13    // usar level shifter 74AHCT125
  #define PIN_TRIG_A      25
  #define PIN_ECHO_A      26    // divisor 1k/2k: el ECHO sale 5 V
  #define PIN_TRIG_B      32
  #define PIN_ECHO_B      33    // divisor 1k/2k
  #define PIN_RF_CH1      14
  #define PIN_RF_CH2      27
  #define PIN_RF_CH3      16
  #define PIN_RF_CH4      17
  #define PIN_BUZZER       4
  #define PIN_BTN_LOCAL   18
  #define PIN_BTN_RESET   19
  #define PIN_LDR         34    // solo entrada
  #define ADC_MAX       4095
#endif

// ---------------------------------------------------------------------------
// 3) GEOMETRIA DEL DISPLAY
//
//  Layout del cartel (equipos uno al lado del otro, indicadores al centro):
//
//   ┌───────────────────────────┬─────────┬───────────────────────────┐
//   │  EQUIPO A                 │ SAQUE   │                 EQUIPO B  │
//   │   ┌────┐┌────┐            │  ◀   ▶  │            ┌────┐┌────┐   │
//   │   │ 4  ││ 0  │  PUNTOS    │         │   PUNTOS   │ 3  ││ 0  │   │
//   │   └────┘└────┘            │ PUNTO   │            └────┘└────┘   │
//   │    [3]    [1]             │  ▲   ▲  │             [2]    [0]    │
//   │   juegos  sets            │         │           juegos  sets    │
//   └───────────────────────────┴─────────┴───────────────────────────┘
//
//  Orden de la guirnalda (hay que respetarlo al soldar):
//    EQUIPO A: puntos decena, puntos unidad, juegos, sets
//    EQUIPO B: puntos decena, puntos unidad, juegos, sets
//    CENTRO:   saque A, saque B, punto A, punto B
//  Dentro de cada digito: segmentos A,B,C,D,E,F,G en ese orden.
// ---------------------------------------------------------------------------
#ifdef LAYOUT_FULL
  #define LEDS_PER_SEG_BIG    6   // PUNTOS   -> segmento 100 mm, digito ~230 mm
  #define LEDS_PER_SEG_SMALL  4   // JUEGOS   -> segmento  67 mm, digito ~150 mm
  #define LEDS_PER_SEG_TINY   3   // SETS     -> segmento  50 mm, digito ~115 mm
  #define LEDS_PER_ICON       3   // flechas y avisos del centro
#endif
#ifdef LAYOUT_SPARSE
  #define LEDS_PER_SEG_BIG    3   // a 30 LED/m: segmento 100 mm, digito ~230 mm
  #define LEDS_PER_SEG_SMALL  2
  #define LEDS_PER_SEG_TINY   2
  #define LEDS_PER_ICON       1
#endif
#ifdef LAYOUT_DEMO
  #define LEDS_PER_SEG_BIG    4   // digito ~155 mm, legible a ~45 m
  #define LEDS_PER_SEG_SMALL  3
  #define LEDS_PER_SEG_TINY   2
  #define LEDS_PER_ICON       2
#endif
#ifdef LAYOUT_BENCH
  #define LEDS_PER_SEG_BIG    1   // 1 LED por segmento: 60 LEDs en total
  #define LEDS_PER_SEG_SMALL  1
  #define LEDS_PER_SEG_TINY   1
  #define LEDS_PER_ICON       1
#endif

#define DIGIT_BIG_LEDS    (LEDS_PER_SEG_BIG   * 7)
#define DIGIT_SMALL_LEDS  (LEDS_PER_SEG_SMALL * 7)
#define DIGIT_TINY_LEDS   (LEDS_PER_SEG_TINY  * 7)

// Por equipo: [puntos decena][puntos unidad][juegos][sets]
#define LEDS_PER_TEAM   (2 * DIGIT_BIG_LEDS + DIGIT_SMALL_LEDS + DIGIT_TINY_LEDS)
// Centro: saque A, saque B, punto A, punto B
#define LEDS_CENTER     (4 * LEDS_PER_ICON)
#define NUM_LEDS        (2 * LEDS_PER_TEAM + LEDS_CENTER)

#define TEAM_A_OFFSET   0
#define TEAM_B_OFFSET   LEDS_PER_TEAM
#define CENTER_OFFSET   (2 * LEDS_PER_TEAM)

// ---------------------------------------------------------------------------
// 3b) TIPO DE TIRA LED
//     WS2812B / WS2812 / WS2811 / WS2813 / SK6812 -> 5 V, un hilo de dato.
//     WS2815 -> 12 V, mismo protocolo, ademas tiene linea de dato de respaldo.
//               Si se usa WS2815 hay que alimentar con 12 V (el dato igual sale
//               del mismo pin) y PSU_VOLTS pasa a 12.
//     APA102 / WS2801 / LPD8806 -> llevan CLOCK aparte: hay que usar la variante
//               de addLeds con CLOCK en display7seg.cpp (esta comentada ahi).
//     El orden de color se descubre con tools/diag_leds (comando 'o').
// ---------------------------------------------------------------------------
#define LED_CHIPSET      WS2812B
#define LED_COLOR_ORDER  GRB

// ---------------------------------------------------------------------------
// 4) BRILLO Y PRESUPUESTO DE CORRIENTE
//    FastLED escala el brillo global solo para no pasarse del limite.
//    Power bank 5 V / 3 A -> 2600 mA con margen.
// ---------------------------------------------------------------------------
#define PSU_VOLTS            5
#define PSU_MILLIAMPS     2600
#define BRIGHTNESS_DAY     255   // sol directo
#define BRIGHTNESS_SHADE   150
#define BRIGHTNESS_NIGHT    60

// Rojo/ambar: maximo contraste bajo sol con filtro acrilico oscuro.
#define COLOR_POINTS      CRGB(255,  0,  0)
#define COLOR_GAMES       CRGB(255, 60,  0)   // ambar
#define COLOR_SETS        CRGB(255,180,  0)   // amarillo
#define COLOR_SERVE       CRGB(  0,255, 60)   // verde
#define COLOR_WIN         CRGB(  0,255,  0)
// Jerarquia del indicador central "estas a un punto de..."
#define COLOR_GAME_POINT  CRGB(  0,180,255)   // cyan   : punto de juego
#define COLOR_SET_POINT   CRGB(255,180,  0)   // amarillo: punto de set
#define COLOR_MATCH_POINT CRGB(255,  0,  0)   // rojo intermitente: punto de partido
#define ICON_BLINK_MS      350                // periodo del titileo del punto de partido

// ---------------------------------------------------------------------------
// 4b) CONTROLES REMOTOS
//     Cuantos botones tienen los llaveros. El mapeo completo esta en el
//     encabezado de padel_scoreboard.ino y en docs/03.
//       3 botones -> B1 +A, B2 +B, B3 deshacer (las funciones extra pasan a
//                    las pulsaciones largas de los mismos 3 botones)
//       4 botones -> igual, y B4 agrega brillo / reset del partido
// ---------------------------------------------------------------------------
#define REMOTE_BUTTONS       3     // 3 o 4
#define BTN_LONG_MS       1000     // pulsacion larga de los controles

// ---------------------------------------------------------------------------
// 4c) FESTEJO DE PARTIDO GANADO
// ---------------------------------------------------------------------------
#define CELEBRA_FUEGOS       3     // rondas de destellos
#define CELEBRA_COMETAS      3     // pasadas del cometa
#define CELEBRA_ARCOIRIS  true     // barrido arcoiris al final
#define PULSO_MIN           50     // brillo minimo del latido del ganador (0-255)
#define RESET_HOLD_MS     2000     // cuanto hay que mantener el boton de reset

// ---------------------------------------------------------------------------
// 5) REGLAS DEL PARTIDO
// ---------------------------------------------------------------------------
#define DEFAULT_GOLDEN_POINT   true   // 40-40 -> punto de oro (true) o ventajas (false)
#define DEFAULT_GAMES_PER_SET     6
#define DEFAULT_SETS_TO_WIN       2   // al mejor de 3 sets
#define DEFAULT_TIEBREAK       true   // 6-6 -> tie break a 7 con 2 de diferencia
#define TIEBREAK_TARGET           7

// ---------------------------------------------------------------------------
// 6) SENSADO POR PROXIMIDAD DE PALETA
//    Equipo A = sensor izquierdo, Equipo B = sensor derecho.
// ---------------------------------------------------------------------------
#define PROX_TRIGGER_CM        20    // distancia que dispara el conteo
#define PROX_REARM_CM          35    // hay que salir mas alla de esto para re-armar
#define PROX_HOLD_MS          250    // la paleta debe sostenerse este tiempo
#define PROX_LOCKOUT_MS      2500    // bloqueo post-conteo (evita dobles)
#define PROX_POLL_MS           60    // periodo de medicion por sensor
#define PROX_MAX_CM           120    // lecturas mayores se descartan

// ---------------------------------------------------------------------------
// 7) VARIOS
// ---------------------------------------------------------------------------
#define BTN_LONG_PRESS_MS    2000    // reset de partido
#define FEEDBACK_FLASH_MS     120
#define ENABLE_BUZZER        true
#define ENABLE_AUTO_BRIGHT   false   // poner true cuando se monte el LDR
#define ENABLE_SERIAL_JSON   true    // publica el estado por Serial en cada cambio

// Profundidad del UNDO. En AVR cada nivel cuesta 28 bytes de RAM.
#ifdef BOARD_NANO
  #define UNDO_DEPTH            8
#else
  #define UNDO_DEPTH           16
#endif

// Futuro (ver docs/04): solo disponible en ESP32.
#define ENABLE_WIFI          false
