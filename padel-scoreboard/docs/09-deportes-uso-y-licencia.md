# 09 - Modos de deporte, contador de uso y licencia temporal

## 1. Modos de deporte

El cartel cambia de deporte **manteniendo 2 segundos el botón local de la caja**.
Muestra la letra del modo 1,5 s, resetea el tanteo y guarda la elección en la
EEPROM (al prender de nuevo arranca en el mismo deporte).

| Letra | Deporte | Cómo cuenta |
|:---:|---|---|
| **P** | Pádel | 0/15/30/40, **punto de oro**, sets a 6 con tie break, al mejor de 3 |
| **t** | Tenis | igual pero **con ventajas** (sin punto de oro) |
| **U** | Vóley | puntos corridos a **25 con 2 de diferencia**, al mejor de 5, **5º set a 15** |
| **F** | Fútbol | contador libre, sin límite |
| **b** | Básquet | contador libre, **hasta 999** |
| **L** | Libre | contador genérico para cualquier cosa |

> La **V** no se puede dibujar en 7 segmentos. Se usa **U**, que es lo estándar
> en los displays de este tipo.

### Lo elegante: es el mismo motor

No hay un tanteador por deporte. Un set de vóley **es exactamente un tie break**
a 25 en lugar de a 7, y eso el motor ya lo sabía hacer. Fútbol, básquet y libre
son el mismo tie break con el objetivo en 0, o sea sin límite.

Agregar un deporte nuevo = **agregar una fila** en la tabla `REGLAS` de
`deportes.h`. No se toca la lógica.

```c
//                   oro   juegos  sets  tieB  objetivo  soloTB  setFinal
/* PADEL   */ { true,     6,      2,   true,     7,    false,     0 },
/* VOLEY   */ { false,    0,      3,   true,    25,    true,     15 },
/* BASQUET */ { false,    0,     99,  false,     0,    true,      0 },
```

### Básquet de más de 99 puntos

Hay sólo 2 dígitos grandes por equipo. En los modos de contador, el **dígito de
juegos se reusa como centena**, así el marcador llega a **999** sin agregar un
solo LED. En pádel ese dígito sigue mostrando los juegos, como siempre.

### El botón de deshacer es el botón de restar

En fútbol y básquet, si alguien aprieta de más, el **botón 3 resta un punto**.
Es el mismo CTRL+Z de siempre, pero en un contador se comporta como un "menos".

## 2. Contador de uso

El cartel acumula **minutos encendido** en la EEPROM. Para verlos:
**pulso corto en el botón de RESET** → muestra las **horas de uso** 2,5 segundos.
(El reset necesita 2 segundos mantenidos, así que el pulso corto queda libre.)

También se guarda la cantidad de partidos terminados y sale todo por el puerto
serie al arrancar.

**Sobre el desgaste de la EEPROM**: aguanta ~100.000 escrituras por celda. El
firmware usa **16 ranuras rotativas** y guarda cada 10 minutos de uso
(`GUARDAR_CADA_MIN`), lo que da décadas de margen. Además **la EEPROM no se
borra al recargar el sketch por USB** — el bootloader sólo escribe flash. Sólo
se pierde con un *chip erase* por ISP.

## 3. Licencia temporal (equipos en comodato)

Para entregar un cartel prestado que deja de contar pasado un tiempo, hasta que
se lo desbloquea con un combo secreto.

### ⚠️ Lo primero: son HORAS DE USO, no días de calendario

El Nano **no tiene reloj**. Al apagarse pierde la hora, y el cartel se apaga
entre partido y partido. **No hay forma de saber cuántos días pasaron.**

Por eso la licencia cuenta **minutos con el cartel encendido**, acumulados en la
EEPROM. Equivalencia práctica:

| Si querés… | Poné |
|---|---|
| ~30 días jugando 2 h por día | `LIC_HORAS 60` |
| ~30 días jugando 4 h por día | `LIC_HORAS 120` |
| ~30 días de club, 8 h por día | `LIC_HORAS 240` |

Si hacen falta **días de calendario de verdad**, hay que sumar un **RTC DS3231**
(~2 USD, con pila de respaldo, I²C a A4/A5). Es la única manera honesta de
medir tiempo con el equipo apagado. El firmware está preparado para eso: se
cambiaría `Almacen::tick()` para leer el RTC en vez de contar `millis()`.

### Configuración

```c
// config.h
#define LIC_HORAS              0    // 0 = SIN licencia (la version normal)
#define LIC_EXTENSION_HORAS   60    // cuanto suma cada desbloqueo
#define LIC_PANTALLA           0    // 0 = "Lc 00" visible | 1 = discreto
```

Con `LIC_HORAS 0` **todo este sistema no existe**: es la versión que se entrega
vendida. La versión de comodato es el mismo firmware con un número cambiado.

### Qué pasa al vencer

El cartel **deja de contar puntos** y queda titilando la pantalla de licencia.
Los botones siguen leyéndose, pero sólo para el combo.

### 💡 Por qué conviene la pantalla visible (`LIC_PANTALLA 0`)

Muestra **`Lc 00`**, que es inequívoco: algo hay que hacer, hay que llamar.

Disfrazarlo de falla parece más astuto pero **juega en contra**:

- si parece roto, el cliente **lo abre, lo sacude o lo devuelve** diciendo
  "se rompió" — y nunca llama, que era justo el objetivo;
- "ese cartel se rompe al mes" es **la peor fama posible** para venderle a clubes;
- si dos clientes comparan notas, o alguien lee el firmware, el problema deja de
  ser comercial y pasa a ser legal: deshabilitar a propósito un equipo prestado
  y atribuirlo a una falla es otra cosa que una licencia que vence.

Un vencimiento declarado es una **condición del comodato**, y se rotula en las
instrucciones (ver abajo). Es más efectivo y no tiene costo.

La opción `LIC_PANTALLA 1` queda disponible igual. Es una decisión comercial, no
técnica.

### El combo de desbloqueo

Se define en `config.h`. Cada byte es un paso: nibble alto = número de botón
(1-4), nibble bajo = 0 corto (<1 s) / 1 largo (1-5 s) / 2 muy largo (>5 s).

```c
#define LIC_COMBO_DEF   { 0x12, 0x22, 0x10, 0x10, 0x31 }
#define LIC_COMBO_PASOS   5
```

Ese ejemplo es: **botón 1 muy largo → botón 2 muy largo → botón 1 corto →
botón 1 corto → botón 3 largo**.

Dos cosas que lo hacen seguro:

- **No da ninguna pista visual** de cuántos pasos van. No se puede sacar por
  prueba y error, hay que conocer la secuencia completa.
- Si pasan más de **8 segundos** entre pasos, o se aprieta algo que no
  corresponde, **vuelve a cero sin avisar**.

Al completarse: suena una escala ascendente, muestra `o00` en verde, suma
`LIC_EXTENSION_HORAS` **desde el uso actual** (así cada desbloqueo da el período
completo, no importa cuánto se pasó) y anota el desbloqueo en la EEPROM.

**Recomendación**: cambiá el combo por defecto antes de entregar el primero. Si
entregás varios equipos, **un combo distinto por cliente** — es cambiar una línea
y recompilar.

## 4. Qué rotular en las instrucciones

Lo que sí va en el manual del usuario:

- Cómo cambiar de deporte (**mantener 2 s el botón de la caja**) y la tabla de letras
- Cómo ver las **horas de uso** (pulso corto en RESET)
- Cómo **resetear el partido** (mantener RESET 2 s hasta que se llene la barra roja)
- El mapa de los botones del control
- Y en la versión de comodato: **que la licencia vence y hay que llamar para
  renovarla**, con el teléfono. Eso es lo que convierte el `Lc 00` en una
  llamada en vez de en una devolución.

Lo que **no** va en el manual: el combo. Ese es tuyo.
