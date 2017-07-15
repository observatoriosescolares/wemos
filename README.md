Blynk: Envía los datos de temperatura, presión, altura y humedad a Blynk
cada 30 segundos. No hace uso de la pantalla OLED.

Thingspeak_SmartCityThing: Utiliza WifiManager para configurar los datos
de conexión. Envía los datos cada 30 segundos y los actualiza en la
pantalla OLED cada segundo. MUY IMPORTANTE: Al usar WifiManager, para que
los datos de configuración queden correctamente grabados, hay que acceder
al portal cautivo (192.168.4.1), configurar todos los datos de conexión
(SSID, Clave, proveedores...), y pulsar SAVE. Aparecerá una pantalla
ofreciéndonos un enlace para comprobar si la configuración
se ha guardado correctamente. En ese momento desconectamos de la red
Observatorios Escolares, esperamos 10 segundos, volvemos a conectar a la
red Observatorios Escolares, y hacemos clic sobre el enlace. Si todo ha ido
bien veremos en la parte inferior de la pantalla las credenciales de la SSID
que hemos configurado.
