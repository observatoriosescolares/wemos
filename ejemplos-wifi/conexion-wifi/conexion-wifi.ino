/*
PRÁCTICA NÚMERO 2: En este caso nos vamos a conectar a una wifi de la que conocemos el nombre (SSID) y la contraseña (pass).
El primer paso que tenemos que hacer antes de enviar este scketc a nuetra Wemos es cabiar las "XXXXX" por los datos de nuestra Wifi.
También tenemos que configurar nuestro monitor serie a 115200 baudios. 
Si la conexión se realiza bien nos mostrará en el monitor serie la direccón IP obtenida. 
*/
#include <ESP8266WiFi.h>

//SSID of your network
char ssid[] = "XXXXX"; //SSID of your Wi-Fi router
char pass[] = "XXXXX"; //Password of your Wi-Fi router

void setup()
{
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to...");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi connected successfully");
  Serial.println(WiFi.localIP());
}

void loop () {}
