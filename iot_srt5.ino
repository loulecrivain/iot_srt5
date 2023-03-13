#include <WiFi.h>
#include <PubSubClient.h>
#include <cJSON.h>
#include <SPI.h>
#include <LoRa.h>

#define DI0     26
#define RST     14
#define MISO    19
#define MOSI    27
#define SS      18

const int MAXCHAR = 200;
char tmp[MAXCHAR];

const char *ssid="n100";
const char *password="ab4e3e3c3221";

const char *mqtt_server="test.mosquitto.org";
const char *mqtt_client_id="esp32mqtt-el";
const char *mqtt_topic="srt/lesbv";
const int  mqtt_reconnect_ms_delay=5000;

struct LoRaParams {
  int f;  // frequency
  int sf; // spreading factor
  int sb; // signal bandwidth
  int sw;  // syncword
} lora_params;

union pack
{
  uint8_t frame[16];
  float data[4];
} sdp;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(DI0,INPUT);
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
}

void setup_wifi(){
  delay(10);
  WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("[WiFi] awaiting connection...");
  }
  Serial.println("[WiFi] connected !");
}

void callback(char *topic, byte *message, unsigned int length){
  int i;
  struct LoRaParams *lora_params;

  Serial.print("[MQTT] message arrived on topic ");
  Serial.print(topic);
  Serial.println();

  for (i = 0; i < length && i <= sizeof(tmp); i++) {
    tmp[i] = (char)message[i];
  }

  tmp[i+1] += '\0';
  lora_params = parseMQTTJSON(tmp);
  setupLoRa(lora_params);
  sendMsgLoRa();
}

void sendMsgLoRa() {
  float d1=420,d2 = 64;
  Serial.println("[LoRa] Sending message");
  LoRa.beginPacket();
  sdp.data[0]=d1;
  sdp.data[1]=d2;
  LoRa.write(sdp.frame,16);
  LoRa.endPacket();
}

void setupLoRa(struct LoRaParams *lora_params) {
  if (!LoRa.begin(lora_params->f)){
    Serial.println("[LoRa] Start failed!");
    while(1);
  }
  LoRa.setSpreadingFactor(lora_params->sf);
  LoRa.setSignalBandwidth(lora_params->sb);
  sprintf(tmp,"[LoRa] started with f=%d, sf=%d, sb=%d\n", lora_params->f,
                                                          lora_params->sf,
                                                          lora_params->sb);
  Serial.println(tmp);
}

struct LoRaParams *parseMQTTJSON(char *message) {
  int freq, syncword, spreadingFactor, signalBandwidth;
  cJSON *json = cJSON_Parse(message);

  // print JSON
  Serial.println(cJSON_Print(json));
  // parse and convert freq from json
  if (cJSON_IsNumber(cJSON_GetObjectItem(json, "freq"))) {
    lora_params.f = cJSON_GetObjectItem(json, "freq")->valueint;
  }
  if (cJSON_IsNumber(cJSON_GetObjectItem(json, "syncword"))) {
    lora_params.sw = cJSON_GetObjectItem(json, "syncword")->valueint;
  }
  if (cJSON_IsNumber(cJSON_GetObjectItem(json, "spreadingFactor"))) {
    lora_params.sf = cJSON_GetObjectItem(json, "spreadingFactor")->valueint;
  }
  if (cJSON_IsNumber(cJSON_GetObjectItem(json, "signalBandwidth"))) {
    lora_params.sb = cJSON_GetObjectItem(json, "signalBandwidth")->valueint;
  }
  // do not forget to free
  cJSON_Delete(json);
  return &lora_params;
}

void mqtt_reconnect() {
  while (!client.connected()) {
    Serial.println("[MQTT] trying to open connection to server...");
    if (client.connect(mqtt_client_id)){
      Serial.println("[MQTT] connected to server !");
      // subscribe to topic
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("[MQTT] failed to connect to server, rc=");
      Serial.print(client.state());
      Serial.println(" trying again...");
      delay(mqtt_reconnect_ms_delay);
    }
  }
}

void loop() {
  if(!client.connected()){
    mqtt_reconnect();
  }
  client.loop();
}