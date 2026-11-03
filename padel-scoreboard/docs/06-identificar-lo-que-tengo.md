# 06 - Identificar lo que tenés y armar el primer dígito

Tres incógnitas definen el plan. Las tres se resuelven con pruebas concretas, no
mirando la caja.

---

## 1. ¿Las luces de navidad sirven? **Depende, y se ve en 10 segundos**

La pregunta real es si son **direccionables** (cada LED se controla por separado)
o no. Hay tres formas de saberlo, de la más rápida a la definitiva:

### Prueba A — contar los cables entre dos bombitas (10 segundos)

Mirá el tramo de cable **entre dos bombitas consecutivas**, en el medio de la guirnalda:

| Cables | Qué es | ¿Sirve? |
|---|---|---|
| **2 cables** | guirnalda común (todas las bombitas en paralelo) | ❌ no, no se puede controlar una sola |
| **3 cables** | probablemente **WS2811 pixel string** | ✅ **sí, y es excelente** (ver abajo) |
| **4 cables** (uno dice CI/CLK) | APA102 / WS2801, direccionable por SPI | ✅ sí, cambia una línea del firmware |

Cuidado con un falso positivo: algunas guirnaldas **no** direccionables de 2 canales
también tienen 3 cables (dos canales + común). Se distinguen por la prueba B.

### Prueba B — mirar cómo se mueve la luz

Encendela con su propio controlador y mirá un efecto de color:

- **El color fluye suavemente a lo largo del hilo**, como una ola, y se pueden ver
  bombitas de **distintos colores al mismo tiempo** → ✅ **direccionable**.
- Parpadean **todas juntas**, o **en dos grupos alternados**, y todas del mismo
  color a la vez → ❌ no direccionable.

Palabras clave en la caja o el aviso de compra que confirman que sí:
**"RGBIC"**, **"dream color"**, **"magic color"**, **"pixel led"**, **"WS2811"**,
**"direccionable"**, **"se controla con app"**.

### Prueba C — la definitiva: `tools/diag_leds`

```bash
arduino-cli lib install FastLED
arduino-cli compile --fqbn arduino:avr:nano tools/diag_leds
arduino-cli upload  --fqbn arduino:avr:nano -p /dev/ttyUSB0 tools/diag_leds
```

Conectá sólo el **dato a D6** y la **masa común**, alimentá la guirnalda con su
propia fuente, abrí el monitor serie a 115200 y escribí `w`:

- **Se enciende un LED a la vez y van avanzando** → ✅ es direccionable. Contalos:
  ese número es tu `count` real.
- **No se enciende nada** → o no es direccionable, o falta la **masa común** entre
  el Nano y la fuente de la guirnalda (es el error más frecuente), o el dato está
  en otro pin.
- **Se enciende todo junto o nada tiene sentido** → no es direccionable.

Después `o` te dice el **orden de color** (si pedís rojo y sale verde, es RGB en
vez de GRB) y `d` te hace contar **0 a 9 en un dígito de 7 segmentos** de verdad.

### 🎯 Si son WS2811 pixel string, es mejor que una cinta

No es un premio consuelo: **los carteles LED profesionales se hacen así**. Son
"bullet pixels" de 12 mm cada 10 cm, 12 V, IP67. Ventajas sobre la cinta:

- Se **doblan libremente**: cada pixel se clava donde querés, no estás atado al
  trazado recto de la cinta.
- Ya vienen **estancos** (van a la intemperie sin más).
- A 12 V hay **mucho menos caída de tensión** en tiras largas.
- 2 o 3 pixels por segmento alcanzan y sobran.

**Detalle importante**: muchas guirnaldas y cintas WS2811 tienen **1 chip por cada
3 LEDs**. Es decir, la unidad direccionable es un **grupo de 3**. Si en la prueba
`w` se te encienden **3 LEDs a la vez**, es eso. No es un problema: en `config.h`
los `LEDS_PER_SEG_*` cuentan **unidades direccionables**, no LEDs físicos. Un
segmento con 2 unidades son 6 LEDs físicos y se ve mejor todavía.

---

## 2. El "display LED de puntos matriz": ¿qué es exactamente?

Fijate cómo se conecta y decime cuál es:

| Cómo se ve | Qué es | Veredicto para el tanteador |
|---|---|---|
| Módulo 8×8 rojo con un chip grande de 24 patas, 5 pines (VCC GND DIN CS CLK) | **MAX7219** | ⚠️ sirve para prototipar en el escritorio, **no para el sol** |
| Panel grande con **cable plano de 16 pines (HUB75)**, dice P10 / P5 / P4 | panel RGB de interior | ❌ no sirve: brillo de interior, y necesita refresco constante (requiere ESP32 con DMA) |
| 4 dígitos de 7 segmentos con 4 pines (CLK/DIO) | **TM1637** | ⚠️ sirve para el banco de pruebas, es muy chico y no se lee al sol |
| Matriz 8×8 pelada, 16 patas, sin chip | matriz cruda | ⚠️ hay que multiplexarla a mano |

Para los cuatro casos la conclusión es la misma: **útil para probar la lógica,
inútil al sol**. Los paneles de interior andan en 800–1500 nits; un cartel que se
lee a pleno sol necesita 5000+ nits, y eso sólo lo dan LEDs discretos a plena
corriente detrás de un filtro oscuro.

Igual no lo descartes: si tenés un MAX7219, es un **excelente banco de pruebas**
para ver el partido completo mientras armás los dígitos grandes. Si querés, te
agrego un modo de salida para ese panel (es un archivo aparte, no toca la lógica).

---

## 3. El CD74HC4009: guardalo para la fase ESP32

Es un **hex buffer inversor con conversión de niveles** (6 inversores en un chip).

- **Para la demo con Nano: no lo necesitás.** El Nano es 5 V y el dato de la tira
  va directo. Ese era justamente el punto a favor del Nano.
- **Para la fase ESP32 (3.3 V): sí sirve** como buffer del dato de la tira. Dos
  cosas a tener en cuenta: **invierte**, así que hay que poner **dos inversores en
  serie** (o invertir la señal por software, que es más lío); y es CMOS clásico,
  más lento que un 74AHCT125. A 800 kHz y con 10 cm de cable anda; con cable largo
  conviene el 74AHCT125.
- Son **SMD**, así que para protoboard hace falta una plaquita adaptadora SOIC→DIP
  o soldar cables finos directo a las patas.

---

## 4. De "transistores e IC varios", buscá específicamente estos

Revisá los cajones con esta lista; cualquiera de estos cambia el plan a mejor:

| Si encontrás | Para qué sirve acá | Prioridad |
|---|---|---|
| **74HC595** | registro de desplazamiento: es la pieza clave del Plan B sin LEDs direccionables | 🔴 muy alta |
| **ULN2803 / ULN2003** | 8 drivers Darlington en un chip: reemplaza 8 transistores + 8 resistencias | 🔴 muy alta |
| **IRLZ44N / IRL540 / IRF3205** (MOSFET **"logic level"**) | switchear segmentos de tira analógica | 🟠 alta |
| **IRF540 / IRF740** (MOSFET común) | sirve, pero necesita ~10 V en el gate: con 5 V del Nano queda a medio abrir | 🟡 media |
| **BC337 / BC547 / 2N2222** (NPN chico) | el buzzer, y segmentos de poca corriente | 🟢 útil |
| **TIP120 / TIP122** | Darlington de potencia, para segmentos grandes | 🟢 útil |
| **LM2596 / XL4015 / XL4016** (módulo buck) | bajar 12 V a 5 V si tus fuentes son de 12 V | 🟠 alta |
| **AMS1117 / LM7805** | reguladores chicos, **no** sirven para la tira (poca corriente) | ⚪ no |
| **TSOP1838 / VS1838 / HX1838** (receptor IR) | ¡los dos controles con un control de TV viejo! | 🔴 muy alta |
| **Módulo RF 433** (FS1000A, XY-MK-5V, PT2272) | los controles remotos | 🔴 muy alta |
| **HC-05 / HC-06** (Bluetooth) | el celular como control, adelanta parte de la app | 🟠 alta |
| **HC-SR04** | los sensores de paleta | 🔴 muy alta |
| Capacitores electrolíticos **≥470 µF / 10 V** | obligatorio a la entrada de la tira | 🔴 muy alta |

De las **fuentes**: anotame lo que dice la etiqueta de cada una (**volts** y
**amperes**). Lo que buscamos es **5 V con 2 A o más**, o **12 V con 2 A o más**
si vamos por WS2815/WS2811 de 12 V.

---

## 5. ¿Conviene conseguir WS2815?

Depende de con qué lo vas a alimentar:

| | **WS2812B (5 V)** | **WS2815 (12 V)** |
|---|---|---|
| Alimentación | **el power bank sirve directo** | hace falta una fuente de 12 V ≥ 3 A |
| Caída de tensión en 4 m | notoria (hay que inyectar en los dos extremos) | **mínima** |
| Si se quema un LED | se corta la cadena ahí | **tiene línea de dato de respaldo**: sigue funcionando |
| Precio | más barato | un poco más caro |

**Recomendación**: si el power bank va a ser la fuente, **WS2812B de 5 V**, que es
lo que asume toda la documentación. Si tenés una fuente de **12 V con 3 A o más**
(o un buck LM2596 para el Nano), **WS2815 es técnicamente mejor** y el cambio en el
firmware es **una línea**:

```c
// config.h
#define LED_CHIPSET   WS2815
#define PSU_VOLTS     12
```

Y si las luces de navidad resultan ser **WS2811 de 12 V**, ya tenés lo mismo gratis.

---

## 6. Verificar los Nano (antes que nada)

```bash
arduino-cli core install arduino:avr
arduino-cli compile --fqbn arduino:avr:nano tools/diag_nano
arduino-cli upload  --fqbn arduino:avr:nano -p /dev/ttyUSB0 tools/diag_nano
```

No necesita **ningún componente externo**. Si en el monitor serie (115200) aparece
`[diag_nano] si lees esto, el Nano y el USB funcionan`, la placa está viva. El
comando `r` te dice la RAM libre, `b` hace parpadear el LED de la placa, y `m`
monitorea D2..D12 para que pruebes pin por pin con un cable a GND.

Si no aparece nada:
- **Clon con CH340**: probá `--fqbn arduino:avr:nano:cpu=atmega328old` (bootloader
  viejo). Es el problema más común con los clones.
- En Linux puede faltar permiso sobre el puerto: `sudo usermod -aG dialout $USER`
  y volver a iniciar sesión.
- Probá otro cable USB. La mitad de los "Nano quemados" son cables de sólo carga.

---

## 7. Plan B: si NADA resulta direccionable

Se puede hacer con lo que tenés (transistores + registros de desplazamiento), pero
quiero ser claro con los números porque cambia bastante el proyecto:

### Opción B1 — multiplexado (pocos componentes, **malo para el sol**)

7 líneas de segmento + 8 líneas de dígito = **15 transistores**. Barato y poco
cableado. El problema: con 8 dígitos cada uno está encendido **1/8 del tiempo**, y
el brillo aparente cae a **1/8**. Para interior está bien; **a pleno sol es un
fracaso**, que es exactamente el requisito del proyecto.

### Opción B2 — encendido estático (se ve bien, mucho trabajo)

Un transistor por segmento, todos encendidos al 100 %:
**7 × 74HC595** (56 salidas, 3 pines del Nano) + **56 transistores** + 56
resistencias. Unos 15–20 USD en componentes y **más de 300 soldaduras**.

### Veredicto honesto

**5 metros de WS2812B cuestan 12–15 USD** y reemplazan los 7 registros, los 56
transistores y las 300 soldaduras, con mejor resultado y control de color. Si tus
luces de navidad no sirven, **comprar la cinta direccionable es el camino más
barato y más rápido**, no el más caro.

El Plan B tiene sentido sólo si no hay forma de conseguir cinta direccionable.
Decime y lo diseño completo.

---

## 8. Armar el primer dígito (el paso de hoy)

Una vez que sepas que tu cinta/guirnalda es direccionable:

**Necesitás**: 14 unidades direccionables (2 por segmento × 7) para un dígito de
prueba, o 21 si querés que se vea más lleno.

1. **Cortá 7 tramos iguales** de 2 (o 3) LEDs cada uno. Respetá la **flecha**
   impresa: marcá con fibra la entrada de cada tramo.
2. **Pegalos en cartón negro** formando el 8, en este orden exacto:
   ```
        AAAA
       F    B        A = arriba        D = abajo
       F    B        B = arriba-der.   E = abajo-izq.
        GGGG         C = abajo-der.    F = arriba-izq.
       E    C        G = el del medio
       E    C
        DDDD
   ```
3. **Soldá en cadena** A→B→C→D→E→F→G con 3 hilos entre tramos (5V, DIN, GND).
   La **salida** de un tramo va a la **entrada** del siguiente.
4. **Alimentá** con la fuente (no por el pin del Nano) y poné el capacitor de
   470–1000 µF entre 5 V y GND a la entrada. **Masa común** con el Nano.
5. Cargá `tools/diag_leds`, poné `s2` (o `s3`) y después `d`.
   **Si cuenta 0,1,2,…,9 correctamente, el dígito está bien cableado.**
   Si algún número sale raro, el orden de los segmentos está cruzado: el comando
   `w` te dice qué tramo es cada índice.
6. **Tabiques y difusor**: cartón negro de 2 cm entre segmentos, papel manteca
   encima, y una carpeta plástica oscura como filtro. Sacalo al sol y comparalo
   con y sin filtro. **Ese es el momento de la verdad del proyecto.**

Con el primer dígito funcionando, los otros 7 son repetición, y el firmware
completo ya está escrito y testeado esperándolos.
