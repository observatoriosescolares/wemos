#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME280.h"
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#define SEALEVELPRESSURE_HPA (1013.25)

/*Para que te funcione la librería del BME tienes que modificar en el archivo
  \Arduino\libraries\Adafruit_BME280_Library\Adafruit_BME280.h la línea 32 para que quede así:
  #define BME280_ADDRESS                (0x76)
*/
Adafruit_BME280 bme; //
float t, p, a, h;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "escriba_aqui_su_token";
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "escriba_aqui_el_ssid";
char pass[] = "escriba_aqui_la_clave";

BlynkTimer timer;

// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  t = bme.readTemperature();// Temperatura
  p = bme.readPressure() / 100.0F;// Presión
  a = bme.readAltitude(SEALEVELPRESSURE_HPA);// Altura
  h = bme.readHumidity();// Humedad
  Serial.println(t);
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, p);
  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V3, a);
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  bme.begin();
  Blynk.begin(auth, ssid, pass);

  // Setup a function to be called every second
  timer.setInterval(30000L, myTimerEvent);
}

void loop()
{

  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

