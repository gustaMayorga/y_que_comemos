# Carcasa 3D para el HC-SR04

Archivo: [`carcasa_hcsr04.scad`](carcasa_hcsr04.scad) · Dos piezas, encastre a presión, sin tornillos para cerrar.

```
  49.8 × 24.8 × 24.2 mm   (71.8 mm de largo con las orejas)
```

## Corte lateral

```
      PANEL (3 mm)
        │
   ┌────┼───────────────────────────────────────────┐
   │    │  ┌─ cara delantera, 2 mm ─┐               │
  ═╪════╪══╡                        ╞═══════════════╡  ← oreja M3
   │ ███│██│                        │               │
   │ ███│██│  ← cápsula Ø16         │   cámara      │
   │ ███│██│     asoma 3 mm         │   trasera     ├─ tapa
   │    │  │                        │   12 mm       │   2 mm
   │ ███│██│                   ╔════╡               │
   │ ███│██│                   ║PCB │  ►postes◄     │
   │ ███│██│                   ╚════╡               │
  ═╪════╪══╡                        ╞═══════════════╡
   └────┼──┴─pestaña 1.2 mm─────────┴───────────────┘
        │        (apoya el PCB)        ↑ salida de cable Ø5
   z=0 ─┘
```

Las cápsulas atraviesan la cara delantera **y el panel**, y quedan al ras de la
cara exterior. Detrás de esos 2 mm no hay nada que las toque: el hueco es de
43 × 17.6 mm. **No quedan metidas en un tubo.**

## Las 3 cosas que importan

1. **Agujeros Ø16.6 para cápsulas de Ø16** → 0.3 mm de aire por lado. El plástico
   nunca toca la cápsula.
2. **Sellado con silicona neutra**, nunca a presión ni con cianoacrilato. La
   silicona tapa el aire, aguanta la lluvia y **desacopla**: sin eso, el pulso
   de la cápsula que emite viaja por el plástico hasta la que recibe y el
   sensor lee una distancia chica constante.
3. **`SALIENTE = espesor de tu panel`.** Viene en 3.0. Si lo dejás mal, las
   cápsulas quedan hundidas en el agujero del panel y leen el borde.

## Parámetros que vas a tocar

| Parámetro | Viene | Cuándo cambiarlo |
|---|---|---|
| `SALIENTE` | `3.0` | **Siempre**: ponelo igual al espesor de tu panel. `0` si va suelta |
| `PROF_TRASERA` | `12.0` | `22` si dejás la ficha DuPont enchufada en vez de soldar |
| `MONTAJE` | `"brida"` | `"ninguno"` si la pegás en vez de atornillarla |
| `HOLGURA_CAP` | `0.60` | Si tu cápsula entra forzada, subilo a `0.8` |
| `HOLGURA_TAPA` / `APRIETE` | `0.25` / `0.60` | Si la tapa no entra, subí `HOLGURA_TAPA`; si queda floja, subí `APRIETE` |

`PIEZA` elige qué generar: `cuerpo`, `tapa`, `ambas` (por defecto, las dos en la
cama), `plantilla` (para marcar los agujeros del panel) o `corte` (vista en
sección, para mirar antes de imprimir).

## Imprimir

Ninguna de las dos piezas necesita soportes.

| | Orientación | |
|---|---|---|
| **cuerpo** | cara **delantera** contra la cama | los Ø16.6 salen redondos |
| **tapa** | cara **exterior** contra la cama | los postes crecen hacia arriba |

0.2 mm de capa · 3 perímetros · 25 % relleno · **PETG** si va a tomar sol (el PLA
se deforma a 55 °C dentro de una caja negra al sol; el PETG no).

## Armado

1. Imprimí también `PIEZA = "plantilla"`, apoyala en el panel y marcá los dos
   centros. Taladrá **Ø16.5** (broca de copa o escalonada, no helicoidal: se
   come el borde).
2. Pasá los cables por la lágrima Ø5 del fondo de la carcasa y **soldalos
   directo** al HC-SR04 — es lo que corresponde en un montaje definitivo y te
   ahorra 10 mm de profundidad.
3. Meté el PCB desde atrás, cápsulas primero, hasta que apoye en la pestaña.
4. **Precinto alrededor del cable, por dentro, pegado a la pared.** Ese es el
   sujeta-cable: no puede tirar del soldado.
5. Cerrá la tapa a presión. Los 4 postes aprietan el PCB contra la pestaña.
   Para abrirla, palanca en el rebaje del borde de arriba.
6. Montá la carcasa por detrás del panel, con las cápsulas asomando por los
   agujeros, y atornillá las orejas (M3).
7. **Último paso, ya montado:** un cordón fino de **silicona neutra** alrededor
   de cada cápsula, por la cara exterior del panel. Dejala curar 24 h antes de
   probar.

## Antes de cerrar, probá

Cargá `tools/diag_nano` o el sketch con `SENSOR_TIPO 4` y verificá que a
20 cm lee 20 cm. Si lee siempre lo mismo (3–5 cm) y no cambia, es el eco
estructural: las cápsulas están tocando plástico en algún punto. Agrandá los
agujeros y volvé a sellar solo con silicona.

## A la intemperie

El HC-SR04 común **no es estanco**. Si el cartel va a tomar lluvia, el
**JSN-SR04T** (~4 USD) es una sola cápsula sellada con cable, misma interfaz
TRIG/ECHO y el mismo código sin tocar nada. Para eso la carcasa cambia: una
sola cápsula de Ø23 enroscada al panel, y esta caja pasa a alojar solo la
plaquita. El agujero de desague de abajo ya está puesto por las dudas.
