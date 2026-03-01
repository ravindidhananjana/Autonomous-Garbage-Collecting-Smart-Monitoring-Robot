

---

# ♻️ Autonomous Garbage Collecting & Smart Monitoring Robot

## 📌 Project Overview

The **Autonomous Garbage Collecting & Smart Monitoring Robot** is an IoT-enabled robotic system designed to automate waste collection and monitoring using embedded systems, wireless communication, and real-time cloud integration.

The robot combines:

* 🤖 Autonomous navigation
* 📲 Bluetooth-based target command control
* 🧱 Real-time obstacle detection
* 📡 Wi-Fi cloud monitoring via Firebase
* 🗑 Smart garbage level detection

This project demonstrates the integration of **robotics, IoT, embedded systems, and real-time monitoring architecture**.

---

## 🔧 Hardware Components

* ESP32 (NodeMCU) with built-in Wi-Fi & Bluetooth
* Ultrasonic Sensors (Obstacle Detection & Garbage Level Monitoring)
* IR Sensors (Line Following)
* Servo Motor (Lid Control)
* Motor Driver Module
* Hobby gear Motors
* Power Supply Unit

---

## 💻 Software & Technologies Used

* Arduino IDE
* ESP32 Bluetooth Serial Communication
* Firebase Realtime Database
* HTML, CSS, JavaScript (Web Dashboard)
* Serial Bluetooth Terminal

---

## 🚀 Key Features

### 🤖 Autonomous Navigation

* Robot moves autonomously toward selected locations.
* Uses motor driver module for movement control.
* Executes predefined path logic based on command.

---

### 📲 Bluetooth-Based Target Command System

* Uses ESP32’s built-in Bluetooth module.
* Receives target location commands wirelessly.
* Each command corresponds to a specific destination.
* After receiving the target command via ESP32 Bluetooth, the robot begins autonomous navigation using an IR sensor-based line-following system, while simultaneously performing real-time obstacle detection.

Example:

* Command 1 → Location A
* Command 2 → Location B
* Command 3 → Location C

This allows structured destination-based movement rather than random navigation.

---

### 🧱 Real-Time Obstacle Detection & Avoidance

* Ultrasonic sensors continuously monitor surroundings.
* If an obstacle is detected:

  * Robot stops immediately.
  * Executes avoidance logic.
  * Continues toward the target.
* Ensures safe and intelligent movement.

---


### 🖐 Touchless Lid Operation

* Servo-controlled lid mechanism.
* Opens automatically using hand detection.
* Promotes hygiene and reduces human contact.

---

### 📡 IoT-Based Real-Time Monitoring

* ESP32 connects to Wi-Fi.
* Sends live data to Firebase Realtime Database.
* Web dashboard displays:

  * Garbage Level (%)
  * Lid Status
  * Robot State
  * Target Location
  * Obstacle Distance

All data updates in real time.

---

## 🔄 Communication Architecture

This system uses a dual communication model:

### 🔹 Bluetooth Layer

* Short-range wireless control.
* Used for sending movement commands.
* Enables manual override and destination selection.

### 🔹 Wi-Fi Layer

* Sends sensor data to cloud.
* Enables real-time monitoring through web dashboard.
* Supports remote observation.

This hybrid architecture improves flexibility, scalability, and reliability.

---

## 🎯 Project Objectives

* Automate waste collection processes.
* Reduce human contact with waste.
* Integrate robotics with IoT cloud systems.
* Demonstrate embedded wireless communication.
* Develop real-time monitoring capabilities.

---

## 💡 Project Impact

This project showcases how robotics and IoT can be combined to create smarter and more hygienic waste management systems.

It demonstrates practical implementation of:

* Embedded systems design
* Multi-sensor integration
* Real-time obstacle avoidance
* Wireless communication protocols
* Cloud database integration
* Modular robotic architecture

---

## 🔮 Future Improvements

* Implement path planning algorithms (A* or SLAM concepts)
* Add camera module for computer vision-based waste detection
* Develop ML-based garbage level prediction
* Deploy dashboard using Firebase Hosting
* Develop mobile application interface


