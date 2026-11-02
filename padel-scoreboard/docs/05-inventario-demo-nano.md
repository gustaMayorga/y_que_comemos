# 05 - Qué componentes necesito revisar para arrancar la demo

Checklist para que me digas qué tenés. Está ordenado por **cuánto cambia el
diseño si no lo tenés**: lo de arriba es lo que bloquea, lo de abajo se resuelve
con lo que haya en el cajón.

---

## 🔴 Crítico: la tira LED (esto puede cambiar TODO el diseño)

Es el único punto donde, según lo que tengas, hay que reescribir el firmware del
display. Mirá la tira (dice el modelo impreso cada tanto) y decime:

| Pregunta | Por qué importa |
|---|---|
| ¿Cuántos **metros** tenés? | define qué perfil podés armar (ver tabla de abajo) |
| ¿Cuántos **LED/m**? (30 / 60 / 144) | define el tamaño del dígito. Lo asumido es 60/m |
| ¿Qué **chip**? | ⬇️ esto es lo determinante |
| ¿**5 V** o **12 V**? | la de 12 V necesita otra alimentación |
| ¿**IP65/IP67** (con silicona) o pelada? | el cartel va a la intemperie |

**Según el chip:**

- **WS2812B / WS2811 / SK6812 / NeoPixel** → ✅ es lo que asume el firmware, no
  se cambia nada. Son las de "cada LED tiene su chip" y 3 pines (5V, DIN, GND).
- **WS2815** (12 V, direccionable) → ✅ funciona igual, solo cambia la
  alimentación a 12 V y el `addLeds` a `WS2815`. Dos líneas.
- **Tira RGB analógica común** (4 pines: 12V, R, G, B — todos los LEDs del mismo
  color a la vez) → ❌ **no sirve para dígitos**. No se puede controlar segmento
  por segmento. Si es la única que tenés, el camino es otro: **74HC595 +
  transistores**, un display multiplexado. Avisame y lo rediseño, se puede, pero
  es otro firmware.
- **Tira de un solo color** (blanca/roja, 2 pines) → ❌ igual que la anterior:
  hay que multiplexar con 595 + MOSFETs. Decime y lo rediseñamos.

**Cuánta tira hace falta** (perfiles ya configurados en `config.h`):

| Perfil | LEDs | Tira a 60 LED/m | Dígito de puntos | Para qué sirve |
|---|---|---|---|---|
| `LAYOUT_BENCH` | **60** | **1.0 m exacto** | 1 LED/segmento | probar TODA la lógica en el escritorio |
| `LAYOUT_DEMO` | **190** | ~3.2 m | ~155 mm, legible a 45 m | demo jugable en cancha |
| `LAYOUT_FULL` | **278** | ~4.7 m | ~230 mm, legible a 80 m | cartel definitivo |

> Con **1 metro de tira** ya se prueba el sistema completo: los 4 dígitos de
> puntos, los de juegos y sets, las flechas de saque y los avisos de punto. Se ven
> chiquitos, pero el firmware es exactamente el final.

---

## 🟠 Importante: alimentación

| Qué buscar | Pregunta |
|---|---|
| Power bank | ¿cuánto dice de salida? **5 V / 2 A** o **5 V / 3 A**. Si dice "PD 20 W / 65 W" mejor todavía |
| Cargador de celular USB | ¿cuántos amperes? (los viejos dan 1 A y no alcanzan) |
| Fuente vieja de 5 V | de router, decodificador, impresora, disco externo. Si dice **5 V 2 A o más**, sirve |
| Fuente de PC (ATX) | el cable rojo da 5 V y muchos amperes. Sirve de sobra para el banco de pruebas |

Decime los amperes y ajusto `PSU_MILLIAMPS` en `config.h`: el firmware limita el
brillo solo para no pasarse, así que **con cualquiera de estas arranca**, solo
cambia qué tan brillante se puede poner.

Para `LAYOUT_BENCH` (60 LEDs) alcanza y sobra con el **USB de la PC** (500 mA).

---

## 🟡 Entradas: sensores de paleta

Lo asumido: **2 × HC-SR04** (ultrasónico, el de los dos "ojos").

Si no tenés, por orden de preferencia:

1. **HC-SR04** → ✅ lo mejor para esto: inmune al sol, barato, 2–400 cm.
2. **VL53L0X / VL53L1X** (ToF láser, módulo chico con una ventanita) → ✅ también
   sirve, haz más angosto; pierde algo de alcance con sol fuerte.
3. **Sensor IR reflectivo** (TCRT5000, el típico de seguidor de línea) → ⚠️ anda
   en sombra, **falla a pleno sol**. Vale para la demo de escritorio.
4. **Sensor capacitivo casero**: un cuadrado de papel aluminio detrás del acrílico
   + la librería `CapacitiveSensor` → ✅ sorprendentemente bueno, costo cero, y
   es "tocar" en vez de "acercar". Buena opción si no tenés sensores.
5. **Nada** → ✅ arrancamos con **2 botones grandes** cableados y los sensores
   quedan para después. El firmware ya trata las dos entradas igual.

---

## 🟡 Entradas: los 2 controles remotos

Lo asumido: **receptor RF 433 MHz de 4 canales (PT2272-M4) + 2 llaveros EV1527**.

Alternativas con lo que puedas tener:

| Qué tenés | Sirve | Qué cambia |
|---|---|---|
| Receptor 433 de 4 canales + llaveros | ✅ ideal | nada |
| Módulos RF 433 sueltos (**FS1000A** + **XY-MK-5V**) | ⚠️ sí, pero | son TX/RX de datos crudos: hay que poner otro micro en cada control. Más trabajo |
| **Receptor IR TSOP1838/VS1838** + cualquier **control de TV** | ✅ muy bueno para la demo | librería `IRremote`, 1 pin. Gratis si tenés controles viejos. Limitación: necesita línea de vista y ~8 m |
| Módulo **HC-05 / HC-06 Bluetooth** | ✅ | el "control" pasa a ser el celular. Adelanta parte de la app |
| **nRF24L01** | ⚠️ | necesita un micro en cada control |
| Nada de esto | ✅ | arrancamos con botones cableados; los remotos entran después sin tocar la lógica |

Mi recomendación si tenés controles de TV viejos tirados: **arrancá con IR**. Es
el camino más corto a tener los dos controles funcionando hoy.

---

## 🟢 Difusor y máscara (se puede improvisar todo)

| Lo ideal | Lo que sirve igual para la demo |
|---|---|
| Acrílico opal blanco 3 mm | **papel manteca** (2–3 capas) o papel vegetal, o una botella de leche cortada |
| Acrílico humo/rojo 2 mm (filtro de contraste) | una **carpeta plástica oscura**, o un pedazo de acetato ahumado. Sin esto igual anda, pero **con sol se nota mucho la diferencia** |
| PVC foam board negro 3 mm | **cartón pluma** o cartón pintado de **negro mate** (témpera o aerosol mate) |
| Tabiques impresos en 3D | tiras de cartón negro de 2 cm pegadas con silicona |
| Caja estanca IP65 | caja de cartón para el banco de pruebas, bandeja de PVC para la demo |

---

## 🟢 Herramientas

- [ ] Soldador + estaño (**imprescindible**: hay que soldar 3 hilos por segmento)
- [ ] Multímetro (muy recomendable para verificar los 5 V en el final de la tira)
- [ ] Trincheta / cutter
- [ ] Pistola de silicona
- [ ] Cable fino de colores (3 colores, ~10 m) y cable 18 AWG para alimentación
- [ ] Termocontraíble o cinta aisladora
- [ ] Protoboard + jumpers macho-macho y macho-hembra

---

## Lo mínimo para arrancar HOY

```
Arduino Nano + cable USB
1 metro de tira WS2812B (60 LED/m)
2 botones (o 2 cables que se tocan)
protoboard y jumpers
```

Con eso compilás `LAYOUT_BENCH` y **el sistema funciona completo**: partido entero
con puntos, juegos, sets, tie break, punto de oro, saque, avisos de punto de
juego/set/partido, undo y reset. Alimentado por el USB de la PC.

Ni siquiera hace falta la tira: con el monitor serie a 115200 el firmware imprime
el estado en JSON en cada punto, así que la lógica se valida con **solo el Nano
y dos botones**.

---

## Formato para contestarme

Pegame algo así y ajusto `config.h`, los pines y la BOM a lo que tengas:

```
Nano:     sí / no          (¿clon con CH340? ¿cable mini-USB o micro-USB?)
Tira:     ___ metros, ___ LED/m, chip ______, 5V/12V, IP__
Fuente:   power bank ___ A  /  cargador ___ A  /  fuente 5V ___ A
Sensores: HC-SR04 x__  /  VL53L0X x__  /  IR x__  /  ninguno
Controles:receptor 433 4ch + __ llaveros  /  TSOP IR + control de TV  /  ninguno
Difusor:  acrílico opal / papel manteca / nada todavía
Varios:   soldador sí/no, multímetro sí/no, protoboard sí/no
```
