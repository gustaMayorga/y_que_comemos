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

## 2b. Cuántos LEDs por segmento: la tabla para decidir

A 60 LED/m (paso 16,67 mm), **todos los LEDs tienen la misma separación**, así que
más LEDs por segmento **no es más brillo por centímetro: es más tamaño**. El
brillo por área es idéntico en todos los casos. Lo que cambia es a qué distancia
se lee.

| LEDs/segmento | Segmento | Dígito (alto × ancho) | Legible a |
|:---:|---|---|---|
| 2 | 33 mm | 103 × 57 mm | 30-40 m |
| **3** | **50 mm** | **136 × 74 mm** | **40-55 m** |
| 4 | 67 mm | 169 × 91 mm | 50-65 m |
| 6 | 100 mm | 236 × 124 mm | 70-90 m |

Una cancha de pádel mide 20 × 10 m y nadie mira desde más de 25 m, así que
**con 3 ya sobra**. Y los perfiles del firmware ya están armados así:

| Perfil | puntos / juegos / sets / iconos | LEDs | Dígito de puntos | Sobran de 300 |
|---|---|:---:|---|:---:|
| **`LAYOUT_SPARSE`** | **3 / 2 / 2 / 1** | **144** | **136 mm** | **156** |
| `LAYOUT_DEMO` | 4 / 3 / 2 / 2 | 190 | 169 mm | 110 |
| `LAYOUT_FULL` | 6 / 4 / 3 / 3 | 278 | 236 mm | 22 |

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

### 🏆 El triángulo dividido en dos zonas (la mejor opción)

Siempre hay **dos informaciones por equipo**: el **saque** y el **aviso de
punto**. En vez de dos ventanas separadas, van en **un solo triángulo partido
en dos zonas** por una pared interna:

```
         ╱│                    CUERPO  = aviso de punto
       ╱  │                              (cyan / amarillo / rojo titilando)
     ╱ C  │ P  ◄── punta       PUNTA   = saque (verde)
       ╲  │
         ╲│
          ▲
     pared divisoria
```

Por qué conviene sobre las 4 piezas sueltas:

- **2 piezas en vez de 4**: menos imprimir, menos montar, centro más compacto
- **Se ven las dos cosas a la vez**, igual que con 4 ventanas
- **Agrupado por equipo**: leés el estado de un equipo de un vistazo, en vez de
  buscar en dos columnas distintas
- La punta apunta al equipo, lo que refuerza de qué lado está el saque

El **cuerpo** lleva el aviso porque es la zona grande y es la información más
importante; la **punta**, más chica, lleva el saque.

> ⚠️ **Hace falta una pared interna de verdad entre las dos zonas.** Sin ella
> los dos colores se mezclan por adentro del difusor y queda un color sucio que
> no se entiende. La pieza `icono_celda` del `.scad` ya la trae.

**Cuántos LEDs por zona**: con triángulos de ~40 mm entra **1 por zona** (2 por
triángulo). Para poner **2 por zona** el triángulo tiene que medir unos **70 mm**
de punta a base.

### Las tres configuraciones posibles

```c
// a) RECOMENDADA: 2 triangulos divididos en 2 zonas
#define ICONOS_POR_EQUIPO  2
#define CENTRO_AGRUPADO    true     // cadena: puntaA, cuerpoA, puntaB, cuerpoB

// b) 4 piezas sueltas (2 flechas de saque + 2 triangulos de aviso)
#define ICONOS_POR_EQUIPO  2
#define CENTRO_AGRUPADO    false    // cadena: saqueA, saqueB, avisoA, avisoB

// c) 2 triangulos SIN dividir: uno por equipo, las dos cosas por color.
//    Si hay punto en juego manda el aviso; si no, muestra el saque en verde.
#define ICONOS_POR_EQUIPO  1
```

`CENTRO_AGRUPADO` sólo cambia **el orden en la cadena**, para que puedas soldar
cada triángulo completo antes de pasar al siguiente.

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

## 6b. Cómo cortar la tira (WS2812B 5 V 60 LED/m IP20)

### Antes de cortar: encontrar la ENTRADA

Una tira direccionable **funciona en un solo sentido**. En cada punto de corte
hay 6 pads, tres de cada lado, y están rotulados:

```
       ... ──►  [ LED ]  GND  DO  +5V  │  +5V  DI  GND  [ LED ]  ──► ...
                         ▲ salida      │      ▲ entrada
                                       └── acá se corta
```

**`DI` = Data In (entrada). `DO` = Data Out (salida).** El dato entra por el
lado DI y sale por el DO. Si conectás el Arduino del lado DO, **no enciende
nada** y parece que la tira está fallada.

Mirá el extremo donde vienen los cables de fábrica y fijate qué dice el pad del
medio. Si dice **DI**, ese es el principio. Si dice DO, el principio es el otro
extremo. Algunas tiras además traen **flechas impresas**: apuntan hacia el DO.

> Si no estás seguro, no importa: conectá, corré `tools/diag_leds` y escribí `w`.
> Si no enciende nada, probá el otro extremo. No se rompe nada por probar.

### Los cables de fábrica: verificar, no asumir

Los colores varían según el fabricante. **Antes de conectar, confirmá con el
multímetro en continuidad** contra los pads de la tira:

| Cable | Debería ir al pad | Verificá |
|---|---|---|
| rojo | `+5V` | continuidad con el pad `+5V` |
| blanco o verde | `DI` | continuidad con el pad del medio |
| blanco o negro | `GND` | continuidad con el pad `GND` |

Si vienen **dos pares** (uno fino de 3 hilos y uno grueso de 2), el de 3 es
alimentación + dato y el grueso de 2 es **sólo para inyectar corriente** — ese
par va a la misma bornera de 5 V, en paralelo. No lleva dato.

### La técnica del corte

1. **Contá LEDs, no midas milímetros.** Para un segmento de 6 LEDs: contás 6 y
   cortás después del sexto. Medir con regla acumula error.
2. **Cortá justo por el medio de la línea de cobre**, entre los pads `DO/+5V/GND`
   y `+5V/DI/GND`. Así cada mitad queda con medio pad para soldar. Si cortás
   corrido, un lado queda sin dónde soldar y perdés ese LED.
3. Tijera bien afilada o trincheta con regla. La tira es fina, no hace falta
   fuerza.
4. **Estañá los pads primero** (una gota en cada uno), después apoyás el cable ya
   estañado y calentás. **2 o 3 segundos por pad, no más**: si insistís, el
   cobre se despega del plástico y ese pad se perdió para siempre.
5. Cables entre segmentos **cortos, de 3 a 5 cm**, tres por salto, y siempre el
   mismo código de color (rojo 5V, otro dato, otro GND). Cuando tengas 60 piezas
   cableadas vas a agradecer no haber improvisado los colores.
6. Al ser **IP20** no hay silicona que sacar: una complicación menos.

### El plan de corte completo (`LAYOUT_FULL`, 278 LEDs)

| Bloque | Dígitos | LEDs/segmento | Piezas | LEDs | Largo de cada pieza |
|---|:---:|:---:|:---:|:---:|---|
| **PUNTOS** | 4 | 6 | 28 | 168 | **100 mm** |
| **JUEGOS** | 2 | 4 | 14 | 56 | 66,7 mm |
| **SETS** | 2 | 3 | 14 | 42 | 50 mm |
| **ICONOS** | — | 3 | 4 | 12 | 50 mm |
| **TOTAL** | | | **60 piezas** | **278** | **59 cortes** |

Un rollo de 5 m a 60 LED/m son **300 LEDs**: sobran **22**, o sea unos
**3 segmentos grandes de margen**. Es poco: alcanza para tres o cuatro errores,
no más.

### ⚠️ No cortes las 60 piezas hoy

**Cortá sólo un dígito**: 7 piezas de 6 LEDs = **42 LEDs ≈ 70 cm de tira**.

Lo armás, lo verificás con `diag_leds` comando `s6` + `d` (tiene que contar 0 a 9
bien dibujado) y **lo sacás al sol con el filtro oscuro**. Recién cuando ese
dígito se lea bien al mediodía, cortás el resto. Si no pasa la prueba, te queda
4,30 m de tira entera para usar de otra forma.

## 6c. Silicona: cuándo sí y cuándo arruina la tira

### ⚠️ Primero: que sea NEUTRA, no acética

La silicona de pomo común (la de baño y cocina) es **acética**: al curar libera
**ácido acético**. Ese ácido **corroe el cobre de las pistas y las soldaduras**
de la tira. No falla el primer día: falla a los meses, con segmentos que se
apagan de a uno y no hay forma de arreglarlos.

| Cómo reconocerla | |
|---|---|
| **Acética** ❌ | huele fuerte a **vinagre** al salir del pomo. Dice "acética" o nada |
| **Neutra** ✅ | casi no huele. Dice **"neutra"**, **"sin ácido"**, "para espejos", "para acuarios" |

Si la que compraste huele a vinagre, **no la pongas sobre la tira**. Sirve para
pegar el marco o sellar la caja por fuera, lejos del cobre.

(Si lo que tenés son **barritas de pistola encoladora**, eso es otra cosa:
termofusible. No tiene ácido, pero tampoco difunde y amarillea con el sol. Para
fijar cables está bien, como difusor no.)

### Silicona blanca directo sobre los LEDs: no conviene

Por dos razones, y las dos juegan en contra justo en lo que importa:

1. **No difunde si no hay distancia.** La difusión necesita que la luz se mezcle,
   y eso pasa en el **aire** entre el LED y el difusor. Con una capa de 1 mm
   pegada al LED la luz no tiene dónde mezclarse: se ve igual el puntito de cada
   LED, sólo que más apagado.
2. **La silicona blanca come muchísima luz.** Es opaca, está cargada de pigmento:
   se pierde fácil **la mitad o más** del brillo. En un cartel que tiene que
   leerse a pleno sol, eso es exactamente lo que no se puede regalar.

Resultado: menos luz **y** se siguen viendo los puntos. Lo peor de los dos mundos.

### La regla de la difusión

> **La distancia entre el LED y el difusor tiene que ser parecida a la
> separación entre LEDs.**

A 60 LED/m los LEDs están cada **16,7 mm** ⇒ el difusor va a **15-20 mm**. Esa es
la razón de que los tabiques sean de 20 mm de alto. No es un capricho: es lo que
hace que un segmento se vea como una **barra pareja** y no como 3 puntitos.

### Qué usar como difusor, de lo más barato a lo mejor

| Opción | Costo | Resultado |
|---|---|---|
| **Papel vegetal / manteca**, 2-3 capas | $0 | ✅ sorprendentemente bueno para el prototipo |
| **Carpeta plástica blanca translúcida** (PP) | $0 | ✅ muy bueno, rígido, se corta con trincheta |
| 🏆 **Difusor de panel LED de techo** | ~$ | ✅ **el mejor por precio**: es PMMA opal hecho justo para esto. En casas de electricidad venden el repuesto suelto |
| Acrílico opal blanco 3 mm | ~9 USD | ✅ la solución prolija definitiva |

El difusor de artefacto LED de techo es el dato que más conviene: está diseñado
exactamente para difundir LEDs puntuales, es barato y viene en plancha grande.

### ¿Qué es el PMMA? ¿Se puede imprimir en 3D?

**PMMA = acrílico** (polimetilmetacrilato). El **"opal"** es la versión blanca
lechosa translúcida: deja pasar la luz pero no se ve la fuente. No se puede
imprimir: no existe filamento de PMMA opal usable en una impresora casera.

**Pero sí se puede imprimir un difusor que funciona bien.** Es una técnica
conocida y vos ya tenés la impresora:

| | |
|---|---|
| **Material** | PLA **natural** (el transparente/lechoso) o PLA blanco. **PETG natural** si el cartel va a estar al sol todo el día — el PLA se deforma a 55 °C |
| **Espesor** | **1,0 a 1,5 mm** |
| **Capas** | 0,2 mm (más capas finas difunden mejor que pocas gruesas) |
| **Relleno** | **100 %** — tiene que ser macizo, si no se ven las líneas del relleno |
| **Orientación** | **plano sobre la cama**, nunca de canto |
| **Cama** | la textura de la cama queda impresa en la cara de abajo y **eso difunde más todavía**: ponela hacia los LEDs |

Transmite alrededor del 40-60 % y difunde muy bien. Imprimís **una plancha por
dígito** (74 × 136 mm) y la apoyás sobre los tabiques, a 15-20 mm de los LEDs.

Comparado con el acrílico opal comprado: transmite un poco menos, pero sale
**cero peso** y lo tenés hoy. Vale totalmente la pena probarlo en el primer
dígito antes de salir a comprar nada.

> Lo que **no** sirve: imprimirlo pegado al LED, ni usar relleno menor al 100 %,
> ni PLA de color (sólo natural o blanco).

### Si igual querés silicona: hacé UNA plancha, no 56 moldes

56 moldes es muchísimo trabajo para un resultado peor. Si querés probar, la forma
razonable es **colar una plancha y cortarla**:

1. Conseguí **silicona translúcida neutra** (no blanca: la blanca come demasiada luz).
2. Tomá un vidrio o un acrílico plano y **forralo con cinta de embalar
   transparente**. La silicona **no se pega** al polipropileno de la cinta: ese
   es todo el secreto del desmolde.
3. Pegá dos tiras de **1 mm de espesor** (dos capas de cinta de papel, o alambre
   de 1 mm) como topes, separadas unos 10 cm.
4. Volcá la silicona en el medio y **nivelá con una espátula o una tarjeta**
   apoyada en los dos topes. Queda una plancha de 1 mm pareja.
5. Rociá con **agua con detergente** y alisá con el dedo: no se pega y queda liso.
6. **24 a 48 h de curado**, a la sombra. La silicona cura de afuera hacia adentro
   y necesita humedad del aire.
7. Despegá la plancha de la cinta tirando de una punta y **cortá los 56 pedazos
   con trincheta** y regla.

**Para desmoldar, lo que funciona**: cinta de embalar, polipropileno, polietileno
(una bolsa gruesa), o vaselina fina si el molde es de PLA. Lo que **no**
funciona: PLA desnudo, madera, cartón — se pega y lo arruinás.

Y si imprimís un molde en 3D, dale **2-5° de ángulo de desmolde** en las paredes
y forralo igual con cinta.

> Aun haciéndolo bien, 1 mm pegado al LED **no reemplaza** la distancia de aire.
> Sirve como protección o como capa extra sobre un difusor, no en lugar de él.

### Dónde la silicona SÍ sirve en este proyecto

- **Neutra, translúcida**: sellar los pasacables y las juntas de la caja.
- **Neutra**: fijar los extremos de cada tramo de tira (el adhesivo 3M se despega
  con el calor del sol).
- **Pistola encoladora**: sujetar cables por dentro, lejos de las soldaduras.

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

### En la tira NO hay nada que diga "A"

El hardware no sabe qué segmento es cuál, y no hace falta. La tira sólo tiene
**posición en la fila**: el primer LED después del cable de datos es el 0.

Lo único identificado en la tira son los pads de corte (**+5V**, **DIN**, **GND**)
y una **flecha impresa** que dice para dónde fluye el dato. Nada más.

**"Segmento A" significa simplemente "el primer pedacito de la cadena".** Si lo
pegás arriba, A es el de arriba. Vos creás el mapeo con el orden en que soldás.

### El truco para no equivocarse: vuelta de reloj, y el del medio al final

```
        ①AAAA                  Nano D6 ──→ entra en ①
       ⑥    ②                  ① sale ──→ entra en ②
       ⑥    ②                  ② sale ──→ entra en ③
        ⑦GGGG                  ③ sale ──→ entra en ④
       ⑤    ③                  ④ sale ──→ entra en ⑤
       ⑤    ③                  ⑤ sale ──→ entra en ⑥
        ④DDDD                  ⑥ sale ──→ entra en ⑦
```

Es un círculo en sentido de las agujas del reloj por los 6 segmentos de afuera
arrancando arriba, y al final te metés al del medio. Los saltos de cable quedan
cortos y es fácil de recordar.

Regla única al soldar: **la SALIDA del pedacito anterior va a la ENTRADA del
siguiente**, siguiendo la flecha. Tres cables por salto (5V, datos, GND).

Dentro de cada segmento la **dirección no importa** (se enciende completo).
Soldá con 3 hilos (5V, DIN, GND) entre segmento y segmento respetando la flecha
impresa en la tira: si invertís un tramo, ese tramo y todo lo que viene después
queda apagado.

### El orden de los segmentos es CONFIGURABLE

No hace falta soldar en A→B→C→D→E→F→G. Soldá en el orden que te quede cómodo y
después se lo decís al firmware en `config.h`:

```c
// a) VUELTA DE RELOJ (el que recomendamos)
#define CADENA_SEGMENTOS { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G }

// b) ZIGZAG DE LECTURA: arriba, los dos de arriba, el medio,
//    los dos de abajo, abajo. Es el que sale natural numerando como se lee.
#define CADENA_SEGMENTOS { SEG_A, SEG_F, SEG_B, SEG_G, SEG_E, SEG_C, SEG_D }
```

El zigzag de lectura es este, que es el que suele salir solo al dibujar:

```
    1 1        posicion 1 -> A (arriba)
  2     3      posicion 2 -> F (arriba izquierda)
    4 4        posicion 3 -> B (arriba derecha)
  5     6      posicion 4 -> G (el del medio)
    7 7        posicion 5 -> E (abajo izquierda)
               posicion 6 -> C (abajo derecha)
               posicion 7 -> D (abajo)
```

**Si no te acordás cómo quedó**, no importa: `tools/test_segmentos` enciende la
cadena posición por posición y anotás qué segmento prendió en cada una. Con esa
tabla completás `CADENA_SEGMENTOS` y los números salen bien dibujados.

**Si quedó cruzado te das cuenta en 30 segundos**: `tools/diag_leds` comando `w`
enciende el LED 0, el 1, el 2... Si los tres primeros prenden el segmento de
arriba, A quedó bien. Si prenden otro, o lo desoldás y reordenás, o se cambia el
mapeo en el firmware. El comando `d` después cuenta 0-9: si los diez números
salen bien dibujados, el cableado está perfecto.

### Verificar que los tabiques no dejan pasar luz

Con el primer dígito armado y el difusor puesto: `tools/test_segmentos` → `1`
(prende sólo el segmento de arriba) y mirá los **segmentos vecinos**. Si brillan
aunque estén apagados, hay fuga de luz entre celdas y un `8` va a parecer un `0`
borroso a 20 m. Se tapa con una tira de cartón negro o cinta negra mate en la
junta. Mejor descubrirlo ahora que con los 8 dígitos pegados.

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
