#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#define RECV_PIN D8
/* //wifi
const char* ssid = "Qualqueruma";
const char* password = "33379281";
IPAddress mqtt_server(192, 168, 0, 100);
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

#define RECV_PIN D8

 //VARIÁVEL DO TIPO IRrecv
decode_results results; //VARIÁVEL QUE ARMAZENA OS RESULTADOS

//MÉTODO RESPONSÁVEL POR FAZER A DECODIFICAÇÃO DO SINAL IR RECEBIDO
//OS DADOS SÃO PASSADOS PARA A BIBLIOTECA IRREMOTE QUE FAZ TODO O
//TRATAMENTO E RETORNA AS INFORMAÇÕES DE ACORDO COM O PROTOCOLO RECONHECIDO
void dump(decode_results *results) {
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  }
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->panasonicAddress, HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  }
  else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  }
  else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  }
  else if (results->decode_type == WHYNTER) {
    Serial.print("Decoded Whynter: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i = 1; i < count; i++) {
    if (i & 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.write('-');
      Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
}

void setup(){
  Serial.begin(115200); //INICIALIZA A SERIAL
  irrecv.enableIRIn();  //INICIALIZA O RECEPTOR
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(1000);
  digitalWrite(BUILTIN_LED, LOW);
}

void loop() {
    //RETORNA NA SERIAL AS INFORMAÇÕES FINAIS SOBRE O COMANDO IR QUE FOI IDENTIFICADO
  if (irrecv.decode(&results)) {
    digitalWrite(BUILTIN_LED, HIGH);
    Serial.println(results.value, HEX);
    dump(&results);
    irrecv.resume(); //RECEBE O PRÓXIMO VALOR
  } */
//}

IRrecv irrecv(RECV_PIN);
decode_results results;
int i = 0;
void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop(){
  if (irrecv.decode(&results)){
        Serial.println(results.value, HEX);
        Serial.println(results.value, DEC);
        irrecv.resume();
        Serial.println();
  }
}