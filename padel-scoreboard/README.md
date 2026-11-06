# 🎾 Tanteador de Pádel — MVP

Cartel marcador para cancha de pádel, legible a pleno sol, con conteo por
**proximidad de paleta** (izquierda / derecha) y por **control remoto** (uno por
equipo). Demo sobre **Arduino Nano**, display de 7 segmentos hecho con tira LED y
difusor, alimentación por power bank o cargador USB.

> Este proyecto vive en una carpeta aparte dentro del repo `y_que_comemos` porque
> es donde se arrancó. Es autocontenido: se mueve a su propio repo moviendo sólo
> `padel-scoreboard/`.

> 🔧 **¿Vas a armarlo ahora?** Empezá por **[`montaje/`](montaje/)**: el plano del
> panel, el mapa de LEDs, el circuito y la checklist, todo en una página.

## 🧭 Índice de montaje — dónde está cada cosa

| Quiero… | Leer | Herramienta |
|---|---|---|
| Saber qué componentes necesito | [`docs/05`](docs/05-inventario-demo-nano.md) · [`docs/07`](docs/07-confirmacion-y-lista-de-compras.md) | — |
| Ver si mi placa funciona | [`docs/06` §6](docs/06-identificar-lo-que-tengo.md) | **`tools/diag_nano`** |
| Identificar / probar mi tira LED | [`docs/06` §1](docs/06-identificar-lo-que-tengo.md) | **`tools/diag_leds`** |
| **Cortar la tira** | [`docs/02` §6b](docs/02-display-7seg-led.md) | — |
| **Armar y probar UN dígito** | [`docs/02` §5 y §7](docs/02-display-7seg-led.md) | **`tools/test_segmentos`** |
| Que se lea a pleno sol | [`docs/02` §6](docs/02-display-7seg-led.md) | — |
| **Conectar todo (el circuito)** | [`docs/01` §5](docs/01-hardware-bom.md) | — |
| Conectar el receptor RF | [`docs/01` §5 y §6](docs/01-hardware-bom.md) | — |
| Alimentación y consumo | [`docs/01` §3](docs/01-hardware-bom.md) | — |
| **Plano del panel y mapa de LEDs** | **[`montaje/`](montaje/)** | `montaje/mapa_leds.py` |
| Piezas 3D e impresión | [`3d/README.md`](3d/README.md) | `3d/cartel.scad` |
| Entender las reglas y el firmware | [`docs/03`](docs/03-reglas-y-firmware.md) | — |
| Modos de deporte / licencia / horas de uso | [`docs/09`](docs/09-deportes-uso-y-licencia.md) | — |
| Problemas al cargar por USB | [`docs/06` §6](docs/06-identificar-lo-que-tengo.md) | — |

**Los 4 firmwares:**

| | Para qué |
|---|---|
| [`tools/diag_nano`](tools/diag_nano) | ¿funciona la placa? No necesita ningún componente |
| [`tools/diag_leds`](tools/diag_leds) | ¿qué tira tengo? Cuenta LEDs, orden de color, consumo |
| [`tools/test_segmentos`](tools/test_segmentos) | **prueba UN dígito segmento por segmento** |
| [`firmware/padel_scoreboard`](firmware/padel_scoreboard) | el tanteador completo |

## Qué muestra

```
 ┌───────────────────────────────┬─────────┬───────────────────────────────┐
 │  EQUIPO A                     │  SAQUE  │                     EQUIPO B  │
 │    ┌──────┐ ┌──────┐          │ ◀     ▶ │          ┌──────┐ ┌──────┐    │
 │    │  4   │ │  0   │  PUNTOS  │  PUNTO  │  PUNTOS  │  3   │ │  0   │    │
 │    └──────┘ └──────┘          │ ▲     ▲ │          └──────┘ └──────┘    │
 │      [3]      [1]             │         │            [2]      [0]       │
 │     JUEGOS   SETS             │         │          JUEGOS    SETS       │
 └───────────────────────────────┴─────────┴───────────────────────────────┘
```

- **4 dígitos grandes** para los puntos (2 por equipo): ` 0` `15` `30` `40` `Ad`
- **2 dígitos chicos** para los juegos, **2 mini** para los sets
- **Flechas de saque** al centro: se enciende la del equipo que saca
- **Aviso de punto** al centro, con el color diciendo de qué punto se trata:
  🔵 punto de juego · 🟡 punto de set · 🔴 punto de partido (titilando)

## Qué hace el MVP

- **6 deportes** en el mismo cartel, se cambia con un botón: **P**ádel, **t**enis,
  vóley (**U**), **F**útbol, **b**ásquet y contador **L**ibre.
- Tanteo completo de pádel: puntos, juegos, sets, **tie break**, **punto de oro**
  configurable (o ventajas), 7-5, partido al mejor de 3, control del saque.
- **Dos formas de contar**, las dos activas a la vez:
  - acercar la paleta al sensor **izquierdo** (Equipo A) o **derecho** (Equipo B);
  - apretar el botón del **control remoto de cada equipo** (433 MHz).
- **Deshacer** de verdad: restaura el estado anterior, incluso si ese punto había
  cerrado un juego, un set o el partido.
- Cambiar el saque, 3 niveles de brillo, reset del partido, confirmación sonora
  y visual de cada punto / juego / set / partido.
- **Límite de corriente por firmware**: funciona con un power bank de 5 V / 3 A
  sin que se corte. 6–7 h de autonomía con 20 000 mAh.
- WiFi **preparado y apagado**: el estado ya se serializa al JSON definitivo que
  va a consumir la app. Pasar a ESP32 es cambiar una línea de `config.h`.

## Arranque rápido

```bash
# 1) Validar la logica y que compilen las 6 variantes (placa x perfil), sin placa
./tests/run_checks.sh

# 2) Compilar y cargar en el Nano
arduino-cli core install arduino:avr
arduino-cli lib install FastLED
arduino-cli compile --fqbn arduino:avr:nano firmware/padel_scoreboard
arduino-cli upload  --fqbn arduino:avr:nano -p /dev/ttyUSB0 firmware/padel_scoreboard
```

Con el monitor serie a 115200 se puede probar **toda** la lógica (incluidos los
controles y los sensores) antes de soldar un solo LED: cada punto imprime el
estado del partido en JSON.

## Tres perfiles de armado

Un `#define` en `config.h` cambia el tamaño de todo:

| Perfil | LEDs | Tira a 60 LED/m | Dígito de puntos | Para qué |
|---|---|---|---|---|
| `LAYOUT_BENCH` | **60** | **1.0 m exacto** | 1 LED/segmento | probar todo en el escritorio |
| `LAYOUT_SPARSE` | **144** | 5 m a **30 LED/m** | **~230 mm**, legible a 80 m | ⭐ cartel full size con la mitad de LEDs y 1.6 A |
| `LAYOUT_DEMO` | **190** | 3.2 m | ~155 mm, legible a 45 m | demo jugable en cancha |
| `LAYOUT_FULL` | **278** | 4.7 m | ~230 mm, legible a 80 m | cartel definitivo |

Con **1 metro de tira** ya funciona el sistema completo. Con **sólo el Nano y dos
botones** ya se valida toda la lógica por el puerto serie.

## Documentación

| Doc | Contenido |
|---|---|
| [`docs/01-hardware-bom.md`](docs/01-hardware-bom.md) | BOM (~55 USD la demo), Nano vs ESP32, presupuesto de corriente **y de RAM**, cableado completo con pines del Nano |
| [`docs/02-display-7seg-led.md`](docs/02-display-7seg-led.md) | Layout, geometría del dígito, los iconos del centro, armado en 4 capas, **los 5 trucos para que se lea a pleno sol**, orden de cableado |
| [`docs/03-reglas-y-firmware.md`](docs/03-reglas-y-firmware.md) | Reglas, diagrama de estados, cómo se calcula el aviso de punto, estructura del firmware, controles, antirrebote |
| [`docs/04-roadmap-wifi-app.md`](docs/04-roadmap-wifi-app.md) | Contrato JSON congelado, paso a ESP32, app web en modo AP, remotos ESP-NOW, OTA |
| [`docs/05-inventario-demo-nano.md`](docs/05-inventario-demo-nano.md) | **Checklist de qué componentes revisar** y con qué se puede sustituir cada cosa |

## Orden de trabajo sugerido

1. **Revisar el inventario** ([doc 05](docs/05-inventario-demo-nano.md)) y ajustar
   `config.h` a lo que haya.
2. **Protoboard con `LAYOUT_BENCH`**: Nano + 1 m de tira + 2 botones. Partido
   completo de punta a punta. (1 tarde)
3. **Calibrar en la cancha** los umbrales de proximidad con la paleta real y con
   gente pasando. Esto no se puede hacer en el taller.
4. **Armar los dígitos** de `LAYOUT_DEMO` con tabiques y difusor. Es el 70 % del
   tiempo total.
5. **Probar al sol** con y sin el filtro oscuro: la diferencia justifica los 9 USD.
6. Cerrar la caja, colgar en la reja, jugar un partido completo.
7. Recién entonces: ESP32, WiFi y app (v1.1).

## Estado

- ✅ Lógica de tanteo: 20+ casos de test, corren en la PC
- ✅ Firmware completo: compila en las 6 combinaciones de placa × perfil
- ⚠️ Uso de RAM del Nano: **estimado** (~49 % con `LAYOUT_DEMO`), falta confirmarlo
  compilando con `arduino-cli` en una máquina con el toolchain de AVR
- ⬜ Nada probado en hardware real todavía
