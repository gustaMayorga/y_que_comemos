#!/usr/bin/env python3
"""Genera el mapa de LEDs y el plano del panel leyendo config.h.

Se regenera solo, asi que nunca queda desfasado del firmware:
    python3 montaje/mapa_leds.py > montaje/mapa-leds.txt
"""
import re, sys, pathlib

CFG = pathlib.Path(__file__).resolve().parent.parent / "firmware/padel_scoreboard/config.h"
txt = CFG.read_text()

def perfil_activo():
    for n in ("LAYOUT_BENCH", "LAYOUT_SPARSE", "LAYOUT_DEMO", "LAYOUT_FULL"):
        if re.search(rf"^#define {n}\s*$", txt, re.M):
            return n
    sys.exit("No hay ningun LAYOUT_* activo en config.h")

def leds_por_seg(perfil):
    # bloque del #ifdef del perfil activo
    m = re.search(rf"#ifdef {perfil}\n(.*?)#endif", txt, re.S)
    b = m.group(1)
    def v(nombre):
        return int(re.search(rf"#define {nombre}\s+(\d+)", b).group(1))
    return v("LEDS_PER_SEG_BIG"), v("LEDS_PER_SEG_SMALL"), v("LEDS_PER_SEG_TINY"), v("LEDS_PER_ICON")

def cadena():
    m = re.search(r"#define CADENA_SEGMENTOS\s*\{([^}]*)\}", txt)
    return [x.strip().replace("SEG_", "") for x in m.group(1).split(",")]

PERFIL = perfil_activo()
BIG, SMALL, TINY, ICON = leds_por_seg(PERFIL)
ORDEN = cadena()
PASO = 1000 / 60.0      # mm entre LEDs a 60 LED/m
CELDA = 12.0            # ancho del hueco del segmento

def medidas(n):
    L = n * PASO
    return L + 2 * CELDA, 2 * L + 3 * CELDA     # ancho, alto

print("=" * 72)
print(f" MAPA DE LEDS  -  perfil {PERFIL}  -  tira de 60 LED/m")
print("=" * 72)
print(f"\nLEDs por segmento: puntos={BIG}  juegos={SMALL}  sets={TINY}  iconos={ICON}")
print(f"Orden de la cadena dentro de cada digito: {' -> '.join(ORDEN)}\n")

for nom, n in (("PUNTOS", BIG), ("JUEGOS", SMALL), ("SETS", TINY)):
    a, h = medidas(n)
    print(f"  digito de {nom:7s} {n} LEDs/seg -> {h:5.0f} x {a:4.0f} mm "
          f"(segmento {n*PASO:.0f} mm)   {n*7:2d} LEDs")

# ---------------------------------------------------------------- mapa
bloques = []
i = 0
for equipo in ("A", "B"):
    for nom, n in (("puntos decena", BIG), ("puntos unidad", BIG),
                   ("juegos", SMALL), ("sets", TINY)):
        bloques.append((f"EQUIPO {equipo}  {nom}", i, i + n * 7 - 1, n))
        i += n * 7
m = re.search(r"#define ICONOS_POR_EQUIPO\s+(\d+)", txt)
POR_EQ = int(m.group(1)) if m else 2
m = re.search(r"#define CENTRO_AGRUPADO\s+(\w+)", txt)
AGRUP = (m.group(1) == "true") if m else False
if POR_EQ < 2:
    nombres = ("triangulo A (saque+aviso)", "triangulo B (saque+aviso)")
elif AGRUP:
    nombres = ("triangulo A - PUNTA (saque)", "triangulo A - CUERPO (aviso)",
               "triangulo B - PUNTA (saque)", "triangulo B - CUERPO (aviso)")
else:
    nombres = ("saque equipo A", "saque equipo B", "aviso punto A", "aviso punto B")
for nom in nombres:
    bloques.append((f"CENTRO    {nom}", i, i + ICON - 1, ICON))
    i += ICON
TOTAL = i

print(f"\n{'-'*72}\n LA CADENA, EN ORDEN (numeros como los cuenta el firmware, desde 0)\n{'-'*72}")
for nom, a, b, n in bloques:
    print(f"  {nom:28s}  LED {a:3d} - {b:3d}   ({n*7 if 'CENTRO' not in nom else n:2d} LEDs)")
print(f"\n  TOTAL: {TOTAL} LEDs   (un rollo de 5 m a 60/m trae 300: sobran {300-TOTAL})")

print(f"\n{'-'*72}\n DENTRO DE CADA DIGITO\n{'-'*72}")
for nom, n in (("PUNTOS", BIG), ("JUEGOS/SETS", SMALL)):
    print(f"\n  {nom} ({n} LEDs por segmento, {n*7} por digito):")
    for pos, seg in enumerate(ORDEN):
        ini = pos * n
        rango = f"{ini}" if n == 1 else f"{ini}-{ini+n-1}"
        print(f"    posicion {pos+1} de la cadena = segmento {seg}   -> LEDs +{rango}")

# ---------------------------------------------------------------- plano
aB, hB = medidas(BIG)
aS, hS = medidas(SMALL)
SEP_D, SEP_F, SEP_C, MARGEN = 12, 18, 25, 25
anc_fila1 = 2 * aB + SEP_D
anc_fila2 = 2 * aS + 20
anc_equipo = max(anc_fila1, anc_fila2)
alto_equipo = hB + SEP_F + hS
anc_centro = 70
panel_a = 2 * anc_equipo + 2 * SEP_C + anc_centro + 2 * MARGEN
panel_h = alto_equipo + 2 * MARGEN

print(f"\n{'-'*72}\n PLANO DEL PANEL\n{'-'*72}")
print(f"  bloque de un equipo : {anc_equipo:.0f} x {alto_equipo:.0f} mm")
print(f"  columna del centro  : {anc_centro} mm de ancho")
print(f"  PANEL COMPLETO      : {panel_a:.0f} x {panel_h:.0f} mm  "
      f"(~{panel_a/10:.0f} x {panel_h/10:.0f} cm)")
print(f"""
   <--------------------------- {panel_a:.0f} mm --------------------------->
   +------------------------------------------------------------------+
   |   EQUIPO A                   SAQUE                    EQUIPO B    |
   |  +-------+ +-------+        +-----+          +-------+ +-------+  |
   |  |       | |       |        | < > |          |       | |       |  |   {hB:.0f} mm
   |  | {BIG} LED | | {BIG} LED |        +-----+          | {BIG} LED | | {BIG} LED |  |
   |  +-------+ +-------+        PUNTO            +-------+ +-------+  |
   |   {aB:.0f}mm     {aB:.0f}mm         +-----+                                |
   |  +----+   +----+            | ^ ^ |          +----+   +----+      |   {hS:.0f} mm
   |  |{SMALL} LD|   |{SMALL} LD|            +-----+          |{SMALL} LD|   |{SMALL} LD|      |
   |  +----+   +----+                              +----+   +----+     |
   |  JUEGOS   SETS                                JUEGOS   SETS       |
   +------------------------------------------------------------------+
        sensor A                                        sensor B
""")
