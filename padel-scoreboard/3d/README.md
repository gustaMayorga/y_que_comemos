# Piezas imprimibles en 3D

`cartel.scad` genera todas las piezas. Es **paramétrico**: se abre en OpenSCAD,
se elige la pieza arriba del archivo, F5 para ver, F6 para renderizar y F7 para
exportar el STL.

> ⚠️ Este archivo **no se pudo renderizar en el entorno donde se escribió**
> (no había OpenSCAD instalado). La geometría y la sintaxis están revisadas,
> pero la primera vez abrilo con **F5** y mirá que se vea como esperás antes de
> mandar 4 horas de impresión.

## 1. Medidas (para `LAYOUT_FULL` con tira de 60 LED/m)

Paso entre LEDs: **16,67 mm**. Ancho de celda: tira de 10 mm + 2 mm de juego a
cada lado = **12 mm** (hueco) → **15,2 mm** con las paredes de 1,6 mm.

| Dígito | LEDs/seg | Largo de luz | Celda | **Dígito completo** |
|---|---|---|---|---|
| **PUNTOS** (4 unidades) | 6 | **100,0 mm** | 104 × 12 mm | **124 × 236 mm** |
| **JUEGOS** (2 unidades) | 4 | 66,7 mm | 71 × 12 mm | 91 × 169 mm |
| **SETS** (2 unidades) | 3 | 50,0 mm | 54 × 12 mm | 74 × 136 mm |

Panel completo estimado: **~62 × 46 cm**.

## 2. ⚠️ El dígito de puntos NO entra en una cama de 220 × 220

236 mm de alto no entran en una Ender 3, Prusa Mini ni la mayoría. Por eso el
archivo tiene el parámetro `MITAD`:

```
MITAD = "inferior";   // imprimís esta
MITAD = "superior";   // después esta
```

Las dos mitades salen con **agujeros de 3,2 mm** para pasadores de alineación
(sirve un clavo, un trozo de varilla o filamento de 3 mm). Se pegan con
cianoacrilato y queda rígido.

Los dígitos de **juegos (169 mm)** y **sets (136 mm)** entran **enteros**.

| Pieza | Cantidad | ¿Entra en 220×220? |
|---|---|---|
| `rejilla` TAMANO="big" | 4 dígitos × 2 mitades = **8** | partida |
| `rejilla` TAMANO="small" | **2** | entera |
| `rejilla` TAMANO="tiny" | **2** | entera |
| `icono` | **4** | entera |
| `powerbank` | **1** | entera |
| | **17 impresiones** | |

## 3. Cuánto filamento y cuánto tiempo (sé honesto con esto)

Con `BASE = false` (recomendado): **~450 g de PLA y 35-45 h de impresión** para
el juego completo. Es media bobina y varios días de impresora.

**Alternativa que cuesta 0 y lleva 2 horas**: los tabiques de **cartón pluma
negro de 3 mm** cortados a 22 mm de alto hacen exactamente lo mismo. El 3D queda
más prolijo y más rígido, pero **ópticamente el resultado es igual**.

Recomendación práctica: para el MVP, **cartón**. Para la versión que va al club,
3D. O un punto medio: imprimí sólo los **4 dígitos de puntos** (los que se miran)
y hacé juegos y sets en cartón.

## 4. Antes de imprimir en serio: medir la tira real

Dos números hay que confirmar cuando llegue la tira, y después ajustarlos arriba
del `.scad`:

**`PASO_LED`** — no midas un solo LED. Contá **10 LEDs**, medí del centro del
primero al centro del décimo con calibre o regla, y dividí por 9.
*Ejemplo: 150 mm / 9 = 16,67 mm.* Si te da distinto de 16,67 cambialo, porque el
error se multiplica por 6 en cada segmento.

**`TIRA_ANCHO`** — el ancho físico de la tira. WS2812B IP20 de 5050 suele ser
**10 mm**; las IP65 con silicona llegan a 12 mm.

**Prueba obligatoria antes de las 17 impresiones**: generá `PIEZA = "celda"` con
`TAMANO = "big"`, imprimí **una sola** (~25 min) y verificá que la tira entra
cómoda y que los 6 LEDs quedan dentro del largo útil. Si calza, imprimí el resto.

## 5. Parámetros que vas a querer tocar

| Parámetro | Default | Cuándo cambiarlo |
|---|---|---|
| `PASO_LED` | 16.67 | tira de 30 LED/m → **33.33** |
| `TIRA_ANCHO` | 10.0 | si medís otra cosa |
| `ALTO` | 22 | distancia LED→difusor. 60 LED/m: 15-20 alcanza. **30 LED/m: 25-30** |
| `LEDS_BIG/SMALL/TINY` | 6/4/3 | tienen que coincidir con `config.h` |
| `BASE` | false | true si no vas a usar placa de fondo |
| `PB_L/PB_W/PB_H` | 160/80/25 | **medí tu power bank** |
| `CAMA_X/CAMA_Y` | 220 | tu impresora |

## 6. Configuración de impresión sugerida

- **Material**: PLA negro mate (el negro importa: las paredes no tienen que
  reflejar luz hacia los segmentos vecinos). PETG si el cartel va a quedar al sol
  todo el día — el PLA se deforma arriba de 55 °C y adentro de una caja negra al
  sol se llega.
- Altura de capa **0,28-0,3 mm** (son paredes, no hace falta detalle)
- Perímetros **3**, relleno **0 %**, sin top/bottom salvo si usás `BASE`
- Sin soporte, sin balsa. Brim de 5 mm en las mitades grandes (son angostas y altas)
- Boquilla 0,4 mm. Con 0,6 va mucho más rápido y queda igual

## 7. La bandeja del power bank

`PIEZA = "powerbank"`. Antes **medí el tuyo** y poné `PB_L`, `PB_W`, `PB_H`.

Trae: hueco a medida, **ventana lateral para el cable USB**, dos ranuras para
pasar una cinta de velcro que lo sujeta, y dos orejas con agujeros de 5 mm para
precintos o tornillos contra la parte de atrás del panel.

Se imprime con el fondo contra la cama, sin soporte.
