// ============================================================================
//  Caja de MUÑECA para el control remoto KL-238A  (3 botones: A, B, borrar)
//
//  - La placa va A LO LARGO del antebrazo, con el LED y los botones hacia la
//    MANO: se aprietan con la otra mano sin torcer la muñeca.
//  - La correa pasa por dos ALAS laterales (una de cada lado), como un reloj.
//  - El fondo y las alas son CURVOS, siguen la muñeca.
//  - Los 3 botones son piezas SUELTAS, cuadradas y gruesas: no son lenguetas
//    flexibles. Quedan atrapados entre la tapa y los pulsadores.
//  - Cierra con 3 tornillos M2 x 8 autorroscantes.
//
//  LAS MEDIDAS SALEN DE FOTOS CON CINTA METRICA: son aproximadas (+-1 mm).
//  Antes de imprimir la caja entera:
//     1) poné  PIEZA = "plantilla"  e imprimi esa lamina (5 minutos)
//     2) apoyala sobre la placa: los tres agujeros tienen que caer sobre los
//        tres pulsadores y el contorno tiene que coincidir
//     3) corregi los numeros de abajo con un calibre y recien ahi imprimi
//
//  Origen de coordenadas: esquina inferior izquierda del CUERPO de la placa
//  (la parte ancha), mirandola con los botones hacia arriba y el LED a la
//  derecha. X hacia el LED (= hacia la mano), Y hacia el brazo de la pila.
// ============================================================================

PIEZA = "todo";   // "todo" | "base" | "tapa" | "botones" | "plantilla" | "armado"

/* ---------------- la placa KL-238A (medir y corregir) ---------------- */
pcb_largo   = 45;    // cuerpo, desde el borde recto hasta la punta redonda
pcb_ancho   = 24;
pcb_esp     = 1.2;   // espesor de la placa
brazo_largo = 7;     // lo que sobresale el brazo de la pila hacia la izquierda
resorte     = 2;     // lugar extra para el resorte, mas alla del brazo
led_extra   = 3;     // lo que sobresale el LED por la punta

// centro de cada pulsador  [x, y]
btn_A      = [25.0,  8.0];   // el de abajo
btn_B      = [30.0, 20.5];   // el de arriba, al lado de la pila
btn_BORRAR = [41.5, 15.5];   // el de la punta, al lado del LED
led_pos    = [46.5, 14.5];

alto_pulsador = 3.0; // del tope de la placa al tope del pulsador
// Pila 27A (8 mm de diametro). Si la tuya es A23 (10,3 mm) poné 5.6 en los dos.
pila_abajo  = 4.5;   // lugar libre debajo de la placa
pila_arriba = 4.5;   // lugar libre encima de la placa

// La media luna del borde inferior de la placa: un perno la traba para que la
// placa no se corra. Si no coincide, poné usar_muesca = false.
usar_muesca = true;
muesca_x    = 36.5;
muesca_r    = 2.2;

/* ---------------- la caja ---------------- */
holgura   = 0.4;     // juego entre la placa y la pared
pared     = 2.0;
piso      = 1.6;     // espesor minimo del fondo (en el centro)
tapa_esp  = 2.0;
apoyo     = 1.0;     // ancho del escalon donde apoya la placa
radio_ext = 3.5;     // redondeo de las esquinas de afuera

/* ---------------- muñeca y correa ---------------- */
radio_muneca = 50;   // curvatura del fondo. Mas chico = mas curvo. 0 = plano
correa_ancho = 20;   // ancho de la correa (velcro o tipo NATO)
correa_esp   = 3.0;  // ancho de la ranura
correa_barra = 4.0;  // material por fuera de la ranura (lo que aguanta el tiron)
ala_esp      = 4.0;  // espesor del ala

tornillo_d      = 1.7;  // agujero para M2 autorroscante en la base
tornillo_paso   = 2.3;  // agujero pasante en la tapa
tornillo_cabeza = 4.2;

/* ---------------- los botones ---------------- */
boton_lado     = 8.0;  // lado de la tecla cuadrada
boton_radio    = 1.5;  // redondeo de sus esquinas
boton_sobresale= 1.5;  // cuanto asoma por encima de la tapa
boton_ala      = 1.2;  // pestaña que lo retiene por debajo de la tapa
boton_juego    = 0.3;  // juego: recorrido libre y holgura en el agujero
grabado        = 0.5;  // profundidad de las letras

$fn = 48;

/* ================= derivados ================= */
x0 = -(brazo_largo + resorte) - holgura;          // pared interior izquierda
x1 = pcb_largo + led_extra + holgura;             // pared interior derecha
y0 = -holgura;
y1 = pcb_ancho + holgura;
r_punta = (y1 - y0) / 2;

ex0 = x0 - pared;  ex1 = x1 + pared;              // limites del cuerpo
ey0 = y0 - pared;  ey1 = y1 + pared;
cx = (ex0 + ex1) / 2;
cy = (ey0 + ey1) / 2;

oreja    = 0.8 + correa_esp + correa_barra;       // cuanto sale cada ala
ala_larg = correa_ancho + 1 + 2*5;
medio    = (ey1 - ey0)/2 + oreja;                 // del centro al borde del ala
R        = radio_muneca;
flecha_c = R > 0 ? R - sqrt(R*R - medio*medio) : 0;   // cuanto baja el borde

z_pcb  = piso + pila_abajo;                        // cara de abajo de la placa
z_tope = z_pcb + pcb_esp + pila_arriba;            // borde superior de la base
vastago = pila_arriba - alto_pulsador - boton_juego - boton_ala;

// tornillos: uno en el hueco libre de la izquierda (sin placa) y dos en las
// esquinas macizas que deja la punta redonda
tornillos = [ [x0 + 2.4, y0 + 2.4],
              [x1 - 1.6, y0 + 1.6],
              [x1 - 1.6, y1 - 1.6] ];
botones = [ btn_A, btn_B, btn_BORRAR ];

echo(str("Cuerpo: ", ex1-ex0, " x ", ey1-ey0, " mm.  Con alas: ", 2*medio,
         " mm.  Alto: ", z_tope + tapa_esp + boton_sobresale, " mm"));
if(vastago < 0) echo("OJO: no hay lugar para la pestaña del boton: subi pila_arriba");

/* ================= formas 2D ================= */
module cavidad2d(){
  hull(){
    translate([x0, y0]) square([0.01, y1 - y0]);
    translate([x1 - r_punta, (y0 + y1)/2]) circle(r = r_punta);
  }
}
module cuerpo2d(){
  hull() for(x = [ex0 + radio_ext, ex1 - radio_ext], y = [ey0 + radio_ext, ey1 - radio_ext])
    translate([x, y]) circle(r = radio_ext);
}
module alas2d(){
  for(s = [-1, 1]) translate([cx, cy + s*((ey1 - ey0)/2 + oreja/2 - 1)])
    difference(){
      hull() for(x = [-1, 1], y = [-1, 1])
        translate([x*(ala_larg/2 - 2.5), y*(oreja/2 + 1 - 2.5)]) circle(r = 2.5);
      // la ranura de la correa
      translate([-(correa_ancho + 1)/2, s > 0 ? -oreja/2 + 1.8 : oreja/2 - 1.8 - correa_esp])
        square([correa_ancho + 1, correa_esp]);
    }
}
module tecla2d(extra = 0){
  offset(r = boton_radio + extra) square(boton_lado - 2*boton_radio, center = true);
}
module flecha2d(){        // flecha de "borrar", apuntando a la izquierda
  s = boton_lado * 0.3;
  polygon([[-s,0],[0,s],[0,s*0.4],[s,s*0.4],[s,-s*0.4],[0,-s*0.4],[0,-s]]);
}
// la muñeca: un cilindro acostado a lo largo del antebrazo, debajo de la caja
module muneca(r){
  translate([cx, cy, -R]) rotate([0, 90, 0]) cylinder(r = r, h = 300, center = true, $fn = 200);
}

/* ================= BASE ================= */
module base(){
  difference(){
    union(){
      translate([0, 0, -flecha_c]) linear_extrude(z_tope + flecha_c) cuerpo2d();
      // alas curvas: una cascara de ala_esp que sigue la muñeca
      if(R > 0) intersection(){
        translate([0, 0, -flecha_c - 1]) linear_extrude(flecha_c + ala_esp + 2) alas2d();
        muneca(R + ala_esp);
      } else linear_extrude(ala_esp) alas2d();
    }
    if(R > 0) muneca(R);                         // el hueco de la muñeca
    // hueco de arriba: del nivel de la placa hasta el borde
    translate([0, 0, z_pcb]) linear_extrude(z_tope) cavidad2d();
    // hueco de abajo, mas angosto: deja el escalon donde apoya la placa
    translate([0, 0, piso]) linear_extrude(z_tope) offset(delta = -apoyo) cavidad2d();
    // tornillos de las esquinas macizas
    for(i = [1, 2]) translate([tornillos[i].x, tornillos[i].y, piso])
      cylinder(d = tornillo_d, h = z_tope);
  }
  // poste del tornillo de la izquierda, en el hueco donde no hay placa
  difference(){
    translate([tornillos[0].x, tornillos[0].y, piso - 0.01]) cylinder(d = 5, h = z_tope - piso);
    translate([tornillos[0].x, tornillos[0].y, piso]) cylinder(d = tornillo_d, h = z_tope);
  }
  // perno que entra en la media luna de la placa
  if(usar_muesca)
    intersection(){
      translate([muesca_x, 0, piso - 0.01]) cylinder(r = muesca_r - 0.2, h = z_pcb + pcb_esp - piso);
      linear_extrude(z_tope) cavidad2d();
    }
}

/* ================= TAPA =================  (z = 0 es su cara de abajo) */
module tapa(){
  difference(){
    union(){
      linear_extrude(tapa_esp) cuerpo2d();
      // labio que centra la tapa dentro de la base
      translate([0, 0, -1.2]) linear_extrude(1.2) difference(){
        offset(delta = -0.25) cavidad2d();
        offset(delta = -1.45) cavidad2d();
      }
    }
    // el labio no puede chocar con el poste
    translate([tornillos[0].x, tornillos[0].y, -2]) cylinder(d = 6, h = 2);
    // agujeros de los botones (atraviesan tapa y labio)
    for(b = botones) translate([b.x, b.y, -2])
      linear_extrude(tapa_esp + 3) tecla2d(boton_juego);
    // lugar para la pestaña de cada boton, donde pisa el labio
    for(b = botones) translate([b.x, b.y, -2])
      linear_extrude(2) tecla2d(boton_ala + boton_juego);
    // tornillos con cabeza embutida
    for(p = tornillos) translate([p.x, p.y, -2]){
      cylinder(d = tornillo_paso, h = tapa_esp + 3);
      translate([0, 0, 2 + tapa_esp - 0.9]) cylinder(d = tornillo_cabeza, h = 1);
    }
    // ventana del LED
    translate([led_pos.x, led_pos.y, -2]) cylinder(d = 2.5, h = tapa_esp + 3);
  }
}

/* ================= BOTON =================
   En su lugar: origen en el centro del pulsador, z = 0 en la cara de abajo de
   la tapa. De arriba hacia abajo: tecla, pestaña en cuña, vastago.          */
module boton(i){
  b = botones[i];
  difference(){
    union(){
      // la tecla que asoma
      translate([0, 0, -boton_juego])
        linear_extrude(boton_juego + tapa_esp + boton_sobresale) tecla2d();
      // lo que queda debajo de la tapa, recortado para no tocar pared ni labio
      intersection(){
        union(){
          hull(){                                  // pestaña en cuña (imprime sin soporte)
            translate([0, 0, -boton_juego - 0.01]) linear_extrude(0.01) tecla2d();
            translate([0, 0, -boton_juego - boton_ala]) linear_extrude(0.01) tecla2d(boton_ala);
          }
          if(vastago > 0)
            translate([0, 0, -boton_juego - boton_ala - vastago]) cylinder(d = 5, h = vastago + 0.01);
        }
        translate([-b.x, -b.y, -30]) linear_extrude(30) offset(delta = -1.7) cavidad2d();
      }
    }
    // letra hundida
    translate([0, 0, tapa_esp + boton_sobresale - grabado]) linear_extrude(grabado + 1){
      if(i == 0) text("A", size = boton_lado*0.55, halign = "center", valign = "center",
                      font = "Liberation Sans:style=Bold");
      if(i == 1) text("B", size = boton_lado*0.55, halign = "center", valign = "center",
                      font = "Liberation Sans:style=Bold");
      if(i == 2) flecha2d();
    }
  }
}
// los tres, dados vuelta: la cara con la letra contra la cama
module botones_para_imprimir(){
  for(i = [0 : 2]) translate([i*(boton_lado + 2*boton_ala + 4), 0, tapa_esp + boton_sobresale])
    rotate([180, 0, 0]) boton(i);
}

/* ================= PLANTILLA de prueba ================= */
module plantilla(){
  linear_extrude(0.8) difference(){
    offset(delta = 2) cavidad2d();
    for(b = botones) translate(b) circle(d = 4);
    translate(led_pos) circle(d = 2.5);
    if(usar_muesca) translate([muesca_x, 0]) circle(r = 0.6);   // marca de la muesca
  }
  // contorno de la placa, en relieve, para comparar
  linear_extrude(1.4) difference(){
    offset(delta = 0.8) cavidad2d();
    cavidad2d();
  }
}

/* ================= salida ================= */
if(PIEZA == "base")      base();
if(PIEZA == "tapa")      translate([0, 0, tapa_esp]) rotate([180, 0, 0]) tapa();
if(PIEZA == "botones")   botones_para_imprimir();
if(PIEZA == "plantilla") plantilla();
if(PIEZA == "armado"){
  base();
  translate([0, 0, z_tope]) tapa();
  for(i = [0 : 2]) translate([botones[i].x, botones[i].y, z_tope]) boton(i);
}
if(PIEZA == "todo"){
  base();
  // la tapa al lado, dada vuelta: cara de afuera contra la cama
  translate([0, 2*ey1 + oreja + 10, tapa_esp]) rotate([180, 0, 0]) tapa();
  translate([x0, ey0 - oreja - 14, 0]) botones_para_imprimir();
}

// NOTAS DE IMPRESION
//  - PETG o PLA, capa de 0,2 mm, 3 perimetros, 40 % de relleno en las alas.
//  - La BASE tiene el fondo curvo: imprimila con soporte desde la cama, o
//    ponela de costado. Con radio_muneca = 0 el fondo es plano y no hace falta.
//  - La TAPA y los BOTONES van con la cara de afuera contra la cama: no
//    necesitan soporte.
//  - Boton duro o que no vuelve: subi boton_juego a 0.4.
//  - Boton que baila o no llega a apretar: corregi alto_pulsador.
