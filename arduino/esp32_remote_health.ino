// esp32_remote_health.ino
#include <WiFi.h>
#include <PubSubClient.h>
#include <MAX30100_PulseOximeter.h> // install PulseOximeter library
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/////////////////////////
// CONFIG
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_WIFI_PASS";
const char* mqtt_server = "broker.hivemq.com"; // or your broker
const int mqtt_port = 1883;
const char* mqtt_topic = "remote-health/tanaya-device-01";

#define DHTPIN 4
#define DHTTYPE DHT11
#define ONE_WIRE_BUS 15

/////////////////////////
WiFiClient espClient;
PubSubClient client(espClient);

MAX30100_PulseOximeter pox;
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 10 * 1000; // 10s

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_HealthClient")) {
      // connected
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  // sensors
  dht.begin();
  sensors.begin();

  // MAX30100
  if (!pox.begin()) {
    // failed
  }  
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  if (millis() - lastSend > SEND_INTERVAL) {
    lastSend = millis();

    // read pulse/SpO2 (note: MAX30100 reading will be averaged)
    float heartRate = pox.getHeartRate();
    float SpO2 = pox.getSpO2();

    // read DHT11
    float tempAir = dht.readTemperature();
    float hum = dht.readHumidity();

    // read DS18B20
    sensors.requestTemperatures();
    float tempSkin = sensors.getTempCByIndex(0);

    // build JSON
    String payload = "{";
    payload += "\"device_id\":\"tanaya-device-01\",";
    payload += "\"timestamp\":" + String((unsigned long)(millis()/1000)) + ",";
    payload += "\"heart_rate\": " + String(heartRate,1) + ",";
    payload += "\"spo2\": " + String(SpO2,1) + ",";
    payload += "\"air_temp_c\": " + String(tempAir,1) + ",";
    payload += "\"air_humidity\": " + String(hum,1) + ",";
    payload += "\"skin_temp_c\": " + String(tempSkin,1);
    payload += "}";

    client.publish(mqtt_topic, payload.c_str());
    Serial.println("Published: " + payload);
  }
}
