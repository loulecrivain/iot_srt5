#include <WiFi.h>
#include <PubSubClient.h>
#include <cJSON.h>

const int MAXCHAR = 200;
char tmp[MAXCHAR];

const char *ssid="n100";
const char *password="ab4e3e3c3221";

const char *mqtt_server="test.mosquitto.org";
const char *mqtt_client_id="esp32mqtt-el";
const char *mqtt_topic="srt/lesbv";
const int  mqtt_reconnect_ms_delay=5000;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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

  Serial.print("[MQTT] message arrived on topic ");
  Serial.print(topic);
  Serial.println();

  for (i = 0; i < length && i <= sizeof(tmp); i++) {
    tmp[i] = (char)message[i];
  }

  tmp[i+1] += '\0';
  parseMQTTJSON(tmp);
}

void parseMQTTJSON(char *message) {
  int freq, syncword, spreadingFactor, signalBandwidth;
  cJSON *json = cJSON_Parse(message);

  // print JSON
  Serial.println(cJSON_Print(json));
  // parse and convert freq from json
  if (cJSON_IsNumber(cJSON_GetObjectItem(json, "freq"))) {
    freq = cJSON_GetObjectItem(json, "freq")->valueint;
  }
  if (cJSON_IsNumber(cJSON_GetObjectItem(json, "syncword"))) {
    syncword = cJSON_GetObjectItem(json, "syncword")->valueint;
  }
  if (cJSON_IsNumber(cJSON_GetObjectItem(json, "spreadingFactor"))) {
    spreadingFactor = cJSON_GetObjectItem(json, "spreadingFactor")->valueint;
  }
  if (cJSON_IsNumber(cJSON_GetObjectItem(json, "signalBandwidth"))) {
    signalBandwidth = cJSON_GetObjectItem(json, "signalBandwidth")->valueint;
  }
  // do not forget to free
  cJSON_Delete(json);
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