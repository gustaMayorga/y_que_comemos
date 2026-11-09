// ===========================================================================
//  carcasa_hcsr04.scad
//  Carcasa justa para el sensor ultrasonico HC-SR04
//  Proyecto: cartel marcador de padel  (padel-scoreboard)
//
//  POR QUE ESTA HECHA ASI
//  ----------------------
//  El ultrasonido NO atraviesa el plastico. Las capsulas tienen que dar al
//  aire libre y, sobre todo, NO pueden quedar apretadas contra la misma
//  pared rigida: si lo estan, el pulso de la que emite viaja por el material
//  hasta la que recibe y el sensor lee una distancia chica constante.
//  Por eso:
//    - los agujeros salen 0.3 mm mas grandes POR LADO -> no tocan el plastico
//    - se sellan con SILICONA NEUTRA, que ademas desacopla
//    - la cara delantera es de solo 2 mm: las capsulas quedan al aire,
//      no metidas en un tubo
//
//  EJES:  Z = profundidad.  Z=0 es la cara delantera de la carcasa y crece
//         hacia atras.  Las capsulas asoman hacia Z negativo.
//
//  IMPRIMIR (sin soportes, ninguna de las dos los necesita):
//    cuerpo -> cara DELANTERA contra la cama
//    tapa   -> cara EXTERIOR  contra la cama
//    0.2 mm de capa, 3 perimetros, 25% relleno. PETG si va a tomar sol.
// ===========================================================================

/* [Que generar] */
// cuerpo | tapa | ambas | plantilla | corte
PIEZA = "ambas";

/* [Montaje en el panel] */
// Cuanto asoman las capsulas por delante de la carcasa.
// Poner = ESPESOR DEL PANEL para que queden al ras de la cara exterior.
// Poner 0 si la carcasa va suelta, sin panel.
SALIENTE = 3.0;
// brida | ninguno
MONTAJE  = "brida";
BRIDA_L  = 11.0;   // cuanto sobresale cada oreja mas alla del cuerpo
BRIDA_W  = 13.0;   // ancho de la oreja
BRIDA_T  = 3.0;    // espesor de la oreja
BRIDA_D  = 3.4;    // agujero de la oreja: 3.4 = M3 pasante

/* [Medidas del HC-SR04] */
PCB_X   = 45.0;    // largo de la plaquita
PCB_Y   = 20.0;    // alto
PCB_T   = 1.2;     // espesor
CAP_D   = 16.0;    // diametro de cada capsula
CAP_SEP = 26.0;    // separacion entre CENTROS de capsulas
CAP_H   = 12.0;    // altura de la capsula sobre el PCB

/* [Caja] */
PARED        = 2.0;    // espesor de la cara delantera y de las paredes
PROF_TRASERA = 12.0;   // camara detras del PCB
                       //   soldadura directa de los cables : 12
                       //   con ficha DuPont enchufada      : 22
APOYO        = 1.2;    // pestana sobre la que apoya el PCB
TAPA_T       = 2.0;    // espesor de la tapa
LABIO        = 3.5;    // cuanto entra el tapon de la tapa
RADIO        = 2.0;    // radio de las esquinas exteriores

/* [Ajustes de impresion] */
HOLGURA      = 0.40;   // PCB <-> caja, POR LADO
HOLGURA_CAP  = 0.60;   // capsulas, DIAMETRAL: 0.3 por lado para la silicona
HOLGURA_TAPA = 0.25;   // tapon de la tapa, por lado
APRIETE      = 0.60;   // cuanto sobresale el reborde de encastre de la tapa
CABLE_D      = 5.0;    // paso de cable
DRENAJE      = true;   // agujerito de desague abajo

$fn = 72;

// --------------------------------------------------------------------------
//  Derivados
// --------------------------------------------------------------------------
int_x = PCB_X + 2*HOLGURA;          // hueco interior
int_y = PCB_Y + 2*HOLGURA;
ext_x = int_x + 2*PARED;            // exterior
ext_y = int_y + 2*PARED;

z_pcb  = CAP_H - SALIENTE;          // cara DELANTERA del PCB
z_back = z_pcb + PCB_T;             // cara TRASERA del PCB
z_rim  = z_back + PROF_TRASERA;     // borde trasero del cuerpo
z_cable = z_back + CABLE_D/2 + 1.5;

// Abertura delantera: tiene que dejar pasar las dos capsulas CON AIRE,
// y a la vez dejar una pestana donde apoye el PCB.
ab_x = max(PCB_X - 2*APOYO, CAP_SEP + CAP_D + 1.0);
ab_y = max(PCB_Y - 2*APOYO, CAP_D + 1.0);

z_cierre = z_rim - LABIO/2;         // altura del reborde de encastre
tap_x = int_x - 2*HOLGURA_TAPA;     // tapon de la tapa
tap_y = int_y - 2*HOLGURA_TAPA;

assert(z_pcb > PARED + 0.5,
  "SALIENTE demasiado grande: las capsulas no entran en la pared delantera");
assert(ab_x < int_x && ab_y < int_y,
  "No queda pestana de apoyo para el PCB: bajar APOYO");
assert(CAP_SEP/2 + (CAP_D + HOLGURA_CAP)/2 < ab_x/2,
  "Las capsulas chocan con la pestana en X");

echo(str("Exterior : ", ext_x, " x ", ext_y, " x ", z_rim + TAPA_T, " mm"));
echo(str("Con brida: ", ext_x + 2*BRIDA_L, " mm de largo"));
echo(str("Agujeros : 2 x diam ", CAP_D + HOLGURA_CAP, "  a ", CAP_SEP, " mm entre centros"));
echo(str("Pestana  : X ", (PCB_X-ab_x)/2, " mm    Y ", (PCB_Y-ab_y)/2, " mm"));

// --------------------------------------------------------------------------
//  Utilidades
// --------------------------------------------------------------------------
module caja_r(x, y, z, r) {           // prisma de esquinas redondeadas
    rr = min(r, x/2 - 0.01, y/2 - 0.01);
    hull() for (i = [-1,1], j = [-1,1])
        translate([i*(x/2 - rr), j*(y/2 - rr), 0]) cylinder(h = z, r = rr);
}

module lagrima(d, largo) {            // agujero horizontal autoportante (apunta a +Z)
    rotate([90,0,0]) union() {
        cylinder(d = d, h = largo, center = true);
        linear_extrude(largo, center = true)
            polygon([[-d/2, 0], [d/2, 0], [0, d*0.72]]);
    }
}

module reborde(largo, r) {            // media cana a lo largo de X
    rotate([0,90,0]) cylinder(r = r, h = largo, center = true);
}

// --------------------------------------------------------------------------
//  CUERPO
// --------------------------------------------------------------------------
module orejas(espesor) {
    for (s = [-1,1])
        translate([s*(ext_x/2 + BRIDA_L/2 - 1.5), 0, 0])
            caja_r(BRIDA_L + 3, BRIDA_W, espesor, 2);
}

module orejas_agujeros(espesor, d) {
    for (s = [-1,1])
        translate([s*(ext_x/2 + BRIDA_L/2), 0, -1])
            cylinder(d = d, h = espesor + 2);
}

module cuerpo() {
    difference() {
        union() {
            caja_r(ext_x, ext_y, z_rim, RADIO);
            if (MONTAJE == "brida") orejas(BRIDA_T);
        }

        // 1) los dos agujeros de las capsulas (solo atraviesan la cara)
        for (s = [-1,1])
            translate([s*CAP_SEP/2, 0, -1])
                cylinder(h = PARED + 2, d = CAP_D + HOLGURA_CAP);

        // 2) hueco delantero: cuerpo de las capsulas y el cristal
        translate([0, 0, PARED])
            caja_r(ab_x, ab_y, z_pcb - PARED + 0.01, 1.0);

        // 3) alojamiento del PCB + camara trasera (abierta atras)
        translate([0, 0, z_pcb])
            caja_r(int_x, int_y, PROF_TRASERA + PCB_T + 1, 1.0);

        // 4) canal donde encastra el reborde de la tapa
        for (s = [-1,1])
            translate([0, s*int_y/2, z_cierre]) reborde(int_x - 8, APRIETE + 0.25);

        // 5) salida de cable (lagrima: se imprime sin soporte)
        translate([0, -ext_y/2 - 0.5, z_cable]) lagrima(CABLE_D, PARED + 6);

        // 6) desague, por si el cartel vive a la intemperie
        if (DRENAJE)
            translate([-int_x/2 + 6, -ext_y/2 - 1, z_back + 3])
                rotate([-90,0,0]) cylinder(d = 2.2, h = PARED + 2);

        // 7) rebaje para hacer palanca y abrir la tapa (no atraviesa)
        translate([0, ext_y/2 + 0.4, z_rim - 1.0])
            cube([12, 2.0, 3.0], center = true);

        if (MONTAJE == "brida") orejas_agujeros(BRIDA_T, BRIDA_D);
    }
}

// --------------------------------------------------------------------------
//  TAPA  (definida en posicion de montaje)
// --------------------------------------------------------------------------
module tapa() {
    pres_x = PCB_X/2 - 3.2;
    pres_y = PCB_Y/2 - 3.2;
    union() {
        // plato
        translate([0, 0, z_rim]) caja_r(ext_x, ext_y, TAPA_T, RADIO);

        // tapon: anillo con punta conica para que entre solo
        translate([0, 0, z_rim - LABIO])
            difference() {
                hull() {
                    caja_r(tap_x - 1.2, tap_y - 1.2, 0.01, 1.0);
                    translate([0, 0, 1.2])
                        caja_r(tap_x, tap_y, LABIO - 1.2, 1.0);
                }
                translate([0, 0, -1])
                    caja_r(tap_x - 3.2, tap_y - 3.2, LABIO + 2, 0.8);
            }

        // rebordes que encastran en el canal del cuerpo
        for (s = [-1,1])
            translate([0, s*tap_y/2, z_cierre]) reborde(int_x - 12, APRIETE);

        // 4 postes que aprietan el PCB contra la pestana delantera
        for (i = [-1,1], j = [-1,1])
            translate([i*pres_x, j*pres_y, z_back])
                cylinder(d1 = 4.0, d2 = 5.5, h = PROF_TRASERA);
    }
}

module tapa_imprimir() {
    rotate([180,0,0]) translate([0, 0, -(z_rim + TAPA_T)]) tapa();
}

// --------------------------------------------------------------------------
//  PLANTILLA de marcado para taladrar el panel
// --------------------------------------------------------------------------
module plantilla() {
    t = 2.0;
    difference() {
        union() {
            caja_r(ext_x + 6, ext_y + 6, t, 3);
            if (MONTAJE == "brida") orejas(t);
        }
        for (s = [-1,1])
            translate([s*CAP_SEP/2, 0, -1])
                cylinder(d = CAP_D + 0.5, h = t + 2);
        if (MONTAJE == "brida") orejas_agujeros(t, 2.5);
        // cruz de centrado grabada
        translate([0, 0, t - 0.6]) cube([ext_x + 10, 0.8, 1.4], center = true);
        translate([0, 0, t - 0.6]) cube([0.8, ext_y + 10, 1.4], center = true);
    }
}

// --------------------------------------------------------------------------
//  Salida
// --------------------------------------------------------------------------
if (PIEZA == "cuerpo")         cuerpo();
else if (PIEZA == "tapa")      tapa_imprimir();
else if (PIEZA == "plantilla") plantilla();
else if (PIEZA == "corte")
    difference() {
        union() { cuerpo(); color("orange") tapa(); }
        translate([-200, 0, -100]) cube([400, 200, 400]);
    }
else {                                      // "ambas": las dos, listas para imprimir
    cuerpo();
    translate([0, ext_y + 12, 0]) tapa_imprimir();
}
