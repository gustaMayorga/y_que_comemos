# 02 - Display de 7 segmentos con tira LED, legible a pleno sol

Esta es la parte crítica del MVP. Un cartel LED que se ve espectacular en el
taller puede ser **invisible** a las 3 de la tarde en una cancha al aire libre.
El problema nunca es la cantidad de lúmenes: es el **contraste**.

## 1. Geometría del dígito

Tira de 60 LED/m ⇒ paso entre LEDs = **16.7 mm**.

```
        AAAAAA                 Dígito GRANDE (puntos): 6 LEDs por segmento
       F      B                  segmento ≈ 100 mm
       F      B                  dígito ≈ 120 mm ancho × 230 mm alto
       F      B                  42 LEDs por dígito
        GGGGGG
       E      C                Dígito CHICO (games): 4 LEDs por segmento
       E      C                  segmento ≈ 67 mm
       E      C                  dígito ≈ 85 mm ancho × 150 mm alto
        DDDDDD                   28 LEDs por dígito
```

**¿Alcanza con 23 cm de altura?** Regla práctica de legibilidad para alto
contraste a la luz del día: ~25 mm de altura de carácter por cada 8–10 m de
distancia. 230 mm ⇒ **legible a 70–90 m**. Una cancha de pádel es de 20 × 10 m y
nadie mira desde más de 25 m. Hay margen de sobra, así que si querés abaratar el
primer prototipo podés bajar `LEDS_PER_SEG_BIG` de 6 a **5** (segmento 83 mm,
dígito ~190 mm, legible a ~55 m): son 28 LEDs menos y ~0.5 A menos de consumo.
Lo que **no** conviene es bajar de 4.

## 2. Layout del cartel

```
 ┌──────────────────────────────────────────────────────────────┐
 │  EQUIPO A                                          ● ● ●     │  ← dots de SET (amarillo)
 │   ┌─────┐ ┌─────┐          ┌───┐                             │
 │   │  4  │ │  0  │          │ 3 │   ← games (ámbar)           │
 │   └─────┘ └─────┘          └───┘                             │
 │     puntos (rojo, 23 cm)                                     │
 ├──────────────────────────────────────────────────────────────┤
 │  EQUIPO B                                          ● ● ●     │
 │   ┌─────┐ ┌─────┐          ┌───┐                             │
 │   │  3  │ │  0  │          │ 2 │                             │
 │   └─────┘ └─────┘          └───┘                             │
 ├──────────────────────────────────────────────────────────────┤
 │   [ ◎ A ]                                          [ ◎ B ]   │  ← sensores de paleta
 └──────────────────────────────────────────────────────────────┘
      izquierda = Equipo A                 derecha = Equipo B
```

Dimensión total aproximada del frente: **55 × 40 cm**. Entra en una bandeja de
PVC o una caja estanca de obra y pesa poco (clave: va colgado de la reja con bridas).

El dot de set libre se usa además como **indicador de saque** (verde). Un detalle
que no cuesta un LED extra.

## 3. Cómo se arma cada dígito (el método barato que funciona)

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
   lápiz y pegá los tramos de tira con su adhesivo + una gota de silicona neutra
   en los extremos (el adhesivo 3M solo se despega con calor).
2. **Tabiques.** Tiras de foam board negro de 20 mm de alto encastradas **entre
   segmento y segmento**. Cada segmento queda en su propia "pileta" de luz. Sin
   esto, los segmentos apagados se iluminan de rebote y un "8" parece un "0"
   borroso a 20 m. **Este paso es el que más se saltea y el que más se nota.**
3. **Difusor.** Acrílico opal blanco de 3 mm apoyado sobre los tabiques, a
   15–20 mm de los LEDs. Menos distancia ⇒ se ven los puntos individuales.
   Alternativa gratis para el prototipo: acrílico transparente + 2 capas de papel
   manteca o papel vegetal.
4. **Máscara.** Encima del difusor, una plancha **negra mate** con las ventanas
   de los 7 segmentos cortadas con trincheta/cutter circular. Los segmentos deben
   quedar **cortados en punta o chaflán** en las uniones (como un 7 segmentos real):
   mejora mucho la lectura del "1" y del "7".

## 4. Los 5 trucos que hacen que se vea a pleno sol

Esto es lo que diferencia un cartel que se lee de uno que no:

1. **Filtro oscuro adelante (el truco grande).** Un acrílico **humo o rojo oscuro
   con 20–30 % de transmisión** sobre toda la cara frontal. La luz del sol que cae
   sobre el cartel se atenúa **dos veces** (entra y sale: queda en ~5 %), mientras
   que la luz del LED se atenúa **una sola vez** (~25 %). El contraste se multiplica
   por 4–5. Es exactamente lo que hacen los semáforos y los carteles de ruta. Con
   LEDs rojos, un filtro rojo oscuro funciona incluso mejor que el humo.
2. **Negro mate en todo lo que se ve.** Máscara, tabiques, marco. Nada blanco,
   nada brillante, nada gris. El blanco refleja el sol y mata el contraste.
3. **Visera.** Un alero de **60–80 mm** arriba y en los costados (la misma chapa
   plástica doblada o pegada a 90°). Mantiene la cara en sombra buena parte del
   día, y como bonus desvía la lluvia.
4. **Rojo y ámbar, no blanco ni azul.** El ojo tiene máxima sensibilidad al
   contraste cromático del rojo sobre fondo negro en exteriores. Los WS2812B en
   blanco pleno además consumen 3× más y se leen peor. Están configurados así en
   `config.h` (`COLOR_POINTS`, `COLOR_GAMES`, `COLOR_SETS`).
5. **Orientación e inclinación.** Colgalo en la reja del fondo, a 2.0–2.5 m,
   **inclinado 10–15° hacia abajo**: la cara deja de reflejar el cielo (que es la
   fuente de luz difusa más grande), apunta a los ojos de los jugadores y el agua
   escurre. Si se puede elegir el lado: el que **no** recibe sol directo de frente.

Extra: `ENABLE_AUTO_BRIGHT` con un LDR en GPIO34 baja el brillo de noche. A pleno
sol querés 255; de noche 255 encandila y gasta batería al triple.

## 5. Orden de cableado (hay que respetarlo)

La guirnalda es **una sola tira** cortada y re-soldada en serie. El firmware
asume este orden exacto:

```
INICIO (DIN desde el ESP32)
 └─ EQUIPO A
     ├─ dígito PUNTOS decena   → segmentos A,B,C,D,E,F,G   (6 LEDs cada uno = 42)
     ├─ dígito PUNTOS unidad   → segmentos A,B,C,D,E,F,G   (42)
     ├─ dígito GAMES           → segmentos A,B,C,D,E,F,G   (28)
     └─ dots de SET            → set1, set2, set3          (2 LEDs cada uno = 6)
 └─ EQUIPO B  (idéntico: 42 + 42 + 28 + 6)
FIN
```

Dentro de cada segmento la **dirección no importa** (se enciende completo), pero
el **orden A→B→C→D→E→F→G sí**. Soldá con 3 hilos (5V, DIN, GND) entre segmento y
segmento, respetando la flecha de la tira: si la invertís, ese tramo y todo lo
que viene después queda apagado.

**Verificación**: al arrancar, el firmware corre `animBoot()`, que enciende LED
por LED en orden a lo largo de toda la guirnalda. Si el barrido va A→B→C→…→G
dígito por dígito y de izquierda a derecha, el cableado está bien. Es la prueba
que conviene hacer **antes** de cerrar la caja.

## 6. Montaje elegante (fase 2, no ahora)

Para cuando el MVP ya esté validado: frente de **aluminio compuesto negro mate**
cortado en CNC con las ventanas, difusor de policarbonato opal, filtro de acrílico
rojo oscuro pegado con cinta VHB, tabiques impresos en PLA negro, perfil de aluminio
en el marco y conectores **aviación GX16** para los sensores y la alimentación.
Mismo firmware, misma placa, misma geometría: `config.h` no cambia.
