# 🔧 Paquete de montaje — `LAYOUT_SPARSE`

Todo lo que hace falta para armar el panel, en un solo lugar. Esta es la carpeta
para tener abierta en el banco de trabajo.

## 0. Buena noticia: no hubo que reconfigurar nada

Los dos tamaños que imprimiste **ya son exactamente** el perfil `LAYOUT_SPARSE`
que estaba en el firmware:

| Lo que imprimiste | Es | LEDs/segmento |
|---|---|:---:|
| **136 × 74 mm** | dígito de **PUNTOS** | **3** |
| **103 × 57 mm** | dígito de **JUEGOS** y de **SETS** | **2** |

El único cambio: los iconos del centro pasaron de 1 LED a **2** (con uno solo
quedaban flojos al lado de los dígitos). Total: **148 LEDs**.

Y sobre la distancia, tu intuición es correcta: 136 mm se lee a **40-55 m** y
103 mm a **30-40 m**. Una cancha de pádel son 20 × 10 m, y nadie mira desde más
de 25 m. **Sobra.** Con la ventaja de que usás **148 de los 300 LEDs** del rollo,
o sea que te sobra la mitad para errores, repuestos y un segundo cartel.

## 1. El panel

```
  <------------------------------ 490 mm ------------------------------>
  +----------------------------------------------------------------------+
  |                                                                      |
  |   EQUIPO A                     SAQUE                      EQUIPO B   |
  |  +--------+ +--------+        +------+         +--------+ +--------+ |
  |  |        | |        |        |  < > |         |        | |        | |
  |  | PUNTOS | | PUNTOS |        +------+         | PUNTOS | | PUNTOS | |  136
  |  | 3 LEDs | | 3 LEDs |         PUNTO           | 3 LEDs | | 3 LEDs | |  mm
  |  +--------+ +--------+        +------+         +--------+ +--------+ |
  |    74 mm      74 mm           | ^  ^ |           74 mm      74 mm    |
  |                               +------+                               |
  |  +------+  +------+                            +------+  +------+    |  103
  |  |JUEGOS|  | SETS |                            |JUEGOS|  | SETS |    |  mm
  |  |2 LEDs|  |2 LEDs|                            |2 LEDs|  |2 LEDs|    |
  |  +------+  +------+                            +------+  +------+    |
  |    57 mm     57 mm                               57 mm     57 mm     |
  |                                                                      |
  +----------------------------------------------------------------------+
     [ ◎ sensor A ]                                   [ ◎ sensor B ]
```

| | |
|---|---|
| **Panel completo** | **490 × 307 mm** (~49 × 31 cm) |
| Bloque de un equipo | 160 × 257 mm |
| Columna del centro | 70 mm de ancho |
| Separación entre dígitos de puntos | 12 mm |
| Separación entre la fila de puntos y la de juegos/sets | 18 mm |
| Margen del borde | 25 mm |

Las separaciones son sugeridas: ajustalas a cómo te quedaron las piezas impresas.
Lo único que **no** se puede cambiar sin tocar el firmware es cuántos LEDs lleva
cada segmento.

## 2. El mapa de LEDs

Está completo en **[`mapa-leds.txt`](mapa-leds.txt)**, con los rangos exactos de
cada bloque. Resumen de la cadena:

```
EQUIPO A   puntos decena  LED   0 –  20        EQUIPO B   LED  70 –  90
           puntos unidad  LED  21 –  41                   LED  91 – 111
           juegos         LED  42 –  55                   LED 112 – 125
           sets           LED  56 –  69                   LED 126 – 139

CENTRO     saque A  140-141    saque B  142-143
           punto A  144-145    punto B  146-147          TOTAL: 148
```

Ese archivo **se regenera solo** desde `config.h`, así que nunca queda
desfasado del firmware:

```bash
python3 montaje/mapa_leds.py > montaje/mapa-leds.txt
```

Si cambiás de perfil o de orden de segmentos, lo corrés de nuevo y listo.

## 3. El orden de los segmentos

Soldá en el orden que te quede cómodo y después declarálo en `config.h`:

```c
// a) vuelta de reloj (el que esta puesto ahora)
#define CADENA_SEGMENTOS { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G }

// b) zigzag de lectura (arriba, los dos de arriba, el medio, los dos de abajo, abajo)
#define CADENA_SEGMENTOS { SEG_A, SEG_F, SEG_B, SEG_G, SEG_E, SEG_C, SEG_D }
```

Si no te acordás cómo quedó: `tools/test_segmentos` enciende la cadena posición
por posición y trae una planilla para anotar.

## 4. El circuito

```
  Power bank 5V ──► bornera 5V ──┬──► tira LED (inyectar en los 2 extremos)
                                 ├──► Nano pin 5V
                                 ├──► receptor RF VCC
                                 └──► HC-SR04 VCC (x2)
  GND de TODO ──► bornera GND          (una sola masa, siempre)

  Nano D6 ──[ 470 Ω ]──► DIN de la tira
  Capacitor 1000 µF entre 5V y GND a la entrada de la tira   ⚠ no es opcional

  D2  TRIG sensor A        D7  RF D0  (+1 Equipo A)      D11  buzzer
  D3  ECHO sensor A        D8  RF D1  (+1 Equipo B)      D12  botón brillo
  D4  TRIG sensor B        D9  RF D2  (deshacer)         A1   botón RESET
  D5  ECHO sensor B        D10 RF D3  (brillo / reset)   A0   LDR (opcional)
```

Detalle completo, con el receptor RF y las reglas de cableado:
**[`../docs/01-hardware-bom.md` §5](../docs/01-hardware-bom.md)**

Consumo de este panel: **148 LEDs ⇒ ~1,2 A típicos** a 5 V. Entra cómodo hasta en
un power bank de 2 A, con mucha autonomía.

## 5. Orden de trabajo

- [ ] **1.** Poner `LAYOUT_SPARSE` en `config.h` (ya está activo)
- [ ] **2.** Cortar **21 LEDs** y armar **UN dígito de puntos** (7 piezas × 3)
- [ ] **3.** `tools/test_segmentos` → `s3` → `8` ⇒ tiene que verse un **8 parejo**
- [ ] **4.** `a` ⇒ anotar qué segmento prende en cada posición → `CADENA_SEGMENTOS`
- [ ] **5.** `c` ⇒ tiene que contar **0 a 9** bien dibujado
- [ ] **6.** 🌞 **Sacarlo al sol del mediodía, con y sin el filtro oscuro**
- [ ] **7.** Recién ahí: cortar y armar los 7 dígitos restantes y los 4 iconos
- [ ] **8.** Cablear todo en cadena según el mapa y verificar con `diag_leds` → `w`
- [ ] **9.** Cargar `firmware/padel_scoreboard` y jugar un partido completo
- [ ] **10.** Calibrar los umbrales de proximidad **en la cancha**

El paso 6 es el que decide el proyecto. No sigas sin hacerlo.

## 6. Enlaces

| | |
|---|---|
| Cortar la tira | [`docs/02` §6b](../docs/02-display-7seg-led.md) |
| Que se lea al sol (los 5 trucos) | [`docs/02` §6](../docs/02-display-7seg-led.md) |
| Circuito completo | [`docs/01` §5](../docs/01-hardware-bom.md) |
| Piezas 3D | [`3d/README.md`](../3d/README.md) |
| Reglas y firmware | [`docs/03`](../docs/03-reglas-y-firmware.md) |
| Deportes / licencia / horas de uso | [`docs/09`](../docs/09-deportes-uso-y-licencia.md) |
