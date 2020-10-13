#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LCDIC2.h>

const char* ssid = "Qualqueruma";
const char* password = "33379281";
IPAddress mqtt_server(192, 168, 0, 100);

//luz
#define LUZ D7
#define STATUS_TIMER 100 //tempo de atualização do estado da lampada
bool status = false;
unsigned long lastExecute;


//wifi
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

#define LDR_PIN A0

int ldrValue;

String text = "hello world";

LCDIC2 lcd(0x27, 16, 2);

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

void callback(char* topic, byte* payload, unsigned int length) {
  String text = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    text += (char)payload[i];
  }

  if(strcmp(topic, "homie/Esp8266a/display/text/set") == 0){
    lcd.clear();
    lcd.print(text);
    client.publish("homie/Esp8266a/display/text", text.c_str(), true);
    Serial.println();
  }
  else if(strcmp(topic, "homie/Esp8266a/luz/estatus/set") == 0)
  {
    if (text == "true"){
      status = true;
      digitalWrite(LUZ, HIGH);
      Serial.println("Luz ligada");
    }
    else if(text == "false"){
      status = false;
      digitalWrite(LUZ, LOW);
    }
    else
    {
      Serial.print(topic);
      Serial.println(",  " + text);
    }
    
  }
  
}

void publishStatus(){
  if (status)
  client.publish("homie/Esp8266a/luz/estatus","true");
  else
  {
    client.publish("homie/Esp8266a/luz/estatus","false");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    lcd.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("homie/Esp8266a/$homie", "3.0", true);
      client.publish("homie/Esp8266a/$name", "Esp8266a", true);
      client.publish("homie/Esp8266a/$state", "init", true);
      client.publish("homie/Esp8266a/$nodes", "display,ldr,luz", true);
      client.publish("homie/Esp8266a/luz/$name", "luz", true);
      client.publish("homie/Esp8266a/luz/$properties", "estatus", true);
      client.publish("homie/Esp8266a/luz/estatus/$name", "estatus", true);
      client.publish("homie/Esp8266a/luz/estatus/$datatype", "boolean", true);
      client.publish("homie/Esp8266a/luz/estatus/$settable", "true", true);
      client.publish("homie/Esp8266a/display/$name", "display", true);
      client.publish("homie/Esp8266a/display/$properties", "text", true);
      client.publish("homie/Esp8266a/display/text", ((String)ldrValue).c_str(), true);
      client.publish("homie/Esp8266a/display/text/$name", "text", true);
      client.publish("homie/Esp8266a/display/text/$datatype", "string", true);
      client.publish("homie/Esp8266a/display/text/$settable", "true", true);
      client.publish("homie/Esp8266a/ldr/$name", "ldr", true);
      client.publish("homie/Esp8266a/ldr/$properties", "luminosidade", true);
      client.publish("homie/Esp8266a/ldr/luminosidade", text.c_str(), true);
      client.publish("homie/Esp8266a/ldr/luminosidade/$name", "luminosidade", true);
      client.publish("homie/Esp8266a/ldr/luminosidade/$datatype", "integer", true);
      client.publish("homie/Esp8266a/ldr/luminosidade/$settable", "false", true);
      client.publish("homie/Esp8266a/$state", "ready", true);
      client.publish("homie/Esp8266a/$stats/interval", "6", true);
      // ... and resubscribe
      client.subscribe("homie/Esp8266a/luz/estatus/set");
      client.subscribe("homie/Esp8266a/display/text/set");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void setup() {

  if (lcd.begin()) lcd.print(text);

  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(LUZ,OUTPUT);
  pinMode(LDR_PIN,INPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();

  if (now - lastMsg > 1000) {
  lastMsg = now;
  ldrValue = analogRead(LDR_PIN);
  Serial.print("LDR VALUE: ");
  Serial.println(((String)ldrValue).c_str());
  client.publish("homie/Esp8266a/ldr/luminosidade", ((String)ldrValue).c_str());
  client.publish("homie/Esp8266a/$state", "ready");
  }
  if ((millis()-lastExecute)>STATUS_TIMER) {
    lastExecute = millis();
    publishStatus();
  }
}

