// ============================================================================
//  diag_nano - Verifica que un Arduino Nano funcione. SIN componentes externos.
//
//  Para que sirve:
//   - confirmar que la placa arranca, que el bootloader anda y que el USB/serie
//     funciona (si esto carga y responde, el Nano esta vivo)
//   - medir la RAM libre real (el dato que importa para el tanteador)
//   - probar los pines digitales de a uno, con un cable a GND
//
//  Cargar y abrir el monitor serie a 115200. Escribir ? para el menu.
// ============================================================================

#include <Arduino.h>

const uint8_t PIN_LED = 13;          // LED de la placa
const uint8_t FIRST_PIN = 2, LAST_PIN = 12;

char line[16];
uint8_t lineLen = 0;

#if defined(__AVR__)
int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
#else
int freeRam() { return -1; }
#endif

void help() {
  Serial.println(F("\n--- diag_nano ---"));
  Serial.println(F("r  RAM libre"));
  Serial.println(F("b  blink del LED de la placa (5 veces)"));
  Serial.println(F("p  estado de D2..D12 (con INPUT_PULLUP: 1 = suelto, 0 = a GND)"));
  Serial.println(F("m  monitor continuo de D2..D12 (cualquier tecla corta)"));
  Serial.println(F("a  lectura de A0..A5"));
  Serial.println(F("i  info de la placa"));
  Serial.println(F("?  este menu"));
}

void showRam() {
  Serial.print(F("RAM libre: "));
  Serial.print(freeRam());
  Serial.println(F(" bytes (el ATmega328P tiene 2048 en total)"));
}

void blink() {
  for (uint8_t i = 0; i < 5; i++) {
    digitalWrite(PIN_LED, HIGH); delay(150);
    digitalWrite(PIN_LED, LOW);  delay(150);
  }
  Serial.println(F("blink listo (si no viste parpadear D13, revisa la placa)"));
}

void showPins() {
  Serial.print(F("D"));
  for (uint8_t p = FIRST_PIN; p <= LAST_PIN; p++) {
    Serial.print(p); Serial.print('='); Serial.print(digitalRead(p));
    if (p < LAST_PIN) Serial.print(F("  D"));
  }
  Serial.println();
}

void monitorPins() {
  Serial.println(F("monitor: toca un pin a GND y mira que cambie. Cualquier tecla corta."));
  while (!Serial.available()) { showPins(); delay(300); }
  while (Serial.available()) Serial.read();
}

void showAnalog() {
  for (uint8_t i = 0; i < 6; i++) {
    Serial.print(F("A")); Serial.print(i); Serial.print('=');
    Serial.print(analogRead(A0 + i)); Serial.print(F("   "));
  }
  Serial.println();
}

void info() {
  Serial.print(F("F_CPU = ")); Serial.print(F_CPU / 1000000UL); Serial.println(F(" MHz"));
#if defined(__AVR_ATmega328P__)
  Serial.println(F("MCU   = ATmega328P (Nano/Uno)  Flash 32K (30720 utiles)  SRAM 2048  EEPROM 1024"));
#elif defined(__AVR__)
  Serial.println(F("MCU   = otro AVR"));
#else
  Serial.println(F("MCU   = no AVR"));
#endif
  showRam();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  for (uint8_t p = FIRST_PIN; p <= LAST_PIN; p++) pinMode(p, INPUT_PULLUP);
  delay(300);
  Serial.println(F("\n[diag_nano] si lees esto, el Nano y el USB funcionan."));
  info();
  help();
  blink();
}

void loop() {
  while (Serial.available()) {
    const char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (lineLen == 0) continue;
      line[lineLen] = 0;
      switch (line[0]) {
        case 'r': showRam();     break;
        case 'b': blink();       break;
        case 'p': showPins();    break;
        case 'm': monitorPins(); break;
        case 'a': showAnalog();  break;
        case 'i': info();        break;
        default:  help();        break;
      }
      lineLen = 0;
    } else if (lineLen < sizeof(line) - 1) {
      line[lineLen++] = c;
    }
  }
}
