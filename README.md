# 🏎️ Internet-Controlled RC Car

A comprehensive IoT project featuring an Internet-controlled RC car using an ESP32 microcontroller, Firebase Realtime Database, a Node.js backend, and a customized web-based control interface. 

## 📑 Table of Contents
- [About the Project](#about-the-project)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Components Required](#components-required)
- [Circuit Diagram & Hardware Setup](#circuit-diagram--hardware-setup)
- [Project Structure](#project-structure)
- [Setup and Installation](#setup-and-installation)
- [Controls](#controls)

## 📖 About the Project
This project demonstrates how to build a remote-controlled car that can be maneuvered from anywhere in the world over the internet. It leverages the ESP32's built-in Wi-Fi and utilizes Firebase Realtime Database (with MQTT fallback alternatives) to ensure low-latency communication between a custom web frontend and the hardware.

## ✨ Features
- **Global Control:** Control the car globally over the internet, no local network required.
- **Dual Backend Approaches:** Integrates both Firebase Realtime Database (via Frontend UI directly) and provides an MQTT-based Node.js backend infrastructure.
- **Web Interface Dashboard:** Responsive HTML/JS frontend with a dark/light mode dashboard, D-pad, and keyboard controls.
- **Real-Time Status Monitoring:** Live connection monitoring and target GPIO state tracking.
- **Emergency Stop:** Immediate halt functionality for safety (`E` key or UI button).
- **Audio Feedback:** Supports active and passive buzzers for hardware signaling and alerts.

## 🏗️ System Architecture
1. **Frontend Interface (`index.html`):** A responsive control dashboard that captures user inputs via clicks, touches, or keystrokes.
2. **Backend/Database:** 
   - **Firebase Realtime Database:** The client writes the target GPIO states directly as a JSON object (e.g., `{"14": 1, "27": 0, "26": 1, "25": 0}`).
   - **Node.js MQTT Server (`server.js`):** An Express.js backend alternative that publishes commands to an MQTT broker.
3. **Hardware (ESP32):** Connects to Wi-Fi, listens to database/MQTT changes, and outputs digital HIGH/LOW signals to the motor driver.
4. **Actuation (L298N):** Modulates the higher voltage from the batteries to drive the DC motors based on the ESP32's low-voltage logic signals.

## 🛠️ Components Required
- **ESP-32 Development Board** (Brain of the car)
- **L298N Motor Driver Module** (Controls motor direction/power)
- **2x DC Motors Generic** (With wheels)
- **RC Car Chassis** 
- **2x 18650 Li-ion Batteries (7.4V total)** with battery holder
- **Active Buzzer** & **Passive Buzzer**
- **Breadboard** & **Jumper wires**

### Hardware Assembly Photos
*(Place your car images in an `images` folder to display them here!)*
<p align="center">
  <img src="./images/car_1.jpg" alt="Car Assembly - Top View" width="45%" />
  <img src="./images/car_2.jpg" alt="Car Assembly - Side View" width="45%" />
</p>

## 🔌 Circuit Diagram & Hardware Setup

Here is the system circuitry mapped out logically:

1. **Power:** The 7.4V Battery connects directly to the `12V` and `GND` inputs on the L298N Motor Driver to provide adequate current to motors.
2. **Motors:** The left DC motor goes to `OUT1` and `OUT2`. The right DC motor goes to `OUT3` and `OUT4`.
3. **MCU Power:** The ESP32 is powered via the L298N's built-in `5V` output (or conversely, separate USB step-down).
4. **Logic Control:**
   - **Left Motor FWD:** ESP32 GPIO 14 ➔ L298N IN1
   - **Left Motor BWD:** ESP32 GPIO 27 ➔ L298N IN2
   - **Right Motor FWD:** ESP32 GPIO 26 ➔ L298N IN3
   - **Right Motor BWD:** ESP32 GPIO 25 ➔ L298N IN4
5. **Peripherals:** Buzzers are wired to ground and specific configurable digital output pins on the ESP32.

### System Circuit Diagram
![Circuit Diagram](./images/circuit_diagram.png)

## 📂 Project Structure
```text
internet-controlled-car/
├── backend/
│   ├── package.json    # Node.js dependencies (express, mqtt, body-parser)
│   └── server.js       # Express server & MQTT publisher config for API integration
└── frontend/
    └── index.html      # Main web controller (Firebase UI, Realtime DB hooks)
```

## 🚀 Setup and Installation

### 1. Hardware Assembly
- Assemble the chassis with the DC motors.
- Wire the motor driver logic pins (`IN1, IN2, IN3, IN4`) to the respective ESP32 GPIO pins (`14, 27, 26, 25`).
- Double-check the common ground connections between the ESP32 and the L298N module.

### 2. Frontend (Firebase) Setup
- Navigate to the `frontend/` directory and open `index.html` in your web browser.
- Head to the "Firebase Settings" card on the web dashboard.
- Input your **Firebase API Key** and **Database URL** (`firebaseio.com` link).
- Ensure your Firebase Realtime database has public read/write rules configured correctly for testing.

### 3. Backend (MQTT) Setup (Optional)
If you are developing custom third-party apps and prefer the Node.js/MQTT approach instead of Firebase:
```bash
cd backend
npm install
node server.js
```
- The backend API will run on `http://localhost:3000`. Set the MQTT broker URL in `server.js` to your preferred broker (Default: `broker.hivemq.com`).

### 4. ESP32 Code
- Upload the corresponding `.ino` C++ file with the Firebase/WiFi libraries to the ESP32.
- The ESP32 should act as a listener on the `"/gpio"` path in your Firebase Realtime DB, pulling values periodically to update its physical pins.

## 🎮 Controls
Simply open `index.html` in any web browser and use the on-screen buttons or keyboard shortcuts:
- **W / ↑ :** Move Forward
- **S / ↓ :** Move Backward
- **A / ← :** Turn Left
- **D / → :** Turn Right
- **Space :** Stop Vehicle
- **E :** Emergency Stop (Kills all motor pins)
