# COEN 320 – Air Traffic Monitoring and Control (ATC)

## 📌 Overview
This project implements a **real-time Air Traffic Monitoring and Control (ATC) system** in **C++** using **QNX Momentics**.  
It simulates radar tracking, collision detection, operator commands, and communication between subsystems via **multithreading** and **client-server architecture**.  
The system ensures safe aircraft separation and maintains an updated view of the airspace.

---

## 🚀 Features
- **Real-time aircraft tracking** using radar input
- **Collision prediction & alerts** (within 3 minutes of violation)
- **Operator console** to:
  - Change speed, position, altitude
  - Request specific aircraft info
  - Adjust collision prediction time
- **Data display** refresh every 5 seconds
- **Thread-safe operations** using mutexes
- **Persistent logs** for aircraft status and operator commands

---

## 🛠️ System Architecture
The ATC system consists of six main subsystems:

1. **Aircraft** – Simulates each aircraft’s movement and responds to commands.
2. **Radar** – Primary and secondary radar track aircraft positions.
3. **Computer System** – Processes radar data, checks for violations, sends alerts.
4. **Data Display** – Shows real-time aircraft positions in a 2D grid.
5. **Operator Console** – Allows controller to send commands to aircraft.
6. **Communication System** – Relays commands from operator to aircraft.

> All subsystems communicate using **QNX message passing** and run in separate threads for real-time responsiveness.

---

## 💻 Implementation Details
- **Language:** C++
- **Platform:** QNX Momentics IDE
- **Key Concepts:**
  - Multithreading (`pthread`)
  - Client-server communication
  - Real-time timers
  - Mutex locks for concurrency control
- **Data Handling:** JSON-like structures for aircraft data
- **Logging:** Status logs every 30 seconds, operator command logs

---

