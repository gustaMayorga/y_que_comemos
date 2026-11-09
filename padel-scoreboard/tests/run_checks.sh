#!/usr/bin/env bash
# Chequeos que corren en la PC, sin placa:
#   1) tests de la maquina de estados del tanteo
#   2) compilacion del sketch completo en las 6 combinaciones placa x perfil
set -u
cd "$(dirname "$0")/.."
FW=firmware/padel_scoreboard
STUB=tests/stub
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0

echo "== 1) tests de la maquina de estados =="
g++ -std=c++17 -Wall -Wextra tests/test_score.cpp "$FW/padel_score.cpp" -o "$TMP/test_score" \
  && "$TMP/test_score" || fail=1

echo
echo "== 2) compilacion de todas las variantes =="
for board in BOARD_NANO BOARD_ESP32; do
  for lay in LAYOUT_BENCH LAYOUT_SPARSE LAYOUT_DEMO LAYOUT_FULL; do
    d="$TMP/${board}_${lay}"
    cp -r "$FW" "$d"
    # Comentar TODOS los perfiles y placas activos y despues descomentar solo
    # los dos que toca. Si se comenta solo el que viene por defecto, al cambiar
    # ese default quedan dos perfiles activos y todo falla por redefinicion.
    sed -i -E 's|^#define (LAYOUT_[A-Z]+)[[:space:]]*$|//#define \1|' "$d/config.h"
    sed -i -E 's|^#define (BOARD_[A-Z0-9]+)[[:space:]]*$|//#define \1|' "$d/config.h"
    sed -i -E "s|^//#define ($board)[[:space:]]*\$|#define \1|" "$d/config.h"
    sed -i -E "s|^//#define ($lay)[[:space:]]*\$|#define \1|" "$d/config.h"
    activos=$(grep -cE "^#define (LAYOUT_|BOARD_)" "$d/config.h")
    if [ "$activos" != "2" ]; then
      printf "  %-12s %-13s FALLO: quedaron %s defines activos, deberian ser 2\n" \
             "$board" "$lay" "$activos"; fail=1; continue
    fi
    cp "$d/padel_scoreboard.ino" "$d/sketch.cpp"
    if [ "$board" = "BOARD_ESP32" ]; then
      # el stub no emula ledc*: se chequea el nucleo, no el .ino
      out=$(g++ -std=c++14 -Wall -Wextra -Wno-unused-parameter -I"$STUB" -I"$d" -fsyntax-only \
                "$d/display7seg.cpp" "$d/inputs.cpp" "$d/padel_score.cpp" \
                "$d/almacen.cpp" "$d/licencia.cpp" 2>&1 | head -5)
    else
      out=$(g++ -std=c++14 -Wall -Wextra -Wno-unused-parameter -I"$STUB" -I"$d" \
                "$d/sketch.cpp" "$STUB/main.cpp" "$d/display7seg.cpp" "$d/inputs.cpp" \
                "$d/padel_score.cpp" "$d/almacen.cpp" "$d/licencia.cpp" -o "$d/sk" 2>&1 | head -5)
    fi
    if [ -n "$out" ]; then printf "  %-12s %-13s FALLO\n%s\n" "$board" "$lay" "$out"; fail=1
    else                   printf "  %-12s %-13s OK\n" "$board" "$lay"; fi
  done
done

echo
echo "== 3) compilacion de las herramientas de diagnostico =="
for t in diag_nano diag_leds test_segmentos demo_digito demo_matriz test_cartel; do
  d="$TMP/tool_$t"; mkdir -p "$d"
  cp "tools/$t/$t.ino" "$d/sketch.cpp"
  out=$(g++ -std=c++14 -Wall -Wextra -Wno-unused-parameter -I"$STUB" \
            "$d/sketch.cpp" "$STUB/main.cpp" -o "$d/sk" 2>&1 | head -5)
  if [ -n "$out" ]; then printf "  %-12s FALLO\n%s\n" "$t" "$out"; fail=1
  else                   printf "  %-12s OK\n" "$t"; fi
done

echo
echo "== 4) compilacion del tanteador matriz =="
d="$TMP/display"; mkdir -p "$d"
cp display/display/display.ino "$d/sketch.cpp"
out=$(g++ -std=c++14 -Wall -Wextra -Wno-unused-parameter -I"$STUB" \
          "$d/sketch.cpp" "$STUB/main.cpp" -o "$d/sk" 2>&1 | head -6)
if [ -n "$out" ]; then printf "  display      FALLO\n%s\n" "$out"; fail=1
else                   printf "  display      OK\n"; fi

echo
[ "$fail" = 0 ] && echo "TODO OK" || echo "HAY FALLOS"
exit $fail
