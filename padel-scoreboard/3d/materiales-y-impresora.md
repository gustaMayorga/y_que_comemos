# 🖨️ Impresora y materiales para cartelería — en orden de prioridad

Qué comprar primero, qué puede esperar, y qué no hace falta. Para una
**Creality Ender 3 Pro**.

---

## 0. El cianoacrilato: el problema es el VAPOR, no el contacto

La gotita, al curar, libera vapores que **se condensan en las superficies
cercanas como una película blanca** (se llama *blooming*). Sobre plástico negro
opaco no se nota. Sobre un difusor transparente o esmerilado queda un **velo
blanco permanente** que no sale con nada.

| Cuándo | |
|---|---|
| ✅ **Sí** | marco al panel, costillas, piezas rígidas chicas, reparaciones — **siempre antes de que el difusor exista en la habitación** |
| ❌ **Nunca** | con el acetato o el acrílico en la misma mesa. Ni para pegar el difusor |

**Regla práctica**: usás gotita → **ventilás 1 hora** → recién ahí traés el difusor.

### Cómo pegar el acetato: **cinta doble faz, no pegamento**

Una tira continua de doble faz fina por todo el perímetro del marco. Nada más.

Y hay una razón de fondo para no pegarlo: **el difusor tiene que poder sacarse**.
El día que se te apague un LED del medio, con cinta lo despegás en 2 minutos;
pegado, rompés el panel entero para llegar.

Si después querés sellarlo contra el agua: un cordón de **silicona neutra** por
fuera del perímetro. Se corta con trincheta cuando haga falta abrirlo.

### ¿Conviene esperar a imprimir el difusor?

**No. Hacé las dos cosas.** Armalo ahora con el acetato pegado con doble faz,
probalo, jugá un partido. Cuando tengas la impresora lista, imprimís el difusor
definitivo y **lo cambiás en 10 minutos** — justamente porque está con cinta.

No bloquees el proyecto esperando un repuesto que es la pieza más fácil de
cambiar.

---

## 1. Lo que comprar primero: **PETG natural** 🥇

Y acá hay una corrección importante a lo que suele recomendarse: **para PETG no
necesitás cambiarle nada a la impresora todavía.**

PETG imprime a **230-245 °C**, y el hotend original con tubo de PTFE aguanta ese
rango. La degradación del teflón empieza a importar **arriba de 250 °C
sostenidos**. Miles de personas imprimen PETG con una Ender 3 Pro de fábrica.

**PETG resuelve el 90 % de este proyecto:**

| Pieza | Material |
|---|---|
| Difusor | **PETG natural** (translúcido), 1-1,5 mm, 100 % relleno |
| Marco, costillas, celdas | **PETG negro** |
| Caja de electrónica, bandeja del power bank | **PETG negro** |
| Soportes de montaje a la reja | **PETG negro** |

Y lo importante para vos: **el PETG no se deforma a 55 °C como el PLA**. Adentro
de un panel negro al sol se llega a esa temperatura, y ahí el PLA se vence.

**Comprá**: 1 rollo **PETG natural/translúcido** + 1 rollo **PETG negro**, 1,75 mm.
Con eso hacés el cartel entero.

### Ajustes de PETG en la Ender 3 Pro

| | |
|---|---|
| Boquilla | 235-245 °C |
| Cama | 75-85 °C |
| Velocidad | 40-50 mm/s (más lento que PLA) |
| Retracción | 4-5 mm — el PETG hila, es normal |
| Ventilador | **30-50 %**, no al 100 %: enfriar de más lo vuelve quebradizo |
| Adherencia | laca en barra o pegamento en barra. **Ojo: el PETG se pega DEMASIADO al vidrio y le arranca pedazos** — siempre con separador |

---

## 2. Adherencia de cama 🥈

Antes que cualquier upgrade del hotend. Es lo que más despegues evita y lo más
barato.

- **Lámina PEI magnética** (~15-25 USD): la pieza sale sola al enfriar, no hay
  que hacer palanca. Es la mejora con mejor relación precio/beneficio de toda la
  impresora.
- Alternativa gratis: vidrio + pegamento en barra.

---

## 3. Hotend all-metal 🥉 — **sólo cuando quieras ASA**

Acá otra corrección: el **tubo Capricorn NO es un upgrade de temperatura**.
Sigue siendo PTFE. Lo que mejora es la tolerancia interna (1,95 mm en vez de
2,0) y la fricción, que ayuda con los flexibles. Y si ponés un hotend all-metal,
el tubo **termina en el disipador** y su resistencia térmica deja de importar.

Y sobre el precio: un kit Micro Swiss completo es caro. El camino barato que
hace lo mismo es un **heatbreak bimetálico** (titanio-cobre, ~5-10 USD): elimina
el PTFE de la zona caliente, que es todo el punto. Es un repuesto, no un kit.

**Necesitás all-metal si**: ASA, ABS, nylon, policarbonato.
**No lo necesitás para**: PLA, PETG, TPU.

### Dos cosas que suelen olvidarse

- **El cartucho calefactor de 40 W original sufre arriba de 250 °C** con el
  ventilador soplando. Para ASA conviene uno de 50-60 W.
- **El firmware viene limitado** (`HEATER_0_MAXTEMP`). Hay que recompilar Marlin
  o pasar a Klipper.

---

## 4. ASA: sólo para lo que vive al sol permanentemente

**ASA es el material correcto para cartelería exterior**: no amarillea con UV,
aguanta el calor. Pero pedí lo que trae:

- Hotend all-metal + cama a 95-105 °C + **cerramiento obligatorio**
- ⚠️ **Los gases del ASA y el ABS (estireno) son realmente molestos y tóxicos.**
  El cerramiento sirve para la temperatura, **no para los gases**. Hace falta
  **ventilación al exterior o un filtro de carbón activado**, no la impresora
  encerrada en tu pieza.

Para el tanteador, **PETG alcanza y sobra**. ASA sería para la versión que va a
estar colgada años al sol sin bajarse.

---

## 5. TPU (flexible) — el último, y con una advertencia

**La Ender 3 Pro es Bowden, y el Bowden y el TPU se llevan mal.** El filamento
flexible se comprime en los 40 cm de tubo y la extrusión se vuelve impredecible.

El extrusor de doble engranaje ayuda, pero **la solución real es convertir a
direct drive** (el motor sobre el cabezal). Es un upgrade de 20-40 USD.

Para qué sirve el TPU acá: **juntas estancas**, burletes del difusor, topes y
protectores de esquina. Útil, pero no bloquea nada.

---

## 6. El láser: para este proyecto, casi inútil

Esto hay que decirlo claro porque se vende como si sirviera para todo.

**Un láser de diodo (5 W, 10 W, 20 W) NO corta acrílico transparente ni opal.**
No es cuestión de potencia: el acrílico es **transparente a la luz azul de
450 nm**, así que el haz lo atraviesa sin depositar energía. Para cortar acrílico
hace falta **CO₂ (10.600 nm)**, que es otra máquina.

| Sirve para | No sirve para |
|---|---|
| Grabar madera, MDF, cuero, cartón | ❌ Cortar acrílico (ni fino) |
| Cortar vinilo, papel, goma eva, cartón fino | ❌ Cortar policarbonato |
| Hacer plantillas y máscaras de pintura | ❌ Metal |

**Para tu cartelería**: el difusor y el filtro los comprás cortados o los
imprimís. El láser no entra en el camino crítico. Es un lindo juguete, no una
prioridad.

---

## 7. Cama más grande: **no la necesitás**

Tu panel mide 422 × 238 mm y la cama es de 220 × 220. Parece que hace falta una
impresora grande, pero **el panel no se imprime** — es placa de cartelería
cortada. Lo que imprimís son difusores, marcos, celdas y cajas.

**Diseñá para azulejar.** Un difusor de 372 mm se imprime en **2 o 3 piezas con
juntas solapadas**, y las uniones quedan **escondidas detrás del marco negro**.
En cartelería profesional se hace exactamente así: nadie imprime un frente de
2 metros de una pieza.

Si igual querés más volumen:

| Opción | Realidad |
|---|---|
| Kit Ender Extender 300/400 | Funciona, pero la cama grande suma inercia: hay que bajar la velocidad y recalibrar todo. Proyecto de fin de semana |
| Impresora grande nativa (CR-10, Ender 3 Max) | Chasis pensado para ese tamaño desde el diseño. Si de verdad vas a producir, es la opción sana |

**Mi recomendación**: quedate con la Ender 3 Pro y diseñá modular. Si el negocio
arranca, comprás una grande **además**, no en lugar de.

---

## 📋 Orden de compra

| # | Qué | Cuándo | Aprox. |
|---|---|---|---|
| 1 | **PETG natural + PETG negro** | ✅ ya | 2 rollos |
| 2 | **Lámina PEI magnética** | ✅ ya | 15-25 USD |
| 3 | Boquillas 0,4 y 0,6 de repuesto | ya | 5 USD |
| 4 | Heatbreak bimetálico | cuando quieras ASA | 5-10 USD |
| 5 | Cartucho calefactor 50 W | con el heatbreak | 5 USD |
| 6 | Cerramiento | sólo con ASA/ABS | 40-70 USD |
| 7 | Rollo de ASA negro | para la versión definitiva al sol | 1 rollo |
| 8 | Conversión a direct drive | si querés TPU en serio | 20-40 USD |
| 9 | Módulo láser | ⬜ no es prioridad | — |
| 10 | Cama más grande | ⬜ no hace falta | — |

Con **1 y 2** ya podés imprimir todo el cartel. El resto es para después.
