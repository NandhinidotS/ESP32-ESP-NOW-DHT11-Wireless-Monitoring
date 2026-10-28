#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>

#define WIFI_CHANNEL 1

// LCD I2C address: 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

struct SensorPacket {
  uint8_t nodeId;
  float temperature;
  float humidity;
  uint32_t sequence;
};

float n1Temp = 0, n1Hum = 0;
float n2Temp = 0, n2Hum = 0;

uint32_t n1LastRx = 0, n2LastRx = 0;
bool n1Received = false, n2Received = false;

portMUX_TYPE dataMux = portMUX_INITIALIZER_UNLOCKED;

#if ESP_ARDUINO_VERSION_MAJOR >= 3
void OnDataRecv(const esp_now_recv_info_t *info,
                const uint8_t *data, int len) {
#else
void OnDataRecv(const uint8_t *mac,
                const uint8_t *data, int len) {
#endif

  if (len != sizeof(SensorPacket)) {
    return;
  }

  SensorPacket packet;
  memcpy(&packet, data, sizeof(packet));

  if (packet.nodeId != 1 && packet.nodeId != 2) {
    return;
  }

  portENTER_CRITICAL(&dataMux);

  if (packet.nodeId == 1) {
    n1Temp = packet.temperature;
    n1Hum = packet.humidity;
    n1LastRx = millis();
    n1Received = true;
  } else {
    n2Temp = packet.temperature;
    n2Hum = packet.humidity;
    n2LastRx = millis();
    n2Received = true;
  }

  portEXIT_CRITICAL(&dataMux);
}

void printLine(uint8_t row, const char *text) {
  char line[17];
  snprintf(line, sizeof(line), "%-16.16s", text);
  lcd.setCursor(0, row);
  lcd.print(line);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Start LCD
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  printLine(0, "ESP-NOW Monitor");
  printLine(1, "Starting...");

  // Start Wi-Fi in station mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  delay(100);

  // Set the wireless channel
  esp_err_t chResult = esp_wifi_set_channel(
    WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE
  );

  // Read the actual station MAC address
  uint8_t receiverMac[6];

  esp_err_t macResult = esp_wifi_get_mac(
    WIFI_IF_STA, receiverMac
  );

  Serial.println();
  Serial.println("===== ESP-NOW RECEIVER =====");

  if (macResult == ESP_OK) {
    Serial.printf(
      "Receiver MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
      receiverMac[0], receiverMac[1],
      receiverMac[2], receiverMac[3],
      receiverMac[4], receiverMac[5]
    );
  } else {
    Serial.printf("MAC read failed: %d\n", macResult);
  }

  if (chResult == ESP_OK) {
    Serial.printf("Wi-Fi channel: %d\n", WIFI_CHANNEL);
  } else {
    Serial.printf("Channel setup failed: %d\n", chResult);
  }

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    printLine(0, "ESP-NOW ERROR");
    printLine(1, "Check Serial");
    while (true) {
      delay(1000);
    }
  }

  // Register the receive callback
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Receiver ready");
  Serial.println("Waiting for Node 1 and Node 2...");

  printLine(0, "Waiting for data");
  printLine(1, "Node 1 / Node 2");
}

void loop() {
  static uint32_t lastScreenChange = 0;
  static uint32_t lastLcdUpdate = 0;
  static uint8_t screen = 1;

  uint32_t now = millis();

  // Switch screens every 3 seconds
  if (now - lastScreenChange >= 3000) {
    lastScreenChange = now;
    screen = (screen == 1) ? 2 : 1;
  }

  if (now - lastLcdUpdate >= 250) {
    lastLcdUpdate = now;

    float temp, hum;
    uint32_t lastRx;
    bool received;

    // Copy the selected node's data safely
    portENTER_CRITICAL(&dataMux);

    if (screen == 1) {
      temp = n1Temp;
      hum = n1Hum;
      lastRx = n1LastRx;
      received = n1Received;
    } else {
      temp = n2Temp;
      hum = n2Hum;
      lastRx = n2LastRx;
      received = n2Received;
    }

    portEXIT_CRITICAL(&dataMux);

    char line[17];

    if (received && (uint32_t)(now - lastRx) < 8000) {
      snprintf(line, sizeof(line), "Node %d ONLINE", screen);
      printLine(0, line);

      snprintf(line, sizeof(line), "T:%.1fC H:%.1f%%",
               (double)temp, (double)hum);
      printLine(1, line);
    } else {
      snprintf(line, sizeof(line), "Node %d OFFLINE", screen);
      printLine(0, line);
      printLine(1, "Waiting data...");
    }
  }

  delay(10);
}
