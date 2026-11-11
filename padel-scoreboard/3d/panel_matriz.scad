// ============================================================================
//  PANEL 23 x 12 - piezas imprimibles
//
//    rejilla     tapa todas las tiras y deja a la vista SOLO los LED.
//                Sale en 4 modulos que se encastran a media madera.
//    zocalo      perfil en C que abraza el canto de la placa y tapa los cables
//                verde / rojo / negro de los costados.
//    pasacable   omega recta: los cables por adentro, las aletas se pegan.
//    caja        caja con tapa para la electronica (placa perforada + Nano).
//    gancho      soporte superior: cuelga de un alambre de la reja.
//    traba       soporte inferior: separa de la reja y se ata con precinto.
//
//  LAS MEDIDAS SALEN DE FOTOS CON CINTA: son aproximadas. Las cuatro que
//  importan estan marcadas con  <<< MEDIR.  Antes de imprimir los 4 modulos,
//  imprimi  PIEZA = "prueba"  (una esquina de 4 x 3 LED, 10 minutos) y apoyala
//  sobre el panel.
// ============================================================================

PIEZA = "prueba";
// "prueba" | "rejilla_1" | "rejilla_2" | "rejilla_3" | "rejilla_4"
// "zocalo" | "pasacable" | "caja" | "caja_tapa" | "gancho" | "traba" | "vista"
//
//   Vista de frente:      rejilla_3 | rejilla_4      (fila de arriba)
//                         rejilla_1 | rejilla_2      (fila de abajo)

/* ---------------- el panel ---------------- */
COLS   = 23;
FILAS  = 12;
PASO_X = 16.67;  // <<< MEDIR: entre centros de LED en la tira (60 LED/m = 16,67)
PASO_Y = 22.0;   // <<< MEDIR: entre centros de dos tiras vecinas
TIRA_ANCHO = 10.5;   // ancho de la tira, con un poco de juego
TIRA_ALTO  = 2.2;    // lo que levanta la tira con sus componentes (sin el LED)

/* ---------------- la rejilla ---------------- */
VENTANA   = 7.0;   // lado del agujero de cada LED (el LED mide 5 x 5)
REJ_ALTO  = 4.0;   // espesor total. Mas alto = mas contraste al sol y menos
                   // angulo de vision. 4 mm deja el LED 1,5 mm hundido.
BISEL     = 1.0;   // la ventana se abre hacia el frente (mejora el angulo). 0 = recta
BORDE     = 4.0;   // cuanto sobra por fuera del ultimo LED
SOLAPA    = 4.0;   // ancho del encastre entre modulos
JUEGO     = 0.25;  // holgura del encastre
COLS_IZQ  = 12;    // columnas de los modulos de la izquierda (los otros: el resto)
FILAS_ABA = 6;     // filas de los modulos de abajo
TORNILLO  = 3.2;   // agujero para fijar a la placa (0 = sin agujeros, se pega)

/* ---------------- el zocalo ---------------- */
PLACA_ESP  = 5.0;  // <<< MEDIR: espesor de la placa negra de fondo
ZOC_FRENTE = 9.0;  // <<< MEDIR: cuanto tapa por el frente, desde el canto.
                   //     Tiene que cubrir las soldaduras SIN llegar al primer LED
ZOC_DETRAS = 15;   // cuanto tapa por detras
ZOC_CABLE  = 5.0;  // lugar libre para los cables (frente, canto y detras)
ZOC_PARED  = 1.6;
ZOC_LARGO  = 132;  // largo de cada tramo: 2 tramos por lateral
ZOC_PIE_0  = 11;   // <<< MEDIR: del extremo del tramo al primer hueco ENTRE dos
                   //     tiras. Ahi van los pies que aprietan la placa (cada
                   //     PASO_Y), para no pisar tiras ni cables.

/* ---------------- el pasacable ---------------- */
PC_ANCHO = 9;      // hueco interior (ancho)
PC_ALTO  = 6;      // hueco interior (alto)
PC_ALETA = 9;      // ancho de cada aleta de pegado
PC_LARGO = 25;
PC_PARED = 1.6;

/* ---------------- la caja de la electronica ---------------- */
PCB_L = 70;        // <<< MEDIR: largo de la placa perforada
PCB_A = 50;        //            ancho
CAJA_ALTO  = 30;   // alto libre interior (el capacitor de 2200 uF va acostado)
CAJA_PARED = 2.0;
CAJA_JUEGO = 1.5;  // aire alrededor de la placa
CAJA_APOYO = 4;    // altura de los apoyos de la placa (lugar para soldaduras)
SALIDA_ANCHO = 14; // ranuras para cables en los dos extremos cortos
SALIDA_ALTO  = 9;
USB_ANCHO  = 13;   // ranura para el cable USB en un lateral largo (0 = sin)
OREJA      = 12;   // aletas para pegar o atornillar la caja al panel

/* ---------------- el soporte para la reja ---------------- */
ALAMBRE     = 5.0; // diametro del alambre de la reja, con su recubrimiento
SOP_ANCHO   = 30;
SOP_ESP     = 4.5; // espesor del plastico
SOP_ALTO    = 60;  // alto de la placa que va contra el panel
GANCHO_BAJA = 18;  // cuanto baja el pico por detras del alambre
PRECINTO    = 5.0; // ancho del precinto que pasa por la traba

$fn = 40;

/* ============================== REJILLA ==============================
   Origen en el centro del LED de abajo a la izquierda. Las tiras corren en X.
   z = 0 es la cara que apoya en la placa.                                  */
function rx0(c0)     = c0 == 0     ? -PASO_X/2 - BORDE : (c0 - 0.5)*PASO_X;
function rx1(c1)     = c1 == COLS  ? (COLS - 0.5)*PASO_X + BORDE : (c1 - 0.5)*PASO_X;
function ry0(f0)     = f0 == 0     ? -PASO_Y/2 - BORDE : (f0 - 0.5)*PASO_Y;
function ry1(f1)     = f1 == FILAS ? (FILAS - 0.5)*PASO_Y + BORDE : (f1 - 0.5)*PASO_Y;

// modulo que cubre las columnas c0..c1-1 y las filas f0..f1-1
module rejilla(c0, c1, f0, f1){
  xa = rx0(c0); xb = rx1(c1); ya = ry0(f0); yb = ry1(f1);
  h2 = REJ_ALTO/2;
  difference(){
    union(){
      translate([xa, ya, 0]) cube([xb - xa, yb - ya, REJ_ALTO]);
      // lengueta inferior hacia la DERECHA y hacia ARRIBA, si hay vecino
      if(c1 < COLS)  translate([xb - 0.01, ya, 0]) cube([SOLAPA, yb - ya, h2]);
      if(f1 < FILAS) translate([xa, yb - 0.01, 0]) cube([xb - xa, SOLAPA, h2]);
    }
    // rebaje inferior a la IZQUIERDA y ABAJO, donde entra la lengueta del vecino
    if(c0 > 0) translate([xa - 0.01, ya - SOLAPA - 1, -0.01])
      cube([SOLAPA + JUEGO, yb - ya + 2*SOLAPA + 2, h2 + JUEGO]);
    if(f0 > 0) translate([xa - SOLAPA - 1, ya - 0.01, -0.01])
      cube([xb - xa + 2*SOLAPA + 2, SOLAPA + JUEGO, h2 + JUEGO]);
    // canal de cada tira, por debajo
    for(f = [f0 : f1 - 1]) translate([xa - SOLAPA - 2, f*PASO_Y - TIRA_ANCHO/2, -0.01])
      cube([xb - xa + 2*SOLAPA + 4, TIRA_ANCHO, TIRA_ALTO]);
    // ventana de cada LED
    for(c = [c0 : c1 - 1], f = [f0 : f1 - 1]) translate([c*PASO_X, f*PASO_Y, 0]) ventana();
    // agujeros de fijacion: en el macizo entre tiras y entre columnas
    if(TORNILLO > 0)
      for(c = [c0 + 1, c1 - 2], f = [f0 + 1, f1 - 2])
        translate([(c + 0.5)*PASO_X, (f + 0.5)*PASO_Y, -1]){
          cylinder(d = TORNILLO, h = REJ_ALTO + 2);
          translate([0, 0, 1 + REJ_ALTO - 1.6]) cylinder(d1 = TORNILLO, d2 = TORNILLO + 3.2, h = 1.61);
        }
  }
}
module ventana(){
  translate([0, 0, -1]) linear_extrude(REJ_ALTO + 2) square(VENTANA, center = true);
  if(BISEL > 0) hull(){
    translate([0, 0, REJ_ALTO - BISEL]) linear_extrude(0.01) square(VENTANA, center = true);
    translate([0, 0, REJ_ALTO]) linear_extrude(0.01) square(VENTANA + 2*BISEL, center = true);
  }
}
module rejilla_n(n){
  if(n == 1) rejilla(0, COLS_IZQ, 0, FILAS_ABA);
  if(n == 2) rejilla(COLS_IZQ, COLS, 0, FILAS_ABA);
  if(n == 3) rejilla(0, COLS_IZQ, FILAS_ABA, FILAS);
  if(n == 4) rejilla(COLS_IZQ, COLS, FILAS_ABA, FILAS);
}

/* ============================== ZOCALO ==============================
   Perfil en C. u = hacia adentro del panel desde el canto, v = hacia el frente.
   Se imprime parado sobre el perfil (sin soporte) o acostado sobre el lomo. */
module zocalo2d(){
  p = ZOC_PARED; c = ZOC_CABLE; e = PLACA_ESP;
  difference(){
    union(){
      translate([-c - p, -e/2]) square([c + p + ZOC_FRENTE, e/2 + c + p]);        // frente
      translate([-c - p, -e - c - p]) square([c + p + ZOC_DETRAS, e/2 + c + p]);  // detras
    }
    translate([-c, 0])      square([c + ZOC_FRENTE + 1, c]);          // cables del frente
    translate([-c, -e - c]) square([c + ZOC_DETRAS + 1, c]);          // cables de atras
    translate([-c, -e - c]) square([c, e + 2*c]);                     // cables del canto
    translate([-0.2, -e - 0.2]) square([ZOC_DETRAS + ZOC_FRENTE, e + 0.4]);  // la placa
  }
}
// pie que baja desde la pared hasta tocar la placa, con rampa para imprimir
module zocalo_pie(frente){
  c = ZOC_CABLE; e = PLACA_ESP; L = frente ? ZOC_FRENTE : ZOC_DETRAS;
  hull(){
    translate([L - 3, frente ? 0.2 : -e - c, 0]) cube([3, c - 0.2 + 0.01, 5]);
    translate([L - 3, frente ? c - 0.01 : -e - c - 0.01, -c]) cube([3, 0.02, 0.1]);
  }
}
module zocalo(){
  linear_extrude(ZOC_LARGO) zocalo2d();
  for(z = [ZOC_PIE_0 : PASO_Y : ZOC_LARGO - 6]) translate([0, 0, z - 2.5]){
    zocalo_pie(true);
    zocalo_pie(false);
  }
}

/* ============================== PASACABLE ============================== */
module pasacable2d(){
  p = PC_PARED;
  difference(){
    union(){
      translate([-PC_ANCHO/2 - p, 0]) square([PC_ANCHO + 2*p, PC_ALTO + p]);
      translate([-PC_ANCHO/2 - p - PC_ALETA, 0]) square([PC_ANCHO + 2*p + 2*PC_ALETA, p]);
    }
    translate([-PC_ANCHO/2, -0.01]) square([PC_ANCHO, PC_ALTO]);
  }
}
// se imprime parado sobre el perfil: sin puentes ni soportes
module pasacable(){ linear_extrude(PC_LARGO) pasacable2d(); }

/* ============================== CAJA ==============================
   Origen en la esquina interior. La placa apoya en 4 escuadras; la tapa la
   sujeta por arriba con 4 tornillos M3 x 10 en los postes de las esquinas. */
ci_l = PCB_L + 2*CAJA_JUEGO;                // interior
ci_a = PCB_A + 2*CAJA_JUEGO;
ce_l = ci_l + 2*CAJA_PARED;                 // exterior
ce_a = ci_a + 2*CAJA_PARED;
c_h  = CAJA_PARED + CAJA_ALTO;              // alto de la base (sin tapa)
POSTE = 8;                                  // diametro de los postes de tornillo
function postes() = [ for(x = [-CAJA_PARED - 1, ci_l + CAJA_PARED + 1],
                          y = [-CAJA_PARED - 1, ci_a + CAJA_PARED + 1]) [x, y] ];

module caja_contorno2d(){
  translate([-CAJA_PARED, -CAJA_PARED]) square([ce_l, ce_a]);
  for(p = postes()) translate(p) circle(d = POSTE);
}
module caja(){
  difference(){
    union(){
      linear_extrude(c_h) caja_contorno2d();
      // aletas para fijar la caja
      if(OREJA > 0) for(s = [0, 1]) translate([ci_l/2 - 12, s ? ci_a + CAJA_PARED : -CAJA_PARED - OREJA, 0])
        difference(){
          cube([24, OREJA, 3]);
          translate([12, OREJA/2 + (s ? 2 : -2), -1]) cylinder(d = 4, h = 5);
        }
    }
    // el hueco
    translate([0, 0, CAJA_PARED]) cube([ci_l, ci_a, c_h]);
    // tornillos de la tapa
    for(p = postes()) translate([p.x, p.y, c_h - 14]) cylinder(d = 2.6, h = 15);
    // salidas de cables, abiertas hacia la tapa
    for(x = [-CAJA_PARED - POSTE, ci_l - 1])
      translate([x, ci_a/2 - SALIDA_ANCHO/2, c_h - SALIDA_ALTO]) cube([CAJA_PARED + POSTE + 1, SALIDA_ANCHO, SALIDA_ALTO + 1]);
    if(USB_ANCHO > 0)
      translate([ci_l/2 - USB_ANCHO/2, -CAJA_PARED - POSTE, c_h - SALIDA_ALTO]) cube([USB_ANCHO, CAJA_PARED + POSTE + 1, SALIDA_ALTO + 1]);
    // ventilacion en el piso
    for(i = [0 : 4]) translate([ci_l/2 - 20 + i*10 - 1.5, ci_a/2 - 12, -1]) cube([3, 24, CAJA_PARED + 2]);
  }
  // escuadras donde apoya la placa
  for(x = [0, ci_l - 6], y = [0, ci_a - 6])
    translate([x, y, CAJA_PARED - 0.01]) cube([6, 6, CAJA_APOYO]);
}
module caja_tapa(){
  difference(){
    union(){
      linear_extrude(CAJA_PARED) caja_contorno2d();
      // labio que centra la tapa
      translate([0, 0, CAJA_PARED - 0.01]) difference(){
        translate([0.3, 0.3, 0]) cube([ci_l - 0.6, ci_a - 0.6, 2]);
        translate([1.8, 1.8, -1]) cube([ci_l - 3.6, ci_a - 3.6, 4]);
      }
    }
    for(p = postes()) translate([p.x, p.y, -1]){
      cylinder(d = 3.4, h = CAJA_PARED + 4);
      cylinder(d = 6.2, h = 1 + 1.2);                 // cabeza embutida
    }
  }
}

/* ============================== SOPORTE ==============================
   GANCHO: dos arriba, atornillados o pegados a la espalda del panel. El panel
   se cuelga de un alambre horizontal de la reja.
   TRABA: dos abajo. Separan el panel de la reja lo mismo que el gancho y se
   atan a un alambre con un precinto: con eso el gancho ya no puede salirse.
   Las dos se imprimen ACOSTADAS (el perfil sobre la cama): asi las capas
   corren a lo largo del gancho y no se parte por el tiron.               */
sep = ALAMBRE + 1.5;                         // boca del gancho
module gancho2d(){
  e = SOP_ESP;
  // placa contra el panel (x de 0 a e), brazo superior, y el pico que baja
  square([e, SOP_ALTO]);
  translate([0, SOP_ALTO - e]) square([2*e + sep, e]);
  translate([e + sep, SOP_ALTO - e - GANCHO_BAJA]) square([e, GANCHO_BAJA + e]);
  // refuerzo en el angulo
  translate([e, SOP_ALTO - e]) polygon([[0, 0], [0, -sep*0.6], [sep*0.6, 0]]);
}
module gancho(){
  difference(){
    linear_extrude(SOP_ANCHO) gancho2d();
    // dos tornillos hacia el panel, por debajo del pico
    for(y = [10, SOP_ALTO - GANCHO_BAJA - SOP_ESP - 12])
      translate([-1, y, SOP_ANCHO/2]) rotate([0, 90, 0]){
        cylinder(d = 4.2, h = SOP_ESP + 2);
        translate([0, 0, 1 + SOP_ESP - 2]) cylinder(d1 = 4.2, d2 = 8.4, h = 2.01);
      }
  }
}
module traba2d(){
  e = SOP_ESP;
  difference(){
    union(){
      square([e, 40]);                                   // placa contra el panel
      translate([0, 12]) square([2*e + sep, 16]);        // taco separador
    }
    // cuna donde apoya el alambre
    translate([2*e + sep, 20]) circle(d = ALAMBRE + 1);
  }
}
module traba(){
  difference(){
    linear_extrude(SOP_ANCHO) traba2d();
    // tunel vertical para el precinto: abraza el alambre que apoya en la cuna
    translate([SOP_ESP + 1.5, 11, SOP_ANCHO/2 - (PRECINTO + 1.5)/2]) cube([2.8, 18, PRECINTO + 1.5]);
    for(y = [6, 34]) translate([-1, y, SOP_ANCHO/2]) rotate([0, 90, 0]){
      cylinder(d = 4.2, h = SOP_ESP + 2);
      translate([0, 0, 1 + SOP_ESP - 2]) cylinder(d1 = 4.2, d2 = 8.4, h = 2.01);
    }
  }
}

/* ============================== SALIDA ============================== */
if(PIEZA == "prueba")    rejilla(0, 4, 0, 3);
if(PIEZA == "rejilla_1") rejilla_n(1);
if(PIEZA == "rejilla_2") rejilla_n(2);
if(PIEZA == "rejilla_3") rejilla_n(3);
if(PIEZA == "rejilla_4") rejilla_n(4);
if(PIEZA == "zocalo")    zocalo();
if(PIEZA == "pasacable") for(i = [0 : 2]) translate([i*(PC_ANCHO + 2*PC_ALETA + 2*PC_PARED + 4), 0, 0]) pasacable();
if(PIEZA == "caja")      caja();
if(PIEZA == "caja_tapa") caja_tapa();
if(PIEZA == "gancho")    gancho();
if(PIEZA == "traba")     traba();
if(PIEZA == "vista"){
  color("dimgray") for(n = [1 : 4]) rejilla_n(n);
}

echo(str("Panel de LEDs: ", COLS*PASO_X, " x ", FILAS*PASO_Y, " mm"));
echo(str("Modulo mas grande: ", COLS_IZQ*PASO_X + BORDE + SOLAPA, " x ",
         max(FILAS_ABA, FILAS - FILAS_ABA)*PASO_Y + BORDE + SOLAPA, " mm"));

// NOTAS DE IMPRESION
//  - REJILLA: PETG o ASA NEGRO MATE (el PLA se deforma al sol). Cara del frente
//    hacia ARRIBA, canales contra la cama. Capa 0,2 mm, sin soporte.
//    Cada modulo mide ~208 x 140 mm: entra en una cama de 220 x 220.
//    Se encastran: 1 y 3 tienen lengueta a la derecha, 1 y 2 hacia arriba.
//    Pegalas a la placa con cinta doble faz o silicona, o usa los 4 agujeros.
//  - ZOCALO: 2 tramos por lateral (4 en total), parado sobre el perfil. Entra
//    a presion por los pies; un punto de silicona lo deja fijo.
//  - CAJA: la placa apoya en las 4 escuadras; fijala con un punto de silicona.
//  - GANCHO y TRABA: PETG, 4 perimetros, 50 % de relleno, acostados.
