# 08 - Versiones de producto y audio

Ideas capturadas para no perderlas. **Nada de esto está implementado todavía.**

## Las dos versiones

### 🟢 Simple — grupos que juegan (~$95.000 ARS)

Lo que está construido hoy. Cerebro Nano, `LAYOUT_FULL` o `LAYOUT_SPARSE`,
power bank, 2 controles RF, 2 sensores de paleta, caja estanca, se cuelga de la
reja con precintos.

Perfil: un grupo fijo que juega siempre junto y se lo lleva en el bolso.
Portátil, sin instalación, sin configuración.

### 🔵 Profesional — clubes

Sobre la misma base lógica, con estos saltos:

| Aspecto | Simple | Profesional |
|---|---|---|
| Tamaño del dígito | 23 cm | **40-50 cm** (módulos LED de cartelería, no tira) |
| Cerebro | Nano | **ESP32** |
| Alimentación | power bank | fija 220 V, con respaldo |
| Controles | RF 433 | **ESP-NOW recargables** + app |
| App | — | **sí** (ver doc 04) |
| Historial | — | **sets guardados, exportables** |
| Montaje | precintos | estructura fija, frente de aluminio compuesto |
| Audio | — | **sí** (abajo) |

Lo importante: **la máquina de estados del tanteo es la misma**. Todo el salto
está en display, conectividad y montaje. No se rehace la lógica.

## Audio (versión profesional)

Mensajes grabados que suenan solos en los momentos del partido.

### Momentos que el firmware ya detecta y puede disparar

El estado ya expone todo lo necesario, no hay que calcular nada nuevo:

| Evento | De dónde sale |
|---|---|
| Punto ganado | `addPoint()` |
| **Punto de juego / set / partido** | `situation(team)` |
| Punto de quiebre | `isBreakPoint(team)` |
| Juego ganado / set ganado | `lastWonGame()` / `lastWonSet()` |
| Partido ganado | `lastWonMatch()` + `winner` |
| Entra en tie break | `phase == PHASE_TIEBREAK` |
| 40-40 / punto de oro | `points[0] == points[1] == P_40` |

### Hardware

**DFPlayer Mini** (~3 USD): módulo MP3 con lector de microSD y amplificador de
3 W integrado. Se controla por serie con dos pines y un parlante de 4 Ω / 3 W.
Los mensajes son archivos `0001.mp3`, `0002.mp3`… en la SD, así que
**se graban con el celular y se copian a la tarjeta**. Cambiar las frases no
requiere tocar el firmware.

Una idea que sale gratis con esto: **locuciones personalizadas por equipo o por
club** (el nombre de los jugadores, la cortina del club) cambiando sólo la SD.

### La complicación en Nano, y por qué va en la versión pro

FastLED en AVR **deshabilita las interrupciones** durante cada `show()` (5-8 ms).
`SoftwareSerial` depende de interrupciones, así que un comando al DFPlayer
mandado justo durante un refresco se corrompe. Se puede trabajar alrededor
(mandar el comando siempre antes del `show()`), pero es frágil.

En **ESP32** desaparece: hay UART por hardware de sobra, y además se puede ir por
**I2S + MAX98357A**, que da mejor calidad y más volumen — que es lo que hace
falta en una cancha al aire libre, donde 3 W no alcanzan.

**Conclusión**: el audio entra naturalmente en la versión profesional, junto con
el paso a ESP32. En la simple se puede dejar el buzzer, que ya está.

### Lo que convendría dejar previsto desde ahora

Que el JSON del estado (ya congelado, doc 04) incluya los eventos — y ya los
incluye: `sit`, `winner`, `phase` y `n` alcanzan para que cualquier cliente
(audio, app, pantalla externa) sepa qué acaba de pasar sin recalcular reglas.
