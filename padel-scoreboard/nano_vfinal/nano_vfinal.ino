// ============================================================================
//  TANTEADOR MULTIDEPORTE  -  Panel LED 23 x 12  -  Arduino Nano
//  VERSION FINAL de la linea Nano.  Ver README.md y ESQUEMATICO.md
//
//  8 deportes · control remoto 433 MHz con grabacion de llaveros ·
//  4 tipos de sensor de paleta · encendido automatico por presencia
//
//  8 deportes con sus reglas reales, control remoto de 3 o 4 botones y
//  2 sensores de proximidad de paleta (izquierda = A, derecha = B).
// ============================================================================

#include <FastLED.h>
#include <EEPROM.h>
#if defined(__AVR__)
  #include <avr/pgmspace.h>
  #include <avr/wdt.h>
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
//  3 = DOS HC-SR04 ultrasonicos (TRIG+ECHO cada uno). Lo mejor: inmune al sol
//  4 = UN SOLO HC-SR04 con DOS ZONAS de distancia:
//        paleta CERCA  -> equipo A        paleta LEJOS -> equipo B
//      No se puede detectar el sentido del movimiento con un solo sensor
//      (mide distancia, no angulo), pero por zonas funciona perfecto.
// 0 = NINGUNO: para bisecar un reset. Deja solo los botones y el RF.
#define SENSOR_TIPO       4
//
//  OJO: la A y la B son el EQUIPO, no una pata del modulo. Cada HC-SR04 tiene
//  sus propios TRIG y ECHO. Asi queda el cableado segun el tipo:
//
//    tipo            sensor equipo A         sensor equipo B
//    ------------------------------------------------------------
//    1 TCRT5000      salida -> A2            salida -> A3
//    2 FC-51         OUT    -> A2            OUT    -> A3
//    3 dos HC-SR04   TRIG -> A2, ECHO -> D3  TRIG -> A3, ECHO -> D5
//    4 UN HC-SR04    TRIG -> A2, ECHO -> D3  (no se usa)
//
#define PIN_SENS_A       A2      // equipo A: la salida, o el TRIG del HC-SR04
#define PIN_ECHO_A        3      // equipo A: el ECHO del HC-SR04
#define PIN_SENS_B       A3      // equipo B: idem (sin uso en el tipo 4)
#define PIN_ECHO_B        5      // equipo B: el ECHO del HC-SR04
#define SENS_ACTIVO_BAJO true    // solo con FC-51
#define TCRT_UMBRAL      90      // cuanto tiene que cambiar la lectura (0-1023)
#define PROX_CM          20      // tipo 3: distancia que dispara
// tipo 4: las dos zonas. Entre ZONA_A_MAX y ZONA_B_MIN queda una banda muerta
// a proposito, para que no haya dudas en el limite.
#define ZONA_A_MIN        4
#define ZONA_A_MAX       20
#define ZONA_B_MIN       27
#define ZONA_B_MAX       55
// --- CONTROL REMOTO ---------------------------------------------------------
//  Receptor CRUDO de 433 MHz (MX-RM-5V / XY-MK-5V / RXB6): entrega el flujo de
//  bits en bruto por UN pin, y el codigo del llavero se decodifica por software.
//  Los botones se APRENDEN (ver GRABAR en el README).
#define PIN_RF_DATA       2      // en el Nano tiene que ser D2 o D3 (interrupcion)
#define RF_CONTROLES      2      // cuantos llaveros se pueden grabar
#define RF_BOTONES        3      // botones por llavero (siempre 3)
#define GRABAR_HOLD_MS 15000     // mantener el boton DEPORTE para grabar
#define PIN_BUZZER       11      // -1 = sin buzzer (para bisecar un reset)
#define PIN_DEPORTE      12      // corto = deporte | 15 s = grabar controles
#define PIN_RESET        A1      // corto = brillo  | 2 s  = reset del partido
#define PIN_LED_PLACA    13      // el LED de la propia placa, acompana los bips

// --- DETECTOR DE PRESENCIA (opcional) ---------------------------------------
//  RCWL-0516: radar Doppler de microondas. Detecta MOVIMIENTO a 5-9 m y
//  atraviesa plastico y madera, asi que puede ir ADENTRO de la caja sellada.
//  No sirve para contar puntos (no distingue equipos ni mide distancia), pero
//  es perfecto para encender el cartel cuando alguien llega a la cancha y
//  apagarlo cuando no hay nadie.
//  SI NO LO TENES INSTALADO, DEJALO EN -1 (como viene). Con -1 todo el codigo
//  de reposo queda fuera de la compilacion y el cartel no se apaga nunca.
//  Si lo conectas, poné 4 (o el pin que uses).
#define PIN_PRESENCIA    -1
#define DORMIR_MIN       10      // minutos sin movimiento para apagar

// Limite de corriente del panel. IMPORTANTE: tiene que ser ~70% de lo que
// REALMENTE da tu fuente, porque el Nano, el sensor y el RF comen aparte y
// FastLED no los cuenta. Si el cartel se reinicia al prender, bajalo primero.
//   power bank de 2 A -> 1400      cargador de 3 A -> 2000
//   fuente de 5 V 4 A -> 2800      fuente de 5 V 10 A -> 7000
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
// Los bips prenden tambien el LED de la placa, para poder grabar los controles
// mirando el Arduino aunque el panel este tapado.
void vigilarVcc();               // definida mas abajo, junto a leerVcc()

// delay() es ciego: mientras dura no se muestrea Vcc, y justo el bip es el
// momento de mas riesgo. esperar() hace lo mismo pero sigue vigilando.
void esperar(uint16_t ms){
  const uint32_t t0 = millis();
  while(millis() - t0 < ms) vigilarVcc();
}

void bip(uint16_t f, uint16_t ms){
  digitalWrite(PIN_LED_PLACA,HIGH);
#if PIN_BUZZER >= 0
  tone(PIN_BUZZER,f,ms);
#else
  (void)f;
#endif
  esperar(ms);
  digitalWrite(PIN_LED_PLACA,LOW);
}
void flash(uint8_t t, const CRGB& c){
  const int16_t x0 = t?ANCHO-11:0;
  for(int16_t y=0;y<ALTO;y++) for(int16_t x=x0;x<x0+11;x++) pix(x,y,c);
  mostrar(); esperar(110); dibujar();
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

/* ================= CONTROL REMOTO POR RADIO (tipo 2) =================
   Decodificador EV1527 / PT2262 minimo, hecho a mano en vez de usar RCSwitch:
   la libreria reserva un buffer de ~135 bytes y en un Nano con 276 LEDs eso es
   mucho. Este usa unos 30.

   Trama EV1527: un pulso corto en alto, 31 cortos en bajo (sincronismo), y
   despues 24 bits. Cada bit es alto+bajo: si el alto dura mas que el bajo es
   un 1, si no es un 0. El llavero repite la trama varias veces por pulsacion.
   ==================================================================== */
volatile uint32_t rfCodigo = 0;
volatile bool     rfListo  = false;

// Validacion POR PROPORCION, no por tiempos absolutos. No todos los llaveros
// usan la misma relacion entre el hueco de sincronismo y el pulso base (el
// EV1527 usa 31, el PT2262 puede usar 4, 8, 16 o 31), asi que derivar el pulso
// dividiendo por 31 rechazaba tramas buenas. Lo que SI es universal:
//   - un bit es 3:1 (un "1") o 1:3 (un "0") entre su parte alta y su parte baja
//   - todos los bits de una trama duran mas o menos lo mismo
#define RF_DEBUG false        // true = imprime cada trama decodificada

void rfISR(){
  static uint32_t ult = 0, dato = 0;
  static uint16_t alto = 0, base = 0;
  static uint8_t  bits = 0, sinc = 0;
  const uint32_t n = micros();
  const uint16_t d = (uint16_t)(n - ult);
  ult = n;

  if(digitalRead(PIN_RF_DATA)){            // flanco de subida: termino un BAJO
    if(d > 2500 && d < 40000){             // hueco de sincronismo
      if(sinc && bits == 24 && !rfListo){ rfCodigo = dato; rfListo = true; }
      bits = 0; dato = 0; base = 0; sinc = 1;
      return;
    }
    if(!sinc) return;
    const bool uno  = alto > (uint16_t)(d * 2);
    const bool cero = d > (uint16_t)(alto * 2);
    if(!uno && !cero){ sinc = 0; return; }        // relacion ~1:1 -> ruido
    const uint16_t tot = alto + d;
    if(!base) base = tot;
    else if(tot < (uint16_t)(base/2) || tot > (uint16_t)(base*2)){ sinc = 0; return; }
    dato = (dato << 1) | (uno ? 1UL : 0UL);
    if(++bits > 24) sinc = 0;
  } else {                                 // flanco de bajada: termino un ALTO
    alto = d;
  }
}

// Una trama suelta no alcanza: el ruido genera tramas validas de vez en cuando.
// Un llavero REPITE la misma trama varias veces por pulsacion, y el ruido nunca
// repite el mismo valor de 24 bits. Por eso se exigen RF_REPES iguales seguidas.
#define RF_REPES   2
#define RF_VENTANA 400

bool rfTrama(uint32_t* out){
  if(!rfListo) return false;
  noInterrupts(); const uint32_t c = rfCodigo; rfListo = false; interrupts();
  if(c == 0 || c == 0xFFFFFFUL) return false;
  if(RF_DEBUG){ Serial.print(F("[rf] trama ")); Serial.println(c); }

  static uint32_t cand = 0, tCand = 0, ultAcep = 0, tAcep = 0;
  static uint8_t  rep = 0;
  const uint32_t n = millis();

  if(c == cand && n - tCand < RF_VENTANA) rep++;
  else { cand = c; rep = 1; }
  tCand = n;

  if(rep < RF_REPES) return false;
  if(c == ultAcep && n - tAcep < 450) return false;   // una pulsacion, un evento
  ultAcep = c; tAcep = n; rep = 0;
  *out = c;
  return true;
}

#define EE_MAGIC_DIR  0
#define EE_CODIGOS    4
#define EE_MAGIC      0x5247u   // "RG"
#define EE_PROX       (EE_CODIGOS + RF_CONTROLES*RF_BOTONES*4)
#define N_CODIGOS     (RF_CONTROLES * RF_BOTONES)

uint32_t rfCod[N_CODIGOS];
uint8_t  rfProxCtrl = 0;        // que control se graba la proxima vez

void rfCargar(){
  uint16_t m; EEPROM.get(EE_MAGIC_DIR, m);
  if(m != EE_MAGIC){ memset(rfCod,0,sizeof(rfCod)); rfProxCtrl=0; return; }
  for(uint8_t i=0;i<N_CODIGOS;i++) EEPROM.get(EE_CODIGOS + i*4, rfCod[i]);
  rfProxCtrl = EEPROM.read(EE_PROX) % RF_CONTROLES;
}
void rfGuardar(){
  EEPROM.put(EE_MAGIC_DIR, (uint16_t)EE_MAGIC);
  for(uint8_t i=0;i<N_CODIGOS;i++) EEPROM.put(EE_CODIGOS + i*4, rfCod[i]);
  EEPROM.update(EE_PROX, rfProxCtrl);
}
// Espera un codigo validado. Devuelve 0 si se agoto el tiempo.
uint32_t rfEsperar(uint16_t segundos){
  const uint32_t t0 = millis();
  uint32_t c;
  while(millis() - t0 < (uint32_t)segundos*1000)
    if(rfTrama(&c)) return c;
  return 0;
}
// Devuelve 1, 2 o 3 segun la FUNCION del boton, de cualquiera de los llaveros.
//   1 = punto equipo A   2 = punto equipo B   3 = deshacer
uint8_t rfLeer(){
  uint32_t c;
  if(!rfTrama(&c)) return 0;
  for(uint8_t i=0;i<N_CODIGOS;i++)
    if(rfCod[i] && rfCod[i] == c) return (uint8_t)(i % RF_BOTONES) + 1;
  Serial.print(F("[rf] codigo sin grabar: ")); Serial.println(c);
  return 0;
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

#if SENSOR_TIPO == 4
// ---------------------------------------------------------------------------
//  UN SOLO HC-SR04, DOS ZONAS
//
//  Con un unico sensor no hay forma de saber de que lado vino la paleta: el
//  HC-SR04 mide distancia, no direccion. Lo que si se puede es partir el
//  alcance en dos zonas y que cada equipo use la suya.
//
//      |<-- A -->|   muerta   |<------- B ------->|
//      4        20 cm      27                   55 cm
//
//  Mismo antirrebote de 3 barreras, y ademas hay que salir de la zona para
//  que vuelva a contar: asi, pasar de la zona B a la A no cuenta dos veces.
// ---------------------------------------------------------------------------
uint8_t zonaDe(uint16_t cm){
  if(cm >= ZONA_A_MIN && cm <= ZONA_A_MAX) return 1;
  if(cm >= ZONA_B_MIN && cm <= ZONA_B_MAX) return 2;
  return 0;
}
uint16_t medirCm(uint8_t trig, uint8_t eco){
  digitalWrite(trig,LOW);  delayMicroseconds(3);
  digitalWrite(trig,HIGH); delayMicroseconds(10);
  digitalWrite(trig,LOW);
  const unsigned long d = pulseIn(eco,HIGH,25000UL);
  return d ? (uint16_t)(d*0.01715f) : 999;
}
// Devuelve 0 = nada, 1 = equipo A, 2 = equipo B
uint8_t sensorUnico(){
  static uint8_t st=0, zona=0; static uint32_t t0=0, tMed=0;
  const uint32_t n = millis();
  if(n - tMed < 55) return 0;              // no medir mas rapido que eso
  tMed = n;
  const uint8_t z = zonaDe(medirCm(PIN_SENS_A, PIN_ECHO_A));
  switch(st){
    case 0: if(z){ st=1; zona=z; t0=n; } break;
    case 1: if(z != zona) st=0;
            else if(n-t0 >= SENS_SOSTENER_MS){
              st=2; t0=n;
              Serial.print(F("[sens] ")); Serial.print(medirCm(PIN_SENS_A,PIN_ECHO_A));
              Serial.print(F(" cm -> ")); Serial.println(zona==1?'A':'B');
              return zona;
            }
            break;
    case 2: if(n-t0 >= SENS_BLOQUEO_MS) st=3; break;
    case 3: if(!z) st=0; break;            // hay que retirar la paleta
  }
  return 0;
}
#endif
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
Boton bDep, bRst;
const uint8_t BRILLOS[3]={BRILLO_DIA,120,45};
uint8_t iBri=0; bool redib=true, barra=false;
bool durmiendo=false; uint32_t tActividad=0;

// Cualquier punto, boton o movimiento despierta el cartel.
void despertar(){
  tActividad = millis();
  if(durmiendo){ durmiendo=false; FastLED.setBrightness(BRILLOS[iBri]); redib=true;
                 Serial.println(F("[presencia] despierta")); }
}
void dormirSiNadie(){
#if PIN_PRESENCIA >= 0
  if(digitalRead(PIN_PRESENCIA)) despertar();
  if(!durmiendo && millis()-tActividad > (uint32_t)DORMIR_MIN*60000UL){
    durmiendo=true; limpiar(); mostrar();
    Serial.println(F("[presencia] sin movimiento: cartel en reposo"));
  }
#endif
}

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
  mostrar(); esperar(1300);
}
void cambiarDeporte(){
  deporte=(uint8_t)((deporte+1)%N_DEPORTES);
  cargarReglas(); resetPartido();
  Serial.print(F("[deporte] ")); Serial.println(deporte);
  bip(1600,80); mostrarDeporte(); redib=true;
}
void onPunto(uint8_t t){
  despertar();
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

/* ================= GRABAR CONTROLES =================
   Se entra manteniendo el boton DEPORTE 15 segundos.
   Ocupa la direccion del control 1, despues la del 2, y vuelve a empezar.
     boton 1 -> punto equipo A    bip bip
     boton 2 -> punto equipo B    bip bip
     boton 3 -> deshacer          biiiiiip y sale
   ==================================================== */
void grabarControles(){
  const uint8_t ctrl = rfProxCtrl;
  Serial.print(F("[grabar] control ")); Serial.println(ctrl+1);
  bip(900,120); bip(900,120); bip(1400,250);

  for(uint8_t b=0; b<RF_BOTONES; b++){
    // en pantalla: C1 / C2 arriba y el numero de boton abajo
    limpiar();
    chr57('C', 2, 0, C_SETS);
    chr57((char)('1'+ctrl), 8, 0, C_SETS);
    chr57((char)('1'+b), ANCHO-8, 0, C_PUNTOS);
    for(uint8_t k=0;k<=b;k++) for(uint8_t f=0;f<2;f++) for(uint8_t c=0;c<2;c++)
      pix(2+k*3+c, ALTO-3+f, C_GANA);
    mostrar();
    Serial.print(F("  apreta el boton ")); Serial.println(b+1);

    uint32_t c = 0;
    const uint32_t lim = millis() + 30000UL;
    while(millis() < lim){
      c = rfEsperar(1);
      if(!c) continue;
      bool rep=false;                       // no aceptar dos veces el mismo
      for(uint8_t i=0;i<b;i++) if(rfCod[ctrl*RF_BOTONES+i]==c) rep=true;
      if(rep){ c=0; continue; }
      break;
    }
    if(!c){
      Serial.println(F("  tiempo agotado, se cancela"));
      bip(300,400); limpiar(); mostrar(); return;
    }
    rfCod[ctrl*RF_BOTONES + b] = c;
    Serial.print(F("  guardado: ")); Serial.println(c);

    if(b < RF_BOTONES-1){ bip(1900,90); delay(70); bip(1900,90); }   // bip bip
    else                  bip(2200,900);                             // biiiiiip
  }
  rfProxCtrl = (uint8_t)((ctrl+1) % RF_CONTROLES);
  rfGuardar();
  Serial.println(F("[grabar] listo"));
  limpiar(); mostrar(); esperar(300);
  redib = true;
}

// Por que arranco el micro. El ATmega lo deja en MCUSR y hay que leerlo
// ANTES que nada. Si el cartel se reinicia solo, esto dice si fue por
// tension baja (BORF) o por el pin RESET (EXTRF), que son problemas muy
// distintos. Ojo: algunos bootloaders borran el registro y queda en 0.
uint8_t causaReset = 0;

// Variables que SOBREVIVEN a un reinicio. La RAM no se borra cuando el micro
// se resetea, solo cuando se le corta la corriente: poniendolas en .noinit
// podemos ver que tension habia ANTES del reset que nos dejo sin saber nada.
#if defined(__AVR__)
  #define NOINIT __attribute__((section(".noinit")))
#else
  #define NOINIT
#endif
uint16_t magiaRam NOINIT;      // 0xA55A = la RAM vale, vengo de un reset
uint16_t vccMin   NOINIT;      // la tension mas baja vista, en mV
uint8_t  nReinicios NOINIT;

// Mide la tension de alimentacion del PROPIO Nano, sin ningun componente
// extra: compara la referencia interna de 1.1 V contra AVcc.
// OJO: esa referencia viene sin calibrar de fabrica (+-10%), asi que el valor
// absoluto puede estar corrido. Comparalo UNA VEZ con el tester y quedate con
// las VARIACIONES, que esas si son exactas.
uint16_t leerVcc(){
#if defined(__AVR__)
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);   // Vbg contra AVcc
  delayMicroseconds(350);                                   // que se estabilice
  ADCSRA |= _BV(ADSC);
  while(ADCSRA & _BV(ADSC)) { }
  const uint16_t cuentas = ADC;
  return cuentas ? (uint16_t)(1125300UL / cuentas) : 0;      // 1.1 * 1023 * 1000
#else
  const uint16_t cuentas = ADC;
  return cuentas ? (uint16_t)(1125300UL / cuentas) : 0;
#endif
}

void vigilarVcc(){
  const uint16_t v = leerVcc();
  if(v && v < vccMin) vccMin = v;
}

void mostrarVolts(const __FlashStringHelper* etq, uint16_t mv){
  Serial.print(etq); Serial.print(mv/1000); Serial.print('.');
  if(mv%1000 < 100) Serial.print('0');
  if(mv%1000 < 10)  Serial.print('0');
  Serial.print(mv%1000); Serial.print(F(" V"));
}

void mostrarCausaReset(){
  Serial.print(F("  arranque: "));
  if(!causaReset){ Serial.println(F("el bootloader borro el motivo")); return; }
  if(causaReset & (1<<PORF))  Serial.print(F("PORF (se le dio corriente) "));
  if(causaReset & (1<<EXTRF)) Serial.print(F("EXTRF <- PIN RESET, cable o ruido! "));
  if(causaReset & (1<<BORF))  Serial.print(F("BORF <- TENSION BAJA, es la fuente! "));
  if(causaReset & (1<<WDRF))  Serial.print(F("WDRF (watchdog) "));
  Serial.println();
}

// Sube el brillo de a poco en vez de pedirle toda la corriente de golpe.
// Muchas power bank y cargadores USB cortan ante un escalon brusco.
void arranqueSuave(){
  const uint8_t objetivo = BRILLOS[iBri];
  for(uint16_t b=0; b<objetivo; b+=4){ FastLED.setBrightness((uint8_t)b); mostrar(); delay(12); }
  FastLED.setBrightness(objetivo); mostrar();
}

void setup(){
  causaReset = MCUSR; MCUSR = 0;
  if(magiaRam != 0xA55A){        // RAM con basura => le acaban de dar corriente
    magiaRam = 0xA55A; vccMin = 9999; nReinicios = 0;
  } else {
    nReinicios++;                // venimos de un reset: la RAM se conservo
  }
#if defined(__AVR__)
  wdt_disable();                 // por si quedo un watchdog armado dando vueltas
#endif
  Serial.begin(BAUD);
#if PIN_BUZZER >= 0
  pinMode(PIN_BUZZER,OUTPUT);
#endif
  FastLED.addLeds<WS2812B,PIN_LED,GRB>(leds,NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,MILIAMPERES);
  FastLED.setBrightness(0);      // apaga la tira YA, antes de cualquier otra cosa
  limpiar(); mostrar();
  pinMode(PIN_LED_PLACA,OUTPUT);
#if PIN_PRESENCIA >= 0
  pinMode(PIN_PRESENCIA,INPUT);
#endif
  tActividad=millis();
#if SENSOR_TIPO == 4
  pinMode(PIN_SENS_A,OUTPUT); digitalWrite(PIN_SENS_A,LOW); pinMode(PIN_ECHO_A,INPUT);
#elif SENSOR_TIPO >= 1
  sA.begin(PIN_SENS_A, PIN_ECHO_A); sB.begin(PIN_SENS_B, PIN_ECHO_B);
#endif
  pinMode(PIN_RF_DATA, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_RF_DATA), rfISR, CHANGE);
  rfCargar();
  bDep.begin(PIN_DEPORTE,false,GRABAR_HOLD_MS);
  bRst.begin(PIN_RESET,false,RESET_LARGO_MS);
  cargarReglas(); resetPartido();
  delay(250);
  Serial.println(F("\n[cartel] tanteador multideporte"));
  Serial.print(F("  panel ")); Serial.print(ANCHO); Serial.print('x'); Serial.print(ALTO);
  Serial.print(F(" = ")); Serial.print(NUM_LEDS); Serial.println(F(" LEDs"));
  Serial.print(F("  RAM libre: ")); Serial.println(ramLibre());
  mostrarCausaReset();
  vigilarVcc();
  mostrarVolts(F("  Vcc ahora: "), leerVcc());
  mostrarVolts(F("   minimo visto: "), vccMin);
  Serial.print(F("   reinicios: ")); Serial.println(nReinicios);
  if(vccMin < 4500)
    Serial.println(F("  !! por debajo de 4.5 V el 328P a 16 MHz esta FUERA DE SPEC"));
  { uint8_t n=0; for(uint8_t i=0;i<N_CODIGOS;i++) if(rfCod[i]) n++;
    Serial.print(F("  controles grabados: ")); Serial.print(n/RF_BOTONES);
    Serial.print(F(" de ")); Serial.println(RF_CONTROLES);
    if(!n) Serial.println(F("  -> manten el boton DEPORTE 15 s para grabar")); }
#if SENSOR_TIPO == 0
  Serial.println(F("  sensores DESACTIVADOS (SENSOR_TIPO 0)"));
#elif SENSOR_TIPO == 4
  Serial.print(F("  1 sensor, 2 zonas: A "));
  Serial.print(ZONA_A_MIN); Serial.print('-'); Serial.print(ZONA_A_MAX);
  Serial.print(F(" cm | B ")); Serial.print(ZONA_B_MIN); Serial.print('-');
  Serial.print(ZONA_B_MAX); Serial.println(F(" cm"));
#elif SENSOR_TIPO == 1
  Serial.print(F("  sensores TCRT5000 en reposo: A="));
  Serial.print(sA.crudo()); Serial.print(F("  B=")); Serial.println(sB.crudo());
  Serial.println(F("  (acerca la paleta y mira cuanto cambia -> ajusta TCRT_UMBRAL)"));
#endif
  dibujar();                     // compone el cuadro con el brillo todavia en 0
  arranqueSuave();               // y recien ahi sube la corriente, escalonada
  mostrarDeporte();
  dibujar();
}

void loop(){
  vigilarVcc();
  bDep.poll(); bRst.poll();
  dormirSiNadie();
  if(durmiendo){                      // en reposo solo se escuchan las entradas
    if(bDep.corto()||bDep.largo()||bRst.corto()||bRst.largo()||rfLeer()) despertar();
    return;
  }

  // --- RESET: barra roja mientras se mantiene ---
  if(bRst.sost()){
    const uint16_t n=(uint16_t)((bRst.desde()*NUM_LEDS)/RESET_LARGO_MS);
    limpiar(); for(uint16_t i=0;i<n&&i<NUM_LEDS;i++) leds[i]=CRGB(255,0,0);
    mostrar(); barra=true; return;
  }
  // --- DEPORTE mantenido: barra ambar hasta los 15 s = grabar controles ---
  if(bDep.sost() && bDep.desde() > 900){
    const uint16_t n=(uint16_t)((bDep.desde()*NUM_LEDS)/GRABAR_HOLD_MS);
    limpiar(); for(uint16_t i=0;i<n&&i<NUM_LEDS;i++) leds[i]=CRGB(255,120,0);
    mostrar(); barra=true; return;
  }
  if(barra){ barra=false; redib=true; }

  if(bRst.corto()){ iBri=(uint8_t)((iBri+1)%3);
    FastLED.setBrightness(BRILLOS[iBri]); bip(1500,40); redib=true; }
  if(bRst.largo()){ resetPartido(); bip(1200,90); bip(1700,120); redib=true; }
  if(bDep.corto()) cambiarDeporte();
  if(bDep.largo()) grabarControles();

  // --- control remoto: 1 = punto A, 2 = punto B, 3 = deshacer ---
  switch(rfLeer()){
    case 1: onPunto(0); break;
    case 2: onPunto(1); break;
    case 3: if(deshacer()) bip(900,70); else bip(350,140); redib=true; break;
  }

#if SENSOR_TIPO == 4
  { const uint8_t z = sensorUnico(); if(z) onPunto(z-1); }
#elif SENSOR_TIPO >= 1
  if(sA.poll()) onPunto(0);
  if(sB.poll()) onPunto(1);
#endif

  static uint32_t tD=0;
  const bool titila = (E.gan==0xFF) && (aviso(0)==3 || aviso(1)==3);
  if(redib || (titila && millis()-tD>150)){ dibujar(); redib=false; tD=millis(); }
}
