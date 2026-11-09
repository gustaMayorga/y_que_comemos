# 🏟️ Tanteador multideporte — versión simple (Arduino Nano)

Todo en una carpeta: **firmware + esquemático + instrucciones**.
Panel LED de **23 × 12 = 276 píxeles**, **8 deportes**, control remoto y
sensores de paleta.

| Archivo | |
|---|---|
| `cartel-nano.ino` | el firmware completo |
| `ESQUEMATICO.md` | el cableado, pin por pin |
| `README.md` | esto |

```bash
arduino-cli lib install FastLED
arduino-cli compile --fqbn arduino:avr:nano cartel-nano
arduino-cli upload  --fqbn arduino:avr:nano -p /dev/ttyUSB0 cartel-nano
```

---

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

## ⚙️ Ajustes rápidos en `cartel-nano.ino`

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
3. **La pila del llavero.** Una CR2032 gastada transmite apenas.
4. **El protocolo.** Corré `c` y apretá el botón: si el volcado muestra tiempos
   pero el decodificador no arma la trama, tu llavero usa otra codificación.
   Pasame el volcado y lo adapto.
5. Poné `RF_DEBUG true` en `cartel-nano.ino` para ver cada trama por el serie.

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
