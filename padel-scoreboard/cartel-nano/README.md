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

### Control remoto (3 o 4 botones)

| Botón | Corto | Largo (1 s) |
|:-:|---|---|
| **1** | **+1 punto Equipo A** | el saque pasa a A |
| **2** | **+1 punto Equipo B** | el saque pasa a B |
| **3** | **DESHACER** (6 pasos) | ciclar brillo |
| **4** *(si hay)* | ciclar brillo | reset del partido |

Con `BOTONES_CONTROL 3` el canal 4 no se usa y nada se pierde.

El **deshacer restaura el estado completo**: también deshace un juego, un set o
el partido si ese punto los había cerrado.

### Botones de la caja

| | |
|---|---|
| **DEPORTE** (D12) | corto: **cambia de deporte** · largo: ciclar brillo |
| **RESET** (A1) | **mantener 2 s**: una barra roja se va llenando; al llenarse, resetea |

### Sensores de paleta

**Izquierdo = equipo A · derecho = equipo B.** Siempre activos, en paralelo con
el control.

Antirrebote de **3 barreras** para que no cuente de más: hay que **sostener la
paleta 250 ms**, después queda **bloqueado 2,5 s**, y además **hay que retirar
la paleta** para que vuelva a contar.

---

## ⚙️ Ajustes rápidos en `cartel-nano.ino`

```c
#define DEPORTE_INICIAL     0   // con cuál arranca
#define BOTONES_CONTROL     3   // 3 o 4
#define SENS_ACTIVO_BAJO true   // false si tu sensor da HIGH al detectar
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

## 🔜 Lo que queda para la versión profesional (ESP32)

- Nombres de los equipos y panel más grande
- Historial de sets en columnas, como los marcadores de TV
- App por WiFi, audio con locuciones, reloj de turno
- Logo del sponsor
