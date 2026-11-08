# 🔨 Armado del panel matriz — paso a paso

Para que quede como de fábrica. Con lo que ya tenés: panel blanco de cartelería,
aerosol negro, cinta doble faz, gotita, silicona caliente, silicona blanca y
acetato transparente.

---

## 0. La decisión: **RÍGIDO**, sin dudarlo

| | Rígido ✅ | Flexible ❌ |
|---|---|---|
| Geometría | las filas quedan a 16,67 mm fijos | se mueve y los dígitos se deforman |
| Soldaduras | quietas | **se rajan al flexionar** ← falla #1 |
| Difusor | distancia constante a los LEDs | distancia variable = luz despareja |
| Montaje en reja | fácil de sellar y colgar | necesita bastidor igual |

La unión entre filas son **3 soldaduras cada una, 33 en total**. Cada flexión
las fatiga. Un panel flexible de verdad no se hace así: se hace con LEDs
encapsulados en silicona sobre PCB flexible, sin grilla rígida — es otro producto.

**Para la v1: rígido.** Si algún día querés uno enrollable, se diseña desde cero.

---

## ⚠️ Las 4 reglas de los pegamentos

Antes de empezar, porque tres de estas arruinan el trabajo:

1. **La gotita NUNCA cerca del acetato.** El cianoacrilato libera vapores que
   **empañan los plásticos transparentes de forma permanente** (efecto
   *blooming*). Un panel con el difusor blanquecino no se arregla. Gotita sólo
   para el marco, **antes** de poner el acetato y con el acetato en otra habitación.
2. **La silicona caliente se ablanda a 60-70 °C.** Adentro de un panel negro al
   sol se llega. Sirve para **fijar cables y reforzar puntas**, nunca como
   adhesivo estructural.
3. **Si la silicona blanca huele a vinagre, es acética**: corroe el cobre y las
   soldaduras. En ese caso, usala sólo para sellar la caja por fuera, lejos de
   las tiras.
4. **El adhesivo 3M de la tira se despega con el calor.** Siempre: cinta doble
   faz + un punto de refuerzo en **los dos extremos** de cada fila.

---

## Fase 1 — Preparar el panel

**1.1** Cortá el panel a **422 × 238 mm** (área activa 372 × 188 + 25 mm de margen).

**1.2** 🎯 **Redondeá las esquinas** con un radio de 8-10 mm, usando una moneda o
un frasco como plantilla. Es un detalle de 5 minutos y es **lo que más hace que
parezca de fábrica**.

**1.3** Marcá las 12 filas con lápiz, desde el borde superior al **centro** del LED:

| | | | |
|---|---|---|---|
| fila 0 → **25,0** | fila 1 → **41,7** | fila 2 → **58,3** | fila 3 → **75,0** |
| fila 4 → **91,7** | fila 5 → **108,3** | fila 6 → **125,0** | fila 7 → **141,7** |
| fila 8 → **158,3** | fila 9 → **175,0** | fila 10 → **191,7** | fila 11 → **208,3** |

> 🔑 **El mejor truco**: en vez de medir con regla, usá **un pedazo de tira como
> plantilla**. Sus LEDs ya están exactamente a 16,67 mm. Apoyala vertical y marcá
> en cada LED. Exacto y sin acumular error.
>
> **No redondees 16,67 a 17 mm.** En 22 pasos el error se acumula a **7,3 mm**:
> la última columna queda corrida casi medio LED.

**1.4** Primera columna a **25 mm** del borde izquierdo.

**1.5** 🖤 **Pintá el panel de NEGRO MATE**, 2 o 3 manos finas. El blanco rebota
luz entre píxeles y arruina el contraste: esto no es opcional.

**1.6** ⏳ **Dejá curar 24 h.** La pintura fresca gasea y la cinta doble faz no
pega. Si apurás este paso, en una semana se te despegan las tiras.

---

## Fase 2 — Pegar y soldar las tiras

**2.1 Prueba en seco primero.** Apoyá las 12 filas sin pegar y verificá que las
**flechas alternen**: fila 0 →, fila 1 ←, fila 2 →… Es el momento de descubrir
que cortaste una al revés.

**2.2** Pegá con **cinta doble faz** (además del adhesivo propio de la tira).
Una tira continua de doble faz por fila, no puntos sueltos.

**2.3** 🔩 Un punto de **silicona caliente en los dos extremos** de cada fila.
Eso es lo que la sostiene cuando el 3M falle con el calor.

**2.4 Soldá los saltos entre filas**: 3 cables de 3-5 cm (5V, DIN, GND),
**salida de una fila → entrada de la siguiente**. Estañá los pads primero,
**2-3 segundos por pad** y afuera.

**2.5** Después de soldar y **probar**, una gota de silicona caliente **sobre
cada soldadura**: alivio de tensión. Si no, el cable hace palanca y arranca el pad.

---

## Fase 3 — 🛑 Probar ANTES de cerrar

Esta fase no se saltea. Reparar con el difusor puesto es rehacerlo todo.

**3.1** Cargá `tools/demo_matriz` y corré:
- **`e`** → esquinas: 🔴 arr-izq · 🟢 arr-der · 🔵 ab-izq · 🟡 ab-der.
  Si no coinciden, ajustá `SERPENTINA` / `FILA0_INVERTIDA` / `ORIGEN_ARRIBA`.
- **`m`** → recorrido LED por LED: tiene que serpentear.
- **`n`**, **`j`** → números y tanteo.

**3.2** 🔥 **Dejalo 30 minutos al 100 % de brillo.** Las soldaduras frías fallan
con el calor, no en frío. Mejor que aparezcan ahora.

**3.3** Con el multímetro: 5 V en el **último** LED de la cadena. Si cayó por
debajo de **4,6 V**, hace falta otra inyección.

---

## Fase 4 — Alimentación

**4.1** **2 puntos de inyección** de 5 V (276 LEDs, uno cada ~150): al principio
de la fila 0 y al final de la fila 11. Cable **18 AWG** o más grueso.

**4.2** **Capacitor de 1000 µF** entre 5 V y GND en la entrada. No es opcional:
sin él el pico de arranque resetea la placa.

**4.3** **470 Ω** en serie con el dato, pegado al pin de la placa.

**4.4** Fusible de 5 A en el positivo.

---

## Fase 5 — Marco y difusor

**5.1** 🚫 **No hagas una grilla de celdas.** Para 7 segmentos los tabiques son
imprescindibles; para una **matriz son contraproducentes**: querés que los
píxeles se fundan un poco para que el texto se vea sólido, no 276 puntitos
aislados. Te ahorrás 276 celdas.

**5.2** Cortá tiras de **20 mm de ancho** del mismo panel para el **marco
perimetral**. Esa altura es la distancia LED → difusor, y es la que convierte
puntos en luz pareja.

**5.3** Pegá el marco con **gotita** (sobre PVC espumado agarra muy bien).
⚠️ **Con el acetato lejos**, en otra habitación, y dejá ventilar 1 hora.

**5.4** **2 costillas verticales** de 3 mm a 1/3 y 2/3 del ancho. El acetato fino
**se panda** en 37 cm sin apoyo. Se ven como dos líneas finas; es mejor eso que
un difusor hundido.

**5.5** Pintá marco y costillas de **negro mate** también.

**5.6** 💡 **Esmerilá el acetato**: lija al agua **400-600** por la cara
**interna** (la que mira a los LEDs), en círculos, hasta que quede parejo y
translúcido. **El acetato transparente no difunde nada** — vas a ver los puntos.
Lijado, se transforma en un difusor decente.

> Probalo antes de pegarlo: sostenelo a 20 mm del panel encendido. Si todavía se
> ven los puntos, lijá más o sumá una capa de papel vegetal por detrás.

**5.7** Pegá el acetato al marco con **doble faz fina** en todo el perímetro,
bien estirado.

---

## Fase 6 — Los detalles que lo hacen parecer de fábrica

Son siete, y ninguno lleva más de 10 minutos:

1. ⬛ **Todo lo visible, negro mate.** Cualquier borde blanco a la vista lo delata.
2. 🖼️ **Borde negro de 5 mm** en el perímetro del acetato (cinta de vinilo negra
   o pintado). Enmarca el área activa y tapa el canto del marco.
3. 🔌 **Ningún cable al frente.** Todo sale por atrás.
4. 🕳️ **Pasacables con prensaestopa** (o un agujero sellado con silicona
   **neutra**) en la parte de atrás, nunca abajo donde junta agua... en realidad
   **sí abajo**: así el agua que entre escurre. Nunca arriba.
5. 📦 **Caja de electrónica atrás**, atornillada, con el Nano y las borneras.
6. 🔩 **Ningún tornillo visible de frente.**
7. ✨ **Esquinas redondeadas** (fase 1.2) y cantos lijados.

---

## Checklist

- [ ] Panel cortado 422×238, esquinas redondeadas
- [ ] 12 filas marcadas con la tira como plantilla
- [ ] Pintado negro mate · **24 h de curado**
- [ ] Prueba en seco: flechas alternadas
- [ ] Tiras pegadas con doble faz + refuerzo en las puntas
- [ ] 33 saltos soldados + silicona sobre cada soldadura
- [ ] **`demo_matriz` → `e` y `m` ✓**
- [ ] **30 min al 100 % de brillo sin fallas**
- [ ] 5 V medidos en el último LED > 4,6 V
- [ ] 2 inyecciones + capacitor + 470 Ω + fusible
- [ ] Marco de 20 mm + 2 costillas, en negro
- [ ] Acetato esmerilado con lija 400-600
- [ ] Borde negro perimetral
- [ ] Caja atrás, pasacables sellado abajo
