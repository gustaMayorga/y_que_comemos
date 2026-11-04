# 01 - Hardware, BOM y alimentación

## 1. Placa: Nano para la demo, ESP32 para la versión con app

La demo se hace con **Arduino Nano**, como pediste. El firmware soporta las dos
placas y se cambia con **una línea** en `config.h`:

```c
#define BOARD_NANO      // demo actual
//#define BOARD_ESP32   // fase 2: habilita WiFi
```

| | **Arduino Nano (demo)** | ESP32 DevKit v1 (fase 2) |
|---|---|---|
| Precio | ~5 USD | ~6 USD |
| Lógica | **5 V: nada de level shifter, nada de divisores** | 3.3 V: necesita shifter + divisores |
| RAM | 2 KB (alcanza, ver §4) | 520 KB |
| WiFi / BLE | no | sí |
| OTA | no | sí |
| Pines usados / disponibles | 11 de 12 digitales | sobran |

**Por qué el Nano es realmente mejor para la demo**: al ser 5 V, el dato de la
tira WS2812B va directo (el ESP32 a 3.3 V está en el límite y conviene un
74AHCT125) y el pin ECHO de los HC-SR04 va directo (con ESP32 hay que poner un
divisor 1k/2k en cada uno o se daña el pin). Son 5 componentes y 10 soldaduras menos.

**Lo que se pierde**: WiFi. Por eso la lógica de tanteo (`padel_score.cpp`) no
tiene una sola línea de Arduino: cuando se pase al ESP32, se cambia el `#define`
y se compila. Nada que reescribir.

## 1b. Si usás un Arduino Pro Mini (ATmega328P)

Sirve exactamente igual que el Nano: mismo chip, misma RAM, misma numeracion de
pines D2..D13 y A0..A5. `BOARD_NANO` en `config.h` lo cubre sin cambios. Las dos
únicas diferencias:

1. **No tiene USB**: se programa con un adaptador USB-TTL (CH340, FT232, CP2102).
2. **Hay dos versiones y hay que saber cuál tenés**: **5 V / 16 MHz** o
   **3.3 V / 8 MHz**. Esto cambia la configuración del IDE y, más importante,
   cambia el cableado del tanteador.

### Cómo saber cuál es, sin instrumentos

Cargá el Blink con **"ATmega328P (5V, 16 MHz)"** seleccionado y mirá el LED:

- parpadea **1 segundo prendido / 1 apagado** → es **5 V / 16 MHz** ✅
- parpadea **al doble de lento** (2 s y 2 s) → es **3.3 V / 8 MHz**: el código
  asume el doble de velocidad de reloj que la real

El otro síntoma del mismo problema: el monitor serie imprime **caracteres basura**
aunque el baudrate esté bien.

### Qué cambia en el tanteador según la versión

| | **Pro Mini 5 V / 16 MHz** | **Pro Mini 3.3 V / 8 MHz** |
|---|---|---|
| Dato de la tira WS2812B | directo, como el Nano | ⚠️ 3.3 V está en el límite: **conviene buffer** (acá entra el CD74HC4009, con dos inversores en serie) |
| HC-SR04 | directo a 5 V | ⚠️ alimentarlos con **5 V aparte** (a 3.3 V no funcionan bien) y poner **divisor 1k/2k en cada ECHO** |
| Receptor RF | directo | divisor en cada canal si sus salidas son de 5 V |
| FastLED | sin problemas | funciona (8 MHz es el mínimo que soporta para WS2812), sin margen de sobra |
| IDE: Processor | ATmega328P (5V, 16 MHz) | ATmega328P (3.3V, 8 MHz) |

**Conclusión**: si tenés las dos versiones, usá la de **5 V / 16 MHz**: te ahorra
el buffer y los divisores, igual que el Nano. Si sólo tenés la de 3.3 V, funciona,
pero sumá el buffer de datos y los divisores de los ECHO.

## 2. BOM

La columna **Demo** es lo mínimo para la versión jugable con Nano; **Final** es el
cartel definitivo. Precios de referencia en USD.

| # | Ítem | Demo | Final | Aprox. | Notas |
|---|------|:----:|:-----:|-------|-------|
| 1 | Arduino Nano (clon CH340 sirve) | 1 | — | 5 | el cerebro de la demo |
| 2 | ESP32 DevKit v1 | — | 1 | 6 | sólo para la fase con app |
| 3 | Tira WS2812B **IP65**, 60 LED/m | 3.2 m | 4.7 m | 12 / 16 | rollo de 5 m cubre las dos |
| 4 | Sensor ultrasónico HC-SR04 | 2 | 2 | 3 | proximidad de paleta izq./der. |
| 5 | Receptor RF 433 MHz **4 canales** (PT2272-M4) | 1 | 1 | 4 | salidas momentáneas |
| 6 | Llavero RF 433 MHz EV1527 4 botones | 2 | 2 | 3 | **un control por equipo** |
| 7 | Buzzer pasivo + BC337 + R 1 kΩ | 1 | 1 | 1 | confirmación sonora |
| 8 | Capacitor 1000 µF / 10 V + R 470 Ω | 1 | 1 | 1 | protección de la tira |
| 9 | Level shifter 74AHCT125 | **no** | 1 | 1 | sólo si se usa ESP32 |
| 10 | Acrílico opal blanco 3 mm (A3) | opc. | 1 | 9 | difusor (en la demo: papel manteca) |
| 11 | Acrílico **humo o rojo oscuro** 2 mm (A3) | reco. | 1 | 9 | filtro de contraste: ver doc 02 §4 |
| 12 | Cartón pluma / PVC foam negro 3 mm | 1 | 1 | 8 | máscara + tabiques |
| 13 | Caja estanca IP65 ~300×200×80 | opc. | 1 | 14 | en la demo: bandeja de PVC |
| 14 | Borneras, fusible 5 A, cable 18 AWG, termocontraíble, bridas, silicona | — | — | 8 | |
| 15 | Power bank 20 000 mAh salida **5 V / 3 A** | 1 | 1 | 20 | o cargador USB-C de pared |
| | **Total** | **~55** | **~95** | | sin contar el power bank: ~35 / ~75 |

Para saber qué de esto ya tenés y qué se puede sustituir, está
**[`05-inventario-demo-nano.md`](05-inventario-demo-nano.md)**.

## 3. Presupuesto de corriente

Conteo de LEDs por perfil (generado desde `config.h`):

| Perfil | Puntos (4 díg.) | Juegos (2) | Sets (2) | Centro (4 iconos) | **Total** | Tira |
|---|---|---|---|---|---|---|
| `LAYOUT_BENCH` | 4 × 7 | 2 × 7 | 2 × 7 | 4 × 1 | **60** | 1.0 m |
| `LAYOUT_DEMO` | 4 × 28 | 2 × 21 | 2 × 14 | 4 × 2 | **190** | 3.2 m |
| `LAYOUT_FULL` | 4 × 42 | 2 × 28 | 2 × 21 | 4 × 3 | **278** | 4.7 m |

| Escenario (`LAYOUT_DEMO`, 190 LEDs) | LEDs encendidos | Corriente @5 V |
|---|---|---|
| Peor caso teórico (todo rojo pleno) | 190 | ~3.8 A |
| Caso real (dígitos típicos ~55 %) | ~105 | ~2.1 A |
| Brillo "sombra" (150/255) | ~105 | ~1.3 A |
| Nano + sensores + receptor | — | ~0.1 A |

Con `LAYOUT_FULL` (278 LEDs) el peor caso sube a ~5.6 A y el caso real a ~3.1 A.

Dos consecuencias prácticas:

1. **El firmware limita la corriente solo.**
   `FastLED.setMaxPowerInVoltsAndMilliamps(5, 2600)` escala el brillo global antes
   de cada `show()` para no pasar de 2.6 A. Con un power bank de 5 V / 3 A nunca
   se corta por sobreconsumo. Si conseguís una fuente más grande, subís
   `PSU_MILLIAMPS` y el cartel se pone más brillante automáticamente, sin tocar
   nada más.
2. **Autonomía**: power bank de 20 000 mAh (74 Wh) a ~11 W reales ⇒ **6–7 h** con
   `LAYOUT_DEMO` a brillo de sol; 10–12 h a brillo de sombra.

`LAYOUT_BENCH` (60 LEDs, ~1.2 A peor caso) funciona **alimentado por el USB de la PC**.

### Si se quiere más brillo que el que da un power bank

Camino de upgrade: módulo **CH224K / ZY12PDN** (disparador USB-C PD) seteado a
12 V + convertidor **12 V → 5 V 10 A**. Con un power bank PD de 65 W se llega al
peor caso de `LAYOUT_FULL`. Se sube `PSU_MILLIAMPS` y listo.

## 4. Presupuesto de RAM del Nano (el riesgo real)

El ATmega328P tiene **2048 bytes** de SRAM. Cálculo (estimado, no medido —
en este entorno no hay toolchain de AVR para confirmarlo con `avr-size`):

| Qué | `LAYOUT_DEMO` | `LAYOUT_FULL` |
|---|---|---|
| Framebuffer de LEDs (3 B por LED) | 570 B | 834 B |
| `Display7Seg` (último estado + flags) | ~20 B | ~20 B |
| `ScoreEngine` (estado + reglas + 8 niveles de undo) | 143 B | 143 B |
| 5 × `Button` | 75 B | 75 B |
| 2 × `ProximityCounter` | 34 B | 34 B |
| Internos de FastLED | ~40 B | ~40 B |
| Buffers de Serial (RX+TX) | 128 B | 128 B |
| **Total estático** | **~1010 B (49 %)** | **~1274 B (62 %)** |

Quedan ~1000 B / ~770 B para la pila. Entra, pero hubo que tomar **dos decisiones
explícitas** para que entre:

1. **`UNDO_DEPTH = 8` en Nano** (16 en ESP32). Cada nivel cuesta 15 bytes.
2. **El JSON del estado se imprime campo por campo, no se arma en un buffer.**
   Un `char buf[200]` en la pila era el consumo evitable más grande. En ESP32 sí
   se arma el string, porque es el que se manda por WebSocket. Las dos ramas
   producen el mismo JSON (`net_stub.h`).
3. Las animaciones **no guardan copia del framebuffer**: pintan encima y restauran
   llamando a `refresh()`. Un backup del bloque de un equipo eran 273 bytes de pila.

**Flash**: el sketch + FastLED entran en ~14–17 KB de los 30 KB útiles. Sin problema.

**Advertencia de timing**: FastLED en AVR deshabilita interrupciones durante
`show()` (~30 µs por LED ⇒ 5.7 ms con 190 LEDs, 8.3 ms con 278). Eso hace que
`millis()` se atrase unos milisegundos **por cada refresco**. Como el refresco
sólo ocurre cuando cambia algo, el atraso total en un partido es de décimas de
segundo y no afecta nada (ni el antirrebote, ni la pulsación larga de 2 s).

## 5. Cableado con Arduino Nano

```
                        ┌──────────────────────────────────────────┐
  Power bank 5V/3A ───►  │ bornera 5V ── fusible 5A ────────────────┼──► 5V tira
   (USB-A o USB-C)       │      │                                   │   (inyectar en
                         │      └──► Nano pin 5V  (NO el VIN)       │    AMBOS extremos)
                         │ GND común ◄─────────────────────────────┤
                         └──────────────────────────────────────────┘

  Nano D6 ──[ 470 Ω ]──► DIN de la tira WS2812B      (sin level shifter: Nano es 5 V)
  Capacitor 1000 µF entre 5V y GND, lo más cerca posible del inicio de la tira.

  HC-SR04 "A" (izquierda, Equipo A)     HC-SR04 "B" (derecha, Equipo B)
    VCC  → 5V                             VCC  → 5V
    TRIG → D2                              TRIG → D4
    ECHO → D3   (directo, sin divisor)     ECHO → D5   (directo)
    GND  → GND                             GND  → GND

  ┌──────────────────────────────────────────────────────────────────┐
  │  RECEPTOR RF 433 MHz de 4 canales (PT2272-M4)                    │
  │                                                                  │
  │    VCC  ──────────► 5V   (de la bornera, NO del pin del Nano)    │
  │    GND  ──────────► GND  (masa común)                            │
  │    D0 / CH1 ──────► D7    botón 1: +1 punto Equipo A             │
  │    D1 / CH2 ──────► D8    botón 2: +1 punto Equipo B             │
  │    D2 / CH3 ──────► D9    botón 3: DESHACER (CTRL+Z)             │
  │    D3 / CH4 ──────► D10   botón 4: brillo / reset (sólo 4 btn)   │
  │    ANT  ──────────► 17 cm de alambre recto, vertical, al aire    │
  └──────────────────────────────────────────────────────────────────┘

  Los pines del receptor suelen venir rotulados D0 D1 D2 D3 (no CH1..CH4).
  D0 es el canal 1. Si además tiene un pin VT (Valid Transmission), se deja sin
  conectar. Si el módulo es de 5 V y sus salidas llegan a 5 V, con el Nano van
  directo; con ESP32 hay que poner divisor 1k/2k en cada canal.

  Buzzer pasivo → D11  (vía BC337: base con R 1 kΩ, emisor a GND, buzzer entre 5V y colector)
  Botón de la caja  → D12 a GND  (brillo; usa INPUT_PULLUP, sin resistor)
  Botón de RESET    → A1  a GND  (mantener 2 s; usa INPUT_PULLUP, sin resistor)
  LDR opcional      → A0  (divisor con 10 kΩ a GND)

  D13 queda libre a propósito: tiene el LED de la placa en paralelo.
  D0/D1 son el puerto serie (USB), no usar.
```

Para el pinout de ESP32 (fase 2) ver la sección `BOARD_ESP32` de `config.h`.

**Reglas de oro del armado eléctrico**

- **Masa común** entre Nano, tira, sensores y receptor. Siempre.
- **Inyectar 5 V al principio y al final** de la guirnalda (y al medio si un tramo
  pasa de ~150 LEDs). Si no, los últimos dígitos quedan rojizos y opacos.
- Alimentación con cable **18 AWG o más grueso**. La caída de tensión a 3 A en
  cable fino es la causa número uno de "los últimos dígitos parpadean".
- **Nunca alimentar la tira desde el pin 5V del Nano**: pasa por la pista de la
  placa y se quema. El 5V del Nano se conecta *a la misma bornera* que la tira,
  no "a través" de la placa.
- El capacitor de 1000 µF no es opcional: el pico de arranque de 190 LEDs hace
  resetear el Nano sin él.

## 6. Los dos controles

Llaveros EV1527 de 4 botones, **ambos aprendidos al mismo receptor de 4 canales**.
Como los llaveros son idénticos, cada equipo tiene físicamente los 4 canales; lo
que se hace es **asignar un botón por equipo** y etiquetarlos con cinta de color:

| Botón del llavero | Canal | Pin Nano | Función | Quién lo usa |
|---|---|---|---|---|
| A | CH1 | D7 | +1 punto Equipo A (izquierda) | Equipo A |
| B | CH2 | D8 | +1 punto Equipo B (derecha) | Equipo B |
| C | CH3 | D9 | corto = deshacer · largo = cambiar saque | cualquiera |
| D | CH4 | D10 | corto = brillo · 2 s = reset del partido | cualquiera |

Sin pairing, sin baterías que gestionar (CR2032, ~1 año), alcance 30–80 m en línea
de vista: sobra para una cancha. Si no tenés el receptor 433, en el doc 05 están
las alternativas (IR con control de TV, Bluetooth, botones cableados).

## 7. Los sensores de proximidad

- **A (izquierda) = Equipo A**, **B (derecha) = Equipo B**, como pediste.
- Se montan en el **borde inferior del cartel**, apuntando al jugador, con un aro
  pintado del color del equipo alrededor de cada ventana ("acercá la paleta acá").
- Altura recomendada: **1.2–1.6 m**, cómodo de alcanzar con la paleta sin
  agacharse. Si el cartel va alto en la reja, los dos sensores van en una
  **cajita satélite** junto al poste de la red, con 4 hilos por sensor.
- Umbral: cuenta cuando la paleta está a **≤20 cm sostenida 250 ms**; después
  queda bloqueado 2.5 s y además **exige que la paleta se aleje más de 35 cm**
  para re-armarse. Eso elimina los dobles conteos y a la gente que pasa cerca.
  Los cuatro umbrales están en `config.h` y **hay que calibrarlos en la cancha**.
- Se eligió **ultrasónico y no infrarrojo**: el ultrasonido es inmune al sol
  directo, que es justo donde fallan los IR reflectivos tipo TCRT5000.
