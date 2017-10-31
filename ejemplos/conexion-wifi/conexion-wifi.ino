//Conexión a una WiFi 
#include <ESP8266WiFi.h>

//SSID of your network
char ssid[] = "Nombre de la wifi"; //SSID of your Wi-Fi router
char pass[] = "Contraseña de la wifi"; //Password of your Wi-Fi router

void setup()
{
  Serial.begin(115200);//numero de baudios 
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
