///////////////////////////////////////////////////////////////
//       ESTACIÓN METEOROLÓGICA OBSERVATORIOS ESCOLARES      //
//                                                           //
//       WEMOS D1 MINI PRO + WEMOS OLED SHIELD + BME280      //
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                        INSTRUCCIONES                                                                         //
// https://www.educa2.madrid.org/web/educamadrid/principal/files/fc1f3186-7eb4-4530-9969-5cd5945fcbcc/articulos/introduccion-proyecto-wemos.pdf?t=1510599450414 //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <Wire.h>
#include <SFE_MicroOLED.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define PIN_RESET 255
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D
#define SEALEVELPRESSURE_HPA (1013.25)

//////////////////////////////////////////////
// DATOS DE CONEXIÓN WIFI Y A LA PLATAFORMA //
//////////////////////////////////////////////
char ssid[32] = "SSID DE LA WIFI";
char password[32] = "CONTRASEÑA DE LA WIFI";
String proveedor = "IDENTIFICADOR DEL PROVEEDOR";
String key_proveedor = "KEY DEL PROVEEDOR";
const char* server = "iot.educa.madrid.org";

////////////////////////////////////////////////
// Definición de variables globales y objetos //
////////////////////////////////////////////////
String texto = " ";
boolean enviar_ahora = true;
unsigned long ultimo_envio;
unsigned long ultimo_intento;
byte intentos_conexion = 0;
String parametro;

WiFiClient client;
MicroOLED oled(PIN_RESET, DC_JUMPER);  // I2C Example
Adafruit_BME280 bme; // I2C

///////////////////////////
// COMIENZO DEL PROGRAMA //
///////////////////////////
void setup() {
  Serial.begin(9600);
  bme.begin();
  escribirIni();  //Escribe la pantalla de bienvenida en la OLED, se debe cambiar el centro educativo en la función escribirIni()
}

void loop(){
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
  // Si no se consigue; se reintenta inmediatamente (sin dejar transcurrir otros 10 minutos)
  if (enviar_ahora == true) {
    Serial.println("CONECTANDO A LA RED WIFI");
    texto = "WIFI ON";
    escribir(texto);
    // CONEXIÓN A LA RED WIFI. Hasta 3 intentos
    while (intentos_conexion < 3 && WiFi.status() != WL_CONNECTED) {
      intentos_conexion++;
      Serial.print("Intento: ");
      Serial.print(intentos_conexion);
      Serial.println("/3");
      WiFi.begin(ssid, password);// Nos conectamos a la red wifi definida.
      ultimo_intento = millis();
      while (millis() < ultimo_intento + 15000 && WiFi.status() != WL_CONNECTED) {
        //Esperamos hasta un máximo de 15 segundos en cada intento
        delay(500);//Es necesaria esta breve pausa para que el wemos no se bloquee
      }
    }
    intentos_conexion = 0;
    if (WiFi.status() == WL_CONNECTED) {
      texto = "WIFI OK";
      escribir(texto);
      Serial.println("Conectado correctamente a la red WiFi");
      Serial.print("Local IP: ");
      Serial.println(WiFi.localIP()); // Muestra por el monitor serie del IDE de arduino la IP de Wemos para comprobar si todo ha ido bien.
      String TemValor = String(t);
      parametro = "temperatura";
      if (enviar(TemValor, parametro) == true) {
        enviar_ahora = false;
      } else {
        enviar_ahora = true;
      }
      String PresionValor = String(p);
      parametro = "presion";
      if (enviar(PresionValor, parametro) == true) {
        enviar_ahora = enviar_ahora || false;
      } else {
        enviar_ahora = true;
      }
      String HumValor = String(h);
      parametro = "humedad";
      if (enviar(HumValor, parametro) == true) {
        enviar_ahora = enviar_ahora || false;
      } else {
        enviar_ahora = true;
      }
      WiFi.disconnect();
    } else {
      texto = "NO WIFI";
      escribir(texto);
      Serial.println("No se pudo conectar a la red WiFi.");
      Serial.print("Motivo: ");
      Serial.println(WiFi.status(), DEC);
      /* MOTIVOS:
        WL_NO_SHIELD = 255,
        WL_IDLE_STATUS = 0,
        WL_NO_SSID_AVAIL = 1
        WL_SCAN_COMPLETED = 2
        WL_CONNECTED = 3
        WL_CONNECT_FAILED = 4
        WL_CONNECTION_LOST = 5
        WL_DISCONNECTED = 6*/
    }
    if (enviar_ahora == false) { //Esto indica que todos los datos se han enviado correctamente
      ultimo_envio = millis();
      texto = "TX OK";
      escribir(texto);
    } else {
      texto = "TX ERROR";
      escribir(texto);
    }
  }
  if (millis() > ultimo_envio + 600000) {
    enviar_ahora = true;
  }
}


//////////////////////////////////////////////////////////////
// NOS CONECTAMOS AL SERVIDOR, ENVIAMOS Y NOS DESCONECTAMOS //
//////////////////////////////////////////////////////////////
boolean enviar(String valor, String parametro) {
  if (client.connect(server, 8081)) {
    texto = "SENDING";
    escribir(texto);
    Serial.println("Conectado correctamente al servidor para enviar el valor de " + parametro);
    client.println("PUT /data/" + proveedor + "/" + parametro + "/" + valor + " HTTP/1.1");
    client.println("IDENTITY_KEY: " + key_proveedor);
    client.println("Content-Length: 0");
    client.println("Content-Type: text/plain; charset=ISO-8859-1");
    client.println("Host: iot.educa.madrid.org:8081");
    client.println("Connection: Keep-Alive");
    client.println("User-Agent: Apache-HttpClient/UNAVAILABLE (java 1.4)");
    client.println();
    while (client.connected()) {
      if (client.available()) {
        String respuesta = client.readStringUntil('\n');
        if (respuesta.indexOf("200 OK") > 0) {
          client.stop();
          Serial.println("Enviado correctamente el dato de " + parametro + ": " + valor);
          return true;
        }
      }
    }
    client.stop();
    Serial.println("No se ha podido enviar correctamente el dato de " + parametro);
    return false;
  } else {
    Serial.println("No se pudo conectar al servidor");
    return false;
  }
}

/////////////////////////////////////
// ESCRIBIMOS EN LA PANTALLAS OLED //
/////////////////////////////////////
void escribir(String valor) {
  oled.setCursor(0, 40);
  oled.print(valor);
  oled.display();
  delay(2000);
  oled.setCursor(0, 40);
  oled.print("         ");
  oled.display();
  delay(2000);
}

//////////////////////////////////////////////////
// ESCRIBIMOS INICIALMENTE EN LA PANTALLAS OLED //
//////////////////////////////////////////////////
void escribirIni() {
  oled.begin();     // Initialize the OLED
  oled.clear(PAGE); // Clear the display's internal memory
  oled.display();   // Display what's in the buffer
  oled.clear(ALL);  // Clear the library's display buffer
  oled.display();   // Display what's in the buffer (splashscreen)
  oled.setFontType(0);
  oled.setCursor(0, 0);  // Set the text cursor to the upper-left of the screen.
  oled.print("OBSERVATORIOS"); // Print a const string
  oled.setCursor(0, 10);  // Set the text cursor to the upper-left of the screen.
  oled.print("ESCOLARES"); // Print a const string
  oled.setCursor(0, 20);  // Set the text cursor to the upper-left of the screen.
  oled.print("BIENVENIDO"); // Print a const string
  oled.display();   // Display what's in the buffer (splashscreen)
  delay(2000);
  oled.clear(PAGE); // Clear the display's internal memory
  oled.display();   // Display what's in the buffer (splash
}
