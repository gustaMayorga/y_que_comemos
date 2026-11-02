#pragma once
// ============================================================================
//  Tanteador de Padel - Configuracion central
//  Todo lo que se toca para adaptar el armado esta en este archivo.
// ============================================================================

// ---------------------------------------------------------------------------
// 1) PLACA / PINES   (ESP32 DevKit v1 - 30 pines)
// ---------------------------------------------------------------------------
#define PIN_LED_DATA      13   // DIN de la primera tira WS2812B (usar level shifter)

// Sensores de proximidad (HC-SR04). ECHO siempre con divisor 1k/2k a 3.3V.
#define PIN_TRIG_A        25
#define PIN_ECHO_A        26
#define PIN_TRIG_B        32
#define PIN_ECHO_B        33

// Receptor RF 433 MHz de 4 canales (PT2272-M4), salidas momentaneas a 3.3V
#define PIN_RF_CH1        14   // +1 punto Equipo A
#define PIN_RF_CH2        27   // +1 punto Equipo B
#define PIN_RF_CH3        16   // UNDO (deshacer ultimo punto)
#define PIN_RF_CH4        17   // MODO: pulso corto = modo saque/brillo, 2 s = reset partido

#define PIN_BUZZER         4   // buzzer pasivo 5 V via transistor, o activo directo
#define PIN_BTN_LOCAL      18  // boton fisico en la caja (mismas funciones que CH4)
#define PIN_LDR            34  // LDR / BH1750 opcional para brillo automatico (solo entrada)

// ---------------------------------------------------------------------------
// 2) GEOMETRIA DEL DISPLAY
//    Cada digito son 7 segmentos hechos con tira WS2812B cortada.
//    Orden de cableado dentro del digito: A,B,C,D,E,F,G (ver docs/02).
// ---------------------------------------------------------------------------
#define LEDS_PER_SEG_BIG    6   // digitos de PUNTOS  -> segmento ~100 mm (tira 60 LED/m)
#define LEDS_PER_SEG_SMALL  4   // digito de GAMES    -> segmento ~67 mm
#define LEDS_PER_SET_DOT    2   // cada punto de SET
#define SET_DOTS_PER_TEAM   3   // cuantos sets como maximo se muestran

#define DIGIT_BIG_LEDS    (LEDS_PER_SEG_BIG * 7)     // 42
#define DIGIT_SMALL_LEDS  (LEDS_PER_SEG_SMALL * 7)   // 28
#define SET_DOTS_LEDS     (LEDS_PER_SET_DOT * SET_DOTS_PER_TEAM)

// Por equipo: [puntos decena][puntos unidad][games][3 dots de set]
#define LEDS_PER_TEAM     (2 * DIGIT_BIG_LEDS + DIGIT_SMALL_LEDS + SET_DOTS_LEDS)
#define NUM_LEDS          (2 * LEDS_PER_TEAM)

// Orden de la guirnalda: primero Equipo A completo, despues Equipo B.
#define TEAM_A_OFFSET     0
#define TEAM_B_OFFSET     LEDS_PER_TEAM

// ---------------------------------------------------------------------------
// 3) BRILLO Y PRESUPUESTO DE CORRIENTE
//    FastLED escala el brillo solo para no pasarse del limite declarado.
//    Ajustar PSU_MILLIAMPS al real de la fuente (power bank 5V/3A -> 2600).
// ---------------------------------------------------------------------------
#define PSU_VOLTS            5
#define PSU_MILLIAMPS     2600   // margen de seguridad sobre 3 A
#define BRIGHTNESS_DAY     255   // sol directo: todo lo que de la fuente
#define BRIGHTNESS_SHADE   150
#define BRIGHTNESS_NIGHT    60

// Color: rojo saturado = maximo contraste bajo sol con filtro acrilico rojo/humo.
#define COLOR_POINTS      CRGB(255,  0,  0)
#define COLOR_GAMES       CRGB(255, 60,  0)   // ambar, para diferenciar jerarquia
#define COLOR_SETS        CRGB(255,255,  0)
#define COLOR_SERVE       CRGB(  0,255, 60)
#define COLOR_WIN         CRGB(  0,255,  0)

// ---------------------------------------------------------------------------
// 4) REGLAS DEL PARTIDO (configurable en caliente desde el menu / futura app)
// ---------------------------------------------------------------------------
#define DEFAULT_GOLDEN_POINT   true   // 40-40 -> punto de oro (true) o ventajas (false)
#define DEFAULT_GAMES_PER_SET     6
#define DEFAULT_SETS_TO_WIN       2   // al mejor de 3 sets
#define DEFAULT_TIEBREAK       true   // 6-6 -> tie break a 7 con 2 de diferencia
#define TIEBREAK_TARGET           7

// ---------------------------------------------------------------------------
// 5) SENSADO POR PROXIMIDAD DE PALETA
//    Equipo A = sensor izquierdo, Equipo B = sensor derecho (o al reves: ver docs).
// ---------------------------------------------------------------------------
#define PROX_TRIGGER_CM        20    // distancia que dispara el conteo
#define PROX_REARM_CM          35    // hay que salir mas alla de esto para re-armar
#define PROX_HOLD_MS          250    // la paleta debe sostenerse este tiempo
#define PROX_LOCKOUT_MS      2500    // bloqueo post-conteo (evita dobles)
#define PROX_POLL_MS           60    // periodo de medicion por sensor
#define PROX_MAX_CM           120    // lecturas mayores se descartan

// ---------------------------------------------------------------------------
// 6) VARIOS
// ---------------------------------------------------------------------------
#define BTN_LONG_PRESS_MS    2000    // reset de partido
#define UNDO_DEPTH             16    // cuantos puntos se pueden deshacer
                                     // (debe coincidir con hist_[] en padel_score.h)
#define FEEDBACK_FLASH_MS     120
#define ENABLE_BUZZER        true
#define ENABLE_AUTO_BRIGHT   false   // poner true cuando se monte el LDR

// Futuro (ver docs/04): no compila nada de red todavia.
#define ENABLE_WIFI          false
