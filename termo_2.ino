#include "DHTesp.h"
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
 
DHTesp dht;
SSD1306Wire  display(0x3c, D2, D1);

const char *mqtt_server = "192.168.1.44";
const int   mqtt_port = 1883;
const char *root_topic_publish = "weatherSensor";

const char *ssid = "MOVISTAR_7C0C";
const char *password = "XXXXXXXXXXXXXXXXXXXXXXXXX";

WiFiClient espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE  (100)
char msg[MSG_BUFFER_SIZE];

void reconnect();
void setup_wifi();
void setup_display();
 
void setup() {
    Serial.begin(115200);
    setup_display();
    dht.setup(D5, DHTesp::DHT22);
    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
}
 
void loop() {

    if ( !client.connected()) {
      reconnect();
    }

    if ( client.connected()) {    

      client.loop();
      
      float h = dht.getHumidity();
      float t = dht.getTemperature();
 
      snprintf(msg, MSG_BUFFER_SIZE, "{\"humedad\": %.2f , \"temperatura\": %.2f}", h, t);
      Serial.print(root_topic_publish);
      Serial.print("\t-->");
      Serial.print(msg);
      Serial.println("<--");
      client.publish(root_topic_publish, msg, true);

      display.clear();
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.setFont(ArialMT_Plain_24);
      snprintf(msg, MSG_BUFFER_SIZE, "%.2f ºC", t);
      display.drawString(64, 5, String(msg));
      snprintf(msg, MSG_BUFFER_SIZE, "%.2f %% ", h);
      display.drawString(64, 30, String(msg));
      display.display();
      
      delay(2000);
    }
}

void setup_wifi() {
  delay(10);

  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);
  
  WiFi.begin((char *)ssid, (char *)password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());  
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    String clientId = "FERNANCODER-";
    clientId += String(random(0xffff), HEX);
    if ( client.connect(clientId.c_str())) {
      Serial.print(clientId);
      Serial.println(" conectado!");
    } else {
      Serial.print("Error: ");
      Serial.print(client.state());
      Serial.println(" reintento en cinco segundos");
      delay(5000);
    }
  }
}

void setup_display() {
  display.init();
  display.clear();
  display.display();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setContrast(255);

  display.drawString(64, 10, String("Connecting WiFi..."));
  display.display();

}
