#include <ESP8266WiFi.h>
#include <Ultrasonic.h>
#include <PubSubClient.h>

//ultrasonic
Ultrasonic ultrasonic(D6, D5);
int distance;
unsigned long lastDistance;
int updateTime = 1000;//taxa de atualização da distancia em ms

//wifi
const char* ssid = "Qualqueruma";
const char* password = "33379281";
//IPAddress mqtt_server(192, 168, 0, 100);

IPAddress mqtt_server(192, 168, 43, 51);
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("homie/Esp8266c/$homie", "3.0", true);
      client.publish("homie/Esp8266c/$name", "Esp8266c", true);
      client.publish("homie/Esp8266c/$state", "init", true);
      client.publish("homie/Esp8266c/$nodes", "ultrasonic", true);
      client.publish("homie/Esp8266c/ultrasonic/$name", "ultrasonic", true);
      client.publish("homie/Esp8266c/ultrasonic/$properties", "distance", true);
      client.publish("homie/Esp8266c/ultrasonic/distance/$name", "distance", true);
      client.publish("homie/Esp8266c/ultrasonic/distance/$datatype", "integer", true);
      client.publish("homie/Esp8266c/ultrasonic/distance/$settable", "false", true);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void publishStatus(){
  distance = ultrasonic.read();
  Serial.println("distance = " + (String)distance + "cm");
  client.publish("homie/Esp8266c/ultrasonic/distance",((String)distance).c_str());
}
void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if ((millis()-lastDistance)>updateTime) {
    client.publish("homie/Esp8266c/$state", "ready", true);
    lastDistance = millis();
    publishStatus();
  }
}