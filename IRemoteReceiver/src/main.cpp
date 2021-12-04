#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>

#define PinIn D1
IRsend irsend(PinIn);

int tamanho = 68; //TAMANHO DA LINHA RAW(68 BLOCOS)
int frequencia = 38; //FREQUÊNCIA DO SINAL IR(32KHz)

unsigned int LD[68] = {4500, 4500, 600, 1600, 600, 1650,
 600, 500, 650, 500, 550, 550, 600, 500, 600, 1650, 600, 500,
  600, 1650, 600, 1650, 600, 500, 600, 500, 600, 1650, 600,
   500, 600, 1650, 600, 500, 600, 1650, 600, 500, 600, 550,
    550, 550, 600, 500, 600, 550, 600, 500, 600, 500, 600,
     550, 600, 1600, 600, 1650, 550, 1650, 600, 1650, 600,
      1650, 550, 1650, 650, 1600, 600};
void setup() {
  irsend.begin(); //INICIALIZA A FUNÇÃO
  Serial.begin(115200); //INICIALIZA A PORTA SERIAL
}

void loop() {

  irsend.sendRaw(LD,tamanho,frequencia);  // PARÂMETROS NECESSÁRIOS PARA ENVIO DO SINAL IR
  Serial.println("Comando enviado: Liga / Desliga");
  delay(1000);
}
/* void setup(){
 pinMode(PinIn, OUTPUT);
 pinMode(BUILTIN_LED, OUTPUT);
}
void loop(){
  digitalWrite(PinIn, HIGH);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(500);
  digitalWrite(PinIn, LOW);
  digitalWrite(BUILTIN_LED, LOW);
  delay(500);
} */