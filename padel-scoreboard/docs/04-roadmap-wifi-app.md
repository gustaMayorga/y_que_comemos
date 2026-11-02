# 04 - Roadmap: WiFi, app y lo que queda pensado desde ahora

El MVP no lleva WiFi encendido, pero **no hay que rehacer nada** para agregarlo.
Lo que ya está resuelto hoy:

1. El cerebro es un **ESP32**: WiFi y BLE están en el chip, sin hardware extra.
2. El estado del partido es un **único struct serializable** (`MatchState`).
3. El **contrato JSON ya está congelado** (`net_stub.h`). La app se puede empezar
   a escribir hoy contra ese formato, mockeado, sin la placa.
4. `publishState()` es el único punto de salida de datos. Hoy escribe en `Serial`;
   mañana además hace `ws.textAll(buf)`. Una línea.

## Contrato de datos (ya definido)

Estado que publica el cartel en cada cambio:

```json
{
  "phase":   "game",          // "game" | "tiebreak" | "over"
  "points":  ["40", "30"],    // "0","15","30","40","AD"  (vacío en tie break)
  "tb":      [0, 0],          // puntos del tie break
  "games":   [3, 2],
  "sets":    [1, 0],
  "serving": 0,               // 0 = equipo A, 1 = equipo B
  "winner":  -1,              // -1 sin ganador, 0 o 1
  "n":       47               // puntos jugados (para detectar mensajes perdidos)
}
```

Comandos que la app le manda al cartel (mismo canal, WebSocket):

```json
{"cmd":"point","team":0}
{"cmd":"undo"}
{"cmd":"reset"}
{"cmd":"serve","team":1}
{"cmd":"rules","goldenPoint":true,"setsToWin":2,"tieBreak":true}
{"cmd":"brightness","value":180}
```

## Fases

### v1.1 — App web sin internet (el paso más barato y el más útil)

ESP32 en **modo Access Point**: SSID `Padel-Cancha-1`, sin router, sin datos.
Los jugadores se conectan con el celular y abren `192.168.4.1`.

- `ESPAsyncWebServer` + `AsyncWebSocket` en `/ws`
- Página servida desde LittleFS: tanteo en vivo, botones +1 / undo / reset,
  selector de reglas (punto de oro, sets), slider de brillo
- Portal cautivo + DNS para que el celular abra la página solo al conectarse
- Flag ya existe: `ENABLE_WIFI` en `config.h`
- Costo en hardware: **cero**

Con esto el control remoto pasa a ser opcional y el cartel gana configuración
de reglas sin recompilar.

### v1.2 — Remotos ESP-NOW (reemplazo elegante del RF 433)

Dos remotos con **ESP32-C3 supermini** + batería LiPo 500 mAh + 2 botones,
impresos en 3D, colgables de la reja o del bolsillo.

- **ESP-NOW**: sin router, latencia ~2 ms, alcance >100 m, pairing por MAC
- Deep sleep con wake on GPIO: ~1 año de batería con uso de club
- Confirmación de recepción: un LED del remoto parpadea verde al registrarse
  el punto (el RF 433 es unidireccional: nunca sabés si llegó)
- Telemetría de batería: el cartel avisa cuando un remoto está por agotarse

### v1.3 — Conectado al club

- Modo **STA** con credenciales guardadas en NVS + portal de configuración
  (WiFiManager), mDNS `padel-1.local`
- **Persistencia del partido en NVS**: si se corta la alimentación, al volver
  retoma el tanteo exacto
- **OTA** (`ArduinoOTA`): actualizar firmware sin bajar el cartel de la reja
- Backend (reutilizable del stack de este repo: Node + Express + TS):
  `POST /matches` al terminar cada partido ⇒ historial, estadísticas, ranking interno

### v2 — Producto

- Varias canchas con un panel único (cada ESP32 reporta por MQTT)
- Integración con el sistema de reservas: nombres de los jugadores en la app,
  partido asociado al turno reservado
- Cronómetro de turno y aviso de "quedan 10 minutos"
- Modo torneo: cuadros, formato configurable por categoría
- Replay: al ser una secuencia de puntos con timestamp, se puede reconstruir el
  partido entero (`pointCount` ya está en el estado)

## Decisiones que conviene NO cambiar después

Estas tres son las que, si se mueven más adelante, obligan a rehacer trabajo:

1. **Geometría del display** (`LEDS_PER_SEG_BIG`, orden A→B→C…→G de la guirnalda).
   Cambiarlo implica volver a soldar.
2. **Contrato JSON.** Si la app ya está escrita, agregar campos está bien;
   renombrar los existentes, no.
3. **5 V con límite de corriente por firmware.** Es lo que hace que cualquier
   power bank sirva. Si algún día se pasa a 12 V con LEDs de potencia, el firmware
   de display se rehace entero.
