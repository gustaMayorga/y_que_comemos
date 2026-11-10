# 📋 Estado del proyecto — para retomar en un chat nuevo

> Pegá este archivo, o pedile al nuevo chat que lea `padel-scoreboard/ESTADO.md`.

## Qué es

Tanteador electrónico **multideporte** (8 deportes) para cancha de pádel.
Panel de LEDs direccionables **23 × 12 = 276 WS2812B**, cerebro **Arduino Nano**.
Suma puntos por **sensor de proximidad de paleta** y por **control remoto RF 433 MHz**.
Alimentación por power bank o cargador USB. Requisito original: **tiene que verse
de día en cualquier cancha**.

## Dónde está todo

Repo `gustaMayorga/y_que_comemos`, rama **`ccr-158db404-xxgxh5`**.

| Ruta | Qué es |
|---|---|
| **`padel-scoreboard/nano_vfinal/nano_vfinal.ino`** | ⭐ **EL sketch.** Es el único que se carga |
| `padel-scoreboard/nano_vfinal/README.md` | Manual + todo el diagnóstico de fallas |
| `padel-scoreboard/nano_vfinal/ESQUEMATICO.md` | Cableado pin por pin |
| `padel-scoreboard/3d/carcasa_hcsr04.scad` | Carcasa imprimible del sensor |
| `padel-scoreboard/3d/carcasa-hcsr04.md` | Guía de armado de la carcasa |
| `padel-scoreboard/tools/` | 7 sketches de diagnóstico + generador de íconos |
| `padel-scoreboard/tests/run_checks.sh` | Compila todo y corre los tests. **Tiene que dar `TODO OK`** |
| `padel-scoreboard/firmware/`, `display/` | Línea vieja de 7 segmentos. **Superada, no tocar** |

## Hardware real

- Panel 23 × 12 serpentina, origen arriba, datos en **D6**
- **Un solo** HC-SR04 (`SENSOR_TIPO 4`): TRIG → A2, ECHO → D3. Dos zonas de
  distancia: 4-20 cm = equipo A, 27-55 cm = equipo B. **Anda bien**
- Receptor RF **MX-RM-5V** (superregenerativo) en **D2**. **0 controles grabados
  todavía.** El Intelbras XAC 2000 v2 es código rodante → no se puede aprender.
  Hace falta un llavero **EV1527** genérico
- Botones: DEPORTE en **D12**, RESET de partido en **A1**. Buzzer en D11
- Nano alimentado por **USB**, tira por **power bank**, masas unidas

## Config actual (primeras 145 líneas del .ino)

```
SENSOR_TIPO 4     PIN_RF_DATA 2    PIN_BUZZER 11    PIN_PRESENCIA -1
MILIAMPERES 1500  BRILLO_DIA 200   BRILLO_MAX 100   WATCHDOG true
SALUD_MS 3000     AUTO_PUNTOS 0    MODO_MINIMO false  PIN_VLED -1
```

⚠️ **Antes de dárselo a probar a alguien:** `AUTO_PUNTOS 0` (si queda en 2000
suma puntos solo cada 2 s) y `SALUD_MS 0`.

## Lo que se resolvió (fue largo)

Reinicios y cuelgues aleatorios. **Dos causas confirmadas en banco:**

1. **Una masa floja** (jumper DuPont). Pasó de reiniciar cada 15 s a aguantar
   minutos. **Lección: antes de instrumentar, apretar los cables.**
2. **Consumo.** Con `BRILLO_DIA 200` muere; con **100 corre una serie entera**
   con sensor y todo. `MILIAMPERES` estaba en 2600, más de lo que el power bank
   puede dar, así que el limitador de FastLED apuntaba a un techo imposible.

Descartado por medición, no por intuición: tensión del Nano (`min` ~4.5 V
estable), tormenta de ruido RF (`rf/s` 300-4500 = ~2 % de CPU), el cristal
(el serie sale legible), el pin RESET (pull-up de 10 k puesto, sin cambios).

**Bug propio encontrado:** el watchdog de 8 s mataba `grabarControles()`, que
espera hasta 30 s por botón. Arreglado con la macro `LATIDO()`.

## Instrumentos que trae el firmware

```
[salud] loops/s 1579  rf/s 301  btn 00  mA~840  Vcc 4.688 V  min 4.447 V
```

- `loops/s` — en miles es normal; si se desploma, algo le roba el CPU
- `rf/s` — flancos de la ISR del RF. La ISR **se desactiva sola** si supera
  `RF_TORMENTA` (50.000/s) y avisa
- `btn` — estado de D12 y A1. `10` sin tocar nada = pin trabado
- `mA~` — consumo estimado del cuadro. Miralo durante el festejo: es el pico
- `Vcc` / `min` — el mínimo **sobrevive al reinicio** (vive en `.noinit`)
- Banner: causa del reset (`MCUSR`), `reinicios`, `estuvo vivo N s antes del
  último reset`, y avisos de pin trabado
- `AUTO_PUNTOS` — suma puntos solo, para reproducir fallas por consumo
- `MODO_MINIMO` — apaga sensores, RF y buzzer de una línea, para bisecar

**Límite importante:** `leerVcc()` compara la referencia de 1.1 V contra AVcc, y
las dos están referidas al pin `GND` del propio chip. **Un rebote de masa es
invisible** para todo este instrumental.

## ⏳ Pendiente

1. **Resistencia de 330 Ω entre D6 y el DIN del primer LED.** No está puesta.
   Explica el síntoma de **panel congelado mientras el micro sigue contando**:
   cuando el riel de la tira se hunde, su VDD baja a 4 V mientras D6 entrega 5,
   el DIN queda 1 V por encima de su alimentación y conduce por su diodo. **La
   causa no es la longitud del cable, es la diferencia entre dos rieles**
2. **¿De dónde sale el `VCC` del HC-SR04 y del RF?** Pregunta sin responder en
   4 turnos. Si sale del riel de los LED y no del pin `5V` del Nano, hay
   alimentación parásita: el usuario **vio** el Nano encenderse con solo la
   fuente de los LED conectada
3. **Correr `AUTO_PUNTOS 2000` y llegar a 200 puntos** sin morir, con la
   resistencia puesta y el tope de brillo
4. **Probar al sol.** Con `BRILLO_DIA 75` (lo que está probando adentro) **no
   va a alcanzar** al sol directo. Ojo con el veredicto de los jugadores
5. Grabar los controles RF: hace falta un llavero EV1527 + antena de 17.3 cm
6. Imprimir la carcasa del HC-SR04 (`3d/carcasa_hcsr04.scad`, poner `SALIENTE`
   = espesor del panel)

## 🔜 Para la v2

- **Fuente conmutada de 5 V 4-10 A** en vez de power bank. Resuelve brillo y
  picos de una. 276 LEDs en blanco a fondo son **16 A**
- **Todo soldado o a bornera.** Ni un DuPont en masa ni en 5 V
- Contraste para el sol: filtro oscuro (atenúa el ambiente **dos** veces y el
  LED **una** → contraste ×4), negro mate, paredes entre celdas, visera,
  10-15° de inclinación hacia abajo. **Vale más que los últimos 50 puntos de
  brillo**
- Inyectar 5 V por los **dos extremos** de la tira
- Difusor y marco impresos
- ESP32: nombres de equipos, app, audio (DFPlayer), historial de sets.
  **No arregla fallas de cableado**, solo da mejor diagnóstico de resets

## Reglas de trabajo

- Commits en **`ccr-158db404-xxgxh5`**, con las líneas `Co-Authored-By: Claude
  Opus 5` y `Claude-Session:`
- `./tests/run_checks.sh` tiene que dar **`TODO OK`** antes de cada push.
  Compila con `-Wall -Wextra -Werror` contra stubs de host en `tests/stub/`
- Al tocar una `#define` de config, **verificar que compila en los dos
  valores** (se rompió varias veces así)
- El **simulador HTML no se commitea**: se entrega como archivo local
