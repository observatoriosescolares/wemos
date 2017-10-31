 #include <ESP8266WiFi.h>

//SSID of your network
char ssid[] = "JAZZTEL_bPKq"; //SSID of your Wi-Fi router
char pass[] = "yqwhataebfum"; //Password of your Wi-Fi router

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
