#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#define DHT_SENSOR_PIN 4
#define DHT_SENSOR_TYPE DHT11

// https://console.firebase.google.com/u/0/

// *** Preencher com a identificação do município ***
#define COD_MUN 1
// **************************************************

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Configuração do Wifi:
#define WIFI_SSID "PREENCHER COM O NOME DO WIFI"
#define WIFI_PASSWORD "PREENCHER COM A SENHA"

// Configuração do Firestore Database 
#define API_KEY "*********** preencher *******************"
#define DATABASE_URL "*********** preencher *******************" 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false; 

void setup(){

  dht_sensor.begin();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando ao WIFI");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado com IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){

  
  float temperatura = dht_sensor.readtemperatura();
  float umidade = dht_sensor.readumidade();
  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    
    sendDataPrevMillis = millis();
    
    if (Firebase.RTDB.setInt(&fbdo, COD_MUN + "/temp", temperatura)){
      Serial.print("Temperatura : ");
      Serial.println(temperatura);
    }
    else {
      Serial.println("Erro ao ler o sensor");
      Serial.println("Código: " + fbdo.errorReason());
    }

   
    if (Firebase.RTDB.setFloat(&fbdo, COD_MUN + "/umidade", umidade)){
      Serial.print("Umidade : ");
      Serial.print(umidade);
    }
    else {
      Serial.println("Erro ao ler o sensor");
      Serial.println("Código: " + fbdo.errorReason());
    }
  }
}