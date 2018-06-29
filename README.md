En este repositorio hay un serie de programas escritos para la placa Wemos (https://www.wemos.cc/). Esta placa nos ofrece la posibilidad de conectarnos a Internet a través de una red Wi-Fi sin tener que incluir ningún otro elemento como ocurre con la placa Arduino 1. Además de esta placa utilizamos sensores, como el BMP280, y otros elementos como pantallas led

Este proyecto está centrado en IoT por lo que en todos los casos enviamos las observaciones registradas por los sensores a servicios disponibles en Internet. Los que hemos utilizado son: Blynk, Thingspeak y Observatorios Escolares.

Blynk: Es un servicio de Internet (https://www.blynk.cc/) que nos permite comunicar nuestra placa con un dispositivo móvil de tal forma que podemos ver las observaciones registradas y muchas cosas más. 

Thingspeak: Este otro servicio WEB nos permite visualizar nuestras observaciones a través de un explorador de Internet. Para los programas que envían datos a este servicio tenemos que tener en cuenta que envía los datos cada 30 segundos y los actualiza en la
pantalla OLED cada segundo. MUY IMPORTANTE: Al usar WifiManager, para que los datos de configuración queden correctamente grabados, hay que acceder al portal cautivo (192.168.4.1), configurar todos los datos de conexión (SSID, Clave, proveedores...), y pulsar SAVE. Aparecerá una pantalla ofreciéndonos un enlace para comprobar si la configuración se ha guardado correctamente. En ese momento desconectamos de la red Observatorios Escolares, esperamos 10 segundos, volvemos a conectar a la red Observatorios Escolares, y hacemos clic sobre el enlace. Si todo ha ido bien veremos en la parte inferior de la pantalla las credenciales de la SSID que hemos configurado.

Observatorios Escolares: este es un servicio de EducaMadrid al que podemos enviar las medidas de los parámetros registrados y visualizarlos gráficamente además de ofrecernos la ubicación geográfica de la estación que los está emitiendo. 

Wemos_bme280, wemos_bme280_no_desconexion y wemos_bme280_OLED utilizan el servicio Observatorios Escolares. En los demás el propio nombre describe el servicio utilizado y además hay dos directorios de ejemplos para familiarizarnos del uso de las redes Wi-Fi con la tarjeta Wemos. 
