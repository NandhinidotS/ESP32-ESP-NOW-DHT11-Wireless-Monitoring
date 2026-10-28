# ESP32 ESP-NOW Wireless Temperature and Humidity Monitoring System

## 📌 Project Overview

This project implements a wireless temperature and humidity monitoring system using three ESP32 development boards and two DHT11 sensors. The system uses **ESP-NOW communication** to transmit sensor readings from two transmitter nodes to a central receiver.

The receiver displays the temperature and humidity readings of Node 1 and Node 2 on a **16×2 I2C LCD**, switching between the two nodes automatically.

## 🎥 Project Demo

Watch the complete working demonstration of the project.

[▶️ View Project Demo](https://drive.google.com/file/d/12UJMg3z_9Kx2PWHZpSHpcdk65CP7yPd8/view?usp=drivesdk)

## 🛠️ Hardware Requirements

- 3 × ESP32 development boards
- 2 × DHT11 temperature and humidity sensors
- 1 × 16×2 I2C LCD
- Jumper wires
- USB cables for programming and power supply

## 🏗️ System Architecture

```text
  Node 1 Transmitter                 Node 2 Transmitter
  ESP32 + DHT11                      ESP32 + DHT11
       |                                  |
       +---------- ESP-NOW ---------------+
                      |
                      v
              ESP32 Receiver
                      |
                      v
               16×2 I2C LCD
          Temperature & Humidity
```

## 🔌 Hardware Setup

### 1. Node 1 — Transmitter

Node 1 reads temperature and humidity from the DHT11 sensor and transmits the sensor data to the receiver using ESP-NOW.

**DHT11 Data Pin:** GPIO 18

![Node 1 Hardware Setup](Node1.jpeg)

### 2. Node 2 — Transmitter

Node 2 independently measures temperature and humidity using another DHT11 sensor and sends its readings to the same receiver.

**DHT11 Data Pin:** GPIO 5

![Node 2 Hardware Setup](Node2.jpeg)

### 3. Receiver — ESP32 with I2C LCD

The receiver collects sensor packets from both transmitter nodes and displays their readings on the LCD. The display switches between Node 1 and Node 2 every 3 seconds.

| LCD Connection | ESP32 Pin |
|---|---|
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| I2C Address | 0x27 |

![Receiver Hardware Setup](Receiver_setup.jpeg)

## 💻 Software and Libraries

- Arduino IDE
- ESP32 Arduino Core
- Wi-Fi and ESP-NOW libraries
- DHT sensor library
- Wire library
- LiquidCrystal_I2C library

## 📟 Serial Monitor Output

### Node 1 — Serial Monitor

Displays the temperature, humidity, and packet queue status for Node 1.

![Node 1 Serial Monitor](Node1_serial.jpeg)

### Node 2 — Serial Monitor

Displays the temperature, humidity, and packet queue status for Node 2.

![Node 2 Serial Monitor](Node2_serial.jpeg)

## ⚙️ Working Principle

1. Both ESP32 transmitter nodes read temperature and humidity from their respective DHT11 sensors.
2. Each transmitter creates a sensor packet containing the node ID, temperature, humidity, and sequence number.
3. The sensor packet is transmitted wirelessly using ESP-NOW on Wi-Fi channel 1.
4. The receiver identifies the source node using the node ID and updates the corresponding sensor values.
5. The 16×2 LCD displays the temperature and humidity readings for each node.
6. If a node's data has not been received for 8 seconds, the receiver displays its status as **OFFLINE**.

## ✨ Key Features

- Wireless communication using ESP-NOW
- Two independent DHT11 transmitter nodes
- Centralized ESP32 receiver
- Real-time temperature and humidity monitoring
- Automatic LCD screen switching between nodes
- Node-specific online/offline status
- Sequence number included in each sensor packet

## 📂 Project Structure

```text
ESP32-ESP-NOW-DHT11-Wireless-Monitoring/
├── Node1_Transmitter/
│   └── Node1_Transmitter.ino
├── Node2_Transmitter/
│   └── Node2_Transmitter.ino
├── Receiver/
│   └── Receiver.ino
├── images/
│   ├── node1-setup.jpg
│   ├── node2-setup.jpg
│   ├── receiver-setup.jpg
│   ├── node1-serial-monitor.png
│   └── node2-serial-monitor.png
├── media/
│   └── project-demo.mp4
└── README.md
```

## 🚀 Future Enhancements

- Add more sensor nodes.
- Implement packet delivery confirmation.
- Add data logging and remote monitoring.
- Integrate additional environmental sensors.

## 🧠 Skills Demonstrated

ESP32 • ESP-NOW • Arduino IDE • Embedded C++ • DHT11 Sensor Interfacing • I2C Communication • LCD Interfacing • Wireless Sensor Networks
