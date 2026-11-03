# 07 - Confirmación: ¿se puede armar el cartel con esto?

## Veredicto: **SÍ**

Tira pixel direccionable + Arduino Nano + 2 HC-SR04 + receptor RF de 4 canales con
2 llaveros + power bank = **cartel completo y funcional**. El firmware está escrito,
testeado y compilando. No falta ningún desarrollo para esa configuración.

**Lo que el firmware ya hace, hoy:**

- [x] Puntos 0/15/30/40/Ad en 4 dígitos grandes (2 por equipo)
- [x] Juegos y sets en dígitos chicos (1 + 1 por equipo)
- [x] Flechas de saque al centro, con cambio automático de saque
- [x] Aviso de punto de juego / set / partido, con jerarquía por color
- [x] Punto de oro o ventajas, 7-5, tie break a 7, partido al mejor de 3
- [x] Conteo por proximidad de paleta (izq. = A, der. = B) con antirrebote de 3 barreras
- [x] Conteo por los 2 controles RF (un botón por equipo)
- [x] Deshacer real, cambio de saque manual, 3 niveles de brillo, reset
- [x] Feedback sonoro y visual de punto / juego / set / partido
- [x] Límite de corriente por firmware para no pasarse de lo que da el power bank
- [x] Estado en JSON por puerto serie (contrato listo para la app)
- [x] 20+ tests de la lógica + compilación verificada en 8 combinaciones placa × perfil

**Lo que NO está probado (hay que ser honesto):** nada se corrió en hardware real
todavía, y el uso de RAM del Nano está **calculado, no medido** (~49 % con
`LAYOUT_DEMO`, ~62 % con `LAYOUT_FULL`). El número exacto lo dice `arduino-cli`
al compilar.

---

## La única corrección al plan: **andá por 5 V, no por el booster**

| | **Tira 5 V directo del power bank** | Tira 12 V + booster |
|---|---|---|
| Rendimiento | ✅ sin conversión, sin pérdidas | ⚠️ ~85 % de eficiencia |
| Techo de corriente | lo que dé el power bank (3 A) | **lo que dé el módulo**, que es el cuello de botella |
| Componentes | ninguno extra | un módulo más, que se calienta y puede fallar |
| Costo | 0 | +3 USD |

El problema concreto: para alimentar 278 LEDs a 12 V hacen falta ~0.9 A **a la
salida** del booster, que son **~2.4 A a la entrada** (a 5 V). Los módulos boost
baratos (**MT3608**, SX1308) no pasan de ~1 A real de entrada: se saturan y se
calientan. El **XL6009** llega justo. Es un punto de falla que no hace falta tener.

**Si igual necesitás 12 V** (porque la tira que conseguís es WS2811/WS2815 de 12 V),
el camino correcto **no es un booster** sino un **disparador USB-C PD**
(**CH224K** o **ZY12PDN**, ~3 USD): le pide al power bank sus propios 12 V
regulados por el protocolo PD. Eso usa la electrónica del power bank, que está
hecha para dar 2-3 A a 12 V, en vez de un módulo de 1 USD. Requisito: el power
bank tiene que soportar **USB-C PD** (si dice "PD 20 W / 30 W / 65 W", sí).

---

## La verificación que define el tamaño: **LEDs por metro**

Lo que importa no son los metros, es la **cantidad de LEDs** y la **densidad**.
5 metros pueden ser 300 LEDs o 150.

| Lo que compres | LEDs en 5 m | Perfil a usar | Dígito de puntos | Corriente típica |
|---|---|---|---|---|
| **30 LED/m** | 150 | **`LAYOUT_SPARSE`** (144) | **~230 mm** | **~1.6 A** |
| 60 LED/m | 300 | `LAYOUT_FULL` (278) | ~230 mm | ~3.1 A (limitada a 2.6 A) |
| 60 LED/m | 300 | `LAYOUT_DEMO` (190) | ~155 mm | ~2.1 A |
| 144 LED/m | 720 | no conviene: mucha corriente para el mismo tamaño | | |

### 🎯 Recomendación: **tira de 30 LED/m + `LAYOUT_SPARSE`**

Es el punto óptimo y contraintuitivo. A 30 LED/m el paso entre LEDs es de 33 mm,
así que **3 LEDs por segmento dan un segmento de 100 mm** — exactamente el mismo
tamaño de dígito que `LAYOUT_FULL` con 60 LED/m, pero con **la mitad de los LEDs**:

- Dígitos de **23 cm**, legibles a 70–90 m (igual que el cartel final)
- **~1.6 A típicos**: entra cómodo hasta en un power bank de 2 A
- 144 de los 150 LEDs del rollo: sobran 6 de margen
- La tira de 30 LED/m es **más barata** que la de 60
- Menos soldaduras, menos calor, más autonomía

**El único costo**: los LEDs quedan más separados, así que hay que poner el difusor
**25–30 mm por encima** (en vez de 15–20) para que no se vean los puntos
individuales. Es un tabique 1 cm más alto, nada más.

Si sólo conseguís de 60 LED/m, usá `LAYOUT_DEMO` (190 LEDs, dígitos de 15 cm,
legibles a 45 m — más que suficiente para una cancha) y te sobran 110 LEDs para
repuestos y pruebas.

---

## Lista de compras cerrada

🔴 = sin esto no funciona · 🟠 = se nota mucho · 🟢 = se puede improvisar

| # | Ítem | Cant. | ~USD | |
|---|---|---|---|---|
| 1 | **Tira pixel direccionable** (WS2812B 5 V, **30 LED/m**, IP65), rollo 5 m | 1 | 10–14 | 🔴 |
| 2 | Arduino Nano | 1 | 5 | 🔴 ya tenés |
| 3 | Sensor **HC-SR04** | 2 | 3 | 🔴 |
| 4 | **Receptor RF 433 MHz de 4 canales** (PT2272-M4) | 1 | 4 | 🔴 |
| 5 | **Llavero RF 433 MHz** EV1527 4 botones | 2 | 3 | 🔴 un control por equipo |
| 6 | Capacitor **1000 µF / 10 V** (o 470 µF) | 1 | 0.3 | 🔴 sin esto el Nano resetea |
| 7 | Resistor **470 Ω** (en serie con el dato) | 1 | 0.1 | 🔴 |
| 8 | Power bank 5 V / **2 A o más** | 1 | — | 🔴 ya tenés |
| 9 | Cable **18 AWG** rojo y negro, 2 m | — | 2 | 🟠 para la alimentación |
| 10 | Cable fino 3 colores, 10 m | — | 2 | 🟠 entre segmentos |
| 11 | **Acrílico humo o rojo oscuro** 2 mm (A3) — *el filtro de contraste* | 1 | 9 | 🟠 es lo que hace que se lea al sol |
| 12 | Acrílico opal blanco 3 mm (A3) | 1 | 9 | 🟢 reemplazable por papel manteca |
| 13 | Cartón pluma / PVC foam **negro** 3 mm | 2 | 8 | 🟢 reemplazable por cartón pintado negro mate |
| 14 | Buzzer pasivo + BC337 + R 1 kΩ | 1 | 1 | 🟢 el feedback sonoro ayuda bastante |
| 15 | Fusible 5 A + portafusible | 1 | 1 | 🟢 recomendado |
| 16 | Caja estanca / bandeja PVC, bridas, silicona neutra | — | 10 | 🟢 |
| | **Total de lo 🔴 crítico** | | **~21** | (sin contar Nano y power bank) |
| | **Total completo** | | **~62** | |

**Si el power bank es de 12 V vía PD**: sumar disparador **CH224K / ZY12PDN** (3 USD)
y usar tira **WS2815 de 12 V**. **No** sumar booster.

---

## Pasos, en orden

1. **`tools/diag_nano`** → confirmar que el Nano vive. (5 min, sin componentes)
2. **`tools/diag_leds`** → con la tira: comando `w` para contar LEDs, `o` para el
   orden de color, `t` para medir la corriente real con el multímetro.
3. **Ajustar `config.h`**: perfil según la densidad, `LED_COLOR_ORDER` si salió
   distinto de GRB, y `PSU_MILLIAMPS` con el amperaje real del power bank.
4. **Armar UN dígito** y verificarlo con `diag_leds` comando `d` (cuenta 0-9).
   Procedimiento paso a paso en [`06-identificar-lo-que-tengo.md`](06-identificar-lo-que-tengo.md) §8.
5. **Probarlo al sol** con y sin el filtro oscuro. Es el momento de la verdad.
6. Armar los 7 dígitos restantes + los 4 iconos del centro.
7. Cargar el firmware completo, calibrar los umbrales de proximidad **en la cancha**.
8. Jugar un partido entero de punta a punta.

Entre el paso 3 y el 4 ya se puede jugar un partido completo mirando el monitor
serie, sin ningún LED armado.
