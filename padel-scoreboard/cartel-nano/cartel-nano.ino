// ============================================================================
//  TANTEADOR MULTIDEPORTE  -  Panel LED 23 x 12  -  Arduino Nano
//  Version simple / entrenamiento.  Ver README.md y ESQUEMATICO.md
//
//  8 deportes con sus reglas reales, control remoto de 3 o 4 botones y
//  2 sensores de proximidad de paleta (izquierda = A, derecha = B).
// ============================================================================

#include <FastLED.h>
#if defined(__AVR__)
  #include <avr/pgmspace.h>
#endif

/* ========================= CONFIGURACION ========================= */
#define ANCHO            23
#define ALTO             12
#define SERPENTINA     true
#define FILA0_INVERTIDA false
#define ORIGEN_ARRIBA  true

#define PIN_LED           6
// --- SENSORES DE PALETA -----------------------------------------------------
//  1 = TCRT5000 crudo  (analogico, hay que armarle el circuito, alcance 5-15 mm)
//  2 = modulo IR de obstaculo FC-51 (salida digital, alcance 2-30 cm)
//  3 = HC-SR04 ultrasonico (TRIG+ECHO, alcance 2-400 cm, inmune al sol)
#define SENSOR_TIPO       1
#define PIN_SENS_A       A2      // TCRT/FC-51: la salida. HC-SR04: el TRIG
#define PIN_SENS_B       A3
#define PIN_ECHO_A        3      // solo con HC-SR04
#define PIN_ECHO_B        5
#define SENS_ACTIVO_BAJO true    // solo con FC-51
#define TCRT_UMBRAL      90      // cuanto tiene que cambiar la lectura (0-1023)
#define PROX_CM          20      // solo con HC-SR04
#define PIN_RF1           7      // +1 punto A
#define PIN_RF2           8      // +1 punto B
#define PIN_RF3           9      // deshacer / brillo
#define PIN_RF4          10      // brillo / reset   (solo con 4 botones)
#define BOTONES_CONTROL   3      // 3 o 4
#define PIN_BUZZER       11
#define PIN_DEPORTE      12      // boton: corto = cambia deporte
#define PIN_RESET        A1      // boton: mantener 2 s = reset

#define MILIAMPERES    2600
#define BRILLO_DIA      200
#define DEPORTE_INICIAL   0      // ver la tabla de abajo

#define PUNTO_SAQUE_ANCHO 2      // tamano del indicador de saque, en pixeles
#define SENS_SOSTENER_MS 250
#define SENS_BLOQUEO_MS 2500
#define BTN_LARGO_MS    1000
#define RESET_LARGO_MS  2000
#define UNDO_PASOS        6

#define C_PUNTOS   CRGB(255,  0,  0)
#define C_JUEGOS   CRGB(255, 60,  0)
#define C_SETS     CRGB(255,180,  0)
#define C_SAQUE    CRGB(  0,255, 60)
#define C_TB       CRGB(  0,180,255)
#define C_GANA     CRGB(  0,255,  0)

#define NUM_LEDS (ANCHO * ALTO)
#if F_CPU >= 16000000L
  #define BAUD 115200
#else
  #define BAUD  57600
#endif

/* ========================= DEPORTES ========================= */
// modo: 0 = raqueta (15/30/40)  1 = contador con objetivo  2 = contador libre
// saque: 0 = cambia al ganar el juego  1 = side-out (lo gana el que anota)
//        2 = cada 2 puntos (ping pong)
struct Reglas { uint8_t modo, oro, juegos, setsGanar, tbObj, objSet, objFinal, saque; };
const Reglas TABLA[] PROGMEM = {
/* 0 PADEL oro  */ {0, 1, 6, 2, 7,  0,  0, 0},
/* 1 PADEL vent */ {0, 0, 6, 2, 7,  0,  0, 0},
/* 2 TENIS      */ {0, 0, 6, 2, 7,  0,  0, 0},
/* 3 TENIS 5    */ {0, 0, 6, 3, 7,  0,  0, 0},
/* 4 VOLEY      */ {1, 0, 0, 3, 0, 25, 15, 1},
/* 5 PING PONG  */ {1, 0, 0, 3, 0, 11,  0, 2},
/* 6 BASQUET    */ {2, 0, 0, 0, 0,  0,  0, 1},
/* 7 FUTBOL     */ {2, 0, 0, 0, 0,  0,  0, 1},
};
#define N_DEPORTES 8
// glifo de 5x7 que se muestra al cambiar de modo
const uint8_t DEP_LETRA[N_DEPORTES] PROGMEM = {14,14,31,31,34,24,13,17}; // P P t t U M b F
uint8_t deporte = DEPORTE_INICIAL;
Reglas R;
void cargarReglas(){ memcpy_P(&R, &TABLA[deporte], sizeof(R)); }

/* ========================= FUENTES ========================= */
// 5x7: 0=' ' 1='-' 2..11='0'..'9' 12..37='A'..'Z'   (bit0 = fila de arriba)
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

CRGB leds[NUM_LEDS];

int16_t XY(int16_t x, int16_t y){
  if(x < 0 || y < 0 || x >= ANCHO || y >= ALTO) return -1;
  const int16_t f = ORIGEN_ARRIBA ? y : (ALTO - 1 - y);
  bool r = FILA0_INVERTIDA;
#if SERPENTINA
  if(f & 1) r = !r;
#endif
  return f * ANCHO + (r ? (ANCHO - 1 - x) : x);
}
void pix(int16_t x, int16_t y, const CRGB& c){ const int16_t i = XY(x,y); if(i>=0) leds[i]=c; }
void limpiar(){ FastLED.clear(); }
void mostrar(){ FastLED.show(); }

uint8_t idx57(char c){
  if(c>='a'&&c<='z') c-=32;
  if(c=='-') return 1;
  if(c>='0'&&c<='9') return 2+c-'0';
  if(c>='A'&&c<='Z') return 12+c-'A';
  return 0;
}
void glifo57(uint8_t g, int16_t x0, int16_t y0, const CRGB& c){
  const uint16_t b = (uint16_t)g*5;
  for(uint8_t i=0;i<5;i++){ const uint8_t v = pgm_read_byte(&F57[b+i]);
    for(uint8_t j=0;j<7;j++) if(v&(1<<j)) pix(x0+i,y0+j,c); }
}
void chr57(char c, int16_t x, int16_t y, const CRGB& col){ glifo57(idx57(c),x,y,col); }
void txt57(const char* t, int16_t x0, int16_t y0, const CRGB& c){
  for(int16_t i=0;t[i];i++) chr57(t[i], x0+i*6, y0, c);
}
void dig35(uint8_t d, int16_t x0, int16_t y0, const CRGB& c){
  const uint16_t b = (uint16_t)(d%10)*3;
  for(uint8_t i=0;i<3;i++){ const uint8_t v = pgm_read_byte(&F35[b+i]);
    for(uint8_t j=0;j<5;j++) if(v&(1<<j)) pix(x0+i,y0+j,c); }
}
CRGB rueda(uint8_t p){
  if(p<85)  return CRGB(255-p*3,p*3,0);
  if(p<170){p-=85; return CRGB(0,255-p*3,p*3);}
  p-=170;   return CRGB(p*3,0,255-p*3);
}
void corre(const __FlashStringHelper* f, const CRGB& c, uint8_t vueltas){
  char t[14]; strncpy_P(t,(const char*)f,sizeof(t)-1); t[sizeof(t)-1]=0;
  const int16_t w=(int16_t)strlen(t)*6;
  for(uint8_t v=0;v<vueltas;v++)
    for(int16_t o=ANCHO;o>-w;o--){ limpiar(); txt57(t,o,(ALTO-7)/2,c); mostrar(); delay(42); }
}

/* ========================= TANTEO ========================= */
// pts: 0..4 = 0/15/30/40/AD   cnt: contador numerico   tb: en tie break
struct Estado { uint8_t pts[2], cnt[2], jue[2], sets[2], saque, gan, tb; };
Estado E;
Estado hist[UNDO_PASOS];
uint8_t nHist=0, cab=0, flags=0;   // flags: 1 juego, 2 set, 4 partido

void resetPartido(){
  memset(&E,0,sizeof(E)); E.gan=0xFF; nHist=cab=flags=0;
}
uint8_t objetivoSet(){
  if(R.objFinal && R.setsGanar && (E.sets[0]+E.sets[1])==(uint8_t)(2*R.setsGanar-2))
    return R.objFinal;
  return R.objSet;
}
void ganaSet(uint8_t t){
  flags|=2; E.sets[t]++;
  E.jue[0]=E.jue[1]=0; E.pts[0]=E.pts[1]=0; E.cnt[0]=E.cnt[1]=0; E.tb=0;
  if(R.setsGanar && E.sets[t]>=R.setsGanar){ E.gan=t; flags|=4; }
}
void ganaJuego(uint8_t t){
  flags|=1; E.pts[0]=E.pts[1]=0; E.jue[t]++; E.saque^=1;   // el saque pasa al rival
  const uint8_t g=E.jue[t], o=E.jue[t^1];
  // 6 con 2 de diferencia, o 7-5 si se llego a 5-5
  if((g>=R.juegos && g-o>=2) || (g==(uint8_t)(R.juegos+1) && o==(uint8_t)(R.juegos-1))){
    ganaSet(t); return; }
  if(R.tbObj && E.jue[0]==R.juegos && E.jue[1]==R.juegos){ E.tb=1; E.cnt[0]=E.cnt[1]=0; }
}
void sumar(uint8_t t){
  if(E.gan!=0xFF) return;
  const uint8_t o=t^1;

  if(R.modo==0 && !E.tb){                    // --- 15 / 30 / 40 / ventaja ---
    if(E.pts[t]==4)                 ganaJuego(t);            // tenia ventaja
    else if(E.pts[o]==4)            E.pts[o]=3;              // vuelve a iguales
    else if(E.pts[t]==3&&E.pts[o]==3){ if(R.oro) ganaJuego(t); else E.pts[t]=4; }
    else if(E.pts[t]==3)            ganaJuego(t);
    else                            E.pts[t]++;
    return;
  }
  // --- contador numerico: tie break, voley, ping pong, libre ---
  if(E.cnt[t]<250) E.cnt[t]++;
  const uint8_t a=E.cnt[t], b=E.cnt[o], tot=(uint8_t)(a+b);

  if(E.tb){                                   // tie break de raqueta
    if(tot==1 || (tot%2)==1) E.saque^=1;      // 1 saque y despues de a 2
    if(a>=R.tbObj && a-b>=2){ E.jue[t]++; flags|=1; ganaSet(t); }
    return;
  }
  if(R.saque==1)      E.saque=t;              // side-out: saca quien anota
  else if(R.saque==2){                        // ping pong: cada 2, y cada 1 desde 10-10
    const uint8_t lim=R.objSet?R.objSet-1:10;
    if((a>=lim&&b>=lim) || (tot%2)==0) E.saque^=1;
  }
  const uint8_t obj=objetivoSet();
  if(obj && a>=obj && a-b>=2) ganaSet(t);
}
void punto(uint8_t t){
  if(E.gan!=0xFF) return;
  hist[cab]=E; cab=(uint8_t)((cab+1)%UNDO_PASOS); if(nHist<UNDO_PASOS) nHist++;
  flags=0; sumar(t);
}
bool deshacer(){
  if(!nHist) return false;
  cab=(uint8_t)((cab+UNDO_PASOS-1)%UNDO_PASOS); E=hist[cab]; nHist--; flags=0; return true;
}
// 0 nada, 1 punto de juego, 2 de set, 3 de partido
uint8_t aviso(uint8_t t){
  if(E.gan!=0xFF) return 0;
  const Estado g=E; const uint8_t gf=flags;
  flags=0; sumar(t);
  const uint8_t f=flags; E=g; flags=gf;
  return (f&4)?3:(f&2)?2:(f&1)?1:0;
}

/* ========================= DIBUJO ========================= */
void etiqueta(uint8_t t, char* o){
  if(R.modo==0 && !E.tb){
    switch(E.pts[t]){
      case 0: o[0]='0'; o[1]=0; return;
      case 1: o[0]='1'; o[1]='5'; o[2]=0; return;
      case 2: o[0]='3'; o[1]='0'; o[2]=0; return;
      case 3: o[0]='4'; o[1]='0'; o[2]=0; return;
      default:o[0]='A'; o[1]='d'; o[2]=0; return;
    }
  }
  // Solo las dos ultimas cifras: un bloque de equipo son 11 columnas y tres
  // digitos de 5x7 necesitan 17. La centena se dibuja aparte, en el digito
  // chico de abajo, igual que en los marcadores de basquet.
  const uint8_t v=E.cnt[t]%100; uint8_t i=0;
  if(v>=10) o[i++]=(char)('0'+(v/10)%10);
  o[i++]=(char)('0'+v%10); o[i]=0;
}
// ---------------------------------------------------------------------------
//  LA GRILLA, para que nada se pise. 23 columnas con fuente 5x7 y 1 de
//  separacion entre digitos dan 11 + 1 + 11, asi que al centro queda UNA sola
//  columna. Por eso el indicador de saque NO va al centro: va al lado de cada
//  equipo, que ademas es la convencion de los marcadores profesionales.
//
//   col:  0 1 2 3 4 . 6 7 8 9 10 | 11 | 12..16 . 18..22
//   f0   [ PUNTOS  ] [ PUNTOS  ]   |    [ PUNTOS ] [ PUNTOS ]
//   ..                             A         (A = aviso de punto)
//   f7   [JUE] [SET] ●             |    [JUE] [SET] ●
//   f11                                               ^ saque
// ---------------------------------------------------------------------------
#define BASE_A        0
#define BASE_B       (ANCHO - 11)
#define COL_AVISO    11
#define OFF_JUEGOS    0
#define OFF_SETS      4
#define OFF_SAQUE     8

void puntoSaque(int16_t base){
  for(uint8_t f=0; f<PUNTO_SAQUE_ANCHO; f++)
    for(uint8_t c=0; c<PUNTO_SAQUE_ANCHO; c++)
      pix(base + OFF_SAQUE + c, ALTO - 1 - PUNTO_SAQUE_ANCHO + f, C_SAQUE);
}
void dibujar(){
  limpiar();
  const int16_t fBaja = ALTO - 5;
  char s[4];
  for(uint8_t t=0;t<2;t++){
    if(E.gan!=0xFF && E.gan!=t) continue;          // el perdedor queda apagado
    const int16_t base = t ? BASE_B : BASE_A;
    etiqueta(t,s);
    const uint8_t n = strlen(s);
    // 1 cifra -> al lugar de las unidades. 3 cifras -> entra justo en los 11
    txt57(s, base + (n==1 ? 6 : 0), 0, C_PUNTOS);

    if(E.tb){                                      // marca de TIE BREAK
      for(uint8_t k=0;k<3;k++) pix(base+OFF_JUEGOS+k, fBaja+2, C_TB);
    } else if(R.modo==0){
      dig35(E.jue[t], base+OFF_JUEGOS, fBaja, C_JUEGOS);
    } else if(E.cnt[t]>=100){                      // centena del contador
      dig35(E.cnt[t]/100, base+OFF_JUEGOS, fBaja, C_JUEGOS);
    }
    if(R.setsGanar) dig35(E.sets[t], base+OFF_SETS, fBaja, C_SETS);
    if(E.gan==0xFF && E.saque==t) puntoSaque(base);
  }
  // Aviso de punto: barra vertical en la unica columna libre del centro.
  if(E.gan==0xFF){
    const uint8_t a0=aviso(0), a1=aviso(1), a = a0>a1 ? a0 : a1;
    if(a){
      const bool on = ((millis()/350)&1)==0;
      const CRGB c = a==1 ? C_TB : a==2 ? C_SETS : (on ? C_PUNTOS : (CRGB)CRGB::Black);
      for(uint8_t f=3; f<=7; f++) pix(COL_AVISO, f, c);
    }
  }
  mostrar();
}

/* ========================= SONIDO Y FESTEJOS ========================= */
void bip(uint16_t f, uint16_t ms){ tone(PIN_BUZZER,f,ms); delay(ms); }
void flash(uint8_t t, const CRGB& c){
  const int16_t x0 = t?ANCHO-11:0;
  for(int16_t y=0;y<ALTO;y++) for(int16_t x=x0;x<x0+11;x++) pix(x,y,c);
  mostrar(); delay(110); dibujar();
}
void ola(uint8_t vueltas){
  for(uint16_t t=0;t<(uint16_t)vueltas*110;t+=6){
    for(uint8_t y=0;y<ALTO;y++) for(uint8_t x=0;x<ANCHO;x++)
      pix(x,y,rueda((uint8_t)(t+x*8+y*5)));
    mostrar(); delay(18);
  }
}
void festejo(uint8_t t){
  bip(2400,300);
  for(uint8_t r=0;r<3;r++){
    for(uint8_t k=0;k<12;k++){
      fadeToBlackBy(leds,NUM_LEDS,120);
      for(uint8_t s=0;s<10;s++) leds[random16(NUM_LEDS)] = k<6?CRGB(255,255,180):CRGB(255,110,0);
      mostrar(); delay(45);
    }
  }
  ola(2);
  corre(t?F("GANA B"):F("GANA A"), C_GANA, 2);
  dibujar();
}

/* ========================= ENTRADAS ========================= */
// ---------------------------------------------------------------------------
//  SENSOR DE PALETA
//
//  Con TCRT5000 la lectura es ANALOGICA y se AUTOCALIBRA: mientras no hay nada
//  cerca, el sensor va promediando su propio nivel de reposo. Dispara cuando la
//  lectura se aparta de ese reposo mas de TCRT_UMBRAL. Asi se adapta solo a la
//  luz ambiente que cambia durante el dia, que es lo que hace fallar a los
//  sensores infrarrojos con umbral fijo.
//
//  Encima va el antirrebote de 3 barreras: sostener, bloquear, y exigir que la
//  paleta se retire antes de volver a contar.
// ---------------------------------------------------------------------------
struct Sensor {
  uint8_t pin, eco, st; uint32_t t0; int16_t reposo;
  void begin(uint8_t p, uint8_t e){
    pin=p; eco=e; st=0; t0=0; reposo=-1;
#if SENSOR_TIPO == 3
    pinMode(pin,OUTPUT); digitalWrite(pin,LOW); pinMode(eco,INPUT);
#else
    pinMode(pin,INPUT);
#endif
  }
  bool ve(){
#if SENSOR_TIPO == 1
    const int16_t v = analogRead(pin);
    if(reposo < 0) reposo = v;                       // primera lectura
    const bool cerca = abs(v - reposo) > TCRT_UMBRAL;
    if(!cerca) reposo += (v - reposo) / 16;          // sigue la luz ambiente
    return cerca;
#elif SENSOR_TIPO == 2
    const bool v = digitalRead(pin);
    return SENS_ACTIVO_BAJO ? !v : v;
#else
    digitalWrite(pin,LOW);  delayMicroseconds(3);
    digitalWrite(pin,HIGH); delayMicroseconds(10);
    digitalWrite(pin,LOW);
    const unsigned long d = pulseIn(eco,HIGH,12000UL);
    return d && (uint16_t)(d*0.01715f) <= PROX_CM;
#endif
  }
  bool poll(){
    const uint32_t n=millis();
    switch(st){
      case 0: if(ve()){st=1;t0=n;} break;
      case 1: if(!ve()) st=0; else if(n-t0>=SENS_SOSTENER_MS){st=2;t0=n;return true;} break;
      case 2: if(n-t0>=SENS_BLOQUEO_MS) st=3; break;
      case 3: if(!ve()) st=0; break;
    }
    return false;
  }
  int16_t crudo(){
#if SENSOR_TIPO == 1
    return analogRead(pin);
#else
    return -1;
#endif
  }
};
struct Boton {
  uint8_t pin; bool alto, est, crudo, evC, evL, yaL; uint32_t tc, td; uint16_t lMs;
  void begin(uint8_t p, bool a, uint16_t l){
    pin=p; alto=a; lMs=l; pinMode(p, a?INPUT:INPUT_PULLUP);
    est=crudo=evC=evL=yaL=false; tc=td=0; }
  void poll(){
    const uint32_t n=millis();
    const bool v = alto ? (digitalRead(pin)==HIGH) : (digitalRead(pin)==LOW);
    if(v!=crudo){ crudo=v; tc=n; return; }
    if(n-tc<30) return;
    if(v&&!est){ est=true; td=n; yaL=false; }
    else if(!v&&est){ est=false; if(!yaL) evC=true; }
    else if(v&&est&&!yaL&&(n-td>=lMs)){ yaL=true; evL=true; }
  }
  bool corto(){ const bool e=evC; evC=false; return e; }
  bool largo(){ const bool e=evL; evL=false; return e; }
  bool sost() const { return est; }
  uint32_t desde() const { return est?(millis()-td):0; }
};
Sensor sA, sB;
Boton rf1, rf2, rf3, bDep, bRst;
#if BOTONES_CONTROL >= 4
Boton rf4;
#endif
const uint8_t BRILLOS[3]={BRILLO_DIA,120,45};
uint8_t iBri=0; bool redib=true, barra=false;

int ramLibre(){
#if defined(__AVR__)
  extern int __heap_start, *__brkval; int v;
  return (int)&v - (__brkval==0?(int)&__heap_start:(int)__brkval);
#else
  return -1;
#endif
}
void mostrarDeporte(){
  limpiar();
  const uint8_t g=pgm_read_byte(&DEP_LETRA[deporte]);
  glifo57(g, 2, (ALTO-7)/2, C_SETS);
  glifo57(g, ANCHO-7, (ALTO-7)/2, C_SETS);
  dig35(deporte, ANCHO/2-1, ALTO-5, C_JUEGOS);
  mostrar(); delay(1300);
}
void cambiarDeporte(){
  deporte=(uint8_t)((deporte+1)%N_DEPORTES);
  cargarReglas(); resetPartido();
  Serial.print(F("[deporte] ")); Serial.println(deporte);
  bip(1600,80); mostrarDeporte(); redib=true;
}
void onPunto(uint8_t t){
  if(E.gan!=0xFF){ bip(300,180); return; }
  punto(t); bip(1800,50); dibujar();
  if(flags&4)      festejo(E.gan);
  else if(flags&2){ bip(2200,200); ola(1); dibujar(); }
  else if(flags&1){ bip(2000,130); flash(t,C_JUEGOS); }
  else              flash(t,C_PUNTOS);
  Serial.print(F("pt ")); Serial.print(t?'B':'A');
  Serial.print(F("  j ")); Serial.print(E.jue[0]); Serial.print('-'); Serial.print(E.jue[1]);
  Serial.print(F("  s ")); Serial.print(E.sets[0]); Serial.print('-'); Serial.println(E.sets[1]);
  redib=true;
}

void setup(){
  Serial.begin(BAUD);
  pinMode(PIN_BUZZER,OUTPUT);
  FastLED.addLeds<WS2812B,PIN_LED,GRB>(leds,NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,MILIAMPERES);
  FastLED.setBrightness(BRILLOS[iBri]);
  limpiar(); mostrar();
  sA.begin(PIN_SENS_A, PIN_ECHO_A); sB.begin(PIN_SENS_B, PIN_ECHO_B);
  rf1.begin(PIN_RF1,true,BTN_LARGO_MS);
  rf2.begin(PIN_RF2,true,BTN_LARGO_MS);
  rf3.begin(PIN_RF3,true,BTN_LARGO_MS);
#if BOTONES_CONTROL >= 4
  rf4.begin(PIN_RF4,true,BTN_LARGO_MS);
#endif
  bDep.begin(PIN_DEPORTE,false,2000);
  bRst.begin(PIN_RESET,false,RESET_LARGO_MS);
  cargarReglas(); resetPartido();
  delay(250);
  Serial.println(F("\n[cartel] tanteador multideporte"));
  Serial.print(F("  panel ")); Serial.print(ANCHO); Serial.print('x'); Serial.print(ALTO);
  Serial.print(F(" = ")); Serial.print(NUM_LEDS); Serial.println(F(" LEDs"));
  Serial.print(F("  RAM libre: ")); Serial.println(ramLibre());
#if SENSOR_TIPO == 1
  Serial.print(F("  sensores TCRT5000 en reposo: A="));
  Serial.print(sA.crudo()); Serial.print(F("  B=")); Serial.println(sB.crudo());
  Serial.println(F("  (acerca la paleta y mira cuanto cambia -> ajusta TCRT_UMBRAL)"));
#endif
  mostrarDeporte();
  dibujar();
}

void loop(){
  rf1.poll(); rf2.poll(); rf3.poll(); bDep.poll(); bRst.poll();
#if BOTONES_CONTROL >= 4
  rf4.poll();
#endif
  if(bRst.sost()){
    const uint16_t n=(uint16_t)((bRst.desde()*NUM_LEDS)/RESET_LARGO_MS);
    limpiar(); for(uint16_t i=0;i<n&&i<NUM_LEDS;i++) leds[i]=CRGB(255,0,0);
    mostrar(); barra=true; return;
  }
  if(barra){ barra=false; redib=true; }
  if(bRst.largo()){ resetPartido(); bip(1200,90); bip(1700,120); redib=true; }

  if(rf1.corto()) onPunto(0);
  if(rf1.largo()){ E.saque=0; bip(1000,60); redib=true; }
  if(rf2.corto()) onPunto(1);
  if(rf2.largo()){ E.saque=1; bip(1000,60); redib=true; }
  if(rf3.corto()){ if(deshacer()) bip(900,70); else bip(350,140); redib=true; }
  if(rf3.largo() || bDep.largo()){
    iBri=(uint8_t)((iBri+1)%3); FastLED.setBrightness(BRILLOS[iBri]); bip(1500,40); redib=true; }
#if BOTONES_CONTROL >= 4
  if(rf4.corto()){ iBri=(uint8_t)((iBri+1)%3); FastLED.setBrightness(BRILLOS[iBri]); bip(1500,40); redib=true; }
  if(rf4.largo()){ resetPartido(); bip(1200,90); bip(1700,120); redib=true; }
#endif
  if(bDep.corto()) cambiarDeporte();

  if(sA.poll()) onPunto(0);
  if(sB.poll()) onPunto(1);

  static uint32_t tD=0;
  const bool titila = (E.gan==0xFF) && (aviso(0)==3 || aviso(1)==3);
  if(redib || (titila && millis()-tD>150)){ dibujar(); redib=false; tD=millis(); }
}
