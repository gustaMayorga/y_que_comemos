// ============================================================================
//  TANTEADOR DE PADEL - Piezas imprimibles (parametrico)
//
//  Abrir en OpenSCAD, elegir PIEZA abajo, F5 para ver, F6 para renderizar,
//  y exportar STL con F7.
//
//  IMPORTANTE: antes de imprimir en serio, medir la tira real y ajustar
//  PASO_LED y TIRA_ANCHO. Ver 3d/README.md.
// ============================================================================

// ------------------------- QUE PIEZA GENERAR --------------------------------
// "rejilla"    -> marco de paredes de un digito (la pieza principal)
// "mascara"    -> plancha frontal con las ventanas de los segmentos
// "celda"      -> una sola celda de segmento suelta (para probar el calce)
// "icono"      -> mascara de un icono del centro (flecha o triangulo)
// "icono_celda"-> paredes del icono, con la divisoria entre punta y cuerpo
// "powerbank"  -> bandeja porta power bank para la parte de atras
// "previo"     -> vista de todo junto (solo para mirar)
PIEZA = "rejilla";

// Tamano del digito a generar: "big" (puntos), "small" (juegos), "tiny" (sets)
TAMANO = "big";

// Forma del icono cuando PIEZA="icono": "flecha_izq", "flecha_der", "triangulo"
ICONO = "flecha_der";

// Partir la pieza si no entra en la cama: "completo", "inferior", "superior"
MITAD = "completo";

// ------------------------- MEDIDAS DE LA TIRA -------------------------------
// MEDIR ESTO EN LA TIRA REAL (ver README): contar 10 LEDs, medir y dividir.
PASO_LED    = 16.67;   // mm entre centros de LED. 60 LED/m -> 16.67
TIRA_ANCHO  = 10.0;    // mm de ancho de la tira (WS2812B IP20 5050 suele ser 10)

// LEDs por segmento, tiene que coincidir con config.h del firmware
LEDS_BIG    = 6;       // LEDS_PER_SEG_BIG   en LAYOUT_FULL
LEDS_SMALL  = 4;       // LEDS_PER_SEG_SMALL
LEDS_TINY   = 3;       // LEDS_PER_SEG_TINY

// ------------------------- PARAMETROS DE IMPRESION --------------------------
JUEGO       = 2.0;     // holgura a cada lado de la tira dentro de la celda
PARED       = 1.6;     // espesor de pared (4 perimetros a 0.4 mm)
ALTO        = 22;      // altura de la pared = distancia LED -> difusor
                       //   60 LED/m: 15-20 mm alcanza
                       //   30 LED/m: usar 25-30 mm (LEDs mas separados)
MASCARA_ESP = 2.4;     // espesor de la plancha frontal
BASE        = false;   // true agrega una base de 0.8 mm que une las 7 celdas.
                       //   false (recomendado) ahorra ~30 g por digito: las
                       //   paredes se pegan sobre la placa de fondo de carton
                       //   o PVC, que de todos modos hace falta.
CAMA_X      = 220;     // tamano de la cama de la impresora
CAMA_Y      = 220;
PIN_D       = 3.2;     // diametro del agujero para el pasador de alineacion
                       //   (se usa un trozo de filamento de 1.75 no, de 3 mm,
                       //    o un clavo; ver README)

$fn = 32;

// ============================================================================
//  GEOMETRIA DERIVADA
// ============================================================================
CELDA_W = TIRA_ANCHO + 2 * JUEGO;        // ancho interno de la celda

function n_leds(t) = t == "big"   ? LEDS_BIG   :
                     t == "small" ? LEDS_SMALL : LEDS_TINY;

function largo_luz(t) = n_leds(t) * PASO_LED;   // largo util del segmento
function digito_w(t)  = largo_luz(t) + 2 * CELDA_W;
function digito_h(t)  = 2 * largo_luz(t) + 3 * CELDA_W;

// Los 7 segmentos como [x, y, ancho, alto] del HUECO (no de la pared).
// Origen abajo a la izquierda del digito.
//        AAAA
//       F    B
//        GGGG
//       E    C
//        DDDD
function segmentos(t) = let (L = largo_luz(t), W = CELDA_W) [
  [ W,     2*L+2*W, L, W ],   // 0 = A  (arriba)
  [ W+L,   L+2*W,   W, L ],   // 1 = B  (arriba derecha)
  [ W+L,   W,       W, L ],   // 2 = C  (abajo derecha)
  [ W,     0,       L, W ],   // 3 = D  (abajo)
  [ 0,     W,       W, L ],   // 4 = E  (abajo izquierda)
  [ 0,     L+2*W,   W, L ],   // 5 = F  (arriba izquierda)
  [ W,     L+W,     L, W ]    // 6 = G  (medio)
];

// ============================================================================
//  MODULOS
// ============================================================================

// Tubo rectangular hueco: las 4 paredes de un segmento.
module celda_paredes(x, y, w, h, alto) {
  translate([x - PARED, y - PARED, 0])
    difference() {
      cube([w + 2*PARED, h + 2*PARED, alto]);
      translate([PARED, PARED, -1]) cube([w, h, alto + 2]);
    }
}

// Marco completo de un digito: las 7 celdas unidas (comparten pared donde se tocan)
module rejilla(t) {
  segs = segmentos(t);
  union() {
    for (s = segs) celda_paredes(s[0], s[1], s[2], s[3], ALTO);
    if (BASE)
      translate([-PARED, -PARED, 0])
        cube([digito_w(t) + 2*PARED, digito_h(t) + 2*PARED, 0.8]);
  }
}

// Plancha frontal con las ventanas de los 7 segmentos recortadas.
// Las ventanas van con un chaflan en las puntas, como un 7 segmentos real.
module mascara(t) {
  segs = segmentos(t);
  difference() {
    translate([-PARED, -PARED, 0])
      cube([digito_w(t) + 2*PARED, digito_h(t) + 2*PARED, MASCARA_ESP]);
    for (s = segs)
      translate([s[0], s[1], -1]) cube([s[2], s[3], MASCARA_ESP + 2]);
  }
}

// Una celda suelta, para imprimir primero y verificar que la tira entra.
module celda_suelta(t) {
  celda_paredes(0, 0, largo_luz(t), CELDA_W, ALTO);
}

// --------------------------- iconos del centro ------------------------------
// ICONO_DIVIDIDO: el triangulo se parte en dos zonas con una pared en el medio.
//   CUERPO (la parte ancha) = aviso de punto   |   PUNTA = saque
// Hacen falta las dos zonas separadas por una PARED REAL: si no, los dos
// colores se mezclan por adentro y queda un color sucio.
ICONO_DIVIDIDO = true;
ICONO_CORTE    = 0.45;   // en que fraccion del largo va la pared divisoria

module tri(forma, lado) {
  if (forma == "flecha_izq")       polygon([[lado,0],[lado,lado],[0,lado/2]]);
  else if (forma == "flecha_der")  polygon([[0,0],[0,lado],[lado,lado/2]]);
  else                             polygon([[0,0],[lado,0],[lado/2,lado]]);
}

// Mascara del icono: una ventana, o dos si esta dividido.
module icono(forma) {
  lado = largo_luz("tiny") * (ICONO_DIVIDIDO ? 2 : 1);
  corte = lado * ICONO_CORTE;
  difference() {
    translate([-PARED, -PARED, 0])
      cube([lado + 2*PARED, lado + 2*PARED, MASCARA_ESP]);
    if (!ICONO_DIVIDIDO) {
      translate([0, 0, -1]) linear_extrude(MASCARA_ESP + 2) tri(forma, lado);
    } else {
      // zona CUERPO: del borde hasta la pared
      translate([0, 0, -1]) linear_extrude(MASCARA_ESP + 2)
        intersection() { tri(forma, lado); translate([0, -1]) square([corte, lado + 2]); }
      // zona PUNTA: de la pared hasta la punta
      translate([0, 0, -1]) linear_extrude(MASCARA_ESP + 2)
        intersection() { tri(forma, lado);
                         translate([corte + PARED, -1]) square([lado, lado + 2]); }
    }
  }
}

// Paredes del icono, con la divisoria entre las dos zonas.
module icono_celda(forma) {
  lado = largo_luz("tiny") * (ICONO_DIVIDIDO ? 2 : 1);
  corte = lado * ICONO_CORTE;
  linear_extrude(ALTO) difference() {
    offset(r = PARED) tri(forma, lado);
    tri(forma, lado);
  }
  if (ICONO_DIVIDIDO)   // la pared que separa punta de cuerpo
    linear_extrude(ALTO) intersection() {
      tri(forma, lado);
      translate([corte, -1]) square([PARED, lado + 2]);
    }
}

// Bandeja porta power bank, se atornilla o se ata con precintos atras del panel.
PB_L = 160;   // largo del power bank  - MEDIR EL TUYO
PB_W = 80;    // ancho
PB_H = 25;    // espesor
PB_PARED = 2.4;
module powerbank() {
  difference() {
    union() {
      // bandeja
      cube([PB_L + 2*PB_PARED, PB_W + 2*PB_PARED, PB_H/2 + PB_PARED]);
      // orejas para precintos / tornillos
      for (x = [15, PB_L + 2*PB_PARED - 15])
        translate([x - 7, -10, 0]) cube([14, 10, 4]);
    }
    // hueco del power bank
    translate([PB_PARED, PB_PARED, PB_PARED])
      cube([PB_L, PB_W, PB_H]);
    // ventana lateral para el cable USB
    translate([-1, PB_PARED + PB_W/2 - 12, PB_PARED + 3])
      cube([PB_PARED + 2, 24, PB_H]);
    // ranura para la cinta de velcro que lo sujeta
    for (x = [PB_L*0.3, PB_L*0.7])
      translate([x, -1, -1]) cube([25, PB_W + 2*PB_PARED + 2, PB_PARED + 2]);
    // agujeros de los precintos en las orejas
    for (x = [15, PB_L + 2*PB_PARED - 15])
      translate([x, -5, -1]) cylinder(d = 5, h = 10);
  }
}

// Corta la pieza por la mitad en Y y agrega agujeros de pasador para pegarla.
module partir(t, mitad) {
  h = digito_h(t) + 2*PARED;
  corte = h / 2;
  intersection() {
    children();
    if (mitad == "inferior") translate([-50, -PARED - 1, -1]) cube([400, corte + 1, 200]);
    else                     translate([-50, -PARED + corte, -1]) cube([400, h, 200]);
  }
}

module agujeros_pin(t) {
  w = digito_w(t);
  for (x = [w * 0.15, w * 0.85])
    translate([x, digito_h(t)/2 - PARED/2, ALTO/2]) rotate([90,0,0])
      cylinder(d = PIN_D, h = 14, center = true);
}

// ============================================================================
//  SALIDA
// ============================================================================
module pieza_elegida() {
  if (PIEZA == "rejilla")        rejilla(TAMANO);
  else if (PIEZA == "mascara")   mascara(TAMANO);
  else if (PIEZA == "celda")     celda_suelta(TAMANO);
  else if (PIEZA == "icono")     icono(ICONO);
  else if (PIEZA == "icono_celda") icono_celda(ICONO);
  else if (PIEZA == "powerbank") powerbank();
  else if (PIEZA == "previo") {
    rejilla("big");
    translate([digito_w("big") + 15, 0, 0]) rejilla("big");
    translate([0, -digito_h("small") - 15, 0]) rejilla("small");
    translate([digito_w("small") + 15, -digito_h("tiny") - 15, 0]) rejilla("tiny");
  }
}

if (PIEZA == "powerbank" || PIEZA == "celda" || PIEZA == "icono"
    || PIEZA == "icono_celda" || PIEZA == "previo")
  pieza_elegida();
else if (MITAD == "completo")
  pieza_elegida();
else
  difference() {
    partir(TAMANO, MITAD) pieza_elegida();
    agujeros_pin(TAMANO);
  }

// Aviso por consola con las medidas resultantes
echo(str("=== ", TAMANO, " : luz ", largo_luz(TAMANO),
         " mm | digito ", digito_w(TAMANO), " x ", digito_h(TAMANO), " mm ==="));
echo(str("=== cama ", CAMA_X, "x", CAMA_Y,
         " -> ", (digito_h(TAMANO) + 2*PARED > CAMA_Y) ? "HAY QUE PARTIR (MITAD)" : "entra completo", " ==="));
