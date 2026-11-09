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
      sensor A  TRIG ──┤ D2        D13 ├── (libre, LED de placa)
      sensor A  ECHO ──┤ D3        D12 ├── botón DEPORTE  → a GND
      sensor B  TRIG ──┤ D4        D11 ├── buzzer (vía BC337)
      sensor B  ECHO ──┤ D5        D10 ├── RF canal 4
   DATO de la tira  ◄──┤ D6         D9 ├── RF canal 3
            RF ch1  ──►┤ D7         D8 ├── RF canal 2
                       │               │
                       │  A0  A1       │
                       └───┬───┬───────┘
                           │   └── botón RESET → a GND
                           └────── LDR (opcional)
```

> Con **sensores IR de obstáculo** (FC-51) se usa **una sola salida digital**:
> conectala a **D2** (equipo A) y **D3** (equipo B), y dejá D4/D5 libres.
> Con **HC-SR04 ultrasónicos** se usan los cuatro: TRIG+ECHO por sensor.

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

## Receptor RF 433 MHz (PT2272-**M4**, momentáneo)

```
   VCC ──► 5 V de la bornera        D0 ──► D7    botón 1  (+1 equipo A)
   GND ──► GND                      D1 ──► D8    botón 2  (+1 equipo B)
   VT  ──► sin conectar             D2 ──► D9    botón 3  (deshacer)
   ANT ──► 17 cm de alambre         D3 ──► D10   botón 4  (brillo / reset)
           recto y vertical
```

⚠️ Tiene que ser **M4 (momentáneo)**, no L4: el L4 queda enclavado y el
firmware lo lee como una pulsación eterna.
⚠️ Los pines vienen rotulados **D0 D1 D2 D3**, no CH1..CH4. `D0` es el canal 1.

## Sensores de paleta

```
   IR de obstáculo (FC-51)           Ultrasónico (HC-SR04)
   ─────────────────────             ─────────────────────
     VCC ──► 5 V                       VCC  ──► 5 V
     GND ──► GND                       TRIG ──► D2 (A) / D4 (B)
     OUT ──► D2 (A) / D3 (B)           ECHO ──► D3 (A) / D5 (B)
                                       GND  ──► GND
```

⚠️ **Los IR se enciegan con el sol directo.** Para cancha techada o para probar
van bien; al aire libre, montalos bajo una visera apuntando hacia abajo, o pasá
a HC-SR04, que son inmunes.

## Botones y buzzer

```
   botón DEPORTE ── D12 ──┐           buzzer pasivo:
   botón RESET   ── A1  ──┤             5 V ──► buzzer ──► colector BC337
                          │             base ──[1 kΩ]── D11
                         GND            emisor ──► GND
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
