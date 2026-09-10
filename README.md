# ESP32 AC Voltage Control Dimmer

An isolated AC phase-cut dimming module controlled by an ESP32 microcontroller. The system utilizes hardware zero-crossing detection for precise power control, features a modern web dashboard, an SPI OLED interface, EEPROM persistence, and Over-The-Air (OTA) updates.

---

## 📸 Circuit Schematics & Layouts

### Schematics
| PCB Schematic |
| :---: |
| ![Breadboard Schematic](hardware/PCBModule/pcb-layout.jpeg) | 

### Module Footprint & Prototype
| PCB Footprint| 
| :---: |
| ![PCB Footprint](hardware/PCB-Module/pcb-3rd-render.jpeg) |

---

## 📁 Repository Structure

* `firmware/` – ESP32 source code (WiFiManager, WebServer, U8g2 OLED, Preferences, OTA)
* `hardware/` – Circuit schematics, PCB footprints, and hardware layout assets
* `KiCAD/` – KiCad source design files and project data

---

## ⚡ Technical Features

* **Phase-Cut Power Control:** Zero-crossing interrupt triggering coupled with dynamic timer-driven TRIAC pulse driving.
* **Web UI Dashboard:** Built-in responsive web panel with live brightness controls.
* **Captive Portal Setup:** `WiFiManager` handles dynamic network pairing without hardcoding SSIDs or credentials.
* **SH1106 SPI Display:** Live dimming display with dynamic RSSI indicator driven via `U8g2`.
* **State Retention:** Preserves dimming settings across reboots using non-volatile `Preferences` storage.
* **mDNS & OTA Updates:** Easily reachable at `http://esp32-dimmer.local` and flashable wirelessly.

---

## 📌 Hardware Pin Configuration

| Function | ESP32 Pin | Details |
| :--- | :--- | :--- |
| **Zero-Crossing Input** | `GPIO 27` | External Interrupt Source |
| **TRIAC Gate Output** | `GPIO 26` | Timer-driven Gate Signal |
| **Status LED** | `GPIO 2` | Connection Indicator |
| **OLED SCK** | `GPIO 18` | SPI Clock |
| **OLED SDA / MOSI** | `GPIO 23` | SPI Data Line |
| **OLED DC** | `GPIO 5` | Data / Command Control |
| **OLED Reset** | `GPIO 4` | Hardware Reset |

---

## 🛠️ Getting Started

1. Flash the firmware provided in the `firmware/` directory using VS Code (PlatformIO) or Arduino IDE.
2. Connect to the `ESP32-DIMMER` AP (Password: `12345678`) on initial boot to set up your local WiFi network.
3. Access the control interface at `http://esp32-dimmer.local` or the assigned local IP address.

---

## ⚠️ Safety Warning

> **HIGH VOLTAGE HAZARD:** This project operates directly on mains AC voltage (110V–240V). Exposed wiring can cause severe shock or death. Ensure mains AC power is completely isolated before servicing hardware.

---

## 📄 License

Distributed under the MIT License.
