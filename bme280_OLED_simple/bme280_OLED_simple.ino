////////////////////////////////////////
// INCLUIMOS LAS LIBRERÍAS NECESARIAS //
////////////////////////////////////////
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <Wire.h>  // Include Wire if you're using I2C
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define PIN_RESET 255  //
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D
#define SEALEVELPRESSURE_HPA (1013.25)
/////////////////////////////////
// DATOS DEL CENTRO EDUCATIVO  //
/////////////////////////////////
char ssid[32] = "CRIF-ACACIAS";
char password[32] = "vistaalegre";
String proveedor = "OE_Nuestra_Sra_Providencia";
String key_proveedor = "ca9ba1d256e44e3f78886bee7f129d216772585097a72d69577e474083546613";
const char* server = "www.smartcitything.es";

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
  oled.begin();     // Initialize the OLED
  oled.clear(PAGE); // Clear the display's internal memory
  oled.clear(ALL);  // Clear the library's display buffer
  oled.display();   // Display what's in the buffer (splashscreen)
  oled.setFontType(0);
  oled.setCursor(0, 0);  // Set the text cursor to the upper-left of the screen.
  oled.print("Configure"); // Print a const string
  oled.setCursor(0, 10);  // Set the text cursor to the upper-left of the screen.
  oled.print("CRIF LAS ACACIAS"); // Print a const string
  oled.setCursor(0, 20);  // Set the text cursor to the upper-left of the screen.
  oled.print("BIENVENIDO"); // Print a const string
  oled.display();   // Display what's in the buffer (splashscreen)
  bme.begin();
  delay(1000);
}

void loop()
{
  //MEDIMOS LOS PARÁMETROS METEOROLÓGICOS.
  float t = bme.readTemperature();// Temperatura
  float p = bme.readPressure() / 100.0F;// Presión
  float a = bme.readAltitude(SEALEVELPRESSURE_HPA);// Altura
  float h = bme.readHumidity();// Humedad
  //ESCRIBIMOS EN LA PANTALLA LED
  oled.setCursor(0, 0);  // Set the text cursor to the upper-left of the screen.
  oled.print("T: "); // Print a const string
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
  oled.display(); // Draw to the screen
  //CADA 30 SEGUNDOS CONEXIÓN A WIFI, CONEXIÓN AL SERVIDOR, ENVÍO Y DESCONEXIÓN
  if (millis() > ultimo_envio + 20000) {//Sólo envío datos a thingspeak cada 30 segundos
    while (WiFi.status() != WL_CONNECTED) {
      texto = "CONEC W";
      escribir(texto);
      //CONEXIÓN A LA RED WIFI
      WiFi.begin(ssid, password);//Nos conectamos a la red wifi definida.
      texto = "CONEC S";
      escribir(texto);
      delay(5000);
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
    client.println("Host: www.smartcitything.es:8081");
    client.println("Connection: Keep-Alive");
    client.println("User-Agent: Apache-HttpClient/UNAVAILABLE (java 1.4)");
    client.println(); //este espacio es imprescindible
  }
  client.stop();
}
/////////////////////////////////////
// ESCRIBIMOS EN LA PANTALLAS OLED //
/////////////////////////////////////
void escribir(String valor){
      oled.setCursor(0, 40);
      oled.print(valor);
      oled.display();   // Display what's in the buffer (splashscreen)
      oled.setCursor(0, 40);
      oled.print("       ");
      oled.display();   // Display what's in the buffer (splashscreen)
      delay(2000);
  }
