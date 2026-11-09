# ⚡ Esquemático — Tanteador multideporte (Arduino Nano)

## Diagrama general

```
    ┌──────────────────────────────────────────────────────────────────┐
    │  POWER BANK 5 V / 3 A  (USB)                                     │
    └────────────────┬─────────────────────────────────────────────────┘
                     │
        ┌────────────┴─────────────┐
        │   BORNERA 5 V  ── fusible 5 A                                 
        │        │                                                      
        │        ├──────────────────┬───────────────┬──────────────┐
        │        │                  │               │              │
        │   ┌────▼─────┐     ┌──────▼──────┐  ┌─────▼─────┐  ┌─────▼─────┐
        │   │ NANO 5V  │     │ PANEL LED   │  │ RECEPTOR  │  │ SENSORES  │
        │   │          │     │ 23×12 = 276 │  │ RF 433    │  │  A  y  B  │
        │   └──────────┘     └─────────────┘  └───────────┘  └───────────┘
        │
    ┌───▼──────────────────────────────────────────────────────────────┐
    │  BORNERA GND  — masa común de TODO. Sin esto nada funciona.      │
    └──────────────────────────────────────────────────────────────────┘
```

## Pines del Nano

```
                       ┌───────────────┐
        RF 433 DATA  ──►┤ D2        D13 ├── LED de la placa (bips)
   HC-SR04 ECHO A  ──►┤ D3        D12 ├── botón DEPORTE → a GND
                      ┤ D4        D11 ├── buzzer (vía BC337)
   HC-SR04 ECHO B  ──►┤ D5        D10 ├── libre
   DATO de la tira ◄──┤ D6         D9 ├── libre
                      ┤ D7         D8 ├── libre
                      │               │
                      │ A0 A1 A2 A3   │
                      └──┬──┬──┬──┬───┘
                         │  │  │  └── sensor B
                         │  │  └───── sensor A
                         │  └──────── botón RESET → a GND
                         └─────────── LDR (opcional, ver abajo)
```

> **D7 a D10 quedaron libres** al pasar al receptor crudo: antes eran los 4
> canales del PT2272.
>
> **El LDR es opcional.** Es un fotorresistor que baja el brillo solo de noche.
> Si no lo ponés no pasa nada: el brillo se cicla a mano con el botón RESET.

## La tira LED

```
   NANO D6 ──[ 470 Ω ]──► DIN de la fila 0

   5 V  ──┬──────────────────────────► inicio de la fila 0
          └──────────────────────────► final de la fila 11   ← 2ª inyección
   GND ───┴──────────────────────────► ambos extremos

   ⚡ CAPACITOR 1000 µF entre 5 V y GND en la entrada de la tira.
      Pata larga a 5 V, raya del cuerpo a GND. NO es opcional:
      sin él, el pico de arranque resetea el Nano.
```

**Serpentina** — la salida de cada fila va a la entrada de la siguiente:

```
   fila  0  →→→→→→→→→→→→→→→→→→→→→→→   (23 LEDs)
   fila  1  ←←←←←←←←←←←←←←←←←←←←←←←
   fila  2  →→→→→→→→→→→→→→→→→→→→→→→
   ...
   fila 11  ←←←←←←←←←←←←←←←←←←←←←←←
```

## Receptor RF 433 MHz — **MX-RM-5V / XY-MK-5V**

⚠️ **Este receptor NO decodifica nada.** Entrega el flujo de bits en bruto por
un solo pin; el código del llavero lo decodifica el firmware. Por eso **no hay
4 canales**: hay **un pin de datos** y los botones se **graban**.

```
   ┌─────────────────────────────┐
   │  MX-RM-5V   (4 patas)       │
   │                             │
   │  VCC  ──────────► 5 V       │
   │  DATA ──────────► D2        │  ← tiene que ser D2 o D3: son los únicos
   │  DATA ──  (repetido, dejar  │     pines de interrupción del Nano
   │           uno sin conectar) │
   │  GND  ──────────► GND       │
   │                             │
   │  ANT  ──► 17,3 cm de alambre recto y vertical
   └─────────────────────────────┘
```

**Las 4 patas**: mirando el lado de los componentes, suelen venir
`VCC · DATA · DATA · GND`. Los dos DATA están unidos internamente — se usa uno
solo. Si el tuyo trae 8 patas, el orden es `GND · DATA · DATA · VCC` de un lado
y la antena del otro.

### 📡 La antena es obligatoria

**17,3 cm de alambre recto** (un cuarto de onda a 433 MHz), soldado al pad `ANT`
y lo más vertical posible. Sin antena el alcance es de **2 metros**; con
antena, **30 a 50 m**.

### 💡 Upgrade recomendado: RXB6

El MX-RM-5V es **super-regenerativo**: barato pero ruidoso y de poco alcance.
El **RXB6** (superheterodino, ~3 USD) es compatible pin a pin y da el doble de
alcance con mucho menos ruido. Si el alcance te queda corto, es el cambio.

### ⚠️ FastLED y las interrupciones

FastLED apaga las interrupciones durante cada refresco (~8 ms con 276 LEDs), y
en ese rato el receptor no puede decodificar. No es grave: **el llavero repite
la trama varias veces por pulsación**, así que alguna entra. Pero si alguna vez
una pulsación no toma, es por esto — se aprieta de nuevo y listo.

## Sensores de paleta

El firmware soporta tres tipos. Se elige con `SENSOR_TIPO` en el `.ino`.

### Tipo 1 — TCRT5000 crudo (el de 4 patas, sin plaquita)

⚠️ **No es un módulo: es el componente pelado.** No trae comparador ni
potenciómetro ni salida digital. Hay que armarle el circuito y leerlo por
entrada **analógica**.

```
            ┌──────────────────────────────────── 5 V
            │              │
         [ 220 Ω ]      [ 10 kΩ ]
            │              │
         ┌──┴──┐        ┌──┴──┐
         │ LED │        │ FOTO│──────────────────► A2  (equipo A)
         │ IR  │        │ TR. │                    A3  (equipo B)
         └──┬──┘        └──┬──┘
            │              │
           GND            GND
```

- **220 Ω** limita la corriente del LED infrarrojo (~15 mA).
- **10 kΩ** es el pull-up del colector del fototransistor.
- La salida va a **A2 / A3** (entradas **analógicas**, no digitales).
- En reposo la lectura es alta; **al acercar algo baja**. El firmware se
  autocalibra, así que no importa el valor exacto.

**Cómo identificar las patas** (las dos mitades se parecen):

1. Mirando de frente, una mitad es **azulada/translúcida** (el LED IR) y la otra
   **negra** (el fototransistor).
2. Con el multímetro en **modo diodo**, la que marca **~1,1-1,3 V** en un sentido
   y nada en el otro es el **LED**. La otra mitad no marca diodo claro.
3. En el LED, la pata del lado que marca con la punta **roja** es el **ánodo**
   (va a la resistencia de 220 Ω).

### ⚠️ Lo más importante del TCRT5000: el alcance

| | |
|---|---|
| Alcance óptimo | **1 a 15 mm** |
| Alcance máximo útil | ~20 mm |

**No sirve para detectar una paleta a 20 cm.** Está hecho para seguidores de
línea y ruedas encoder, no para proximidad a distancia.

**Lo que sí funciona: cambiar la interacción a TOCAR.** Un círculo pintado en el
panel y el jugador **apoya la paleta ahí**. Y tiene una ventaja real: **cero
falsos positivos** de gente que pasa caminando, que era el problema del otro
enfoque.

### 🕳️ El truco de montaje: embutirlo

```
      ┌─────────────────┐  ← frente del panel
      │   ╔═════════╗   │
      │   ║ agujero ║   │  10-15 mm de profundidad
      │   ║  Ø 8 mm ║   │  pintado de NEGRO por dentro
      │   ╚════╤════╝   │
      │     [TCRT]      │  ← el sensor al fondo
      └─────────────────┘
```

Embutido al fondo de un agujero de 10-15 mm, **la luz ambiente no llega al
fototransistor**: sólo entra lo que se refleja desde algo que está justo en la
boca. Eso es lo que lo hace usable al sol, que es su gran debilidad.

### Tipo 2 — Módulo FC-51 (con plaquita y potenciómetro)

```
   VCC ──► 5 V      GND ──► GND      OUT ──► A2 (A) / A3 (B)
```
Salida digital, alcance 2-30 cm regulable con el potenciómetro.
⚠️ Se ciega con el sol directo.

### Tipo 3 — HC-SR04 ultrasónico

```
   VCC  ──► 5 V       TRIG ──► A2 (A) / A3 (B)
   GND  ──► GND       ECHO ──► D3 (A) / D5 (B)
```
Alcance 2-400 cm e **inmune al sol**. Es el único que funciona de verdad a
20 cm al aire libre.

## Botones y buzzer

```
   botón DEPORTE ── D12 ──┐           buzzer pasivo:
   botón RESET   ── A1  ──┤             5 V ──► buzzer ──► colector BC337
                          │             base ──[1 kΩ]── D11
                         GND            emisor ──► GND

   D13 = LED de la propia placa, acompaña los bips. No hay que cablear nada.
```

Los dos botones usan `INPUT_PULLUP`: **no llevan resistencia**, van directo a GND.

## Consumo

| | |
|---|---|
| Tanteo normal (~25 % encendido) | ~1,4 A |
| Festejo a pantalla llena | hasta 5,5 A |
| Blanco pleno (nunca pasa) | 16,6 A |

`MILIAMPERES 2600` en el firmware limita el brillo solo para no pasarse de un
power bank de 3 A. Si usás fuente de pared, subilo.

## Reglas de oro

1. **Masa común** entre Nano, tira, receptor, sensores y fuente. Siempre.
2. **La tira NO se alimenta del pin 5V del Nano**: va a la misma bornera, en
   paralelo. Si pasás 3 A por la placa, se quema.
3. **Inyectar 5 V en los dos extremos**, cable 18 AWG.
4. **Capacitor y resistencia de 470 Ω** no son opcionales.
