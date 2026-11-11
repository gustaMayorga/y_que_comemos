// ============================================================================
//  Caja tipo RELOJ para el control remoto KL-238A  (3 botones: A, B, borrar)
//
//  Dos piezas: BASE (bandeja con orejas para la correa) y TAPA (con las tres
//  teclas flexibles impresas en la misma pieza, sin partes sueltas).
//  Cierra con 3 tornillos M2 x 8 autorroscantes.
//
//  LAS MEDIDAS SALEN DE FOTOS CON CINTA METRICA: son aproximadas (+-1 mm).
//  Antes de imprimir la caja entera:
//     1) poné  PIEZA = "plantilla"  e imprimi esa lamina (5 minutos)
//     2) apoyá la placa encima: los tres agujeros tienen que caer sobre los
//        tres pulsadores y el contorno tiene que coincidir
//     3) corregi los numeros de abajo con un calibre y recien ahi imprimi
//
//  Origen de coordenadas: esquina inferior izquierda del CUERPO de la placa
//  (la parte ancha), mirandola con los botones hacia arriba y el LED a la
//  derecha. X hacia el LED, Y hacia el brazo de la pila.
// ============================================================================

PIEZA = "todo";      // "todo" | "base" | "tapa" | "plantilla" | "armado"

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
piso      = 1.6;
tapa_esp  = 1.6;
apoyo     = 1.0;     // ancho del escalon donde apoya la placa
radio_ext = 5;       // redondeo de las esquinas de afuera

correa_ancho = 20;   // ancho de la correa (velcro o tipo NATO)
correa_esp   = 3.0;  // ancho de la ranura
correa_barra = 3.5;  // material que queda por fuera de la ranura

tornillo_d      = 1.7;  // agujero para M2 autorroscante en la base
tornillo_paso   = 2.3;  // agujero pasante en la tapa
tornillo_cabeza = 4.2;

/* ---------------- las teclas ---------------- */
tecla_d      = 6.0;  // diametro de la cabeza
tecla_cuello = 4.0;  // ancho del brazo flexible
tecla_largo  = 11;   // del centro del pulsador a la bisagra
tecla_ranura = 0.7;  // corte alrededor de la tecla
tecla_juego  = 0.3;  // aire entre el tetón y el pulsador en reposo
grabado      = 0.5;  // profundidad de las letras (van hundidas)

$fn = 48;

/* ================= derivados ================= */
x0 = -(brazo_largo + resorte) - holgura;          // pared interior izquierda
x1 = pcb_largo + led_extra + holgura;             // pared interior derecha
y0 = -holgura;
y1 = pcb_ancho + holgura;
r_punta = (y1 - y0) / 2;

oreja = correa_esp + correa_barra;
ex0 = x0 - pared - oreja;                          // limites exteriores
ex1 = x1 + pared + oreja;
ey0 = y0 - pared;
ey1 = y1 + pared;

z_pcb  = piso + pila_abajo;                        // cara de abajo de la placa
z_tope = z_pcb + pcb_esp + pila_arriba;            // borde superior de la base
teton  = pila_arriba - alto_pulsador - tecla_juego;

// tornillos: uno en el hueco libre de la izquierda (sin placa) y dos en las
// esquinas macizas que deja la punta redonda
tornillos = [ [x0 + 2.4, y0 + 2.4],
              [x1 - 1.6, y0 + 1.6],
              [x1 - 1.6, y1 - 1.6] ];

echo(str("Caja: ", ex1-ex0, " x ", ey1-ey0, " x ", z_tope+tapa_esp, " mm"));
echo(str("Teton de las teclas: ", teton, " mm"));

/* ================= formas 2D ================= */
module cavidad2d(){
  hull(){
    translate([x0, y0]) square([0.01, y1 - y0]);
    translate([x1 - r_punta, (y0 + y1)/2]) circle(r = r_punta);
  }
}
module exterior2d(){
  hull() for(x = [ex0 + radio_ext, ex1 - radio_ext], y = [ey0 + radio_ext, ey1 - radio_ext])
    translate([x, y]) circle(r = radio_ext);
}
module ranuras_correa2d(){
  for(x = [ex0 + correa_barra, ex1 - correa_barra - correa_esp])
    translate([x, (ey0 + ey1)/2 - correa_ancho/2]) square([correa_esp, correa_ancho]);
}
// tecla en el origen, con la bisagra hacia -X
module tecla2d(){
  hull(){
    circle(d = tecla_d);
    translate([-tecla_largo, -tecla_cuello/2]) square([0.01, tecla_cuello]);
  }
}
module ranura_tecla2d(){
  intersection(){
    difference(){ offset(delta = tecla_ranura) tecla2d(); tecla2d(); }
    translate([-tecla_largo + 0.01, -50]) square([100, 100]);
  }
}
// [posicion, angulo de la bisagra]  0 = bisagra a la izquierda, 90 = abajo
teclas = [ [btn_A, 0], [btn_B, 0], [btn_BORRAR, 90] ];

module flecha2d(){        // flecha de "deshacer", apuntando a la izquierda
  s = tecla_d * 0.36;
  polygon([[-s,0],[0,s],[0,s*0.4],[s,s*0.4],[s,-s*0.4],[0,-s*0.4],[0,-s]]);
}

/* ================= BASE ================= */
module base(){
  difference(){
    linear_extrude(z_tope) difference(){ exterior2d(); ranuras_correa2d(); }
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
    translate([tornillos[0].x, tornillos[0].y, 0]) cylinder(d = 5, h = z_tope);
    translate([tornillos[0].x, tornillos[0].y, piso]) cylinder(d = tornillo_d, h = z_tope);
  }
  // perno que entra en la media luna de la placa
  if(usar_muesca)
    intersection(){
      translate([muesca_x, 0, 0]) cylinder(r = muesca_r - 0.2, h = z_pcb + pcb_esp);
      translate([0, 0, 0]) linear_extrude(z_tope) cavidad2d();
    }
}

/* ================= TAPA ================= */
module tapa(){
  difference(){
    union(){
      linear_extrude(tapa_esp) difference(){ exterior2d(); ranuras_correa2d(); }
      // labio que centra la tapa dentro de la base
      translate([0, 0, -1.2]) linear_extrude(1.2) difference(){
        offset(delta = -0.25) cavidad2d();
        offset(delta = -1.45) cavidad2d();
      }
    }
    // el labio no puede chocar con el poste ni con el perno
    translate([tornillos[0].x, tornillos[0].y, -2]) cylinder(d = 6, h = 2);
    // cortes de las teclas (atraviesan tapa y labio)
    for(t = teclas) translate([t[0].x, t[0].y, -2]) rotate(t[1])
      linear_extrude(tapa_esp + 3) ranura_tecla2d();
    // tornillos con cabeza embutida
    for(p = tornillos) translate([p.x, p.y, -2]){
      cylinder(d = tornillo_paso, h = tapa_esp + 3);
      translate([0, 0, 2 + tapa_esp - 0.8]) cylinder(d = tornillo_cabeza, h = 1);
    }
    // el labio tampoco va debajo de las teclas: tienen que moverse libres
    for(t = teclas) translate([t[0].x, t[0].y, -2]) rotate(t[1])
      linear_extrude(2) offset(delta = tecla_ranura) tecla2d();
    // letras hundidas en la cara de afuera
    translate([0, 0, tapa_esp - grabado]) linear_extrude(grabado + 1){
      translate(btn_A) text("A", size = tecla_d*0.62, halign = "center", valign = "center",
                            font = "Liberation Sans:style=Bold");
      translate(btn_B) text("B", size = tecla_d*0.62, halign = "center", valign = "center",
                            font = "Liberation Sans:style=Bold");
      translate(btn_BORRAR) flecha2d();
    }
    // ventana del LED
    translate([led_pos.x, led_pos.y, -2]) cylinder(d = 2.5, h = tapa_esp + 3);
  }
  // tetones que empujan los pulsadores
  for(t = teclas) translate([t[0].x, t[0].y, -teton]) cylinder(d = 3.5, h = teton);
}

/* ================= PLANTILLA de prueba ================= */
module plantilla(){
  linear_extrude(0.8) difference(){
    offset(delta = 2) cavidad2d();
    for(b = [btn_A, btn_B, btn_BORRAR]) translate(b) circle(d = 4);
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
if(PIEZA == "tapa")      tapa();
if(PIEZA == "plantilla") plantilla();
if(PIEZA == "armado"){   base(); translate([0, 0, z_tope]) tapa(); }
if(PIEZA == "todo"){
  base();
  // la tapa al lado, dada vuelta: cara de afuera contra la cama
  translate([0, 2*ey1 + 8, tapa_esp]) rotate([180, 0, 0]) tapa();
}

// NOTAS DE IMPRESION
//  - PETG o PLA, capa de 0,2 mm, 3 perimetros. Las teclas flexionan mejor en PETG.
//  - La tapa va con la cara de AFUERA contra la cama (asi sale en "todo"): los
//    tetones y el labio quedan hacia arriba y no hace falta soporte.
//  - Si una tecla queda dura, subi tecla_largo o baja tapa_esp a 1.2.
//  - Si una tecla queda apretada todo el tiempo, subi tecla_juego.
