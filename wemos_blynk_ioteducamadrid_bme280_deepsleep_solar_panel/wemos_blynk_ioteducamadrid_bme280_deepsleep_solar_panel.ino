#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <BlynkSimpleEsp8266.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#define BLYNK_DEBUG
#define SEALEVELPRESSURE_HPA (1013.25)

/*Para que te funcione la librería del BME tienes que modificar en el archivo
  \Arduino\libraries\Adafruit_BME280_Library\Adafruit_BME280.h la línea 32 para que quede así:
  #define BME280_ADDRESS                (0x76)
*/
Adafruit_BME280 bme; //
WiFiClient client;
float t, p, a, h, batt_level;

String proveedor = "PROVEEDOR DE IOT_EDUCA_MADRID";
String key_proveedor = "CLAVE DEL PROVEEDOR IOT_EDUCA_MADRID";
const char* server = "iot.educa.madrid.org";

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "CLAVE DE BLYNK";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "NOMBRE DE LA RED WIFI";
char pass[] = "CLAVE DE LA RED WIFI";


// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.

void setup() {
  // Debug console
  Serial.begin(9600);
  //Leemos la tensión de la batería a través del pin de entrada analógico A0
  pinMode(A0, INPUT);

  bme.begin();

}

void loop()
{
  Blynk.run();
  analogRead(A0);
  delay(1000);
  batt_level = analogRead(A0) / 1023.0 * 4.2;
  Serial.println("");
  Serial.println("Fin de la siestecita... a trabajar");
  t = bme.readTemperature();// Temperatura
  p = bme.readPressure() / 100.0F;// Presión
  a = bme.readAltitude(SEALEVELPRESSURE_HPA);// Altura
  h = bme.readHumidity();// Humedad

  Serial.print("t=");
  Serial.println(t);
  Serial.print("p=");
  Serial.println(p);
  Serial.print("h=");
  Serial.println(h);
  Serial.print("a=");
  Serial.println(a);
  Serial.print("Battery level=");
  Serial.println(batt_level);
  Blynk.begin(auth, ssid, pass);
  while (Blynk.connect() == false) {
    //Esperamos hasta que la conexión con Blynk se establezca
    //Blynk establece por defecto un timeout de 30 segundos entre conexiones
  }

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, p);
  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V3, a);
  Blynk.virtualWrite(V4, batt_level);

  Blynk.run();

  while (WiFi.status() != WL_CONNECTED) {
    //CONEXIÓN A LA RED WIFI
    WiFi.begin(ssid, pass);//Nos conectamos a la red wifi definida.
    Serial.println("CONECTADO A LA WIFI ...");
    delay(5000);
    Serial.println(WiFi.localIP());
  }
  
  enviar_a_smartcitything(String(t), "temperatura");
  enviar_a_smartcitything(String(p), "presion");
  enviar_a_smartcitything(String(h), "humedad");


  Serial.println("Me voy a echar una siestecita de 10 minutos");

  delay(1000);
  ESP.deepSleep(10 * 60 * 1000000); //Me voy a echar una siesta de 10 minutos
  delay(300);
}

////////////////////////////////////////////////////////////////////////
// FUNCIÓN "enviar": CONECTAMOS AL SERVIDOR, ENVIAMOS Y DESCONECTAMOS //
////////////////////////////////////////////////////////////////////////
void enviar_a_smartcitything(String valor, String parametro) {
  if (client.connect(server, 8081)) {
    Serial.print("Conectado al servidor y enviando datos de:");
    Serial.println(parametro);
    client.println("PUT /data/" + proveedor + "/" + parametro + "/" + valor + " HTTP/1.1");
    client.println("IDENTITY_KEY: " + key_proveedor);
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

