/*
  ___ _          _   _      _              ___    _ _           _ _ _ 
 | _ \ |__ _ _ _| |_(_)_ _ | |_  __ _ ___ | __|__| (_)______ __| | | |
 |  _/ / _` | ' \  _| | ' \| ' \/ _` (_-< | _/ -_) | |_ / -_|_-<_|_|_|
 |_| |_\__,_|_||_\__|_|_||_|_||_\__,_/__/ |_|\___|_|_/__\___/__(_|_|_)

  Este projeto foi feito pra uma disciplina do IME-USP no 1o semestre
  de 2018. É um sistema de irrigação automático de plantinhas de casa
  que envia dados ao PubNub para monitoramento remoto.

  É um sensor de umidade ligado ao Arduíno por entrada analógica (A0)
  Que se liga a um relé que abre uma válvula solenóide ligada à água.
  
  Feito por Lourenço Fernandes Neto e Silva,
  Com agradecimentos muitíssimo especiais a Júlio Turolla, namorido :)
  Também a Antônio Deusany (DJ) e Marcelo Schmidtt, que conheci pela
  disciplina referida: MAC6929 - Introdução à Internet das Coisas.

  (Talvez a primeira, sem dúvida uma das primeiras vezes que um curso
  desse tema é dado no Brasil.)
  
*/
#include <ESP8266WiFi.h>
#include <SPI.h>
#define PubNub_BASE_CLIENT WiFiClient
#define PUBNUB_DEBUG
#include <PubNub.h>

const char* redeWifi = "lalalala";
const char* pass = "txurururururu";

const static char pubkey[] = "huhuhuhuhuhu";
const static char subkey[] = "galopeeeeeeeeeeeee";
const static char channel[] = "plantinhas-felizes";

int status = WL_IDLE_STATUS;

WiFiClient client;

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int valveOpener = 4;   // O que significa o pino D2 do Arduino

int sensorValue = 0;        // value read from the pot

void setup() {
  Serial.begin(9600);
  pinMode(valveOpener, OUTPUT);
  wifiConnection();
  PubNub.begin(pubkey, subkey);
  printLogo();
}

void openValve() {
  digitalWrite(valveOpener, HIGH);
  Serial.println("OPEN");
}

void closeValve() {
  digitalWrite(valveOpener, LOW);
  Serial.println("CLOSED");
}

void loop() {
  sensorValue = analogRead(analogInPin);

  Serial.print("sensor = ");
  Serial.println(sensorValue);

  if (sensorValue > 450) {
    Serial.println("valor alto demais!");
    return;
  }

  if (sensorValue < 0) {
    Serial.println("valor baixo demais!");
    return;
  }

  if (sensorValue > 100) {
    openValve();
    delay(200);
    closeValve();
  }

  if (WiFi.status() != WL_CONNECTED) {
    wifiConnection();
    sendMessage(sensorValue);
  } else {
    sendMessage(sensorValue);
  }

  delay(10000);
}

void sendMessage(int sensorValue) {
  WiFiClient *client;

  String message = String("\"" + String(sensorValue) + "\"");
  char msg[20];
  message.toCharArray(msg, 20);

  client = PubNub.publish(channel, msg);

  if (!client) {
    Serial.println("publishing error");
    delay(1000);
    return;
  }

  if (PubNub.get_last_http_status_code_class() != PubNub::http_scc_success) {
    Serial.print("Got HTTP status code error from PubNub, class: ");
    Serial.print(PubNub.get_last_http_status_code_class(), DEC);
  }

}

void wifiConnection() {
  Serial.print("Connecting to ");
  Serial.println(redeWifi);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(redeWifi, pass);

  int count = 0;
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    count++;
    
    if (count > 120) {
      break;
    }
  }
  
  printIp();
}

void printIp() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void printLogo() {
  Serial.println("  ___ _          _   _      _              ___    _ _           _ _ _ ");
  Serial.println(" | _ \ |__ _ _ _| |_(_)_ _ | |_  __ _ ___ | __|__| (_)______ __| | | |");
  Serial.println(" |  _/ / _` | ' \  _| | ' \| ' \/ _` (_-< | _/ -_) | |_ / -_|_-<_|_|_|");
  Serial.println(" |_| |_\__,_|_||_\__|_|_||_|_||_\__,_/__/ |_|\___|_|_/__\___/__(_|_|_)");
}
