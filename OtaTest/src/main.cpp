#include <PubSubClient.h>
#include <ArduinoOTA.h>

/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char *ssid = "Qualqueruma";
const char *password = "33379281";

bool led1State = 0;
const int led1Pin = 15;

bool led2State = 0;
const int led2Pin = 13;

const int button1 = 12;
byte lastButtonState = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>ESP WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - GPIO 2</h2>
      <p class="state">state: <span id="state1">%STATE%</span></p>
      <p><button id="button1" class="button">Toggle</button></p>

      <h2>Output - GPIO 2</h2>
      <p class="state">state: <span id="state2">%STATE%</span></p>
      <p><button id="button2" class="button">Toggle</button></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }

  function onMessage(event) {
    var state1, state2;
    //manda 1020 (todas desligadas)
    //manda 1120 (led 1 ligado led 2 desligado)
    //manda 1021 (led 1 desligado led 2 ligado)
    //manda 1121 (dois leds ligados)
    console.log(event.data[1], event.data[3]);

    if(event.data[1] == "1") {
      state1 = "ON"
          document.getElementById('button1').innerHTML = "DESLIGAR";
      } else {
          state1 = "OFF"
          document.getElementById('button1').innerHTML = "LIGAR";
      }
    
      if(event.data[3] == "1") {
          state2 = "ON"
          document.getElementById('button2').innerHTML = "DESLIGAR";
      } else {
          state2 = "OFF"
          document.getElementById('button2').innerHTML = "LIGAR";
      }

    document.getElementById('state2').innerHTML = state2;
    document.getElementById('state1').innerHTML = state1;
  }

  
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button1').addEventListener('click', toggle);
    document.getElementById('button2').addEventListener('click', toggle1);
  }
  function toggle(){
    websocket.send('l1');
  }

  function toggle1(){
    websocket.send('l2');
  }
</script>
</body>
</html>
)rawliteral";

void notifyClients()
{
  Serial.println(String("1" + String(led1State) + "2" + String(led2State)));
  ws.textAll(String("1" + String(led1State) + "2" + String(led2State)));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    if (strcmp((char *)data, "l1") == 0)
    {
      led1State = !led1State;
      Serial.println("led1 state: " + String(led1State));
      notifyClients();
    }

    if (strcmp((char *)data, "l2") == 0)
    {
      led2State = !led2State;
      Serial.println("led2 state: " + String(led2State));
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (led1State)
    {
      return "ON";
    }
    else
    {
      return "OFF";
    }
  }
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(led1Pin, OUTPUT);
  digitalWrite(led1Pin, LOW);

  pinMode(led2Pin, OUTPUT);
  digitalWrite(led2Pin, LOW);

  pinMode(button1, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  ArduinoOTA.setPassword("teste");

  ArduinoOTA.onStart([]()
                     {
                       String type;
                       if (ArduinoOTA.getCommand() == U_FLASH)
                       {
                         type = "sketch";
                       }
                       else
                       { // U_FS
                         type = "filesystem";
                       }

                       // NOTE: if updating FS this would be the place to unmount FS using FS.end()
                       Serial.println("Start updating " + type);
                     });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
                       Serial.printf("Error[%u]: ", error);
                       if (error == OTA_AUTH_ERROR)
                       {
                         Serial.println("Auth Failed");
                       }
                       else if (error == OTA_BEGIN_ERROR)
                       {
                         Serial.println("Begin Failed");
                       }
                       else if (error == OTA_CONNECT_ERROR)
                       {
                         Serial.println("Connect Failed");
                       }
                       else if (error == OTA_RECEIVE_ERROR)
                       {
                         Serial.println("Receive Failed");
                       }
                       else if (error == OTA_END_ERROR)
                       {
                         Serial.println("End Failed");
                       }
                     });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html, processor); });

  // Start server
  server.begin();
}

void loop()
{
  ws.cleanupClients();
  digitalWrite(led1Pin, led1State);
  digitalWrite(led2Pin, led2State);

  byte buttonState = digitalRead(button1);
  if (buttonState != lastButtonState)
  {
    lastButtonState = buttonState;
    if (buttonState == LOW)
    {
      led1State = !led1State;
      Serial.println("led1 state: " + String(led1State));
      notifyClients();
    }
  }
  ArduinoOTA.handle();
}
