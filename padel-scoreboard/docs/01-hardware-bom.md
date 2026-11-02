# 01 - Hardware, BOM y alimentación

## 1. Por qué ESP32 y no un Arduino UNO/Nano

Pediste "Arduino cerebro", y el Nano alcanza para el MVP. Pero pediste también
**dejar pensada la conexión WiFi para la app**. Con un Nano eso obliga a sumar
después un ESP8266/ESP32 como módem y reescribir la mitad del firmware.

| | Arduino Nano | **ESP32 DevKit v1 (elegido)** |
|---|---|---|
| Precio | ~5 USD | ~6 USD |
| WiFi / BLE | no | **sí, integrado** |
| CPU / RAM | 16 MHz / 2 KB | 240 MHz x2 / 520 KB |
| Lógica | 5 V (directo a WS2812B) | 3.3 V (necesita level shifter) |
| OTA (update sin cable) | no | sí |

Mismo precio, y el camino a la app ya queda abierto. El firmware igual está
escrito con la lógica de tanteo separada del hardware (`padel_score.cpp` no
incluye nada de Arduino), así que se puede portar si hace falta.

## 2. BOM del MVP

Precios de referencia (USD, mercado local aprox., noviembre 2026):

| # | Ítem | Cant. | Aprox. | Notas |
|---|------|-------|--------|-------|
| 1 | ESP32 DevKit v1 (30 pines) | 1 | 6 | el cerebro |
| 2 | Tira WS2812B **IP65**, 60 LED/m, rollo 5 m | 1 | 15 | se necesitan ~4 m: 236 LEDs |
| 3 | Sensor ultrasónico HC-SR04 | 2 | 3 | proximidad de paleta izq./der. |
| 4 | Receptor RF 433 MHz 4 canales (PT2272-M4) | 1 | 4 | salidas momentáneas |
| 5 | Llavero RF 433 MHz EV1527 4 botones | 2 | 3 | **un control por equipo** |
| 6 | Level shifter 74AHCT125 | 1 | 1 | 3.3 V → 5 V del dato de la tira |
| 7 | Capacitor 1000 µF / 10 V + resistor 470 Ω | 1 | 1 | protección de la tira |
| 8 | Buzzer pasivo + transistor BC337 | 1 | 1 | confirmación sonora |
| 9 | Acrílico **opal blanco 3 mm** (A3) | 1 | 9 | difusor |
| 10 | Acrílico **humo / rojo oscuro 2 mm** (A3) | 1 | 9 | filtro de contraste (ver doc 02) |
| 11 | PVC foam board negro 3 mm o chapa plástica | 1 | 8 | máscara frontal + tabiques |
| 12 | Caja estanca IP65 ~300×200×80 o bandeja de PVC | 1 | 14 | cuerpo del cartel |
| 13 | Borneras, fusible 5 A, cable 18 AWG, termocontraíble, bridas, silicona neutra | — | 8 | |
| 14 | Power bank 20 000 mAh con salida **5 V / 3 A** | 1 | 20 | o cargador USB-C de pared |
| | **Total con power bank** | | **~100** | **~80 sin el power bank** |

Lo que **no** hay que comprar en el MVP: módulos de 7 segmentos comerciales
(un dígito de 20 cm cuesta más que todo esto), fuente de PC, PCB.

## 3. Presupuesto de corriente (esto es lo que define la fuente)

Conteo de LEDs (definido en `config.h`):

```
Por equipo:  2 dígitos PUNTOS × 42 LEDs (6 LEDs/segmento)  =  84
             1 dígito GAMES  × 28 LEDs (4 LEDs/segmento)   =  28
             3 dots de SET   ×  2 LEDs                     =   6
                                                    por equipo = 118
                                              TOTAL 2 equipos = 236 LEDs
```

| Escenario | LEDs encendidos | Corriente @5 V |
|---|---|---|
| Peor caso teórico (todo en rojo pleno) | 236 | ~4.7 A |
| Caso real (dígitos típicos, ~55 %) | ~130 | ~2.6 A |
| Con brillo "sombra" (150/255) | ~130 | ~1.6 A |
| ESP32 + sensores + receptor | — | ~0.2 A |

Dos consecuencias prácticas:

1. **El firmware limita la corriente solo.** `FastLED.setMaxPowerInVoltsAndMilliamps(5, 2600)`
   escala el brillo global antes de cada `show()` para no pasar de 2.6 A. Un
   power bank de 5 V / 3 A nunca se corta por sobreconsumo. Si usás una fuente
   más grande, subí `PSU_MILLIAMPS` en `config.h` y el cartel se pone más brillante
   automáticamente.
2. **Autonomía**: power bank de 20 000 mAh (74 Wh) a ~13 W reales ⇒ **5–6 h de
   juego continuo**. Una tarde de pádel entra sin problema. A brillo de sombra,
   9–10 h.

### Si querés más brillo que el que da un power bank

Camino de upgrade (no hace falta para el MVP): módulo **CH224K / ZY12PDN**
(disparador USB-C PD) seteado a 12 V, de ahí un convertidor **12 V → 5 V 10 A**.
Con un power bank PD de 65 W se llega a los 4.7 A del peor caso y el cable USB-C
queda fino (menos caída de tensión). Subís `PSU_MILLIAMPS` a 4700 y listo.

## 4. Cableado

```
                       ┌────────────────────────────────────────┐
  Power bank 5V/3A ──► │ bornera 5V ─── fusible 5A ─────────────┼──► 5V tira (inyectar
   (USB-C o USB-A)     │        │                               │     en AMBOS extremos)
                       │        ├──► ESP32 pin 5V (VIN)         │
                       │        └──► 74AHCT125 VCC              │
                       │ GND común ◄───────────────────────────┤
                       └────────────────────────────────────────┘

  ESP32 GPIO13 ──► 74AHCT125 (1A) ──(1Y)── 470 Ω ──► DIN tira WS2812B
  Capacitor 1000 µF entre 5V y GND, lo más cerca posible del inicio de la tira.

  HC-SR04 "A" (izquierda)        HC-SR04 "B" (derecha)
    VCC → 5V                       VCC → 5V
    TRIG → GPIO25                  TRIG → GPIO32
    ECHO → divisor → GPIO26        ECHO → divisor → GPIO33
    GND → GND                      GND → GND

    Divisor obligatorio (el ECHO sale 5 V, el ESP32 es 3.3 V):
      ECHO ──[ 1 kΩ ]──┬── GPIO
                       └──[ 2 kΩ ]── GND

  Receptor RF 4 canales (alimentado a 5 V, salidas 3.3–5 V "momentáneas"):
    CH1 → GPIO14   (+1 punto Equipo A)
    CH2 → GPIO27   (+1 punto Equipo B)
    CH3 → GPIO16   (corto: deshacer / largo: cambiar saque)
    CH4 → GPIO17   (corto: brillo / 2 s: reset del partido)
    Si las salidas del receptor son 5 V, poné el mismo divisor 1k/2k en cada canal.

  Buzzer  → GPIO4 (vía BC337, con resistor 1 kΩ en base)
  Botón de la caja → GPIO18 a GND (usa INPUT_PULLUP)
  LDR opcional → GPIO34 (divisor con 10 kΩ a GND)
```

**Reglas de oro del armado eléctrico**

- Masa común entre ESP32, tira, sensores y receptor. Siempre.
- Inyectar 5 V al principio **y al final** de la guirnalda (y al medio si pasás
  de ~150 LEDs por tramo): si no, los últimos dígitos quedan rojizos y opacos.
- Cable de alimentación 18 AWG como mínimo. La caída de tensión a 3 A en cable
  fino es la causa número uno de "los últimos dígitos parpadean".
- Nunca alimentar la tira desde el pin 5V del ESP32: pasa por la protección de
  la placa y se quema.

## 5. Los dos controles

Llaveros EV1527 de 4 botones, **ambos aprendidos al mismo receptor de 4 canales**.
Como los dos llaveros son idénticos, cada equipo tiene físicamente los 4 canales;
lo que se hace es **asignar un botón por equipo** (y etiquetarlos con cinta de
color):

| Botón del llavero | Canal | Función | Quién lo usa |
|---|---|---|---|
| A | CH1 | +1 punto Equipo A (izquierda) | Equipo A |
| B | CH2 | +1 punto Equipo B (derecha) | Equipo B |
| C | CH3 | corto = deshacer · largo = cambiar saque | cualquiera |
| D | CH4 | corto = brillo · 2 s = reset del partido | cualquiera |

Simple, sin pairing, sin baterías que gestionar (CR2032, ~1 año), alcance 30–80 m
en línea de vista: sobra para una cancha. El camino elegante (remotos ESP-NOW con
batería recargable y telemetría) está descrito en `04-roadmap-wifi-app.md`.

## 6. Los sensores de proximidad

- **A (izquierda) = Equipo A**, **B (derecha) = Equipo B**, tal como pediste.
- Se montan en el **borde inferior del cartel**, apuntando hacia el jugador, con
  un aro pintado del color del equipo alrededor de cada ventana ("acercá la paleta acá").
- Altura de montaje recomendada: **1.2–1.6 m**, cómodo de alcanzar con la paleta
  sin agacharse. Si el cartel va alto en la reja, los dos sensores van en una
  **cajita satélite** al lado del poste de la red, con 4 hilos (5V, GND, TRIG, ECHO
  por sensor) hasta el cerebro.
- Umbral: cuenta cuando la paleta está a **≤20 cm sostenida 250 ms**, después
  queda bloqueado 2.5 s y además **exige que la paleta se aleje más de 35 cm**
  para re-armarse. Eso elimina los dobles conteos y a la gente que pasa cerca.
- Se eligió **ultrasónico (HC-SR04) y no infrarrojo**: el ultrasonido es inmune
  al sol directo, que es justo donde fallan los sensores IR reflectivos tipo
  TCRT5000. Upgrade opcional: **VL53L1X** (ToF láser, haz angosto de 27°, muy
  rápido, I²C) — más preciso y menos falsos positivos por gente caminando, pero
  pierde alcance con sol muy fuerte y hay que manejar dos direcciones I²C con XSHUT.
  Está previsto como alternativa en `inputs.cpp`.
