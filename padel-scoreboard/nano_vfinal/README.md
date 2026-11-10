# 🏟️ Tanteador multideporte — **versión final Nano**

Todo en una carpeta: **firmware + esquemático + instrucciones**.
Panel LED de **23 × 12 = 276 píxeles**, **8 deportes**, control remoto y
sensores de paleta.

**Ruta:** `padel-scoreboard/nano_vfinal/`

| Archivo | |
|---|---|
| `nano_vfinal.ino` | el firmware completo |
| `ESQUEMATICO.md` | el cableado, pin por pin |
| `README.md` | esto |

```bash
arduino-cli lib install FastLED
arduino-cli compile --fqbn arduino:avr:nano nano_vfinal
arduino-cli upload  --fqbn arduino:avr:nano -p /dev/ttyUSB0 nano_vfinal
```

---

## ✅ Qué trae esta versión

- **8 deportes** con reglas reales (pádel oro y ventajas, tenis 3 y 5 sets,
  vóley, ping pong, básquet, fútbol)
- **Control remoto 433 MHz con grabación de llaveros**: receptor crudo
  MX-RM-5V/RXB6, decodificador EV1527 propio, 2 llaveros × 3 botones,
  filtro de ruido y guardado en EEPROM
- **4 tipos de sensor de paleta** seleccionables: TCRT5000, FC-51,
  dos HC-SR04, o **un solo HC-SR04 por zonas**
- **Encendido automático por presencia** con RCWL-0516 (opcional)
- Indicador de saque **al lado de cada equipo** (convención profesional)
- Aviso de punto de juego / set / partido en la columna del centro
- Deshacer de 6 pasos que restaura el estado completo
- Festejo con fuegos artificiales y `GANA A` / `GANA B` en texto corrido

## 🏅 Los 8 deportes

Se cambia con un **pulso corto en el botón de deporte (D12)**. Muestra la letra
y el número del modo, y resetea el tanteo.

| # | Deporte | Cómo cuenta |
|:-:|---|---|
| **0** | **Pádel — punto de oro** | 15/30/40, en 40-40 **un punto decide**. Set a 6 (7-5 si va 5-5), tie break a 7. Al mejor de 3 |
| **1** | **Pádel — ventajas** | igual, pero 40-40 → **`Ad`** y vuelta a iguales |
| **2** | **Tenis — 3 sets** | con ventajas |
| **3** | **Tenis — 5 sets** | con ventajas, al mejor de 5 |
| **4** | **Vóley** | 25 con 2 de diferencia · al mejor de 5 · **5º set a 15** · side-out |
| **5** | **Ping pong** | 11 con 2 de diferencia · al mejor de 5 · saque **cada 2 puntos**, y **cada 1 desde 10-10** |
| **6** | **Básquet** | contador libre hasta 999 |
| **7** | **Fútbol** | contador libre |

### Lo que implementa de las reglas de pádel

- ✅ 15 / 30 / 40, juego al 4º punto con 2 de diferencia
- ✅ **40-40**: punto de oro **o** ventaja (`Ad`) con vuelta a iguales — es el
  deporte 0 o el 1
- ✅ Set a **6 con 2 de diferencia**; si llega a **5-5 se extiende a 7-5**
- ✅ **6-6 → tie break**: el tanteo pasa a números corridos (0,1,2…), se gana a
  **7 con 2 de diferencia**, y una **barra cyan** avisa que estás en tie break
- ✅ **Rotación del saque en el tie break**: el primero saca **1 punto** y después
  **cada uno saca 2**
- ✅ El saque **pasa al rival al cerrarse cada juego**
- ✅ Al mejor de **3 sets**

### Cómo se muestra (convención de marcador profesional)

```
   ┌─────────────────────────┬─┬─────────────────────────┐
   │   EQUIPO A              │ │              EQUIPO B   │
   │   ┌────┐┌────┐          │ │          ┌────┐┌────┐   │
   │   │ 4  ││ 0  │  PUNTOS  │▮│  PUNTOS  │ 3  ││ 0  │   │
   │   └────┘└────┘          │ │          └────┘└────┘   │
   │   [3] [1]  ●            │ │            [2] [0]  ●   │
   │   jue set saque         │ │         jue set saque   │
   └─────────────────────────┴─┴─────────────────────────┘
                    ▮ = aviso de punto
```

- **Cada equipo tiene su lado fijo.** El tanteo "del que saca primero" es para
  **cantarlo**, no para el marcador: los displays profesionales usan posición
  fija + indicador de saque, y así está hecho.
- **● indicador de saque**: un punto verde **al lado del equipo que saca**,
  igual que la pelotita de los marcadores de TV. Cambia solo al cerrarse el juego.
- **▮ aviso de punto** en la columna del centro:
  🔵 punto de **juego** · 🟡 punto de **set** · 🔴 **punto de partido** (titila)
- **Tie break**: desaparecen los juegos y aparece una **barra cyan**; el tanteo
  pasa a números corridos.
- **Fin de partido**: el perdedor se apaga, fuegos artificiales, ola de color y
  **`GANA A` / `GANA B`** en texto corrido.

---

## 🎮 Controles

### Control remoto — hay que **grabarlo** primero

El receptor MX-RM-5V no decodifica: el firmware aprende el código de cada botón.
Se graban **2 llaveros de 3 botones** cada uno, y los dos hacen lo mismo.

| Botón grabado | Hace |
|:-:|---|
| **1º** | **+1 punto Equipo A** |
| **2º** | **+1 punto Equipo B** |
| **3º** | **DESHACER** (6 pasos) |

El orden lo define **cómo los grabás**, no qué botón del llavero sea.

El deshacer restaura el estado completo: también deshace un juego, un set o el
partido si ese punto los había cerrado. Por eso con 3 botones alcanza — no hace
falta un cuarto.

### 🔴 Cómo grabar un control

1. **Mantené apretado el botón DEPORTE 15 segundos.** Una **barra ámbar** se va
   llenando en el panel para que sepas que está contando.
2. Suena **bip bip biiip** y el panel muestra **`C1`** (o `C2`) y el número de
   botón a grabar.
3. **Apretá el botón 1 del llavero** → **bip bip** ✔
4. **Apretá el botón 2** → **bip bip** ✔
5. **Apretá el botón 3** → **biiiiiiiip** y sale solo. Listo.

Todos los bips **prenden también el LED de la placa** (D13), así que podés
grabar mirando el Arduino aunque el panel esté tapado.

- La **primera vez** ocupa la dirección del **control 1**; la **segunda vez**, la
  del **control 2**. Después vuelve a empezar por el 1.
- Si apretás dos veces el mismo botón, **lo ignora** y sigue esperando otro.
- Si pasan **30 segundos** sin apretar nada, **cancela** (bip grave) y no toca
  nada de lo ya grabado.
- Los códigos quedan en la **EEPROM**: sobreviven al apagado y hasta a recargar
  el sketch.

Por el puerto serie vas viendo todo, y si apretás un llavero sin grabar te dice:
`[rf] codigo sin grabar: 12345678`

### Botones de la caja

| | |
|---|---|
| **DEPORTE** (D12) | corto: **cambia de deporte** · **15 s: grabar controles** |
| **RESET** (A1) | corto: **ciclar brillo** · 2 s: **resetea el partido** (barra roja) |

### Sensores de paleta

**Izquierdo = equipo A · derecho = equipo B.** Siempre activos, en paralelo con
el control. Tres tipos soportados, se elige con `SENSOR_TIPO`:

| | Alcance | Al sol | Interacción |
|---|---|---|---|
| **1 · TCRT5000** crudo | 5-15 mm | ⚠️ sólo si va embutido | **apoyar** la paleta |
| **2 · FC-51** módulo | 2-30 cm | ❌ se ciega | acercar |
| **3 · dos HC-SR04** | 2-400 cm | ✅ inmune | acercar a 20 cm · **el mejor** |
| **4 · UN HC-SR04** | dos zonas | ✅ inmune | **cerca = A · lejos = B** |

**Cableado de los HC-SR04** — la A y la B son el **equipo**, no una pata:

| | Equipo A | Equipo B |
|---|---|---|
| **TRIG** | A2 | A3 |
| **ECHO** | **D3** | **D5** |

Con **un solo sensor** usás nada más que la columna de la izquierda: `TRIG → A2`
y `ECHO → D3`.

### Modo 4: un solo sensor, dos zonas

Con un único HC-SR04 **no se puede saber de qué lado vino la paleta**: mide
distancia, no ángulo ni dirección. Pero sí se puede partir el alcance en dos:

```
   |<---- EQUIPO A ---->|  muerta  |<-------- EQUIPO B -------->|
   4                   20 cm      27                          55 cm
```

La **banda muerta** entre 20 y 27 cm está a propósito: evita dudas en el límite.
Y hay que **retirar la paleta** antes de que vuelva a contar, así pasar de la
zona B a la A no suma dos puntos.

```c
#define ZONA_A_MIN  4
#define ZONA_A_MAX 20
#define ZONA_B_MIN 27
#define ZONA_B_MAX 55
```

Funciona bien y se aprende en dos puntos, pero **el segundo HC-SR04 cuesta
1,50 USD** y con dos sensores la interacción es obvia sin explicar nada:
izquierda = A, derecha = B. Si podés, comprá el segundo.

Con el **TCRT5000** la lectura es analógica y **se autocalibra**: va siguiendo su
propio nivel de reposo, así que se adapta a la luz que cambia durante el día.
Al arrancar imprime las lecturas en reposo para que ajustes `TCRT_UMBRAL`.

Antirrebote de **3 barreras** para que no cuente de más: hay que **sostener la
paleta 250 ms**, después queda **bloqueado 2,5 s**, y además **hay que retirar
la paleta** para que vuelva a contar.

---

## ⚙️ Ajustes rápidos en `nano_vfinal.ino`

```c
#define DEPORTE_INICIAL     0   // con cuál arranca
#define RF_CONTROLES        2   // cuántos llaveros se pueden grabar
#define GRABAR_HOLD_MS  15000   // cuánto mantener DEPORTE para grabar
#define SENSOR_TIPO         1   // 1 TCRT5000 · 2 FC-51 · 3 HC-SR04
#define TCRT_UMBRAL        90   // cuánto tiene que cambiar la lectura (0-1023)
#define PUNTO_SAQUE_ANCHO   2   // tamaño del indicador de saque
#define MILIAMPERES      2600   // el amperaje real de tu fuente
#define BRILLO_DIA        200
#define SENS_SOSTENER_MS  250   // calibrar EN LA CANCHA
#define SENS_BLOQUEO_MS  2500
```

Si cambiaste el cableado del panel:

```c
#define SERPENTINA     true
#define FILA0_INVERTIDA false
#define ORIGEN_ARRIBA  true
```

Para saber cuáles van, corré `tools/test_cartel` y mirá la fase de esquinas.

---

## 📐 Por qué el saque no va al centro

Con 23 columnas, fuente 5×7 y 1 columna de separación entre dígitos:

```
  equipo A : 0-4 | 5 | 6-10      = 11 columnas
  CENTRO   : 11                  =  1 columna   ← todo lo que queda
  equipo B : 12-16 | 17 | 18-22  = 11 columnas
                                   ─────────────
                                   23  ✓
```

Una flecha de 3 columnas en el centro **se pisa con el último dígito de A y el
primero de B**. Por eso el saque se movió al bloque de cada equipo —que además
es como lo hacen los marcadores profesionales— y la única columna libre del
centro quedó para el aviso de punto.

Si en el futuro querés la flecha grande al centro, hay dos caminos: pasar a
**fuente 4×7** (deja 5 columnas libres) o **ampliar el panel a 27 columnas**.

---

## 💾 Memoria

El Nano tiene 2048 bytes. El framebuffer son **828** (276 LEDs × 3) y el resto
del firmware ronda los 400. Al arrancar imprime por serie:

```
  RAM libre: 6xx
```

**Por debajo de 300 bytes libres un AVR se vuelve inestable** y falla de formas
raras (se reinicia solo, el display se congela). Si te pasa, bajá `UNDO_PASOS`
o achicá el panel.

---

## 🌙 Encendido automático por presencia (opcional)

Si tenés un **RCWL-0516**, el cartel se enciende solo cuando llega alguien y se
apaga cuando la cancha queda vacía.

```c
#define PIN_PRESENCIA   4     // poné 4 sólo si lo conectás
#define DORMIR_MIN     10     // minutos sin movimiento
```

> **Viene desactivado (`-1`).** Si no tenés el sensor, no toques nada: el cartel
> queda siempre encendido y listo.

Es un **radar Doppler de microondas**: detecta movimiento a 5-9 m y **atraviesa
plástico y madera**, así que va **adentro de la caja sellada**, sin agujeros.

**No sirve para contar puntos**: no distingue equipos, no mide distancia y se
dispara con cualquiera que pase. Para eso van los HC-SR04.

## 🔁 El cartel se reinicia solo al encender

**Primero mirá el monitor serie.** Si el banner `[cartel] tanteador multideporte`
se repite una y otra vez, son reinicios de verdad. Si aparece **una sola vez** y
lo que parpadea son los LEDs, el micro no se reinicia: es la tira (ver el
último punto).

El sketch ahora te dice **por qué** arrancó:

```
  arranque: BORF <- TENSION BAJA, es la fuente!
```

| Lo que dice | Qué significa |
|---|---|
| `PORF` | Arranque normal, se le dio corriente. Si se repite, la fuente **corta y vuelve** |
| `BORF` | **Brown-out**: la tensión bajó de 4.3 V. Es alimentación, seguro |
| `EXTRF` | Entró por el **pin RESET**: cable suelto, ruido o el botón |
| `WDRF` | Watchdog. No lo usamos, así que sería un bootloader raro |
| `el bootloader borro el motivo` | Normal en algunos bootloaders del Nano; usá el resto de la tabla |

### Si separaste la alimentación del Nano y de los LEDs

Separar los **positivos** está bien. Pero las **masas tienen que seguir unidas**,
y unidas en el lugar correcto. Si quedaron separadas, la línea de datos no
tiene retorno: el WS2812 ve basura y el pin `D6` del Nano busca camino por
donde no debe.

**Dónde unirlas: en el negativo de la fuente de los LEDs, en la fuente.**
Nunca al final de la tira.

```
   fuente LEDs  ──── + ────────────┬──────────── tira (inyectar también acá) ─┐
                │                  │                                          │
                └─ − ──┬───────────┴────── masa de la tira ───────────────────┘
                       │
                       └──── GND del Nano   ← acá, en la FUENTE
```

Y acá está la explicación de tu medición. Esos **0.5 V de caída** no están
todos en el cable del `+`: se reparten entre el positivo **y el negativo**.
O sea que la masa del final de la tira está unos **0.25 V por encima** de la
masa de la fuente, y ese valor **cambia con cada cuadro** que mostrás. Si el
Nano toma su masa de ahí, toda su referencia se mueve 0.25 V al ritmo de los
LEDs: eso es ruido inyectado directo en el micro, y da exactamente resetes
aleatorios y datos corruptos.

### Por qué 4.35 V importa, y por qué "se quedó bloqueado"

El ATmega328P **a 16 MHz necesita 4.5 V como mínimo**. No es un margen de
cortesía: es la hoja de datos. Por debajo de eso el micro no se apaga ni se
reinicia prolijo — **ejecuta basura**.

Y acá está la trampa: el detector de brown-out del Nano viene configurado en
**2.7 V**. Entre 2.7 V y 4.5 V hay una ventana en la que el micro está fuera
de especificación **pero el brown-out no lo resetea**. Resultado: se cuelga.
Eso es tu *"quedó bloqueado"*.

Para los LEDs, en cambio, 4.35 V es tolerable (el WS2812B arranca desde
~3.5 V). Lo único que vas a notar es el color: el azul es el primero en caer,
así que el blanco se va a amarillo. **La caída te molesta más por el micro que
por los LEDs.**

### Medir sin instrumental: el Nano se mide a sí mismo

No hace falta tester ni osciloscopio. El 328P puede comparar su referencia
interna de 1.1 V contra su propia alimentación y calcular cuánto tiene. Y el
mínimo **se guarda en una zona de RAM que sobrevive al reinicio** (la RAM solo
se borra al cortar la corriente, no al resetear), así que vas a ver la tensión
que había **justo antes** del reset que te dejó a ciegas:

```
[cartel] tanteador multideporte
  arranque: BORF <- TENSION BAJA, es la fuente!
  Vcc ahora: 4.920 V   minimo visto: 4.310 V   reinicios: 7
  !! por debajo de 4.5 V el 328P a 16 MHz esta FUERA DE SPEC
```

- **`reinicios`** cuenta desde la última vez que le cortaste la corriente. Si
  sube solo, se está reiniciando aunque no lo veas.
- **`minimo visto`** es el dato que buscás. Si baja de 4500, el problema es
  alimentación y no hay nada más que discutir.
- Si `minimo visto` se mantiene arriba de 4.7 V y aun así se resetea, **no es
  la tensión**: mirá `arranque:`. Si dice `EXTRF`, es el pin RESET (ruido o
  cable); si dice `PORF` repetido, es la fuente que corta y vuelve.

**Calibración:** esa referencia de 1.1 V viene sin ajustar de fábrica (±10 %),
así que el valor absoluto puede estar corrido. Comparalo **una vez** con el
tester en reposo y quedate con las variaciones, que ésas sí son exactas.

### Entra solo a grabar controles, o cambia de deporte solo

```
[deporte] 1
[grabar] control 1
  apreta el boton 1
```

**Eso no es ruido ni es un bug: es `D12` leído como apretado.** Para que
dispare `largo()` el pin tiene que estar en bajo **15 segundos seguidos**, y el
antirrebote exige 30 ms estables antes de aceptar cualquier cambio. Ninguna
interferencia aguanta eso. **Algo mantiene `D12` a masa.**

El banner y la telemetría ahora te lo dicen:

```
  !! D12 (DEPORTE) se lee APRETADO sin que nadie lo toque
[salud] loops/s 1272  rf/s 2388  btn 10  Vcc 4.728 V  min 4.555 V
                                     ▲▲
                                     ││ A1  (reset del partido)
                                     └┴ D12 (deporte)   1 = apretado
```

Si ves `btn 10` sin tocar nada, ahí está tu problema. Buscalo en este orden:

1. **Desenchufá el cable de `D12`.** Sin nada conectado, el pull-up interno lo
   deja en alto y no puede disparar. Si los fantasmas paran, es el botón o su
   cable.
2. **Cable suelto tocando masa**, o botón trabado. Es lo más común y lo más
   fácil de descartar: mirá `btn` mientras movés el cable con la mano.
3. **Pull-up interno demasiado flojo.** Son 30-50 kΩ. Con un cable largo, un
   **pull-up externo de 10 kΩ** de `D12` a `5V` más un **capacitor de 100 nF**
   de `D12` a `GND` lo endurece muchísimo. Acá el capacitor **sí va** (son
   botones de persona, 1 ms de constante de tiempo no se nota) — al contrario
   del pin `RESET`, donde rompería la programación.
4. **`D11` es el buzzer y está al lado de `D12` en el zócalo.** Si los cables
   van juntos, el buzzer conmutando acopla ahí. Separalos.

**Protección en firmware (ya incluida):** si un pin de botón se lee apretado
**al arrancar**, no es nadie apretando — es un cable suelto o ruido. El botón
queda **bloqueado hasta que se vea suelto una vez**, así un pin clavado en bajo
no puede disparar nada nunca. Es lo que hace la diferencia entre un pin trabado
que te borra los controles y uno que solo te avisa.

### Se clavó con `loops/s` normal en la última línea

```
[salud] loops/s 1593  rf/s 3158  Vcc 4.728 V  min 4.555 V
                                          ← y nada mas, hubo que apretar RESET
```

El loop corría a full 3 segundos antes de morir y paró de golpe. Eso es un
**cuelgue**, no un reset, y la telemetría no lo puede reportar: **sale del
loop**. Si el loop se detiene, la última línea que ves es siempre normal.

Hay tres guardias para eso, y las tres **avisan** en vez de tapar:

> ⚠️ **Toda espera que pueda durar más de 8 s tiene que llamar a `LATIDO()`.**
> Si no, el watchdog la corta por la mitad. Pasó con `grabarControles()`, que
> espera hasta 30 s por cada botón: el watchdog la mataba a los 8 s y parecía
> un reset espontáneo. Ya está, pero tenelo en cuenta si agregás esperas.

**1. Watchdog (`WATCHDOG true`).** Si el micro se cuelga, se reinicia solo a
los 8 s. No tapa nada: el cartel **vuelve solo, sin tocar el botón**, y eso
mismo es la prueba de que se colgó. Mirá `estuvo vivo N s` para el patrón.
Se arma como último paso del `setup()`, después de todo el arranque lento.

> Si tu bootloader es viejo, un reset por watchdog puede dejar la placa en
> bucle. **Se recupera cortándole la corriente** (un corte limpia el flag).
> Si te pasa, poné `WATCHDOG false`.

**2. Guardia de tormenta de RF.** El punto ciego real: si el receptor entrega
ruido muy rápido, la ISR se come todo el CPU, **el loop no corre nunca más** y
el cartel parece colgado — y la telemetría no lo puede denunciar porque también
sale del loop. Ahora **la ISR se vigila a sí misma**: si pasa de
`RF_TORMENTA` flancos/s (50.000 por defecto, contra 2.000-4.000 normales) se
desactiva sola, avisa y se reactiva a los 10 s.

```
[rf] TORMENTA de ruido: interrupcion desactivada 10 s
[rf] interrupcion reactivada
```

Si ves eso, el culpable del cuelgue era el RF y la solución es la antena de
17.3 cm, alejarlo de la fuente, o un RXB6 superheterodino.

**3. Tope en la espera del ADC.** `leerVcc()` esperaba la conversión con un
`while` sin salida. Una conversión son ~340 vueltas; ahora corta a las 60.000 y
devuelve 0. Un ADC que no responde ya no puede colgar el cartel — irónico,
porque era mi propio medidor el que podía provocar lo que estaba midiendo.

### Reset instantáneo con todo lo interno sano → mirá el pin RESET

Si la telemetría muestra esto justo antes de un reinicio:

```
[salud] loops/s 1290  rf/s 1400  Vcc 4.728 V  min 4.593 V
```

…entonces **el reset no viene de adentro del micro**. `loops/s` estable
descarta que algo le robe el CPU; `min` que no baje descarta hundimientos de
tensión; `rf/s` bajo descarta la ISR del RF. Si además el reinicio llega sin
degradación previa y sin relación con un punto o un bip, queda **el pin
`RESET`**.

**Y el camino más probable es el USB.** En el Nano, la línea **DTR** del
conversor serie está atada al `RESET` a través de un capacitor de 100 nF: así
es como el IDE resetea la placa para programarla. Con el monitor serie abierto,
DTR está activo. **Un cable USB marginal o movido glitchea DTR y la placa se
resetea al instante**, sin que nada adentro del micro se entere. Encaja con
todo: instantáneo, aleatorio, la RAM intacta, y ninguna señal eléctrica previa.

**Prueba decisiva (gratis): desenchufá el USB.** Alimentá el Nano solo por el
pin `5V` y mirá el panel. Cada arranque muestra la letra del deporte 1.3 s:
**ésa es la firma visible de un reinicio.** Si sin USB deja de reiniciarse,
era el USB.

Si confirmás que es ruido en `RESET`, el endurecimiento es un **pull-up de
10 kΩ de `RESET` a `5V`**. Refuerza el pull-up interno (que son 30-50 kΩ, muy
flojo) y **no** impide programar, porque el capacitor de DTR sigue
funcionando. No le pongas un capacitor a GND: eso sí rompe la programación.

### Cuánto estuvo vivo

El banner informa el uptime previo:

```
  estuvo vivo 47 s antes del ultimo reset
```

Sirve para buscar patrón. **Siempre el mismo número** = algo periódico
(desborde, timeout, un ciclo de la fuente). **Números dispares** = ruido o algo
externo. Un solo dato no dice nada: hacen falta varios.

### Se cuelga (no se reinicia): mirá `loops/s`

Un cuelgue y un reset son problemas distintos. Si se **reinicia**, `reinicios`
sube. Si se **cuelga**, el micro sigue vivo pero el loop no avanza, y ahí el
medidor de Vcc tampoco sirve: `vccMin` solo se actualiza mientras el loop
corre. Si el loop se detuvo, no mide nada.

Por eso ahora hay telemetría cada 3 segundos:

```
[salud] loops/s 4120  rf/s 2400  Vcc 4.708 V  min 4.555 V
```

| `loops/s` | Qué significa |
|---|---|
| **miles** | Normal |
| **decenas** | Algo le está robando el CPU |
| **0, o la línea deja de salir** | Cuelgue de verdad |

Y `rf/s` dice **quién** se lo roba. El receptor MX-RM-5V es
**superregenerativo**: sin señal, su salida es ruido amplificado, y entrega
flancos permanentemente. Cada flanco llama a la ISR, y la ISR tarda unos
microsegundos. Las cuentas:

| `rf/s` | CPU que se come la ISR |
|---|---|
| 2.000 | ~2 % — inofensivo |
| 50.000 | ~40 % — el cartel va lento |
| 150.000 | **~100 % — el loop no corre nunca: parece colgado** |

El ruido ambiente cambia todo el tiempo (un portón, un motor, una fuente
conmutada, **tu propia fuente de los LEDs**), así que esto aparece y
desaparece sin motivo aparente. Es exactamente el síntoma de "a veces se
bloquea".

**Prueba decisiva:** `#define PIN_RF_DATA -1`. Si deja de colgarse, era la ISR
del RF ahogando al loop. Las soluciones, en orden: ponerle **antena** de
17.3 cm al receptor (un receptor sin antena es el que más ruido entrega),
alejarlo de la fuente conmutada y de la tira, o pasar a un
**superheterodino RXB6**, que tiene squelch y no entrega nada sin señal.

Para desactivar la telemetría cuando ya no la necesites: `#define SALUD_MS 0`.

### Lo que NO es

- **El cristal.** Si el oscilador estuviera mal, el baud rate se correría y el
  monitor serie mostraría basura. Texto legible = oscilador dentro del 2 %.
- **`millis()` perdiendo tiempo por FastLED.** Es cierto que `show()` apaga las
  interrupciones 8.3 ms para 276 LEDs, y en ese rato `millis()` no cuenta. Pero
  el loop solo redibuja cuando algo cambia, así que en reposo no pasa.
  Con aviso de punto titilando son 8.3 ms cada 150 ms: un 5 %, irrelevante.

### Bisecar: apagar cosas hasta que pare

Dos interruptores en el config, para aislar la causa en dos cargas:

| Poné | Qué apaga | Si los resets paran, era |
|---|---|---|
| `#define PIN_BUZZER -1` | El buzzer, entero | El buzzer: pico inductivo o exceso de corriente en el pin |
| `#define PIN_RF_DATA -1` | El receptor RF y su interrupción | La ISR del RF ahogando al loop (mirá `rf/s` primero) |
| `#define SENSOR_TIPO 0` | Los sensores | El HC-SR04 o su cableado |

El buzzer es el primer sospechoso cuando el reset llega **justo después de un
punto**. Si es magnético (una bobina), engancharlo directo a un pin del Nano
es pedirle 30-80 mA cuando el pin da 40 mA como máximo, y cada ciclo genera un
pico inductivo. Necesita un **1N4148 en paralelo** (cátodo al positivo) y, si
es de los que consumen, un transistor. Si es piezoeléctrico es capacitivo y
mucho más benigno.

### El punto ciego del medidor de Vcc

`vigilarVcc()` se llama una vez por vuelta del loop. **Durante un `delay()` no
se muestrea nada**, y el bip era justo un `delay(50)`. O sea: un `minimo
visto` sano no descartaba la alimentación, porque el medidor estaba ciego en
el momento exacto de más riesgo.

Ya está arreglado: `bip()` y todas las esperas largas usan `esperar()`, que
sigue midiendo mientras espera. Si ahora `minimo visto` sigue arriba de
4.7 V con resets, ahí sí el problema **no es la tensión**.

Lo que el medidor **nunca** va a ver, por más que muestree, es un pico de
microsegundos: el ADC tarda ~100 µs por conversión. Para eso hace falta un
osciloscopio, o directamente descartar por bisección.

### Qué disparó el sensor

Cuando un sensor suma un punto ahora dice a qué distancia vio algo:

```
[sens] 12 cm -> A
pt A  j 0-0  s 0-0
```

Si los centímetros son plausibles, el sensor funciona y lo que falta es
reacomodar las zonas. Si imprime valores saltando por todos lados, es
cableado o el `ECHO` al aire.

### Sobre poner un watchdog

Se podría, y recuperaría solo de los cuelgues. **No lo activé a propósito**, por
dos razones: taparía justo el síntoma que estás tratando de encontrar, y con el
bootloader viejo del Nano un reset por watchdog deja la placa en un bucle del
que no sale ni reprogramando fácil. Cuando la alimentación esté sana lo
agregamos.

### Las causas, en orden de probabilidad

**1. El Nano alimentado por `VIN` con 5 V.** El `VIN` pasa por el regulador
interno, que necesita **7 V como mínimo**: con 5 V entrega ~3.5 V y el micro
entra en brown-out. Con 5 V hay que entrar por el **pin `5V`**, no por `VIN`.
Es la causa número uno y explica perfecto el *"a veces arranca bien"*.

**2. La fuente corta por pico de arranque.** El capacitor de 1000 µF
descargado es un cortocircuito en el instante del encendido. Muchas power
bank y cargadores USB-C ven ese pico, cortan, el cap se descarga, vuelven a
intentar: **bucle**. El estándar USB admite 10 µF, no 1000. Probá
arrancar **sin el capacitor**: si arranca siempre, es eso. La solución es
bajarlo a **470 µF**, que sigue haciendo su trabajo.

**3. `MILIAMPERES` más alto de lo que da la fuente.** Está en 2600. Un power
bank de 2 A no lo aguanta, y además el Nano, el sensor y el RF comen aparte
sin que FastLED los cuente. **Bajalo a 1400 y probá.** Si arranca, el problema
era ése y de ahí lo vas subiendo.

**4. La power bank se apaga sola.** Casi todas cortan por debajo de ~75 mA.
Con el tanteador en 0-0 el panel consume poquísimo → corta → el cap aguanta un
instante → vuelve → **bucle**. Probá con un cargador de pared: si con el
cargador no pasa, era la power bank.

**5. Un solo punto de inyección, o cables finos.** Con 276 LEDs, los 5 V tienen
que entrar por los **dos extremos** de la tira, con cable de 1 mm² o más. Si el
Nano toma los 5 V del final de la tira, ve la tensión ya caída.

**6. USB y 5 V externos conectados a la vez.** Desconectá uno.

**7. Si el micro NO se reinicia y es la tira la que parpadea:** falta la
resistencia **de 330 Ω en serie con la línea de DATOS**, entre `D6` y el
primer LED. Que el cable sea corto no la hace opcional: sin ella la entrada
del primer LED tiene ringing y la tira se corrompe. Y la masa del Nano y la de
la tira tienen que estar unidas en **un solo punto**.

### Lo que ya hace el firmware

- Apaga la tira en el primer instante del `setup()`, antes de cualquier otra cosa
- Arma el cuadro con el brillo en 0 y después lo **sube escalonado** (`arranqueSuave()`),
  así la corriente no entra de golpe
- Desarma cualquier watchdog que haya quedado armado

Eso ayuda, pero **no reemplaza** arreglar los puntos 1 a 3: el bootloader del
Nano tarda ~1 s antes de que el sketch corra, y en ese tiempo el firmware no
puede hacer nada.

## 🔧 Problemas al compilar

### `No space left on device` en `AppData\Local\Temp`

```
fatal error: error writing to C:\Users\...\AppData\Local\Temp\ccKerTsI.s:
No space left on device
```

**No es un error del código: el disco C: está lleno.** Arduino escribe todos los
archivos intermedios en `%TEMP%`.

**1. Vaciar el temporal** — `Win+R` → `%TEMP%` → Enter → seleccionar todo y
borrar (lo que esté en uso se saltea solo). Suele liberar varios GB.

**2. Vaciar la caché de compilación de Arduino** — en esa misma carpeta hay
subcarpetas `arduino_build_*`, `arduino_cache_*` y `arduino-sketch-*`. Son las
más grandes.

**3. `cleanmgr`** (Liberador de espacio de Windows) y revisar que C: tenga al
menos **5 GB libres**.

### 🚀 La causa de fondo: la versión de FastLED

Si en el error ves rutas como `src/fl/build/...` o `platforms/wasm/...`, tenés
una **FastLED 3.9 o 3.10**. Esas versiones reorganizaron el código y compilan
**cientos de archivos** —incluido el soporte de WebAssembly— que a un AVR no le
sirven para nada. Resultado: compilación lentísima y temporales enormes.

**Solución: bajar a FastLED 3.6.0.**

> Arduino IDE → **Herramientas → Administrar bibliotecas** → buscar `FastLED` →
> en el desplegable de versión elegir **3.6.0** → **Instalar**

O por consola:

```bash
arduino-cli lib install FastLED@3.6.0
```

**Todo este proyecto usa sólo API que existe en 3.6.0**: `addLeds`,
`setMaxPowerInVoltsAndMilliamps`, `setBrightness`, `show`, `clear`,
`fill_solid`, `fadeToBlackBy`, `HeatColor`, `CHSV`, `sin8`, `cos8`, `sqrt16`,
`random16`. No se pierde nada, y compila **varias veces más rápido**.

### 📻 El serie se llena de `[rf] codigo sin grabar` sin tocar nada

```
[rf] codigo sin grabar: 25272576
[rf] codigo sin grabar: 2
[rf] codigo sin grabar: 0
...
```

**Es ruido del receptor, no un problema del cartel.** El MX-RM-5V es
**super-regenerativo**: sin transmisor cerca amplifica el ruido de radio
ambiente y saca bits al azar por el pin de datos. Es su comportamiento normal,
no está fallado.

El firmware lo filtra con dos barreras:

1. **Validación de tiempos**: el pulso corto de un EV1527 mide 250-500 µs y el
   sincronismo son 31 de esos. Si lo medido cae fuera de rango, se descarta la
   trama entera.
2. **Repetición**: hace falta recibir **la misma trama 2 veces seguidas** en
   400 ms. Un llavero repite la trama 5-10 veces por pulsación; el ruido **nunca
   repite el mismo valor de 24 bits**.

Con eso el log queda limpio. Si todavía ves algún código suelto cada tanto,
subí `RF_REPES` de 2 a 3.

> 💡 **El arreglo de hardware** es pasar al **RXB6** (superheterodino, ~3 USD,
> compatible pin a pin): tiene squelch y prácticamente no saca ruido, además del
> doble de alcance. Con el MX-RM-5V el ruido también genera **muchísimas
> interrupciones**, que le roban CPU al refresco de los LEDs.

### 🔇 No detecta ninguna trama del llavero

Primero, el diagnóstico dedicado: **`tools/diag_rf`** — sólo el receptor, sin
LEDs ni nada más.

```bash
arduino-cli compile --fqbn arduino:avr:nano tools/diag_rf
arduino-cli upload  --fqbn arduino:avr:nano -p /dev/ttyUSB0 tools/diag_rf
```

| Comando | |
|---|---|
| `t` | muestra cada trama decodificada, su pulso base y cuántas veces se repitió |
| `c` | **volcado crudo de tiempos** → sirve para descubrir el protocolo del llavero |
| `e` | estadística: tramas válidas vs descartadas por ruido, cada 5 s |

**Qué tiene que pasar al apretar un botón**: la **misma trama repetida varias
veces seguidas**, con `<-- VALIDA` a partir de la segunda.

Si no sale nada:

1. **La antena.** 17,3 cm de alambre recto en el pad `ANT`. Sin ella el alcance
   es de 2 m, y a veces ni eso.
2. **El pin.** `DATA` tiene que ir a **D2 o D3**: son los únicos del Nano con
   interrupción.
3. **La pila del llavero.** Es lo que más falla y lo más engañoso.

   | Pila | Nueva | Gastada |
   |---|---|---|
   | **A23 / 23A** (12 V, la típica de llavero de portón) | 12,0-12,6 V | **por debajo de 11 V** |
   | CR2032 (3 V) | 3,0-3,3 V | por debajo de 2,8 V |

   Las A23 son 8 pilitas apiladas y tienen **mucha resistencia interna**: una
   a 10,4 V en vacío se desploma a 7-8 V cuando el transmisor pide corriente, y
   ahí el oscilador de RF **ni arranca**.

   ⚠️ **Que el LED del llavero encienda NO prueba que esté transmitiendo.** El
   LED se conforma con 2 mA; la etapa de radio necesita bastante más y además
   que la tensión no caiga. Con la pila floja, **el LED prende igual y no sale
   nada al aire**.

   Medila **mientras apretás el botón**: si cae por debajo de 9 V, cambiala.
4. **El protocolo.** Corré `c` y apretá el botón: si el volcado muestra tiempos
   pero el decodificador no arma la trama, tu llavero usa otra codificación.
   Pasame el volcado y lo adapto.
5. Poné `RF_DEBUG true` en `nano_vfinal.ino` para ver cada trama por el serie.

> ⚠️ **No derivamos el pulso base del sincronismo.** El EV1527 usa una relación
> de 31:1, pero el PT2262 puede usar 4, 8, 16 o 31. La validación es **por
> proporción** (un bit es 3:1 o 1:3 entre su parte alta y su baja), que es
> universal.

### Otros errores frecuentes

| Error | Qué pasa |
|---|---|
| `Hay poca memoria disponible` con menos de 300 bytes libres | el panel es muy grande para el Nano: bajá `UNDO_PASOS` o achicá `ALTO` |
| `programmer is not responding` | ver `docs/06`: timing del reset, Processor equivocado o RX/TX sin cruzar |
| `cannot open port COMx` | el puerto no existe: driver o cable. Ver `docs/06` |

## 🔜 Lo que queda para la versión profesional (ESP32)

- Nombres de los equipos y panel más grande
- Historial de sets en columnas, como los marcadores de TV
- App por WiFi, audio con locuciones, reloj de turno
- Logo del sponsor
