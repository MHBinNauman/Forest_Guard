# Forest Guard – IoT-Based Forest Monitoring and Early Warning System

Forest Guard is an Internet of Things (IoT)–based environmental monitoring and early warning prototype designed to support forest and disaster-management authorities. It focuses on early detection of **forest fire indicators**, **seismic activity (earthquake simulation)**, and **rising water levels** by collecting sensor data on an **ESP32** and visualizing it in real time on the **Blynk IoT Dashboard**.

The project is built as a **scalable, modular prototype** that can be expanded into a larger multi-node monitoring network.

---

## Project Overview

Forest Guard monitors environmental conditions and provides early warning signals through real-time visualization and alerting. The system integrates multiple sensors with an ESP32 microcontroller to demonstrate how IoT can improve disaster preparedness and reduce environmental and human loss.

> Note: GPS functionality was evaluated using the ESP32 only, as outdoor deployment of a Raspberry Pi was not feasible during evaluation.

---

## Objectives

- Early detection of forest fires using gas and temperature sensing
- Simulation of earthquake detection using vibration sensing
- Monitoring water levels near dams and forest regions
- Real-time data visualization using the Blynk IoT Dashboard
- Demonstration of a single IoT node that can be integrated into a larger network

---

## System Architecture (High-Level)

- **ESP32** acts as the primary IoT node and sensor interface
- Sensor readings are transmitted over Wi-Fi to the **Blynk IoT Platform**
- Blynk dashboard provides:
  - live data visualization
  - threshold-based alerts
- Modular design supports future expansion to many nodes and alternative communication stacks

---

## Hardware Components

| Component | Purpose |
|---|---|
| ESP32 | Main IoT controller |
| MQ-2 Gas Sensor | Fire and smoke detection |
| DHT11 | Temperature and humidity monitoring |
| Vibration Sensor | Earthquake simulation |
| Ultrasonic (SONAR) Sensor | Water level measurement |
| GPS Module | Location tracking (outdoor use only) |
| Power Supply | System power |

---

## Software & Tools

- **Arduino IDE** – firmware development and flashing
- **Blynk IoT Platform** – cloud dashboard, visualization, alerting
- **EasyEDA** – PCB design (single-layer PCB)
- **Git & GitHub** – version control and collaboration

---

## Dashboard & Data Visualization

Sensor data is streamed to the Blynk dashboard in real time, where you can:

- Display live sensor values (gas/smoke, temperature/humidity, vibration, distance/water level, GPS)
- Configure alerts based on thresholds
- Extend the dashboard to support multiple nodes in future

Screenshots of the hardware setup and Blynk dashboard are included in the project documentation/report.

---

## Repository Structure

```text
Forest_Guard/
│
├── ESP32_Project/        # Microcontroller firmware
├── RPi_Project/          # Raspberry Pi firmware
└── Statics/              # Documentation, Video and System Level Diagram
```

---

## How to Run

1. **Clone the repository**
   ```bash
   git clone <repo-url>
   cd Forest_Guard
   ```

2. **Open the ESP32 firmware**
   - Launch **Arduino IDE**
   - Open the code inside `ESP32_Code/`

3. **Install required libraries**
   - Install the sensor libraries used in the firmware (as referenced in the code)
   - Install the **Blynk** library for ESP32 (Blynk IoT)

4. **Configure credentials**
   - Set your **Wi-Fi SSID/password**
   - Set your **Blynk Template ID / Device Name / Auth Token** (as required by Blynk IoT)

5. **Upload to ESP32**
   - Select your ESP32 board and COM port in Arduino IDE
   - Upload firmware

6. **Monitor on Blynk**
   - Open your Blynk dashboard
   - Confirm live values and alerts

---

## Scalability & Future Enhancements

Forest Guard is designed as a scalable prototype and can be expanded to:

- Integrate **LoRaWAN** for long-range, low-power communication
- Deploy a large number of nodes across forest regions
- Incorporate **MQTT** for robust local communication and resilience during cloud outages
- Replace vibration sensors with real **seismometers** for accurate earthquake detection
- Deploy SONAR-based water level monitoring near dams nationwide

Such scalability can support a nationwide early warning system (e.g., across Pakistan) for enhanced disaster monitoring and response.

---

## Team

**Muhammad Hisham Bin Nauman**  
**Muhammad Shamveel Athar** 
**Muhammad Hashim** 
Department of Computer Engineering  
NUST EME

---

## License

Developed for academic and educational purposes.  
All rights reserved © 2026.
