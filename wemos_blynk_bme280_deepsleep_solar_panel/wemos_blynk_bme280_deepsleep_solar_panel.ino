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
float t, p, a, h, batt_level;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "############################";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "############################";
char pass[] = "############################";


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

  Serial.println("Me voy a echar una siestecita de 30 segundos");
  Blynk.run();
  delay(1000);
  ESP.deepSleep(10 * 60 * 1000000); //Me voy a echar una siesta de 10 minutos
  delay(300);
}

