# 03 - Reglas del partido, máquina de estados y firmware

## 1. Qué cuenta el tanteador

| Nivel | Cómo se muestra | Valores |
|---|---|---|
| Puntos | 2 dígitos grandes rojos | ` 0`, `15`, `30`, `40`, `Ad` |
| Puntos en tie break | los mismos 2 dígitos | `0`…`12`… (numérico) |
| Games | 1 dígito ámbar | `0`…`7` |
| Sets ganados | dots amarillos | hasta 3 |
| Saque | el dot de set libre, en verde | equipo A o B |
| Partido terminado | ganador muestra `Po`, perdedor `--` | + animación verde |

## 2. Reglas implementadas (`padel_score.cpp`)

Configurables en `config.h`, y en la v2 desde la app sin recompilar:

- **Punto de oro** (`DEFAULT_GOLDEN_POINT = true`): en 40-40 el siguiente punto
  gana el juego. Es el formato habitual de club y de la World Padel Tour.
  Poniéndolo en `false` se juega con **ventajas** (40-40 → `Ad` → si el rival
  gana el punto vuelve a 40-40).
- **Set a 6 games con 2 de diferencia**. 7-5 cierra el set. 6-6 va a **tie break
  a 7 con 2 de diferencia** (`DEFAULT_TIEBREAK`); si se desactiva, el set sigue
  hasta los 2 de diferencia.
- **Partido al mejor de 3 sets** (`DEFAULT_SETS_TO_WIN = 2`). Para partidos
  cortos de club, poner `1`.
- **Saque**: cambia al cerrarse cada juego, y en el tie break al primer punto y
  después cada 2 puntos. Es informativo, se puede corregir a mano (CH3 largo).
- Terminado el partido no se aceptan más puntos (suena un beep grave).

### Diagrama de estados

```
                   ┌──────────────────────────────────────────┐
                   │              PHASE_GAME                  │
                   │   0 → 15 → 30 → 40                       │
                   │   40-40 ─┬─ punto de oro → gana el juego │
                   │          └─ ventajas → Ad ⇄ 40-40        │
                   └───────┬──────────────────────────────────┘
                           │ gana juego
                           ▼
                 games[t]++ , cambia el saque
                           │
            ┌──────────────┼───────────────────────────┐
            │ ≥6 y dif ≥2  │ 7-5                       │ 6-6
            ▼              ▼                           ▼
        gana el SET    gana el SET              ┌──────────────┐
            │              │                    │PHASE_TIEBREAK│
            ▼              ▼                    │  a 7, dif 2  │
      sets[t]++ , games a 0 ◄───────────────────┴──────┬───────┘
            │                                  gana el set
            │ sets[t] == setsToWin
            ▼
     PHASE_MATCH_OVER
```

El estado completo vive en un único `struct MatchState` (28 bytes). Eso hace que
tres cosas sean triviales:

- **UNDO real**: antes de cada punto se guarda una copia del struct en un buffer
  circular de 16. Deshacer es copiar hacia atrás, no "restar un punto" (que sería
  incorrecto cuando el punto cerró un juego o un set).
- **Serialización**: `scoreToJson()` en `net_stub.h` lo convierte al JSON que va
  a consumir la app. El contrato ya está definido hoy para no reescribir nada después.
- **Persistencia** (fase 2): guardar el struct en NVS cada punto permite que si se
  corta la alimentación el partido se retome donde estaba.

## 3. Estructura del firmware

```
firmware/padel_scoreboard/
├── padel_scoreboard.ino   setup/loop, mapeo de eventos → acciones, feedback
├── config.h               TODO lo configurable: pines, geometría, colores, reglas
├── padel_score.h/.cpp     máquina de estados del tanteo (cero dependencias de Arduino)
├── display7seg.h/.cpp     glifos de 7 segmentos, render sobre FastLED, animaciones
├── inputs.h/.cpp          HC-SR04 con antirrebote de gesto + pulsadores corto/largo
└── net_stub.h             serialización a JSON + punto de enganche del WiFi (apagado)
```

La separación no es cosmética: `padel_score.cpp` **se compila y testea en la PC**,
sin placa (`tests/test_score.cpp`, 9 casos: progresión, punto de oro, ventajas,
6-0, 7-5, tie break 8-6, mejor de 3, undo, saque).

```bash
g++ -std=c++17 tests/test_score.cpp firmware/padel_scoreboard/padel_score.cpp \
    -o /tmp/test_score && /tmp/test_score
```

## 4. Controles

| Acción | Control remoto | Sensor de paleta | Botón de la caja |
|---|---|---|---|
| +1 punto Equipo A | CH1 (botón A) | acercar paleta al sensor **izquierdo** | — |
| +1 punto Equipo B | CH2 (botón B) | acercar paleta al sensor **derecho** | — |
| Deshacer | CH3 corto | — | — |
| Cambiar saque | CH3 largo (2 s) | — | — |
| Ciclar brillo (día/sombra/noche) | CH4 corto | — | pulso corto |
| Reset del partido | CH4 largo (2 s) | — | 2 s |

Feedback de cada acción:

- punto → beep corto agudo + flash del bloque del equipo
- juego ganado → 2 flashes ámbar + beep
- set ganado → 4 flashes amarillos + beep largo
- partido ganado → 8 flashes verdes + `Po` en el display del ganador
- undo → beep medio; acción inválida → beep grave

## 5. Antirrebote del gesto de paleta

El problema real de un sensor de proximidad en una cancha no es detectar: es
**no detectar de más**. Una paleta apoyada contra el cartel, o un jugador que
pasa caminando, sumarían 20 puntos.

```
   ARMED ──(dist ≤ 20 cm)──► HOLDING ──(sostenido 250 ms)──► +1 PUNTO
     ▲                          │                                │
     │                   (se retira antes)                       ▼
     │                          │                            LOCKOUT 2.5 s
     │                          ▼                                │
     └──────────────────────  ARMED                              ▼
     └────────────(dist ≥ 35 cm)───────────────────────  WAIT_CLEAR
```

Tres barreras en cadena: **tiempo de sostén**, **bloqueo temporal** e **histéresis
de re-armado** (hay que alejar la paleta más de 35 cm para que vuelva a contar).
Los cuatro umbrales están en `config.h` y conviene calibrarlos en la cancha, no
en el taller.

## 6. Dependencias y cómo compilar

- Arduino IDE 2.x o arduino-cli
- Board package: **esp32 by Espressif** (Boards Manager) → placa "ESP32 Dev Module"
- Librería: **FastLED** (≥ 3.6) — la única dependencia externa

```bash
arduino-cli core install esp32:esp32
arduino-cli lib install FastLED
arduino-cli compile --fqbn esp32:esp32:esp32doit-devkit-v1 firmware/padel_scoreboard
arduino-cli upload  --fqbn esp32:esp32:esp32doit-devkit-v1 -p /dev/ttyUSB0 firmware/padel_scoreboard
```

Monitor serie a 115200: cada cambio de tanteo imprime el JSON del estado. Sirve
para probar toda la lógica **antes** de tener un solo LED soldado.
