# ESP32 AC Voltage Control Dimmer System & Module

An isolated AC phase-cut dimming solution built for the ESP32, featuring hardware zero-crossing detection for precise power control. 

This repository provides two complete hardware implementations:
1. **Full Smart Dimmer System:** Features an SH1106 OLED display, captive portal setup, glassmorphism web control UI, mDNS, non-volatile state storage, and OTA wireless updates.
2. **Standalone Plug-and-Play AC Dimmer Module:** A compact, breakout PCB module designed specifically for quick integration into external projects without the display or web overhead.

---

## 📸 Circuit Schematics & PCB Layouts

### Schematics
| Breadboard System Schematic |
| :---: |
| ![Breadboard Schematic](hardware/PCB%20Module/Screenshot%202026-05-31%20001912.png) |

### PCB Design & Layout
| PCB Module 3D Render | PCB Module Layout |
| :---: | :---: |
| ![PCB 3D Render](hardware/PCB%20Module/pcb-3d-render.jpeg) | ![PCB Layout](hardware/PCB%20Module/pcb-layout.jpeg) |

---

## 📁 Repository Structure

* `firmware/` – Complete ESP32 system firmware (WiFiManager, WebServer, U8g2 OLED, Preferences, OTA)
* `hardware/` – Schematics, layouts, renders, and manufacturing Gerber files for both the breadboard prototype and PCB breakout module
* `KiCAD/` – Source KiCad project files and footprints

---

## ⚡ Key Technical Features

* **Dual Hardware Options:** Use the full-featured OLED/Web system or deploy the standalone AC Dimmer module for bare-metal builds.
* **Production-Ready Module:** Includes ready-to-order Gerber ZIP files in `hardware/PCB Module/` for instant PCB manufacturing (JLCPCB, PCBWay, etc.).
* **Optocoupled Safety:** High-voltage AC mains logic is completely isolated from low-voltage DC microcontroller pins using `PC817` and `MOC3021` optoisolators.
* **Phase-Cut Power Control:** Precision zero-crossing interrupt detection driving hardware timer interrupts for smooth TRIAC firing.
* **Full-Featured System Build:**
  * **Captive Portal Provisioning:** `WiFiManager` handles dynamic network setup without hardcoded credentials.
  * **Web UI Dashboard:** Built-in dark-mode interface with live brightness sliders and presets.
  * **SH1106 SPI Display:** Live dimming percentage bar and RSSI signal indicator driven via `U8g2`.
  * **State Retention:** Saves brightness levels to non-volatile `Preferences` EEPROM memory across reboots.
  * **mDNS & Wireless OTA:** Accessible at `http://esp32-dimmer.local` and flashable over-the-air.

---

## 📌 Pinout & Connections

### Full System Configuration
| Function | ESP32 Pin | Details |
| :--- | :--- | :--- |
| **Zero-Crossing Input** | `GPIO 27` | External Interrupt Signal Source |
| **TRIAC Gate Output** | `GPIO 26` | Timer-Driven Gate Trigger Signal |
| **Status LED** | `GPIO 2` | Connection Indicator LED |
| **OLED SCK** | `GPIO 18` | SPI Clock |
| **OLED SDA / MOSI** | `GPIO 23` | SPI Data Line |
| **OLED DC** | `GPIO 5` | Data / Command Selection Pin |
| **OLED Reset** | `GPIO 4` | Hardware Reset Pin |

### Standalone Module Breakout Pins
Connect `VCC` (3.3V/5V), `GND`, `ZERO_CROSS` (`GPIO 27`), and `TRIAC_GATE` (`GPIO 26`) directly to any microcontroller pin with external interrupt support.

---

## 🚀 Roadmap

- [x] Breadboard prototype & zero-crossing firmware
- [x] Standalone optoisolated AC dimmer PCB breakout module
- [x] Full system build (OLED + Web UI + OTA + EEPROM)
- [ ] Dedicated C++ Arduino/ESP-IDF Driver Library release for the standalone module

---

## 🛠️ Quick Start Guide

1. Upload the firmware in `firmware/` using VS Code (PlatformIO) or Arduino IDE.
2. Connect to the `ESP32-DIMMER` AP (Password: `12345678`) to configure WiFi credentials.
3. Access `http://esp32-dimmer.local` or the assigned IP address in your browser to adjust dimmer levels.

---

## ⚠️ High Voltage Safety Warning

> **HIGH VOLTAGE HAZARD:** This project operates directly on mains AC voltage (110V–240V). Exposed wiring can cause severe shock, injury, or death. Always ensure AC mains power is completely disconnected before touching or servicing the hardware.

---

## 📄 License

Distributed under the MIT License.
