# 02 - Display de 7 segmentos con tira LED, legible a pleno sol

Esta es la parte crítica del MVP. Un cartel LED que se ve espectacular en el
taller puede ser **invisible** a las 3 de la tarde en una cancha al aire libre.
El problema nunca es la cantidad de lúmenes: es el **contraste**.

## 1. Qué dígitos lleva el cartel

| Bloque | Cantidad | Qué muestra |
|---|---|---|
| **Puntos** | **4 dígitos grandes** (2 por equipo) | ` 0`, `15`, `30`, `40`, `Ad`, y `0..12` en tie break |
| **Juegos** | 2 dígitos chicos (1 por equipo) | `0..7` |
| **Sets** | 2 dígitos mini (1 por equipo) | `0..3` |
| **Saque** | 2 flechas al centro | se enciende la del equipo que saca |
| **Aviso de punto** | 2 triángulos al centro | punto de juego / set / partido |

## 2. Layout

```
 ┌───────────────────────────────┬─────────┬───────────────────────────────┐
 │  EQUIPO A                     │  SAQUE  │                     EQUIPO B  │
 │    ┌──────┐ ┌──────┐          │ ◀     ▶ │          ┌──────┐ ┌──────┐    │
 │    │  4   │ │  0   │          │         │          │  3   │ │  0   │    │
 │    │      │ │      │  PUNTOS  │  PUNTO  │  PUNTOS  │      │ │      │    │
 │    └──────┘ └──────┘          │ ▲     ▲ │          └──────┘ └──────┘    │
 │      ┌───┐    ┌──┐            │         │            ┌───┐    ┌──┐      │
 │      │ 3 │    │1 │            │         │            │ 2 │    │0 │      │
 │      └───┘    └──┘            │         │            └───┘    └──┘      │
 │      JUEGOS   SETS            │         │            JUEGOS   SETS      │
 ├───────────────────────────────┴─────────┴───────────────────────────────┤
 │   [ ◎ A ]                                                     [ ◎ B ]   │
 └─────────────────────────────────────────────────────────────────────────┘
     sensor izquierdo = Equipo A                sensor derecho = Equipo B
```

Medidas aproximadas del frente:

| Perfil | Dígito de puntos | Frente completo | Legible a |
|---|---|---|---|
| `LAYOUT_DEMO` | 100 × 155 mm | ~45 × 30 cm | ~45 m |
| `LAYOUT_FULL` | 120 × 230 mm | ~65 × 45 cm | ~80 m |

Una cancha de pádel es de 20 × 10 m y nadie mira desde más de 25 m, así que
**`LAYOUT_DEMO` ya es legible de sobra** y usa 3.2 m de tira en vez de 4.7 m.
`LAYOUT_FULL` es para que se lea también desde las canchas de al lado o desde el bar.

## 3. Geometría del dígito

Tira de 60 LED/m ⇒ paso entre LEDs = **16.7 mm**.

```
        AAAAAA          LEDs por segmento (config.h)      segmento   dígito alto
       F      B         ───────────────────────────────────────────────────────
       F      B         PUNTOS  LEDS_PER_SEG_BIG   = 6      100 mm      230 mm
       F      B                                    = 4 (demo) 67 mm     155 mm
        GGGGGG          JUEGOS  LEDS_PER_SEG_SMALL = 4       67 mm      150 mm
       E      C                                    = 3 (demo) 50 mm     115 mm
       E      C         SETS    LEDS_PER_SEG_TINY  = 3       50 mm      115 mm
       E      C                                    = 2 (demo) 33 mm      80 mm
        DDDDDD
```

Regla práctica de legibilidad con alto contraste a la luz del día: ~25 mm de
altura de carácter por cada 8–10 m de distancia. No conviene bajar de 4 LEDs por
segmento en los dígitos de puntos (salvo en `LAYOUT_BENCH`, que es sólo para probar).

## 4. Los iconos del centro: la forma la hace la máscara, no los LEDs

Este es el truco que hace que las flechas y los avisos salgan gratis: **detrás de
cada icono hay simplemente 2 o 3 LEDs en línea**. La forma (flecha, triángulo) la
da la ventana recortada en la máscara negra. El firmware sólo enciende el grupo
con un color.

```
   Ventana en la máscara          Detrás hay esto
   ─────────────────────          ───────────────
        ◀  (flecha a la izq.)      ● ● ●   ← 3 LEDs en línea
        ▶  (flecha a la der.)      ● ● ●
        ▲  (triángulo aviso)       ● ● ●
```

**Flechas de saque**: se enciende en **verde** la del equipo que saca. Apuntan
hacia el equipo, así que de un vistazo se ve de qué lado está el saque.

**Aviso de punto**: un triángulo por equipo, y **el color dice de qué punto se
trata**:

| Color | Significado | Terminología correcta |
|---|---|---|
| 🔵 cyan | estás a **un punto de ganar el juego** | punto de juego (si saca el rival: **punto de quiebre**) |
| 🟡 amarillo | estás a **un punto de ganar el set** | punto de set |
| 🔴 rojo **titilando** | estás a **un punto de ganar el partido** | punto de partido |
| apagado | nada de lo anterior | |

Ojo con la **ventaja**: es otra cosa. "Ventaja" (`Ad` en los dígitos de puntos)
sólo existe jugando **sin punto de oro**: 40-40 y ganás un punto. Eso ya se ve en
los dígitos grandes, no en el centro. Con **punto de oro** activado, en 40-40 los
**dos** avisos se encienden a la vez — que es exactamente lo que está pasando.

Los colores se definen en `config.h` (`COLOR_GAME_POINT`, `COLOR_SET_POINT`,
`COLOR_MATCH_POINT`) y el periodo del titileo en `ICON_BLINK_MS`.

> Detalle de implementación que vale la pena: el firmware no tiene una segunda
> copia de las reglas para decidir si es punto de set o de partido. **Simula el
> próximo punto** con la misma función de transición que usa para avanzar el
> partido (`applyPoint` sobre una copia del estado) y mira qué se cerró. Si las
> reglas cambian, el aviso cambia solo y no se puede desincronizar.

## 5. Cómo se arma cada dígito (el método barato que funciona)

Son 4 capas. El orden importa.

```
   ┌─────────────────────────────────┐  ← 1. FILTRO: acrílico humo o rojo oscuro 2 mm
   ├─────────────────────────────────┤  ← 2. MÁSCARA negra mate 3 mm, con ventanas
   │ ░░░░░ aire 15–20 mm ░░░░░░░░░░░ │
   ├─────────────────────────────────┤  ← 3. DIFUSOR: acrílico opal blanco 3 mm
   │ ███ tabiques negros ███ 20 mm   │     + TABIQUES entre segmentos
   │  ●●●●●●  tira WS2812B           │  ← 4. FONDO: chapa/foam + tira pegada
   └─────────────────────────────────┘
```

1. **Fondo.** Plancha de foam board negro de 3 mm. Dibujá los 7 segmentos con
   lápiz y pegá los tramos de tira con su adhesivo **más una gota de silicona
   neutra en los extremos** (el adhesivo 3M de la tira se despega con el calor).
2. **Tabiques.** Tiras de foam board negro de 20 mm de alto encastradas **entre
   segmento y segmento**. Cada segmento queda en su propia "pileta" de luz. Sin
   esto, los segmentos apagados se iluminan de rebote y un `8` parece un `0`
   borroso a 20 m. **Este paso es el que más se saltea y el que más se nota.**
3. **Difusor.** Acrílico opal blanco de 3 mm apoyado sobre los tabiques, a
   15–20 mm de los LEDs. Menos distancia ⇒ se ven los puntos individuales.
   Alternativa gratis para el prototipo: acrílico transparente + 2 capas de papel
   manteca.
4. **Máscara.** Encima del difusor, una plancha **negra mate** con las ventanas
   de los 7 segmentos (y de los iconos) cortadas con trincheta. Los segmentos
   deben quedar **cortados en punta o chaflán** en las uniones, como un 7
   segmentos real: mejora mucho la lectura del `1` y del `7`.

## 6. Los 5 trucos que hacen que se vea a pleno sol

Esto es lo que diferencia un cartel que se lee de uno que no:

1. **Filtro oscuro adelante (el truco grande).** Un acrílico **humo o rojo oscuro
   con 20–30 % de transmisión** sobre toda la cara frontal. La luz del sol que cae
   sobre el cartel se atenúa **dos veces** (entra y sale: queda en ~5 %), mientras
   que la luz del LED se atenúa **una sola vez** (~25 %). El contraste se
   multiplica por 4–5. Es exactamente lo que hacen los semáforos y los carteles
   de ruta. Con LEDs rojos, un filtro rojo oscuro funciona incluso mejor que el humo.
2. **Negro mate en todo lo que se ve.** Máscara, tabiques, marco. Nada blanco,
   nada brillante, nada gris: el blanco refleja el sol y mata el contraste.
3. **Visera.** Un alero de **60–80 mm** arriba y en los costados. Mantiene la cara
   en sombra buena parte del día, y de paso desvía la lluvia.
4. **Rojo y ámbar, no blanco ni azul.** Máximo contraste cromático sobre fondo
   negro en exteriores. El blanco pleno además consume 3× más y se lee peor. Ya
   está configurado así en `config.h`.
5. **Orientación e inclinación.** Colgalo de la reja del fondo a 2.0–2.5 m,
   **inclinado 10–15° hacia abajo**: la cara deja de reflejar el cielo (la fuente
   de luz difusa más grande), apunta a los ojos de los jugadores y el agua escurre.
   Si se puede elegir el lado, el que **no** recibe sol directo de frente.

Extra: `ENABLE_AUTO_BRIGHT` con un LDR en A0 baja el brillo de noche. A pleno sol
querés 255; de noche 255 encandila y gasta batería al triple.

## 7. Orden de cableado (hay que respetarlo)

La guirnalda es **una sola tira** cortada y re-soldada en serie. El firmware
asume este orden exacto:

```
INICIO (DIN desde el pin D6 del Nano)
 └─ EQUIPO A
     ├─ dígito PUNTOS decena   → segmentos A,B,C,D,E,F,G
     ├─ dígito PUNTOS unidad   → segmentos A,B,C,D,E,F,G
     ├─ dígito JUEGOS          → segmentos A,B,C,D,E,F,G
     └─ dígito SETS            → segmentos A,B,C,D,E,F,G
 └─ EQUIPO B   (idéntico: puntos decena, puntos unidad, juegos, sets)
 └─ CENTRO
     ├─ flecha de SAQUE equipo A
     ├─ flecha de SAQUE equipo B
     ├─ aviso de PUNTO equipo A
     └─ aviso de PUNTO equipo B
FIN
```

Dentro de cada segmento la **dirección no importa** (se enciende completo), pero
el **orden A→B→C→D→E→F→G sí**. Soldá con 3 hilos (5V, DIN, GND) entre segmento y
segmento respetando la flecha impresa en la tira: si invertís un tramo, ese tramo
y todo lo que viene después queda apagado.

**Verificación**: al arrancar, el firmware corre `animBoot()`, que enciende LED
por LED en orden a lo largo de toda la guirnalda. Si el barrido va
A→B→C→…→G dígito por dígito, equipo A, equipo B y termina en los 4 iconos del
centro, el cableado está bien. Es la prueba que conviene hacer **antes** de
cerrar la caja.

## 8. Montaje elegante (fase 2, no ahora)

Para cuando el MVP esté validado: frente de **aluminio compuesto negro mate**
cortado en CNC con las ventanas, difusor de policarbonato opal, filtro de acrílico
rojo oscuro pegado con cinta VHB, tabiques impresos en PLA negro, perfil de
aluminio en el marco y conectores **aviación GX16** para los sensores y la
alimentación. Mismo firmware, misma geometría: `config.h` no cambia.
