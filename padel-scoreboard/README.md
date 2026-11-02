# 🎾 Tanteador de Pádel — MVP

Cartel marcador para cancha de pádel, legible a pleno sol, con conteo por
**proximidad de paleta** (izquierda / derecha) y por **control remoto** (uno por
equipo). Cerebro ESP32, display de 7 segmentos hecho con tira LED y difusor,
alimentación por power bank o cargador USB-C.

> Nota: este proyecto vive en una carpeta aparte dentro del repo `y_que_comemos`
> porque es donde se arrancó. Es autocontenido: se puede mover a su propio repo
> moviendo solo `padel-scoreboard/`.

## Qué hace el MVP

- Tanteo completo de pádel: **puntos** (0/15/30/40/Ad), **games**, **sets**,
  **tie break**, **punto de oro** configurable, partido al mejor de 3.
- **Dos formas de contar**, las dos activas a la vez:
  - acercar la paleta al sensor **izquierdo** (Equipo A) o **derecho** (Equipo B);
  - apretar el botón del **control remoto de cada equipo** (433 MHz).
- **Deshacer** el último punto (de verdad: restaura el estado, incluso si ese
  punto había cerrado un juego o un set), cambiar el saque, 3 niveles de brillo,
  reset del partido.
- Confirmación sonora y visual de cada punto, juego, set y partido.
- **Límite de corriente por firmware**: funciona con un power bank de 5 V / 3 A
  sin que se corte, ~5–6 h de autonomía con 20 000 mAh.
- WiFi **preparado y apagado**: el estado ya se serializa al JSON definitivo que
  va a consumir la app.

## Arranque rápido

```bash
# 1) Validar la lógica de tanteo sin hardware
g++ -std=c++17 tests/test_score.cpp firmware/padel_scoreboard/padel_score.cpp \
    -o /tmp/test_score && /tmp/test_score

# 2) Compilar y cargar el firmware
arduino-cli core install esp32:esp32
arduino-cli lib install FastLED
arduino-cli compile --fqbn esp32:esp32:esp32doit-devkit-v1 firmware/padel_scoreboard
arduino-cli upload  --fqbn esp32:esp32:esp32doit-devkit-v1 -p /dev/ttyUSB0 firmware/padel_scoreboard
```

Con el monitor serie a 115200 se puede probar **toda** la lógica (incluidos los
controles y los sensores) antes de soldar un solo LED.

## Documentación

| Doc | Contenido |
|---|---|
| [`docs/01-hardware-bom.md`](docs/01-hardware-bom.md) | BOM con precios (~100 USD), por qué ESP32, presupuesto de corriente, cableado completo, los dos controles, los sensores |
| [`docs/02-display-7seg-led.md`](docs/02-display-7seg-led.md) | Geometría del dígito, layout del cartel, armado en 4 capas, **los 5 trucos para que se lea a pleno sol**, orden de cableado |
| [`docs/03-reglas-y-firmware.md`](docs/03-reglas-y-firmware.md) | Reglas implementadas, diagrama de estados, estructura del firmware, controles, antirrebote del gesto de paleta |
| [`docs/04-roadmap-wifi-app.md`](docs/04-roadmap-wifi-app.md) | Contrato JSON ya congelado, app web en modo AP, remotos ESP-NOW, OTA, producto |

## Cifras del diseño

| | |
|---|---|
| LEDs totales | 236 (118 por equipo) |
| Altura del dígito de puntos | ~230 mm → legible a 70–90 m |
| Consumo típico / peor caso | ~2.6 A / 4.7 A a 5 V (limitado a 2.6 A por firmware) |
| Autonomía con power bank 20 000 mAh | 5–6 h a brillo de sol, 9–10 h en sombra |
| Tamaño del frente | ~55 × 40 cm |
| Costo del MVP | ~80 USD + power bank |

## Orden de trabajo sugerido

1. **Protoboard**: ESP32 + 1 dígito armado con 42 LEDs + 1 HC-SR04 + el receptor
   RF. Verificar `animBoot()` y el gesto de paleta. (1 tarde)
2. **Calibrar en la cancha** los umbrales de proximidad de `config.h` con la
   paleta real y con gente pasando. Esto no se puede hacer en el taller.
3. **Armar los 6 dígitos** con tabiques y difusor. Es el 70 % del tiempo total.
4. **Probar al sol** con y sin el filtro oscuro: la diferencia justifica los 9 USD.
5. Cerrar la caja, colgar en la reja, jugar un partido completo.
6. Recién entonces: WiFi y app (v1.1).
