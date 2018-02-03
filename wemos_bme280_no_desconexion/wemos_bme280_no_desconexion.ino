///////////////////////////////////////////////////////////////
//       ESTACIÓN METEOROLÓGICA OBSERVATORIOS ESCOLARES      //
//                                                           //
//       WEMOS D1 MINI PRO + BME280                          //
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                               INSTRUCCIONES PARA EL USO DE LA WEMOS EN EL IDE DE ARDUINO                                                     //
// https://www.educa2.madrid.org/web/educamadrid/principal/files/fc1f3186-7eb4-4530-9969-5cd5945fcbcc/articulos/introduccion-proyecto-wemos.pdf?t=1510599450414 //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ejemplo simple de utilización de la Wemos y el sensor BME280 para enviar datos a Observatorios escolares
// En el monitor serie se muestra la conexión a la WiFi utilizada, la dirección IP adjudicada y los datos enviados al sevidor.
// En este caso se mantiene la conexión a la WiFi de forma permanente
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// INCLUSIÓN DE LIBRERÍAS Y CONSTANTES               //
///////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <Wire.h>
#include <SFE_MicroOLED.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define PIN_RESET 255
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D
#define SEALEVELPRESSURE_HPA (1013.25)

/////////////////////////////////
// DATOS DEL CENTRO EDUCATIVO  //
/////////////////////////////////
char ssid[32] = "NOMBRE DE LA WIFI";
char password[32] = "CONTRASEÑA DE LA WIFI";
String proveedor = "IDENTIFICADOR DEL PROVEEDOR";
String key_proveedor = "KEY DEL PROVEEDOR";
const char* server = "iot.educa.madrid.org";

////////////////////////////////////////////////
// DEFINICIÓN DE VARIABLES GLOBALES Y OBJETOS //
////////////////////////////////////////////////
String parametro;
WiFiClient client;
unsigned long ultimo_envio = 0;
Adafruit_BME280 bme; // I2C

///////////////////////////
// COMIENZO DEL PROGRAMA //
///////////////////////////
void setup() {
  Serial.begin(9600);
  bme.begin();
  delay(1000);
  conectarWifi();
}

void loop()
{
  //MEDIMOS LOS PARÁMETROS METEOROLÓGICOS.
  float t = bme.readTemperature();// Temperatura
  float p = bme.readPressure() / 100.0F;// Presión
  float a = bme.readAltitude(SEALEVELPRESSURE_HPA);// Altura
  float h = bme.readHumidity();// Humedad
  // Cada 10 minutos nos conectamos a la wifi y enviamos los datos al servidor
  // En la fase de pruebas podemos aumentar la frecuencia de envío a 30 segundos por ejemplo 30000
    if (millis() > ultimo_envio + 600000) {// Enviamos datos cada 10 minutos (600000 microsengundos).
    if (WiFi.status() != WL_CONNECTED){   
    conectarWifi();  
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
  }
}
////////////////////////////////////////////////////////////////////////
// FUNCIÓN "enviar": CONECTAMOS AL SERVIDOR, ENVIAMOS Y DESCONECTAMOS //
////////////////////////////////////////////////////////////////////////
void enviar(String valor, String parametro){
  if(client.connect(server, 8081)) {
    Serial.print("Conectado al servidor y enviando datos de:");
    Serial.println(parametro);
    client.println("PUT /data/"+proveedor+"/"+parametro+"/"+valor+" HTTP/1.1");
    client.println("IDENTITY_KEY: "+ key_proveedor);
    client.println("Content-Length: 0");
    client.println("Content-Type: text/plain; charset=ISO-8859-1");
    client.println("Host: iot.educa.madrid.org:8081");
    client.println("Connection: Keep-Alive");
    client.println("User-Agent: Apache-HttpClient/UNAVAILABLE (java 1.4)");
    client.println(); //este espacio es imprescindible
  }
  client.stop();
  Serial.println("Desconectado del servidor");
  delay(2000);
}
void conectarWifi(){
   while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      Serial.println("CONECTADO A LA WIFI ...");
      delay(5000);
      Serial.println(WiFi.localIP());
    }
  
  }
