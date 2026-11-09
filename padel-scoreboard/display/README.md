> ⚠️ **SUPERSEDIDO por [`../nano_vfinal/`](../nano_vfinal/)**, que hace todo
> esto y además 8 deportes, grabación de controles RF y 4 tipos de sensor.
> Esta carpeta queda como referencia; **para armar, usá `nano_vfinal`**.

# 🖥️ Tanteador de pádel — versión PANEL MATRIZ (Nano)

Línea "moderna": en vez de 7 segmentos, una **grilla de píxeles** donde se dibuja
cualquier cosa. Mismo tanteo de pádel, pero con letras, texto corrido y festejos
de verdad.

Es **autónomo**: no depende de `../firmware`. La versión con WiFi, app, más
deportes y licencia va en ESP32 y se arma aparte.

## 📐 El panel: 23 columnas × **12** filas = 276 LEDs

```
12 tiras de 23 LEDs, en SERPENTINA

  fila  0   →→→→→→→→→→→→→→→→→→→→→→→   23 LEDs
  fila  1   ←←←←←←←←←←←←←←←←←←←←←←←
  fila  2   →→→→→→→→→→→→→→→→→→→→→→→
   ...                                   383 × 217 mm
  fila 11   ←←←←←←←←←←←←←←←←←←←←←←←
```

La salida de cada fila va a la entrada de la siguiente: como alternan de
dirección, el salto es de 2 cm.

**`ALTO` es configurable**: 12 usa 276 LEDs, 13 usa 299 (los 5 m justos). Con 13
queda una fila de separación entre los puntos y los juegos/sets; con 12 van
pegadas, y se distinguen igual porque son de distinto color. La fila de
juegos/sets se apoya sola abajo (`FILA_BAJA = ALTO - 5`).

### Los LEDs no tienen identidad propia

Un WS2812B es intercambiable con cualquier otro: **lo que define la dirección de
un LED es su posición en la cadena, no de qué parte del rollo salió**. El primero
después del Arduino es el 0, siempre.

Consecuencia práctica: podés **cortar un pedazo de cualquier lado del rollo y
usarlo donde quieras**. Si cortaste D1-D4 y te queda cómodo ponerlo como los
últimos 4 LEDs de la fila 11, hacelo: esos 4 pasan a ser las direcciones 272-275
y el firmware no se entera.

**La única regla**: el **orden eléctrico** (DIN → DO, pedazo tras pedazo) tiene
que seguir el **recorrido físico** que espera el firmware — fila 0 de izquierda
a derecha, fila 1 de derecha a izquierda, y así. Si movés un tramo de lugar,
re-soldalo para que siga ocupando el mismo lugar del recorrido. Mientras el
recorrido se respete, de dónde salió cada pedazo da exactamente igual.

## 🎨 Qué muestra

```
   col: 0...4 5...9 |10 11 12| 13..17 18..22
        +-----+-----+--------+-----+-----+
   f0   |     |     |   ◄    |     |     |
   ..   |  4  |  0  |        |  3  |  0  |   PUNTOS   fuente 5×7
   f6   |     |     |        |     |     |
   f7    [3] [1]     |  ▣  |      [2] [0]   JUEGOS y SETS  fuente 3×5
   f11
```

- **Puntos**: `0` `15` `30` `40` `AD` en rojo, dígitos de **117 mm**
- **Juegos y sets**: ámbar y amarillo, fuente chica
- **Centro arriba**: flecha de **saque**, apunta al equipo que saca
- **Centro abajo**: bloque de **aviso** — 🔵 punto de juego · 🟡 de set · 🔴 de partido (titila)
- **Fin de partido**: perdedor apagado, fuegos artificiales, olas de color y
  **`GANA A` / `GANA B` en texto corrido**

## 🔌 Cableado

```
  tira +5V → bornera 5V de la fuente      (NO del pin del Nano)
  tira GND → bornera GND + GND del Nano   (masa común)
  tira DIN → D6  con 470 Ω en serie
  capacitor 1000 µF entre 5V y GND a la entrada de la tira

  sensor IR A (equipo A, izquierda) → D2      salida OUT del módulo
  sensor IR B (equipo B, derecha)   → D3
  receptor RF 433:  D0→D7   D1→D8   D2→D9   D3→D10    antena 17 cm
  buzzer D11 · botón brillo D12 a GND · botón reset A1 a GND
```

### ⚠️ Los sensores IR y el sol

Los módulos IR de obstáculo (FC-51 y parecidos) **se enceguecen con el sol
directo**: el infrarrojo del sol satura el receptor y el sensor queda detectando
siempre o nunca. Para probar y para cancha techada van bien.

Al aire libre, dos salidas: montarlos **bajo una visera**, apuntando hacia abajo
y a la sombra, o pasar a **HC-SR04 ultrasónicos**, que son inmunes al sol. El
cambio en el firmware es chico: el HC-SR04 necesita TRIG + ECHO en vez de una
sola salida digital.

## 🎮 Controles

| Botón | Corto | Largo (1 s) |
|---|---|---|
| **1** | +1 punto Equipo A | saca el Equipo A |
| **2** | +1 punto Equipo B | saca el Equipo B |
| **3** | **DESHACER** (8 pasos) | ciclar brillo |
| **4** *(si hay)* | ciclar brillo | reset del partido |

Botón de la caja: brillo. Botón **RESET**: mantener 2 s, con **barra roja**
llenándose en el panel.

Sensores de paleta: izquierdo = A, derecho = B, siempre activos en paralelo.

## ▶️ Cargar

```bash
arduino-cli lib install FastLED
arduino-cli compile --fqbn arduino:avr:nano display/display
arduino-cli upload  --fqbn arduino:avr:nano -p /dev/ttyUSB0 display/display
```

Al arrancar imprime por serie (115200) el tamaño del panel y la **RAM libre**.
Con 23 × 12 deberían quedar unos **470 bytes libres**. Si bajara de 300, achicá
el panel con `ANCHO` / `ALTO`.

## 🔧 Antes del primer partido: calibrar la grilla

Con `tools/demo_matriz` (el mismo panel, sketch de prueba):

1. **`e`** → marca las 4 esquinas: 🔴 arriba-izq · 🟢 arriba-der · 🔵 abajo-izq · 🟡 abajo-der.
   Si no coinciden, ajustá `SERPENTINA`, `FILA0_INVERTIDA` u `ORIGEN_ARRIBA`.
2. **`m`** → enciende LED por LED en orden físico: tiene que serpentear.

Esos dos valores son los mismos en `demo_matriz` y en `display.ino`: una vez que
los calibrás, los copiás.

## 📊 Consumo

| Qué | Corriente @5 V |
|---|---|
| Tanteo normal (~25 % de píxeles) | ~1,4 A |
| Festejo a pantalla llena, rojo | ~6 A |
| Blanco pleno | ~18 A |

`MILIAMPERES` en el sketch limita el brillo para no pasarse de tu fuente. Con un
power bank de 3 A los festejos a pantalla completa van a verse atenuados: es
esperable, y es el argumento para que la versión matriz sea la **enchufada del
club**.

## 🆚 Contra la versión 7 segmentos

| | 7 segmentos (`../firmware`) | **Matriz (esta)** |
|---|---|---|
| LEDs | 148 | 276 |
| Alto del dígito | 136 mm | 117 mm |
| Brillo al sol | 🏆 imbatible | bueno |
| Letras, nombres, texto corrido | ❌ | ✅ |
| Festejos | básicos | 🏆 animaciones |
| Deportes, licencia, horas de uso | ✅ | ⬜ van en la versión ESP32 |
