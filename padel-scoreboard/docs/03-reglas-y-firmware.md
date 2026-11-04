# 03 - Reglas del partido, máquina de estados y firmware

## 1. Qué muestra el tanteador

| Nivel | Cómo se muestra | Valores |
|---|---|---|
| Puntos | 4 dígitos grandes rojos (2 por equipo) | ` 0`, `15`, `30`, `40`, `Ad` |
| Puntos en tie break | los mismos dígitos | `0`…`12`… (numérico) |
| Juegos | 1 dígito ámbar por equipo | `0`…`7` |
| Sets ganados | 1 dígito amarillo por equipo | `0`…`3` |
| Saque | flecha verde al centro, apuntando al equipo que saca | |
| Punto de juego | triángulo **cyan** al centro | |
| Punto de set | triángulo **amarillo** al centro | |
| Punto de partido | triángulo **rojo titilando** al centro | |
| Partido terminado | el ganador muestra `Po`, el otro `--` | + animación verde |

## 2. Reglas implementadas (`padel_score.cpp`)

Configurables en `config.h`, y en la fase 2 desde la app sin recompilar:

- **Punto de oro** (`DEFAULT_GOLDEN_POINT = true`): en 40-40 el siguiente punto
  gana el juego. Es el formato habitual de club y de la World Padel Tour.
  Poniéndolo en `false` se juega con **ventajas**: 40-40 → `Ad` → si el rival
  gana el punto vuelve a 40-40.
- **Set a 6 juegos con 2 de diferencia**. 7-5 cierra el set. 6-6 va a **tie break
  a 7 con 2 de diferencia** (`DEFAULT_TIEBREAK`); si se desactiva, el set sigue
  hasta los 2 de diferencia.
- **Partido al mejor de 3 sets** (`DEFAULT_SETS_TO_WIN = 2`). Para partidos
  cortos de club, poner `1`.
- **Saque**: cambia al cerrarse cada juego, y en el tie break al primer punto y
  después cada 2 puntos. Se puede corregir a mano (CH3 largo).
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

## 3. El indicador de "estás a un punto de..."

Terminología, porque es fácil mezclarla:

| Término | Qué es | Dónde se ve |
|---|---|---|
| **Ventaja** (`Ad`) | sólo sin punto de oro: ganaste el punto estando 40-40 | dígitos de puntos |
| **Punto de juego** | estás a un punto de ganar el **juego** | triángulo cyan |
| **Punto de quiebre** | punto de juego **con el rival al saque** | expuesto en la API (`isBreakPoint`) |
| **Punto de set** | estás a un punto de ganar el **set** | triángulo amarillo |
| **Punto de partido** | estás a un punto de ganar el **partido** | triángulo rojo titilando |

Cómo se calcula, que es la parte elegante: **no hay una segunda copia de las
reglas**. Se simula el próximo punto con la misma función de transición que hace
avanzar el partido:

```c
Situation ScoreEngine::situation(uint8_t team) const {
  MatchState tmp = s_;                  // copia de 15 bytes
  uint8_t f = 0;
  applyPoint(tmp, r_, team, &f);        // MISMA funcion que usa addPoint()
  if (f & FLAG_MATCH) return SIT_MATCH_POINT;
  if (f & FLAG_SET)   return SIT_SET_POINT;
  if (f & FLAG_GAME)  return SIT_GAME_POINT;
  return SIT_NONE;
}
```

Consecuencia: si mañana cambian las reglas (punto de oro, sets al mejor de 1, tie
break desactivado), el aviso se ajusta solo y **no se puede desincronizar** de lo
que realmente pasa. Hay un test que verifica que la simulación no modifica el
partido.

Caso que vale la pena entender: con **punto de oro**, en 40-40 los **dos** avisos
se encienden, porque los dos equipos están a un punto de ganar el juego. Es
correcto, y visualmente queda muy bien.

## 4. Estructura del firmware

```
firmware/padel_scoreboard/
├── padel_scoreboard.ino   setup/loop, eventos → acciones, feedback sonoro y visual
├── config.h               TODO lo configurable: PLACA, perfil de armado, pines,
│                          geometría, colores, reglas, umbrales
├── padel_score.h/.cpp     máquina de estados del tanteo (cero dependencias de Arduino)
├── display7seg.h/.cpp     glifos de 7 segmentos, iconos, render sobre FastLED, animaciones
├── inputs.h/.cpp          HC-SR04 con antirrebote de gesto + pulsadores corto/largo
└── net_stub.h             serialización a JSON + punto de enganche del WiFi (apagado)
```

La separación no es cosmética: `padel_score.cpp` **se compila y se testea en la
PC**, sin placa.

### Dos interruptores en `config.h`

```c
#define BOARD_NANO       // o BOARD_ESP32  -> cambia pines, ADC, profundidad de undo
#define LAYOUT_DEMO      // o LAYOUT_BENCH / LAYOUT_FULL -> cambia LEDs por segmento
```

| Perfil | LEDs | Tira a 60 LED/m | Para qué |
|---|---|---|---|
| `LAYOUT_BENCH` | 60 | 1.0 m exacto | probar toda la lógica en el escritorio |
| `LAYOUT_DEMO` | 190 | 3.2 m | demo jugable en cancha |
| `LAYOUT_FULL` | 278 | 4.7 m | cartel definitivo |

## 5. Controles

El firmware soporta controles de **3 o 4 botones**, y se elige con
`REMOTE_BUTTONS` en `config.h`. Con 3 botones las funciones extra se mueven a
las pulsaciones largas de esos mismos 3, así que **no se pierde nada**.

| Botón | Pulso corto | Pulso largo (1 s) |
|---|---|---|
| **1** | **+1 punto Equipo A** | saca el Equipo A |
| **2** | **+1 punto Equipo B** | saca el Equipo B |
| **3** | **DESHACER** (repetible) | ciclar brillo |
| **4** *(sólo si hay 4)* | ciclar brillo | reset del partido |

Y en la caja:

| Botón de la caja | Acción |
|---|---|
| Local (D12) | ciclar brillo (día / sombra / noche) |
| **RESET (A1)** | **mantener 2 s**: mientras se mantiene, una barra roja se va llenando en el panel; al llenarse, resetea el partido. Si se suelta antes, no pasa nada |

Y los sensores de paleta, siempre activos en paralelo con los controles:
**izquierdo = +1 Equipo A**, **derecho = +1 Equipo B**.

### El botón 3 es un CTRL+Z de verdad

Cada pulsación retrocede **un punto**, sea de quien fuera, hasta `UNDO_DEPTH`
pasos (8 en Nano, 16 en ESP32). Y como restaura el **estado completo**, también
deshace un juego, un set o incluso el partido si ese punto los había cerrado.
Hay un test que verifica exactamente eso.

Feedback de cada acción:

- punto → beep corto agudo + flash del bloque del equipo
- juego ganado → 2 flashes ámbar + beep
- set ganado → 4 flashes amarillos + beep largo
- partido ganado → **festejo**: se apaga el equipo perdedor y el centro, el
  ganador recibe fuegos artificiales, un cometa que lo barre y un barrido
  arcoíris; después queda su `Po` **latiendo** hasta el reset
- deshacer → beep medio; acción inválida → beep grave

## 6. Antirrebote del gesto de paleta

El problema real de un sensor de proximidad en una cancha no es detectar: es
**no detectar de más**. Una paleta apoyada contra el cartel, o un jugador que
pasa caminando, sumarían 20 puntos.

```
   ARMED ──(dist ≤ 20 cm)──► HOLDING ──(sostenido 250 ms)──► +1 PUNTO
     ▲                          │                                │
     │                   (se retira antes)                       ▼
     │                          │                            LOCKOUT 2.5 s
     │                          ▼                                │
     ├──────────────────────  ARMED                              ▼
     └────────────(dist ≥ 35 cm)─────────────────────────  WAIT_CLEAR
```

Tres barreras en cadena: **tiempo de sostén**, **bloqueo temporal** e **histéresis
de re-armado** (hay que alejar la paleta más de 35 cm para que vuelva a contar).
Los umbrales están en `config.h` y conviene calibrarlos **en la cancha**, no en el
taller.

## 7. El undo

Antes de cada punto se guarda una copia del `MatchState` completo en un buffer
circular (8 niveles en Nano, 16 en ESP32). Deshacer es **copiar hacia atrás**, no
"restar un punto" — que sería incorrecto cuando el punto cerró un juego, un set o
el partido.

## 8. Chequeos que corren sin placa

```bash
./tests/run_checks.sh
```

Hace dos cosas:

1. **Tests de la máquina de estados** (`tests/test_score.cpp`): progresión de
   puntos, punto de oro, ventajas, 6-0, 7-5, tie break 8-6, partido al mejor de 3,
   undo, cambio de saque, punto de juego/set/partido, punto de quiebre, que la
   simulación no altere el estado, y los flags de la transición pura.
2. **Compilación del sketch completo en las 6 combinaciones** de placa × perfil,
   usando stubs de Arduino y FastLED (`tests/stub/`).

Los stubs **no** reemplazan al compilador de AVR: no dicen el uso real de RAM ni
flash. Para eso hay que compilar con `arduino-cli` (ver abajo). El presupuesto de
RAM estimado está en `01-hardware-bom.md` §4.

## 9. Dependencias y cómo compilar para el Nano

- Arduino IDE 2.x o `arduino-cli`
- Librería: **FastLED** (≥ 3.6) — la única dependencia externa

```bash
arduino-cli core install arduino:avr
arduino-cli lib install FastLED
arduino-cli compile --fqbn arduino:avr:nano firmware/padel_scoreboard
arduino-cli upload  --fqbn arduino:avr:nano -p /dev/ttyUSB0 firmware/padel_scoreboard
```

Si el Nano es un clon con chip CH340 y el bootloader viejo:
`--fqbn arduino:avr:nano:cpu=atmega328old`.

Para la fase 2 con ESP32: cambiar a `BOARD_ESP32` en `config.h` y usar
`--fqbn esp32:esp32:esp32doit-devkit-v1`.

**Verificar el uso de RAM al compilar** (es el número que importa en un Nano):

```
Sketch uses XXXXX bytes (NN%) of program storage space. Maximum is 30720 bytes.
Global variables use XXXX bytes (NN%) of dynamic memory...  Maximum is 2048 bytes.
```

Si "Global variables" pasa de ~1500 bytes, bajá a `LAYOUT_DEMO` o reducí
`UNDO_DEPTH`.

Monitor serie (115200 a 16 MHz, 57600 a 8 MHz): cada cambio de tanteo imprime el JSON del estado. Sirve
para probar **toda** la lógica —incluidos los controles y los sensores— antes de
soldar un solo LED.
