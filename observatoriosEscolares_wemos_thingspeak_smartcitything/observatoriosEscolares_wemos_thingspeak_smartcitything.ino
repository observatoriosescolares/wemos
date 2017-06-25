//Se requieren las siguientes librerías

//ArduinoJson
//Adafruit_Sensors (Adafruit Unified Sensor)
//SSD1306 (Versión de Daniel Eichhorn; no la de Adafruit)

//FS (FileSystem)
//ESP8266WiFi
//DNSServer
//ESP8266WebServer

//Configure los siguientes parámetros
#define PORTAL_CAUTIVO_SSID     "Observatorios Escolares"
#define PORTAL_CAUTIVO_PSK      "cambie_esta_clave"
#define PORTAL_CAUTIVO_TIMEOUT  120  //Valor recomendado 120

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal

#include <FS.h> //this needs to be first, or it all crashes and burns...
//Usamos la versión de WiFiManager de kentaylor (https://github.com/kentaylor/WiFiManager) en lugar de la de tzapu (https://github.com/tzapu/WiFiManager)
//Desafortunadamente, la que se puede instalar desde Gestionar librerías es la de tzapu
//Así que tendremos que instalar la de kentaylor manualmente
#include "WiFiManager.h"
const char* CONFIG_FILE = "/config.json";

//Nuestro OLED tiene un tamaño de 64x48 y su driver es el SSD1306
//https://github.com/squix78/esp8266-oled-ssd1306
//Pero la librería que hemos elegido es para pantallas de 128x64
//Así que tendremos que introducir un offset de 32 en las abscisas
//y de ¿14? en las ordenadas
//Es posible que en el futuro (https://github.com/squix78/esp8266-oled-ssd1306/issues/73) haya que cambiar esto
//#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <SSD1306.h> // alias for `#include "SSD1306Wire.h"`
#define OLED_HORIZONTAL_OFFSET  32
#define OLED_VERTICAL_OFFSET  14
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

#include <Adafruit_Sensor.h>
#include "Adafruit_BME280.h"

#define SEALEVELPRESSURE_HPA (1013.25)

// Initialize the OLED display using Wire
// D2 -> SDA
// D1 -> SCL
SSD1306  oled(0x3c, D2, D1);

/*Para que te funcione la librería del BME tienes que modificar en el archivo
  \Arduino\libraries\Adafruit_BME280_Library\Adafruit_BME280.h la línea 32 para que quede así:
  #define BME280_ADDRESS                (0x76)
*/
Adafruit_BME280 bme; // I2C

WiFiClient client;

#include "logo_crif.h"

char ssid[32] ;
char password[32] ;
bool use_thingspeak = true;
bool use_smartcitything = true;
char thingspeak_ApiKey[17];
char smartcitything_proveedor[33] ;
char smartcitything_key[65];
const char* serverThingSpeak = "api.thingspeak.com";
const char* serverSmartCityThing = "www.smartcitything.es";
unsigned long ultimo_envio = 0;
float t, p, a, h;
String mensaje = "";

// 0-Reposo
// 1-Iniciar conexión WiFi
// 2-Esperando conexión WiFi
// 3-Conexión WiFi establecida
byte estado = 0;
byte intentos_conexion = 30;

// Function Prototypes
bool readConfigFile();
bool writeConfigFile();

void setup() {
  //clean FS, for testing
  //SPIFFS.format();

  Serial.begin(9600);

  oled.init();
  oled.flipScreenVertically();
  oled.setFont(ArialMT_Plain_10);
  oled.clear();
  oled.drawXbm(OLED_HORIZONTAL_OFFSET + 12, OLED_VERTICAL_OFFSET + 3, logo_crif_width, logo_crif_height, logo_crif_40x38);
  oled.drawString(OLED_HORIZONTAL_OFFSET + 20, OLED_VERTICAL_OFFSET + 40, "CRIF"); // Print a const string
  oled.display();
  delay(1000);

  oled.clear();
  oled.drawXbm(OLED_HORIZONTAL_OFFSET + 12, OLED_VERTICAL_OFFSET + 3, logo_crif_width, logo_crif_height, logo_crif_40x38);
  oled.drawString(OLED_HORIZONTAL_OFFSET + 0, OLED_VERTICAL_OFFSET + 40, "Las ACACIAS"); // Print a const string
  oled.display();
  delay(1000);

  oled.setTextAlignment(TEXT_ALIGN_LEFT);

  oled.clear();
  oled.drawString(OLED_HORIZONTAL_OFFSET, OLED_VERTICAL_OFFSET + 0, "Configure"); // Print a const string
  oled.drawString(OLED_HORIZONTAL_OFFSET, OLED_VERTICAL_OFFSET + 10, "WiFi at"); // Print a const string
  oled.drawString(OLED_HORIZONTAL_OFFSET, OLED_VERTICAL_OFFSET + 20, "192.168.4.1"); // Print a const string
  oled.drawString(OLED_HORIZONTAL_OFFSET, OLED_VERTICAL_OFFSET + 30, "before"); // Print a const string
  oled.drawString(OLED_HORIZONTAL_OFFSET, OLED_VERTICAL_OFFSET + 40, "2 minutes"); // Print a const string
  oled.display();

  // Mount the filesystem
  bool result = SPIFFS.begin();
  Serial.println("SPIFFS opened: " + result);

  if (!readConfigFile()) {
    Serial.println("Failed to read configuration file, using default values");
  }

  WiFi.printDiag(Serial); //Remove this line if you do not want to see WiFi password printed

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // // Format: <ID> <Placeholder text> <default value> <length> <custom HTML> <label placement>
  char customhtml[24] = "type=\"checkbox\"";
  if (use_thingspeak) {
    strcat(customhtml, " checked");
  }
  WiFiManagerParameter custom_use_thingspeak("Use_ThingSpeak", "Use ThingSpeak", "T", 2, customhtml, WFM_LABEL_AFTER);
  WiFiManagerParameter custom_thingspeak_apiKey("ThingSpeak", "ThingSpeak Write API Key", thingspeak_ApiKey, 17);//Parece que necesita un carácter más en la longitud 16-->17
  if (use_smartcitything) {
    strcat(customhtml, " checked");
  }
  WiFiManagerParameter custom_use_smartcitything("Use_SmartCityThing", "Use SmartCityThing", "T", 2, customhtml, WFM_LABEL_AFTER);
  WiFiManagerParameter custom_smartcitything_proveedor("SmartCityThing_Proveedor", "SmartCityThing Proveedor", smartcitything_proveedor, 33);
  WiFiManagerParameter custom_smartcitything_key("SmartCityThing_Key", "SmartCityThing Key", smartcitything_key, 65);


  WiFiManager wifiManager;

  //add all your parameters here
  wifiManager.addParameter(&custom_use_thingspeak);
  wifiManager.addParameter(&custom_thingspeak_apiKey);
  wifiManager.addParameter(&custom_use_smartcitything);
  wifiManager.addParameter(&custom_smartcitything_proveedor);
  wifiManager.addParameter(&custom_smartcitything_key);

  wifiManager.setConfigPortalTimeout(PORTAL_CAUTIVO_TIMEOUT);//Esperar un par de minutos en el portal cautivo
  //first parameter is name of access point, second is the password
  if (!wifiManager.startConfigPortal(PORTAL_CAUTIVO_SSID, PORTAL_CAUTIVO_PSK)) {
    Serial.println("failed to connect and hit timeout");

  } else {
    //read updated parameters
    //wifiManager.getSSID().toCharArray(ssid, 32);
    //wifiManager.getPassword().toCharArray(password, 32);
    WiFi.SSID().toCharArray(ssid, 32);
    WiFi.psk().toCharArray(password, 32);
    Serial.println(String(ssid) + "---" + String(password));
    use_thingspeak = (strncmp(custom_use_thingspeak.getValue(), "T", 1) == 0);
    strcpy(thingspeak_ApiKey, custom_thingspeak_apiKey.getValue());
    use_smartcitything = (strncmp(custom_use_smartcitything.getValue(), "T", 1) == 0);
    strcpy(smartcitything_proveedor, custom_smartcitything_proveedor.getValue());
    strcpy(smartcitything_key, custom_smartcitything_key.getValue());
    //save the custom parameters to FS
    writeConfigFile();
    // If you get here you have connected to the WiFi
    Serial.println("Connected...yeey :)");
  }




  WiFi.persistent(false);//Con persistent=true, el WEMOS trata de mantener la conexión WiFi abierta constantemente

  bme.begin();
  delay (1000);// Pequeña pausa para que el BME tenga tiempo de inicializarse
}

void loop() {
  actualizarLecturas();
  switch (estado) {
    case 0:
      actualizarLecturas();
      if (millis() > ultimo_envio + 30000 && (use_thingspeak || use_smartcitything)) {//Sólo envío datos a thingspeak cada 30 segundos
        estado = 1;
        mensaje = "CONNECT";
      }
      break;
    case 1:
      WiFi.mode(WIFI_OFF);   // this is a temporary line, to be removed after SDK update to 1.5.4
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);

      estado = 2;
      break;
    case 2:
      if (WiFi.status() != WL_CONNECTED) {
        intentos_conexion--;
        if (intentos_conexion == 0) {
          intentos_conexion = 30;
          estado = 0;
          ultimo_envio = millis();
          mensaje = "ERROR-1";
        }
      } else {
        intentos_conexion = 30;
        if (use_thingspeak) {
          enviar_a_thingspeak();
        }
        if (use_smartcitything) {
          enviar_a_smartcitything(String(t), "temperatura");
          enviar_a_smartcitything(String(p), "presion");
          enviar_a_smartcitything(String(h), "humedad");
        }
        WiFi.disconnect();
        estado = 0;
        ultimo_envio = millis();
      }
      break;
  }
  delay(1000);
}

void actualizarLecturas() {
  char cadena_redondeada[5];
  t = bme.readTemperature();// Temperatura
  p = bme.readPressure() / 100.0F;// Presión
  a = bme.readAltitude(SEALEVELPRESSURE_HPA);// Altura
  h = bme.readHumidity();// Humedad

  oled.clear();

  oled.drawString(OLED_HORIZONTAL_OFFSET, OLED_VERTICAL_OFFSET + 0, "T: "); // Print a const string
  dtostrf(t, 6, 1, cadena_redondeada);
  oled.drawString(OLED_HORIZONTAL_OFFSET + 18, OLED_VERTICAL_OFFSET + 0, cadena_redondeada);
  oled.drawString(OLED_HORIZONTAL_OFFSET + 18 + 30, OLED_VERTICAL_OFFSET + 0, " *C");

  oled.drawString(OLED_HORIZONTAL_OFFSET, OLED_VERTICAL_OFFSET + 10, "P: "); // Print a const string
  dtostrf(p, 5, 1, cadena_redondeada);
  oled.drawString(OLED_HORIZONTAL_OFFSET + 18, OLED_VERTICAL_OFFSET + 10, cadena_redondeada);
  oled.drawString(OLED_HORIZONTAL_OFFSET + 18 + 25, OLED_VERTICAL_OFFSET + 10, " hPa");

  oled.drawString(OLED_HORIZONTAL_OFFSET, OLED_VERTICAL_OFFSET + 20, "A: "); // Print a const string
  dtostrf(a, 6, 1, cadena_redondeada);
  oled.drawString(OLED_HORIZONTAL_OFFSET + 18, OLED_VERTICAL_OFFSET + 20, cadena_redondeada);
  oled.drawString(OLED_HORIZONTAL_OFFSET + 18 + 30, OLED_VERTICAL_OFFSET + 20, " m");

  oled.drawString(OLED_HORIZONTAL_OFFSET, OLED_VERTICAL_OFFSET + 30, "H: "); // Print a const string
  dtostrf(h, 6, 2, cadena_redondeada);
  oled.drawString(OLED_HORIZONTAL_OFFSET + 18, OLED_VERTICAL_OFFSET + 30, cadena_redondeada);
  oled.drawString(OLED_HORIZONTAL_OFFSET + 18 + 30, OLED_VERTICAL_OFFSET + 30, " %");

  oled.drawString(OLED_HORIZONTAL_OFFSET, OLED_VERTICAL_OFFSET + 40, mensaje);

  oled.display(); // Draw to the screen
}

bool readConfigFile() {
  // this opens the config file in read-mode
  File f = SPIFFS.open(CONFIG_FILE, "r");

  if (!f) {
    Serial.println("Configuration file not found");
    return false;
  } else {
    // we could open the file
    size_t size = f.size();
    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);

    // Read and store file contents in buf
    f.readBytes(buf.get(), size);
    // Closing file
    f.close();
    // Using dynamic JSON buffer which is not the recommended memory model, but anyway
    // See https://github.com/bblanchon/ArduinoJson/wiki/Memory%20model
    DynamicJsonBuffer jsonBuffer;
    // Parse JSON string
    JsonObject& json = jsonBuffer.parseObject(buf.get());
    // Test if parsing succeeds.
    if (!json.success()) {
      Serial.println("JSON parseObject() failed");
      return false;
    }
    json.printTo(Serial);

    // Parse all config file parameters, override
    // local config variables with parsed values

    if (json.containsKey("ssid")) {
      strcpy(ssid, json["ssid"]);
    }
    if (json.containsKey("password")) {
      strcpy(password, json["password"]);
    }
    if (json.containsKey("use_thingspeak")) {
      use_thingspeak = json["use_thingspeak"];
    }
    if (json.containsKey("thingspeak_ApiKey")) {
      strcpy(thingspeak_ApiKey, json["thingspeak_ApiKey"]);
    }
    if (json.containsKey("use_smartcitything")) {
      use_smartcitything = json["use_smartcitything"];
    }
    if (json.containsKey("smartcitything_proveedor")) {
      strcpy(smartcitything_proveedor, json["smartcitything_proveedor"]);
    }
    if (json.containsKey("smartcitything_key")) {
      strcpy(smartcitything_key, json["smartcitything_key"]);
    }
  }
  Serial.println("\nConfig file was successfully parsed");
  return true;
}

bool writeConfigFile() {
  Serial.println("Saving config file");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  // JSONify local configuration parameters
  json["ssid"] = ssid;
  json["password"] = password;
  json["use_thingspeak"] = use_thingspeak;
  json["thingspeak_ApiKey"] = thingspeak_ApiKey;
  json["use_smartcitything"] = use_smartcitything;
  json["smartcitything_proveedor"] = smartcitything_proveedor;
  json["smartcitything_key"] = smartcitything_key;


  // Open file for writing
  File f = SPIFFS.open(CONFIG_FILE, "w");
  if (!f) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.prettyPrintTo(Serial);
  // Write data to file and close it
  json.printTo(f);
  f.close();

  Serial.println("\nConfig file was successfully saved");
  return true;
}
void enviar_a_thingspeak() {
  if (client.connect(serverThingSpeak, 80)) {
    //String postStr = thingspeak_ApiKey;
    String postStr = "field1=";
    //postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(p);
    postStr += "&field3=";
    postStr += String(a);
    postStr += "&field4=";
    postStr += String(h);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: "+String(serverThingSpeak)+"\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + String(thingspeak_ApiKey) + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    client.stop();
    mensaje = "";
  } else {
    //No se ha podido conectar con el servidor de thinkspeak
    mensaje = "ERROR-2";
  }
}
void enviar_a_smartcitything(String valor, String parametro) {
  if (client.connect(serverSmartCityThing, 8081)) {
    client.print("PUT /data/");
    client.print(smartcitything_proveedor);
    client.println(+"/" + parametro + "/" + valor + " HTTP/1.1");
    client.print("IDENTITY_KEY: ");
    client.println(smartcitything_key);

    client.println("Content-Length: 0");
    client.println("Content-Type: text/plain; charset=ISO-8859-1");
    client.println("Host: www.smartcitything.es:8081");
    client.println("Connection: Keep-Alive");
    client.println("User-Agent: Apache-HttpClient/UNAVAILABLE (java 1.4)");
    client.println(); //este espacio es imprescindible
  } else {
    if (mensaje == "ERROR-2") {
      mensaje = "ERROR-4";
    } else {
      mensaje = "ERROR-3";
    }
  }
  client.stop();
}
