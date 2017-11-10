#include <ESP8266WiFi.h>  
#include <DNSServer.h>
#include <Wire.h>  	  
#include <SFE_MicroOLED.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define PIN_RESET 255  //
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D
#define SEALEVELPRESSURE_HPA (1013.25)
//////////////////////////////////////////////
// DATOS DE CONEXIÓN WIFI Y A LA PLATAFORMA //
//////////////////////////////////////////////
char ssid[32] = "SSID DE LA WIFI";
char password[32] = "CONTRASEÑA DE LA WIFI";
String proveedor = "IDENTIFICADOR DEL PROVEEDOR";
String key_proveedor = "KEY DEL PROVEEOR";
const char* server = "iot.educa.madrid.org";

////////////////////////////////////////////////
// Definición de variables globales y objetos //
////////////////////////////////////////////////
String texto = " ";
String parametro;
WiFiClient client;
unsigned long ultimo_envio = 0;
MicroOLED oled(PIN_RESET, DC_JUMPER);  // I2C Example
Adafruit_BME280 bme; // I2C

///////////////////////////
// COMIENZO DEL PROGRAMA //
///////////////////////////
void setup() {
  Serial.begin(9600);
  bme.begin();
  escribirIni();  //Escribe la pantalla de bienvenida en la OLED, se debe cambiar el centro educativo en la función escribirIni()
  delay(1000);
}

void loop()
{
  //MEDIMOS LOS PARÁMETROS METEOROLÓGICOS.
  float t = bme.readTemperature();// Temperatura
  float p = bme.readPressure() / 100.0F;// Presión
  float a = bme.readAltitude(SEALEVELPRESSURE_HPA);// Altura
  float h = bme.readHumidity();// Humedad
  //ESCRIBIMOS EN LA PANTALLA LED LOS DATOS MEDIDOS EN CADA LOOP
  oled.setCursor(0, 0);
  oled.print("T: ");
  oled.print(t, 1);
  oled.print(" *C");
  oled.setCursor(0, 10);
  oled.print("P: ");
  oled.print(p, 0);
  oled.println(" hPa");
  oled.setCursor(0, 20);
  oled.print("A: ");
  oled.print(a, 1);
  oled.println(" m");
  oled.setCursor(0, 30);
  oled.print("H: ");
  oled.print(h, 1);
  oled.println(" %");
  oled.display();

  // CADA 10 MINUTOS SE REALIZA LA CONEXIÓN A LA WIFI, CONEXIÓN AL SERVIDOR, ENVÍO Y DESCONEXIÓN DEL SERVIDOR Y DESCONEXIÓN DE LA WIFI.
  if (millis() > ultimo_envio + 600000) {
    Serial.println("CONEXIÓN A LA WIFI");
    while (WiFi.status() != WL_CONNECTED) {
      texto = "CONEC W";
      escribir(texto);
      // CONEXIÓN A LA RED WIFI
      WiFi.begin(ssid, password);// Nos conectamos a la red wifi definida.
      texto = "CONECTADO A LA WIFI";
      escribir(texto);
      delay(5000);
      Serial.println(WiFi.localIP());  // Muestra por el monitor serie del IDE de arduino la IP de Wemos para comprobar si todo ha ido bien.
    }
      String TemValor = String(t);
      parametro = "temperatura";
      enviar(TemValor,parametro);
      String PresionValor = String(p);
      parametro = "presion";
      enviar(PresionValor,parametro);
      String HumValor = String(h);
      parametro = "humedad";
      enviar(HumValor, parametro);
      ultimo_envio = millis();
      WiFi.disconnect();
  }
}
//////////////////////////////////////////////////////////////
// NOS CONECTAMOS AL SERVIDOR, ENVIAMOS Y NOS DESCONECTAMOS //
//////////////////////////////////////////////////////////////
void enviar(String valor, String parametro){
  if(client.connect(server, 8081)) {
    texto="SENDING";
    escribir(texto);
    client.println("PUT /data/"+proveedor+"/"+parametro+"/"+valor+" HTTP/1.1");
    client.println("IDENTITY_KEY: "+ key_proveedor);
    client.println("Content-Length: 0");
    client.println("Content-Type: text/plain; charset=ISO-8859-1");
    client.println("Host: iot.educa.madrid.org:8081");
    client.println("Connection: Keep-Alive");
    client.println("User-Agent: Apache-HttpClient/UNAVAILABLE (java 1.4)");
    client.println();
  }
  client.stop();
  delay(2000);
}
/////////////////////////////////////
// ESCRIBIMOS EN LA PANTALLAS OLED //
/////////////////////////////////////
void escribir(String valor){
      oled.setCursor(0, 40);
      oled.print(valor);
      oled.display();
      oled.setCursor(0, 40);
      oled.print("       ");
      oled.display();
      delay(2000);
}
//////////////////////////////////////////////////
// ESCRIBIMOS INICIALMENTE EN LA PANTALLAS OLED //
//////////////////////////////////////////////////
void escribirIni(){
  oled.begin();     // Initialize the OLED
  oled.clear(PAGE); // Clear the display's internal memory
  oled.clear(ALL);  // Clear the library's display buffer
  oled.display();   // Display what's in the buffer (splashscreen)
  oled.setFontType(0);
  oled.setCursor(0, 0);  // Set the text cursor to the upper-left of the screen.
  oled.print("Configure"); // Print a const string
  oled.setCursor(0, 10);  // Set the text cursor to the upper-left of the screen.
  oled.print("NOMBRE DEL CENTRO EDUCATIVO"); // Print a const string
  oled.setCursor(0, 20);  // Set the text cursor to the upper-left of the screen.
  oled.print("BIENVENIDO"); // Print a const string
  oled.display();   // Display what's in the buffer (splashscreen)
}
