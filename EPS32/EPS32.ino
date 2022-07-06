//autor:Vitor Pollefrone
//data:06/07/22  12:42

//----------------------- inclusão de blibliotecas --------------------------------
#include <WiFi.h> // inclui a bliblioteca do WIFI
#include<PubSubClient.h> // inclui a bliblioteca do MQTT
WiFiClient esp32; //cria o cliente wifi esp2866
PubSubClient MQTT(esp32); // cria o cliente MQTT utilizando o cliente WIFI para a publicação


#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

//------------------------- Definições Wfi ----------------------------------------------
char ssid [ ] = "teste"; //nome da rede wifi a conectar
char senha[ ] = "12345678"; //senha do wifi a conecta

//------------------------ definições MQTT ----------------------------------------------
const char* broker_id = "137.135.83.217"; // endereço do broker a conectar (mqtt.eclipseprojects.io 137.135.83.217)
int porta = 1883; //porta de comunicação com broker
#define client_id "esp_32_eniac_205192018" // id para indentificação no broker (DEVER SER ÚNICO)

//#define login_mqtt "login"
//#define senha_mqtt "senha"

//-----------------------------------------variaveis globais-----------------------------------
unsigned long tempo_passado = millis();
char* erro  = "NAO AUTORIZADO!!!";


int num_pessoas = 80;
String ID[50][2] = {
  {"84 A4 00 E0", "Pollefra"},
  {"54 20 CC DF", "Pollefra2"},
  {"6B 93 F2 B9", "Miguel"},
  {"6D 0E 56 1B", "Luzenho"},
  {"F3 FF 67 DF", "Will"},
  {"44 21 23 BB", "Vinicius"},
  {"9D 32 62 83", "Igor"},

  {"ultimo"}//Sempre deixar esse por ultimo!!!!!!!!!!!!!!!!!!!!!!!!!!!
};




//define alguns pinos do esp32 que serao conectados aos modulos e componentes

#define SS_PIN 5
#define RST_PIN 36
#define LED 2


MFRC522 mfrc522(SS_PIN, RST_PIN);   // define os pinos de controle do modulo de leitura de cartoes RFID

void setup() {
  Serial.begin(9600);

  pinMode(LED, OUTPUT);
  inicia_wifi();
  inicia_mqtt();

  SPI.begin();   // inicia a comunicacao SPI que sera usada para comunicacao com o mudulo RFID

  mfrc522.PCD_Init();  //inicia o modulo RFID

  Serial.println("RFID + ESP32");
  Serial.println("Aguardando tag RFID para verificar o id da mesma.");

}

void loop() {
  reconecta_wifi();//reconecta o wifi caso a conexao seja perdida
  reconecta_mqtt();//reconecta o mqtt caso a conexao seja perdida
  MQTT.loop();//mantem viva a conexao com o broker (keep alive)

  leitura_RFID();

}

void leitura_RFID() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;                 // se nao tiver um cartao para ser lido recomeça o void loop
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;                  //se nao conseguir ler o cartao recomeça o void loop tambem
  }

  Serial.print("UID da tag :");
  String conteudo = "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) {      // faz uma verificacao dos bits da memoria do cartao
    //ambos comandos abaixo vão concatenar as informacoes do cartao...
    //porem os 2 primeiros irao mostrar na serial e os 2 ultimos guardarao os valores na string de conteudo para fazer as verificacoes
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Mensagem : ");
  conteudo.toUpperCase();

  for (int i = 0; i < num_pessoas; i++)
  {
    if (ID[i][0] == conteudo.substring(1))
    {
      Serial.print("Liberado para ");
      Serial.println(ID[i][1]);
      char Buf[50];
      ID[i][1].toCharArray(Buf, 50);
      Serial.print(Buf);
      envia_dados(Buf);

      digitalWrite(LED, HIGH);
      delay(1000);
      digitalWrite(LED, LOW);
      break;

    }
    else if (ID[i][0] == "ultimo")
    {
      Serial.println("NÂO AUTORIZADO!!!");
      envia_dados(erro);
      delay(10000);
      break;
    }
  }

}

void inicia_wifi() {
  delay(10);
  Serial.println(); // pula linha no serial monitor
  Serial.println("-------------Iniciando conexao Wifi------------");
  Serial.print (" conectando-se na rede:"); Serial.println(ssid);
  Serial.print("  Aguarde");
  reconecta_wifi();
}
void reconecta_wifi() {
  if (WiFi.status() == WL_CONNECTED) // verifica se ja esta conectado
    return;// se tiver conectado não faz nada retorna

  while (WiFi.status() != WL_CONNECTED) { // enquanto estiver desconectado
    WiFi.begin(ssid, senha); // tenta fazer a conexao wifi
    Serial.println(".");
    delay(4000);
  }
  Serial.println();
  Serial.print(" Conectado com sucesso na rede: "); Serial.println( ssid);
  Serial.print(" IP obtido: "); Serial.println(WiFi.localIP()); //imprime o IP obtido pelo esp32
  //  Serial.print(" Endereço MAC: "); Serial.println(WiFi.macAddress()); //imprime o end MAC do esp32
}

void inicia_mqtt() {

  Serial.println("------------------ Iniciando conexao ao broker -------------------");
  Serial.println("");
  MQTT.setCallback(mqtt_callback);
  reconecta_mqtt();
}

void reconecta_mqtt() {

  while (!MQTT.connected()) {
    Serial.print("Tentando se conectar ao broker: "); Serial.println(broker_id);
    MQTT.setServer(broker_id, porta); // tenta conectar ao broker
    Serial.println("Aguarde");
    delay(1000);

    if (MQTT.connect(client_id)) { // se conectar ao broker
      Serial.println("conectado ao broker com sucesso");
      MQTT.subscribe("v3r_setor_senai");
    }
    else {
      Serial.print("falha de conexao");
      Serial.println(" havera nova tentativa de conexao em 2 segundos");
      MQTT.setServer(broker_id, porta); // tenta conectar ao broker
      MQTT.setCallback(mqtt_callback);
      delay(2000);
    }
  }
}

void envia_dados(char* a) {

  //if ((millis() - tempo_passado) >= 2000) //verifica se já passou 2 segundos
  //{
  Serial.println("---------------------------- Enviado Dados ------------------");
  Serial.print("dado enivado: "); Serial.println(a);
  MQTT.publish("v3r_nome_senai", a);

  //Reseta o tempo passado
  //tempo_passado = millis();
  // }
}

void mqtt_callback(char* topic, byte * payload, unsigned int length) {

  String msg;

  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i]; //pega caractere por caractere da informação vinda
    msg += c; //acumula os valores de c
  }
  Serial.print("mensagem do broker = "); Serial.println(msg);
}
