/*
 * ==============================NODEMCU MQTT NODE-RED================================
 * 
 * =========================SENSOR SUHU DHT 11 TELEGRAM BOT===========================
 * 
 * ==================================AANG RAPE'I======================================
 * 
 * ==================================27-01-2021=======================================
 * 
 * ============================= KONEKSI PIN DAN DHT11 ===============================
`* D7 --> OUT
 * 3v --> +
 * G  --> -
`* 
 */
#include <ArduinoJson.h> //Library JSON
#include <ESP8266WiFi.h> //Library  ESP8266
#include<PubSubClient.h> //Library MQTT
#include <dht.h> //Library DHT11
dht DHT;

#define DHTPIN D7

//Setup WIFI
const char*   ssid        = "xxx"; //Nama SSID Wifi yang akan diakses!
const char*   pass        = "XXXX"; //Password Wifi

//Setup MQTT broker
const char*   mqtt_server = "broker.hivemq.com"; //Server MQTT/Broker
const int     mqtt_port   = 1883; //MQTT Port
const char*   mqttuser    = ""; //MQTT Username 
const char*   mqttpass    = ""; //MQTT Password
String        clientId    = "ESP8266Client-"; //MQTT ClientID

boolean kirim = false;

WiFiClient espclient;
PubSubClient client(espclient);

//Fungsi Menyambungkan ke Wifi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

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

//======CALLBACK/PESAN MASUK MQTT=====
void callback(String topic, byte* payload, unsigned int length1) {
  Serial.println();
  Serial.print("Pesan Datang[");
  Serial.print(topic);
  Serial.println("]");
  String msgg;

//cek panjang pesan
  for (int i = 0; i < length1; i++){
    Serial.print((char)payload[i]);
    msgg += (char)payload[i];
  }
  
  if (topic == "ke_alat"){
    if (msgg == "cek") {  
     kirim = true;
    }
  }
  
}
//Fungsi Menyambungkan Ulang ke MQTT Broker
void reconnect() {
  while (!client.connected()) {
    clientId += String(random(0xffff), HEX);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(mqtt_server);
    if (client.connect(clientId.c_str(), mqttuser, mqttpass )) {
      Serial.println("connected");
      client.subscribe("ke_alat");
    } else {
      Serial.print("failed with state, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

//Fungsi ini dijalankan sekali ketika NodeMCU mulai start
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

//Fungsi ini dijalakan berulang-ulang selama NodeMCU menyala
void loop(){
   
 if (!client.connected()) { //menyambungkan kembali jika terputus dengan MQTT Broker
    reconnect();
 }
 
 if(!client.loop()){ //menyambungkan kembali menerima pesan jika terputus dengan MQTT Broker
    client.connect("AangESP8266Client",  mqttuser, mqttpass );
 }


  DHT.read11(DHTPIN);
  Serial.print("Kelembaban udara = ");
  Serial.print(DHT.humidity);
  Serial.print("% ");
  Serial.print("Suhu = ");
  Serial.print(DHT.temperature); 
  Serial.println(" C ");
  
DynamicJsonDocument doc(100);

doc["hum"] = DHT.humidity;
doc["temp"] = DHT.temperature;

char buffer[256];
size_t n = serializeJson(doc, buffer);
if(kirim == true){
client.publish("dari_alat", buffer, n); //Publish/kirim pesan ke MQTT broker
kirim = false;
}
delay(2000);

}
