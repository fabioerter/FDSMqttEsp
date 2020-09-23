#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define motor D1


//wifi
const char* ssid = "Qualqueruma";
const char* password = "33379281";
IPAddress mqtt_server(192, 168, 0, 100);
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

//ultrasonic
#define INTERVALO_LEITURA 250 //(ms)
#define PIN_TRIGGER   4
#define PIN_ECHO      5

bool status = false;
#define STATUS_TIMER 50//tempo de publicacao do estado do motor em ms
unsigned long lastExecute;

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
  String temp;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    temp += (char)payload[i];
  }
  Serial.println();
  if (temp == "mon"){
    status = true;
     digitalWrite(motor, HIGH);
  }
  else if(temp == "moff")
  {
    status = false;
    digitalWrite(motor,LOW);
  }
  
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Create a random client ID
    String clientId = "Ultrasonic";
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("inMotor");
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

  pinMode(LED_BUILTIN,OUTPUT);

  pinMode(motor,OUTPUT);

  lastExecute = millis();
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

///Função para enviar o estado do motor ao topico
void publishStatus(){
  client.publish("MotorOut",((String)status).c_str());
  Serial.println("Estado enviado: " + (String)status);
}



void loop() {
  //if ((millis()-lastExecute)>STATUS_TIMER) publishStatus();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
