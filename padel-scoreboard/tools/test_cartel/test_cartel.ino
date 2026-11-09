// ============================================================================
//  test_cartel - PRUEBA DE ACEPTACION del panel 23 x 12
//
//  Se carga, se mira 2 minutos y listo. No hay que apretar nada.
//  Si las 6 fases pasan, el cartel esta terminado y podes cargar el firmware.
//
//   1  ESQUINAS     5 s   verifica orientacion y serpentina
//   2  RECORRIDO   14 s   LED por LED en orden: no puede saltear ninguno
//   3  COLORES     14 s   los 6 colores + blanco: detecta LEDs con un canal muerto
//   4  PARTIDO     45 s   un partido de padel completo, acelerado
//   5  LUCES       40 s   laser, anillos, fuego, estrobo, cometa, arcoiris
//   6  VEREDICTO    4 s
//                 ~2 min, y vuelve a empezar
//
//  CONEXION
//    tira +5V -> fuente (NO el pin de la placa)   tira GND -> GND comun
//    tira DIN -> D6 con 470 ohm                   cap 1000 uF en la entrada
//
//  Monitor serie a 115200: narra cada fase y que hay que mirar.
// ============================================================================

#include <FastLED.h>

#define ANCHO            23
#define ALTO             12
#define SERPENTINA     true
#define FILA0_INVERTIDA false
#define ORIGEN_ARRIBA  true
#define PIN_LED           6
#define MILIAMPERES    2600
#define BRILLO          110

#define NUM_LEDS (ANCHO * ALTO)
#if F_CPU >= 16000000L
  #define BAUD 115200
#else
  #define BAUD  57600
#endif

CRGB leds[NUM_LEDS];

// fuente 5x7: ' ' - 0..9 A..Z   (bit0 = fila de arriba)
const uint8_t F57[] PROGMEM = {
 0,0,0,0,0, 8,8,8,8,8,
 0x3E,0x51,0x49,0x45,0x3E, 0,0x42,0x7F,0x40,0, 0x42,0x61,0x51,0x49,0x46,
 0x21,0x41,0x45,0x4B,0x31, 0x18,0x14,0x12,0x7F,0x10, 0x27,0x45,0x45,0x45,0x39,
 0x3C,0x4A,0x49,0x49,0x30, 1,0x71,9,5,3, 0x36,0x49,0x49,0x49,0x36, 6,0x49,0x49,0x29,0x1E,
 0x7E,0x11,0x11,0x11,0x7E, 0x7F,0x49,0x49,0x49,0x36, 0x3E,0x41,0x41,0x41,0x22,
 0x7F,0x41,0x41,0x22,0x1C, 0x7F,0x49,0x49,0x49,0x41, 0x7F,9,9,9,1,
 0x3E,0x41,0x49,0x49,0x7A, 0x7F,8,8,8,0x7F, 0,0x41,0x7F,0x41,0,
 0x20,0x40,0x41,0x3F,1, 0x7F,8,0x14,0x22,0x41, 0x7F,0x40,0x40,0x40,0x40,
 0x7F,2,0x0C,2,0x7F, 0x7F,4,8,0x10,0x7F, 0x3E,0x41,0x41,0x41,0x3E,
 0x7F,9,9,9,6, 0x3E,0x41,0x51,0x21,0x5E, 0x7F,9,0x19,0x29,0x46,
 0x46,0x49,0x49,0x49,0x31, 1,1,0x7F,1,1, 0x3F,0x40,0x40,0x40,0x3F,
 0x1F,0x20,0x40,0x20,0x1F, 0x3F,0x40,0x38,0x40,0x3F, 0x63,0x14,8,0x14,0x63,
 7,8,0x70,8,7, 0x61,0x51,0x49,0x45,0x43 };
const uint8_t F35[] PROGMEM = {0x1F,0x11,0x1F, 0,0x1F,0, 0x1D,0x15,0x17, 0x15,0x15,0x1F,
 7,4,0x1F, 0x17,0x15,0x1D, 0x1F,0x15,0x1D, 1,1,0x1F, 0x1F,0x15,0x1F, 0x17,0x15,0x1F};

int16_t XY(int16_t x, int16_t y){
  if(x < 0 || y < 0 || x >= ANCHO || y >= ALTO) return -1;
  const int16_t f = ORIGEN_ARRIBA ? y : (ALTO - 1 - y);
  bool r = FILA0_INVERTIDA;
#if SERPENTINA
  if(f & 1) r = !r;
#endif
  return f * ANCHO + (r ? (ANCHO - 1 - x) : x);
}
void px(int16_t x, int16_t y, const CRGB& c){ const int16_t i = XY(x,y); if(i >= 0) leds[i] = c; }
void limpiar(){ FastLED.clear(); }
void mostrar(){ FastLED.show(); }

uint8_t idx(char c){
  if(c >= 'a' && c <= 'z') c -= 32;
  if(c == '-') return 1;
  if(c >= '0' && c <= '9') return 2 + c - '0';
  if(c >= 'A' && c <= 'Z') return 12 + c - 'A';
  return 0;
}
void chr57(char c, int16_t x0, int16_t y0, const CRGB& col){
  const uint8_t b = idx(c) * 5;
  for(uint8_t i = 0; i < 5; i++){
    const uint8_t v = pgm_read_byte(&F57[b+i]);
    for(uint8_t j = 0; j < 7; j++) if(v & (1<<j)) px(x0+i, y0+j, col);
  }
}
void txt57(const char* t, int16_t x0, int16_t y0, const CRGB& col){
  for(int16_t i = 0; t[i]; i++) chr57(t[i], x0 + i*6, y0, col);
}
void dig35(uint8_t d, int16_t x0, int16_t y0, const CRGB& col){
  const uint8_t b = (d % 10) * 3;
  for(uint8_t i = 0; i < 3; i++){
    const uint8_t v = pgm_read_byte(&F35[b+i]);
    for(uint8_t j = 0; j < 5; j++) if(v & (1<<j)) px(x0+i, y0+j, col);
  }
}
CRGB rueda(uint8_t p){
  if(p < 85)  return CRGB(255-p*3, p*3, 0);
  if(p < 170){ p -= 85;  return CRGB(0, 255-p*3, p*3); }
  p -= 170;   return CRGB(p*3, 0, 255-p*3);
}
void corre(const __FlashStringHelper* f, const CRGB& c, uint8_t vueltas){
  char t[12];
  strncpy_P(t, (const char*)f, sizeof(t)-1); t[sizeof(t)-1] = 0;
  const int16_t w = (int16_t)strlen(t) * 6;
  for(uint8_t v = 0; v < vueltas; v++)
    for(int16_t o = ANCHO; o > -w; o--){
      limpiar(); txt57(t, o, (ALTO-7)/2, c); mostrar(); delay(45);
    }
}

int ramLibre(){
#if defined(__AVR__)
  extern int __heap_start, *__brkval;
  int v; return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
#else
  return -1;
#endif
}
void fase(uint8_t n, const char* nom, const char* mirar){
  Serial.print(F("\n[")); Serial.print(n); Serial.print(F("/6] "));
  Serial.println(nom);
  Serial.print(F("      mirá: ")); Serial.println(mirar);
}

/* ------------------------- 1. ESQUINAS ------------------------- */
void f1_esquinas(){
  fase(1, "ESQUINAS", "ROJO arr-izq | VERDE arr-der | AZUL ab-izq | AMARILLO ab-der");
  limpiar();
  px(0,0,CRGB(255,0,0));             px(ANCHO-1,0,CRGB(0,255,0));
  px(0,ALTO-1,CRGB(0,80,255));       px(ANCHO-1,ALTO-1,CRGB(255,255,0));
  mostrar(); delay(5000);
}
/* ------------------------- 2. RECORRIDO ------------------------ */
void f2_recorrido(){
  fase(2, "RECORRIDO DE LA CADENA", "serpentea: fila 0 ->, fila 1 <-, fila 2 -> ...");
  for(uint16_t i = 0; i < NUM_LEDS; i++){
    limpiar();
    for(uint16_t k = (i > 14 ? i-14 : 0); k < i; k++) leds[k] = CRGB(0, 18, 34);
    leds[i] = CRGB(255,255,255);
    mostrar(); delay(45);
  }
  limpiar(); mostrar(); delay(300);
}
/* ------------------------- 3. COLORES -------------------------- */
void f3_colores(){
  fase(3, "COLORES", "cada pantalla pareja y sin LEDs apagados ni de otro color");
  const CRGB c[7] = {CRGB(255,0,0), CRGB(0,255,0), CRGB(0,80,255), CRGB(255,180,0),
                     CRGB(0,255,220), CRGB(255,0,230), CRGB(255,255,255)};
  for(uint8_t i = 0; i < 7; i++){
    fill_solid(leds, NUM_LEDS, c[i]); mostrar();
    Serial.print(F("      "));
    switch(i){
      case 0: Serial.println(F("rojo")); break;
      case 1: Serial.println(F("verde")); break;
      case 2: Serial.println(F("azul")); break;
      case 3: Serial.println(F("amarillo")); break;
      case 4: Serial.println(F("cyan")); break;
      case 5: Serial.println(F("magenta")); break;
      default:Serial.println(F("BLANCO (consumo maximo)")); break;
    }
    delay(2000);
  }
  limpiar(); mostrar();
}
/* ------------------------- 4. PARTIDO -------------------------- */
uint8_t pA, pB, jA, jB, sA, sB, saque;

// Escribe la etiqueta del punto sin usar literales: en AVR cada cadena
// constante se copia a RAM, y acá no sobra ni un byte.
void etiq(uint8_t p, char* o){
  switch(p){
    case 0: o[0]='0'; o[1]=0; break;
    case 1: o[0]='1'; o[1]='5'; o[2]=0; break;
    case 2: o[0]='3'; o[1]='0'; o[2]=0; break;
    case 3: o[0]='4'; o[1]='0'; o[2]=0; break;
    default:o[0]='A'; o[1]='d'; o[2]=0; break;
  }
}
void tablero(){
  limpiar();
  char a[3], b[3];
  etiq(pA, a); etiq(pB, b);
  txt57(a, a[1] ? 0 : 6,  0, CRGB(255,0,0));
  txt57(b, b[1] ? 12 : 17, 0, CRGB(255,0,0));
  dig35(jA, 0, 7, CRGB(255,60,0));   dig35(sA, 5, 7, CRGB(255,180,0));
  dig35(jB, 14, 7, CRGB(255,60,0));  dig35(sB, 19, 7, CRGB(255,180,0));
  const uint8_t iz[5] = {4,6,7,6,4}, de[5] = {1,3,7,3,1};
  for(uint8_t f = 0; f < 5; f++)
    for(uint8_t c = 0; c < 3; c++)
      if((saque ? de[f] : iz[f]) & (1<<c)) px(10+c, 1+f, CRGB(0,255,60));
  mostrar();
}
void gana(uint8_t t){
  if(t) jB++; else jA++;
  pA = pB = 0; saque ^= 1;
  if(jA >= 6 && jA-jB >= 2){ sA++; jA = jB = 0; }
  if(jB >= 6 && jB-jA >= 2){ sB++; jA = jB = 0; }
}
void punto(uint8_t t){
  uint8_t &p = t ? pB : pA, &o = t ? pA : pB;
  if(p == 3 && o == 3) gana(t);          // punto de oro
  else if(p == 3)      gana(t);
  else                 p++;
  tablero(); delay(320);
}
void f4_partido(){
  fase(4, "PARTIDO DE PADEL", "0/15/30/40, juegos, sets, saque y el festejo final");
  pA = pB = jA = jB = sA = sB = saque = 0;
  tablero(); delay(700);
  for(uint8_t j = 0; j < 7 && sA < 2; j++){          // A gana rapido
    for(uint8_t k = 0; k < 4 && sA < 2; k++) punto(0);
  }
  for(uint8_t j = 0; j < 4 && sB < 1; j++)
    for(uint8_t k = 0; k < 4; k++) punto(1);
  while(sA < 2){ punto(0); }
  Serial.println(F("      -> partido ganado por A"));
  for(uint8_t k = 0; k < 6; k++){                    // festejo
    fill_solid(leds, NUM_LEDS, k%2 ? CRGB(0,255,0) : CRGB::Black); mostrar(); delay(120);
  }
  corre(F("GANA A"), CRGB(0,255,0), 1);
}
/* ------------------------- 5. LUCES ---------------------------- */
void luzLaser(uint16_t ms){
  const uint32_t t0 = millis(); uint8_t tono = 0;
  while(millis() - t0 < ms){
    fadeToBlackBy(leds, NUM_LEDS, 60);
    const uint8_t x = ((millis()/28) % (ANCHO+6));
    for(uint8_t y = 0; y < ALTO; y++){
      if(x < ANCHO)   leds[XY(x,y)]   += rueda(tono);
      if(x >= 1 && x-1 < ANCHO) leds[XY(x-1,y)] += rueda(tono) % 70;
    }
    if(x == 0) tono += 43;
    mostrar(); delay(12);
  }
}
void luzAnillos(uint16_t ms){
  const uint32_t t0 = millis(); uint8_t r = 0, tono = 0;
  while(millis() - t0 < ms){
    fadeToBlackBy(leds, NUM_LEDS, 45);
    for(uint8_t y = 0; y < ALTO; y++) for(uint8_t x = 0; x < ANCHO; x++){
      const int16_t dx = x - ANCHO/2, dy = (y - ALTO/2) * 2;
      const uint8_t d = sqrt16(dx*dx + dy*dy) * 3;
      if(d > r-3 && d < r+3) leds[XY(x,y)] += rueda(tono + d);
    }
    if(++r > 60){ r = 0; tono += 60; }
    mostrar(); delay(28);
  }
}
// Olas de plasma. Es SIN ESTADO a proposito: el fuego necesitaba un mapa de
// calor de NUM_LEDS bytes (276 de los 2048 del Nano) y no vale la pena en un
// test. El fuego con estado va en el firmware del ESP32.
void luzPlasma(uint16_t ms){
  const uint32_t t0 = millis(); uint8_t t = 0;
  while(millis() - t0 < ms){
    for(uint8_t y = 0; y < ALTO; y++) for(uint8_t x = 0; x < ANCHO; x++){
      const uint8_t v = (uint8_t)((sin8(x*26 + t) >> 1) + (cos8(y*30 - t) >> 1));
      leds[XY(x,y)] = HeatColor(v);
    }
    t += 5; mostrar(); delay(26);
  }
}
void luzEstrobo(uint16_t ms){
  const uint32_t t0 = millis(); uint8_t t = 0;
  while(millis() - t0 < ms){
    fill_solid(leds, NUM_LEDS, rueda(t)); mostrar(); delay(45);
    limpiar(); mostrar(); delay(90);
    t += 37;
  }
}
void luzCometa(uint16_t ms){
  const uint32_t t0 = millis(); uint16_t p = 0;
  while(millis() - t0 < ms){
    fadeToBlackBy(leds, NUM_LEDS, 38);
    leds[p % NUM_LEDS] = rueda(p);
    leds[(p+1) % NUM_LEDS] = rueda(p);
    p += 2; mostrar(); delay(8);
  }
}
void luzArcoiris(uint16_t ms){
  const uint32_t t0 = millis(); uint8_t t = 0;
  while(millis() - t0 < ms){
    for(uint8_t y = 0; y < ALTO; y++) for(uint8_t x = 0; x < ANCHO; x++)
      leds[XY(x,y)] = rueda(t + x*7 + y*5);
    t += 4; mostrar(); delay(22);
  }
}
void f5_luces(){
  fase(5, "SHOW DE LUCES", "fluido, sin parpadeos raros y sin LEDs que queden prendidos");
  Serial.println(F("      laser"));   luzLaser(7000);
  Serial.println(F("      anillos")); luzAnillos(7000);
  Serial.println(F("      plasma"));  luzPlasma(7000);
  Serial.println(F("      cometa"));  luzCometa(6000);
  Serial.println(F("      arcoiris"));luzArcoiris(7000);
  Serial.println(F("      estrobo")); luzEstrobo(4000);
  limpiar(); mostrar();
}
/* ------------------------- 6. VEREDICTO ------------------------ */
void f6_fin(){
  fase(6, "FIN", "si las 5 fases se vieron bien, el cartel esta listo");
  corre(F("OK"), CRGB(0,255,0), 1);
  limpiar(); mostrar(); delay(800);
}

void setup(){
  Serial.begin(BAUD);
  FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILIAMPERES);
  FastLED.setBrightness(BRILLO);
  limpiar(); mostrar();
  delay(400);
  Serial.println(F("\n================================================"));
  Serial.println(F("  TEST DE ACEPTACION DEL CARTEL"));
  Serial.print(F("  panel "));   Serial.print(ANCHO); Serial.print('x');
  Serial.print(ALTO); Serial.print(F(" = ")); Serial.print(NUM_LEDS);
  Serial.println(F(" LEDs"));
  Serial.print(F("  RAM libre: ")); Serial.print(ramLibre());
  Serial.println(F(" bytes (si baja de 300, achicar el panel)"));
  Serial.println(F("  duracion ~2 min, despues vuelve a empezar"));
  Serial.println(F("================================================"));
}

void loop(){
  f1_esquinas();
  f2_recorrido();
  f3_colores();
  f4_partido();
  f5_luces();
  f6_fin();
}
