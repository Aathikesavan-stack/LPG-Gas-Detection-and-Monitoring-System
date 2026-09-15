# 🔥 LPG Gas Detection & Automatic Booking System

An Arduino-based **LPG Gas Detection and Automatic Cylinder Booking System** designed to improve household LPG safety and monitor cylinder gas level.

The system continuously monitors **gas leakage** and **cylinder weight**. When a gas leak is detected, it activates a buzzer and relay and sends an SMS alert through a GSM module. When the cylinder gas level becomes low, the system automatically sends a low-level alert and an LPG booking request through SMS.

---

## 📌 Project Overview

LPG cylinders are widely used for cooking, but gas leakage and unexpected cylinder depletion can create safety and convenience issues.

This project combines:

* 🔥 LPG gas leakage detection
* ⚖️ Cylinder weight monitoring
* 📱 GSM-based SMS alerts
* 📲 Automatic LPG booking request
* 🚨 Buzzer-based leakage warning
* 🔌 Relay control
* 🖥️ 16×2 LCD monitoring

The system is implemented using an Arduino-compatible microcontroller with sensors and a GSM communication module.

---

## 🎯 Objectives

1. Detect LPG gas leakage at an early stage.
2. Continuously monitor the approximate gas cylinder weight.
3. Provide a local warning using a buzzer.
4. Control a relay during gas leakage detection.
5. Send an SMS alert when gas leakage is detected.
6. Detect when the cylinder gas level becomes low.
7. Automatically send an LPG booking request through GSM.
8. Display gas level and cylinder weight on a 16×2 LCD.

---

## 🧩 System Components

| Component            | Purpose                                 |
| -------------------- | --------------------------------------- |
| Arduino              | Main controller                         |
| LPG Gas Sensor       | Detects gas concentration/level         |
| Load Cell            | Measures cylinder weight                |
| HX711 Module         | Interfaces load cell with Arduino       |
| SIM800L / GSM Module | Sends SMS alerts and booking requests   |
| 16×2 LCD             | Displays weight and gas level           |
| Buzzer               | Provides audible leakage warning        |
| Relay Module         | Activated during gas leakage            |
| Mode Switch          | Hardware input configured in the system |
| Power Supply         | Provides power to the system            |

---

## 🔌 Pin Configuration

| Component     | Arduino Pin |
| ------------- | ----------- |
| LCD RS        | D13         |
| LCD EN        | D12         |
| LCD D4        | D11         |
| LCD D5        | D10         |
| LCD D6        | D9          |
| LCD D7        | D8          |
| Buzzer        | D7          |
| Relay         | D6          |
| GSM RX        | D2          |
| GSM TX        | D3          |
| Load Cell DT  | A0          |
| Load Cell SCK | A1          |
| Gas Sensor    | A2          |
| Mode Switch   | A3          |

> **Note:** GSM RX/TX labeling can depend on the module's perspective. The Arduino SoftwareSerial interface in the source code uses pins D2 and D3.

---

## ⚙️ Working Principle

The system operates continuously using two main monitoring parameters:

### 1. Gas Leakage Monitoring

The gas sensor is connected to analog pin **A2**.

The analog reading is converted into a simplified gas-level value:

```text
Gas Level = Analog Reading / 10
```

When:

```text
Gas Level ≥ 3
```

the system considers it a gas leakage condition.

The system then:

* Activates the buzzer.
* Activates the relay.
* Displays `ALRT` on the LCD.
* Sends an SMS warning through the GSM module.

When the gas level returns below the threshold, the buzzer and relay are turned OFF and the LCD displays `SAFE`.

---

### 2. Cylinder Weight Monitoring

A load cell is connected through the **HX711 ADC module**.

The measured weight is continuously read by the Arduino.

The calibration factor used in the program is:

```cpp
10000.0
```

If the measured weight becomes:

```text
Weight ≤ 2
```

the system considers the cylinder level to be low.

It then:

* Displays `LOW` on the LCD.
* Sends a low-gas-level SMS.
* Sends an LPG booking request SMS to the booking number.

When the weight becomes greater than 2, the LCD displays `OK`.

---

## 📱 GSM SMS Communication

The GSM module communicates with the Arduino using `SoftwareSerial`.

Communication speed:

```text
9600 baud
```

The system uses AT commands to send SMS messages.

The main commands used are:

```text
AT+CMGF=1
```

Sets the GSM module to SMS text mode.

```text
AT+CMGS="PHONE_NUMBER"
```

Specifies the SMS recipient.

The message is then transmitted and terminated using:

```text
Ctrl + Z
```

---

## 🚨 Alert Conditions

### Gas Leakage Alert

**Condition:**

```text
Gas Level ≥ 3
```

**Actions:**

```text
Gas Sensor
     ↓
Gas Level ≥ 3
     ↓
Gas Leakage Detected
     ↓
 ┌───────────────┐
 │ Buzzer ON     │
 │ Relay ON      │
 │ LCD → ALRT    │
 │ SMS Alert     │
 └───────────────┘
```

---

### Low Gas Level / Booking Alert

**Condition:**

```text
Cylinder Weight ≤ 2
```

**Actions:**

```text
Load Cell
    ↓
Weight ≤ 2
    ↓
Low Gas Level
    ↓
 ┌─────────────────────────┐
 │ LCD → LOW               │
 │ Low-Level SMS           │
 │ Booking Request SMS     │
 └─────────────────────────┘
```

---

## 🖥️ LCD Display

The 16×2 LCD displays the monitored values.

Example:

```text
W[g]:  5.25
Gas:   2
```

During abnormal conditions, status information is displayed.

Examples:

```text
ALRT
```

Gas leakage detected.

```text
SAFE
```

Gas level returned below the leakage threshold.

```text
LOW
```

Cylinder weight is at or below the low-level threshold.

```text
OK
```

Cylinder weight is above the low-level threshold.

---

## 🧠 Program Logic

The basic control flow of the program is:

```text
             START
               │
               ▼
       Initialize Arduino
               │
               ▼
     Initialize LCD / GSM
               │
               ▼
       Initialize HX711
               │
               ▼
        Read Sensors
          │         │
          ▼         ▼
     Gas Sensor   Load Cell
          │         │
          ▼         ▼
    Check Gas    Check Weight
          │         │
     ┌────┴────┐ ┌──┴──────┐
     │         │ │         │
   Leak      Safe Low      OK
     │         │ │         │
     ▼         ▼ ▼         ▼
 Buzzer ON   OFF SMS +   No Alert
 Relay ON        Booking
 SMS Alert
     │         │ │         │
     └─────────┴─┴─────────┘
               │
               ▼
          Repeat Loop
```

---

## 💻 Software & Libraries

### Development Environment

* Arduino IDE
* Embedded C / Arduino C++

### Libraries Used

```cpp
#include <HX711_ADC.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
```

### Main Functions

The program is organized into:

* `setup()`
* `loop()`
* `sendSMS()`
* `readSerial()`

---

## 📂 Project Structure

A recommended GitHub repository structure is:

```text
LPG-Gas-Detection-Automatic-Booking/
│
├── README.md
│
├── src/
│   └── LPG_Gas_Detection_Automatic_Booking.ino
│
├── images/
│   └── project.jpg
│
└── docs/
    └── LPG_Gas_Detection_Automatic_Booking_Report.pdf
```

---

## 📸 Project Images

### Hardware Prototype

Add your project photograph here:

```text
images/project.jpg
```

Then use:

```markdown
![LPG Gas Detection and Automatic Booking System](images/project.jpg)
```

---

## 📄 Project Report

The detailed project report is available in:

```text
docs/
└── LPG_Gas_Detection_Automatic_Booking_Report.pdf
```

---

## ⭐ Key Features

* ✅ LPG gas leakage detection
* ✅ Cylinder weight monitoring
* ✅ GSM SMS notification
* ✅ Automatic booking request
* ✅ Audible leakage warning
* ✅ Relay control
* ✅ LCD status display
* ✅ Sensor-based monitoring
* ✅ Arduino-based embedded system
* ✅ Simple and low-cost prototype

---

## ⚠️ Limitations

This project is an educational prototype and has some practical limitations:

* The gas sensor value is a simplified analog level and is **not a calibrated LPG concentration in ppm**.
* The low-cylinder threshold depends on the load-cell calibration and the value configured in the program.
* GSM communication depends on network availability and module power stability.
* The automatic booking is implemented as an **SMS booking request**; actual LPG distributor integration is not implemented.
* The relay output is controlled by the program but the exact external load connected to the relay depends on the hardware implementation.
* The mode-switch input is configured in the program but is not currently used for control logic.

---

## 🚀 Future Enhancements

Possible improvements include:

* 📡 IoT/cloud-based monitoring
* 📱 Mobile application
* 📊 Real-time weight monitoring
* 🔥 Calibrated gas concentration measurement
* 🗺️ GPS-based location reporting
* 🔐 Secure communication
* ☁️ Cloud data logging
* 🤖 Smart consumption prediction
* 🏠 Integration with a home safety system
* 🏢 Direct integration with LPG distributor APIs

---

## 👨‍💻 Author

**Aathikesavan B**

B.E. Electronics and Communication Engineering
Kangeyam Institute of Technology, Tiruppur

### Areas of Interest

* Embedded Systems
* Electronics
* IoT
* Robotics
* Microcontrollers
* Automation

---

## 📜 Disclaimer

This project was developed as an **academic/educational prototype** for demonstrating embedded systems, sensor monitoring, GSM communication, and automation concepts.

It should not be treated as a certified gas-safety system without proper sensor calibration, electrical protection, enclosure design, testing, and relevant safety certification.

---

## ⭐ If You Like This Project

If this project is useful or interesting, feel free to ⭐ **star the repository** and explore the source code.
