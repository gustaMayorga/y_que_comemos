# Stubs de Arduino / FastLED

Shims mínimos que permiten **compilar el sketch completo en la PC**, sin placa y
sin el toolchain de Arduino. No emulan hardware: sirven para detectar errores de
compilación y para verificar que **todas las combinaciones de placa y perfil de
config.h compilan**. Los usa `tests/run_checks.sh`.

Lo que esto NO reemplaza: el compilador de AVR (uso real de RAM/flash) ni el
comportamiento de los LEDs. Para eso hay que compilar con `arduino-cli`.
