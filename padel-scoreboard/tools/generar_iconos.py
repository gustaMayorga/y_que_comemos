ICONOS = {
"PALETA DE PADEL": """
...#####...
..#######..
.##.#.#.##.
.#.#.#.#.#.
.##.#.#.##.
.#.#.#.#.#.
.##.#.#.##.
..#######..
....###....
....###....
....###....""",
"RAQUETA DE TENIS": """
...#####...
..##...##..
.#..#.#..#.
.#.#####.#.
.#..#.#..#.
.#.#####.#.
.#..#.#..#.
..##...##..
....#.#....
....#.#....
....###....""",
"PALETA DE PING PONG": """
...#####...
..#######..
.#########.
.#########.
.#########.
.#########.
..#######..
...#####...
....###....
....###....
....###....""",
"PELOTA DE BASQUET": """
...#####...
.##..#..##.
.#...#...#.
#....#....#
#....#....#
#.#######.#
#....#....#
#....#....#
.#...#...#.
.##..#..##.
...#####...""",
"PELOTA DE VOLEY": """
...#####...
.##.#...##.
.#..#.##.#.
#...#.#...#
#.###.#...#
#.#...#.###
#.#...#...#
#...#.#...#
.#..#.##.#.
.##.#...##.
...#####...""",
"PELOTA DE FUTBOL": """
...#####...
.##..#..##.
.#..###..#.
#..#####..#
#.#######.#
#..#####..#
#...###...#
#..#...#..#
.#.#...#.#.
.##.....##.
...#####...""",
}
W, H = 11, 11
out=[]
for nombre, arte in ICONOS.items():
    filas=[l for l in arte.strip().split("\n")]
    assert len(filas)==H, (nombre, len(filas))
    for i,f in enumerate(filas): assert len(f)==W, (nombre, i, len(f), repr(f))
    pal=[sum(1<<x for x,ch in enumerate(f) if ch=='#') for f in filas]
    out.append((nombre, pal, filas))
    print(f"\n  {nombre}   ({sum(f.count('#') for f in filas)} pixeles encendidos)")
    for f in filas: print("   " + f.replace('#','██').replace('.','  '))
print("\n--- tabla C ---")
print(f"#define ICO_ANCHO {W}\n#define ICO_ALTO  {H}")
print("const uint16_t ICONOS[][ICO_ALTO] PROGMEM = {")
for nombre,pal,_ in out:
    print(f"/* {nombre:<20} */ {{" + ",".join(f"0x{v:03X}" for v in pal) + "},")
print("};")
