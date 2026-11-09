// ============================================================================
//  diag_rf - Ver que recibe de verdad el receptor 433 MHz
//
//  Sin LEDs ni nada mas: solo el receptor. Sirve para saber si el llavero llega,
//  cuanto ruido hay, y que tiempos usa el protocolo del control.
//
//  CONEXION
//    receptor VCC ──► 5 V      DATA ──► D2      GND ──► GND
//    ANT ──► 17,3 cm de alambre recto
//
//  Monitor serie a 115200. Comandos:
//    t  tramas decodificadas (lo normal)
//    c  volcado CRUDO de tiempos: sirve para descubrir el protocolo
//    e  estadistica: cuantas tramas por segundo entran
//    r  reiniciar contadores
//    ?  ayuda
// ============================================================================

#include <Arduino.h>

#define PIN_DATA 2
#if F_CPU >= 16000000L
  #define BAUD 115200
#else
  #define BAUD  57600
#endif

/* ---------- captura cruda de flancos ---------- */
#define N_CRUDO 160
volatile uint16_t crudo[N_CRUDO];
volatile uint8_t  nCrudo = 0;
volatile bool     crudoListo = false;
volatile bool     capturando = false;

/* ---------- decodificador ---------- */
volatile uint32_t codigo = 0;
volatile uint16_t codBase = 0;
volatile bool     codListo = false;
volatile uint16_t nTramas = 0, nRuido = 0;
volatile uint32_t nFlancos = 0;        // CUALQUIER cambio en el pin

void isr(){
  static uint32_t ult = 0, dato = 0;
  static uint16_t alto = 0, base = 0;
  static uint8_t  bits = 0, sinc = 0;
  const uint32_t n = micros();
  const uint16_t d = (uint16_t)(n - ult);
  ult = n;
  nFlancos++;

  if(capturando && nCrudo < N_CRUDO){      // volcado crudo
    crudo[nCrudo++] = d;
    if(nCrudo >= N_CRUDO){ crudoListo = true; capturando = false; }
  }

  if(digitalRead(PIN_DATA)){               // subida: termino un BAJO
    if(d > 2500 && d < 40000){             // sincronismo
      if(sinc && bits == 24 && !codListo){
        codigo = dato; codBase = base; codListo = true; nTramas++;
      } else if(sinc && bits > 4) nRuido++;
      bits = 0; dato = 0; base = 0; sinc = 1;
      return;
    }
    if(!sinc) return;
    const bool uno  = alto > (uint16_t)(d * 2);
    const bool cero = d > (uint16_t)(alto * 2);
    if(!uno && !cero){ sinc = 0; nRuido++; return; }
    const uint16_t tot = alto + d;
    if(!base) base = tot;
    else if(tot < (uint16_t)(base/2) || tot > (uint16_t)(base*2)){ sinc=0; nRuido++; return; }
    dato = (dato << 1) | (uno ? 1UL : 0UL);
    if(++bits > 24) sinc = 0;
  } else alto = d;
}

char modo = 't';
uint32_t tEst = 0;
uint32_t ultCod = 0; uint8_t rep = 0; uint32_t tRep = 0;

void ayuda(){
  Serial.println(F("\n--- diag_rf ---"));
  Serial.println(F("t  tramas decodificadas"));
  Serial.println(F("c  volcado CRUDO de tiempos (apreta el llavero despues)"));
  Serial.println(F("e  estadistica cada 5 s"));
  Serial.println(F("r  reiniciar contadores"));
  Serial.println(F("?  esta ayuda"));
}

void setup(){
  Serial.begin(BAUD);
  pinMode(PIN_DATA, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_DATA), isr, CHANGE);
  delay(300);
  // --- test decisivo: hay actividad en el pin? ---
  Serial.println(F("\n[diag_rf] midiendo actividad en D2 durante 2 s..."));
  nFlancos = 0; delay(2000);
  const uint32_t f = nFlancos;
  Serial.print(F("  flancos: ")); Serial.println(f);
  if(f == 0){
    Serial.println(F("  >> NO LLEGA NADA AL PIN. El receptor no esta alimentado,"));
    Serial.println(F("     falta masa comun, o DATA no esta en D2."));
  } else if(f < 200){
    Serial.println(F("  >> poca actividad. Puede ser un receptor silencioso"));
    Serial.println(F("     (superheterodino) o mala alimentacion."));
  } else {
    Serial.println(F("  >> EL RECEPTOR ESTA VIVO. Eso es ruido normal de un"));
    Serial.println(F("     super-regenerativo sin transmisor cerca."));
  }
  Serial.println(F("\n[diag_rf] escuchando en D2"));
  Serial.println(F("Con el llavero LEJOS no deberia salir casi nada."));
  Serial.println(F("Apreta un boton: la MISMA trama tiene que repetirse varias veces."));
  ayuda();
  tEst = millis();
}

void loop(){
  if(Serial.available()){
    const char c = Serial.read();
    if(c=='t'||c=='e'){ modo=c; Serial.print(F("modo ")); Serial.println(c); }
    else if(c=='c'){ modo='c'; nCrudo=0; crudoListo=false; capturando=true;
      Serial.println(F("capturando... APRETA EL LLAVERO AHORA")); }
    else if(c=='r'){ nTramas=nRuido=0; Serial.println(F("contadores en cero")); }
    else if(c=='?') ayuda();
  }

  if(codListo){
    noInterrupts();
    const uint32_t v=codigo; const uint16_t b=codBase; codListo=false;
    interrupts();
    if(v == ultCod && millis()-tRep < 400) rep++; else { ultCod=v; rep=1; }
    tRep = millis();
    if(modo=='t'){
      Serial.print(F("trama ")); Serial.print(v);
      Serial.print(F("   bit base ")); Serial.print(b); Serial.print(F(" us"));
      Serial.print(F("   repeticion ")); Serial.print(rep);
      if(rep>=2) Serial.print(F("   <-- VALIDA"));
      Serial.println();
    }
  }

  if(crudoListo){
    crudoListo=false;
    Serial.println(F("\n--- tiempos crudos (us), alternando alto y bajo ---"));
    for(uint8_t i=0;i<N_CRUDO;i++){
      Serial.print(crudo[i]); Serial.print(i%10==9?'\n':'\t');
    }
    Serial.println(F("\nBusca el valor GRANDE (>2500): ese es el sincronismo."));
    Serial.println(F("Los que siguen son los bits: tienen que alternar corto/largo."));
    Serial.println(F("sincronismo / pulso corto = la proporcion del protocolo."));
    modo='t';
  }

  if(modo=='e' && millis()-tEst > 5000){
    tEst = millis();
    Serial.print(F("[est] flancos: ")); Serial.print(nFlancos);
    Serial.print(F("   tramas validas: ")); Serial.print(nTramas);
    Serial.print(F("   descartadas: ")); Serial.println(nRuido);
    nTramas=nRuido=0; nFlancos=0;
  }
}
