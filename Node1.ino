#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <DHT.h>

#define DHT_PIN 18
#define DHT_TYPE DHT11
#define WIFI_CHANNEL 1

DHT dht(DHT_PIN, DHT_TYPE);

// Receiver MAC: 58:2A:BD:76:85:C0
uint8_t receiverMAC[] = {
  0x58, 0x2A, 0xBD, 0x76, 0x85, 0xC0
};

struct SensorPacket {
  uint8_t nodeId;
  float temperature;
  float humidity;
  uint32_t sequence;
};

SensorPacket packet;
uint32_t sequenceNumber = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  if (esp_wifi_set_channel(
        WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE) != ESP_OK) {
    Serial.println("Channel setup failed!");
    while (true) delay(1000);
  }

  Serial.println();
  Serial.println("===== NODE 1 TRANSMITTER =====");
  Serial.print("Node MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed!");
    while (true) delay(1000);
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = WIFI_CHANNEL;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add receiver!");
    while (true) delay(1000);
  }

  Serial.println("Node 1 ready");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT11 reading failed!");
    delay(2000);
    return;
  }

  packet.nodeId = 1;
  packet.temperature = temperature;
  packet.humidity = humidity;
  packet.sequence = ++sequenceNumber;

  esp_err_t result = esp_now_send(
    receiverMAC,
    (uint8_t *)&packet,
    sizeof(packet)
  );

  Serial.printf(
    "Node 1 | Temp: %.1f C | Humidity: %.1f%% | ",
    temperature, humidity
  );

  if (result == ESP_OK) {
    Serial.println("Packet queued");
  } else {
    Serial.printf("Send error: %d\n", result);
  }

  delay(2000);
}
