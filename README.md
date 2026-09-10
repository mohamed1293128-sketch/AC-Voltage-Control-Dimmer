# ESP32 Smart AC Phase-Cut Dimmer Module

An intelligent, isolated AC phase-cut dimming module controlled by an ESP32. Features hardware zero-crossing detection, dynamic TRIAC pulse triggering, a responsive web interface, an SH1106 OLED UI, local mDNS support, and OTA update capabilities.

---

## 📸 Media & Demonstration

| PCB Layout (KiCad) | 3D Render |
| :---: | :---: |
| ![PCB Layout]([docs/images/pcb-layout.jpeg](https://github.com/mohamed1293128-sketch/AC-Voltage-Control-Dimmer/blob/23477a91314eaefc83f5e0714745d45410fbef9e/hardware/PCB%20Module/pcb-layout.jpeg)) | ![3D Render](docs/images/pcb-3d-render.jpeg) |

### Demo

![Demonstration](docs/images/demo.gif)

---

## ✨ Features

- **Precise Phase-Cut Dimming** — Zero-crossing interrupt triggering with timer-driven hardware TRIAC pulse output.
- **Web UI Control** — Built-in glassmorphism dark-theme control panel, no app required.
- **Captive Portal Provisioning** — Uses `WiFiManager` for WiFi setup, no hardcoded credentials.
- **SH1106 SPI OLED Display** — Live dimming percentage bar, connection status, and RSSI signal meter.
- **Persistent Settings** — Dimming state survives power cycles via ESP32 `Preferences`.
- **mDNS Addressing** — Reachable at `http://esp32-dimmer.local` on your local network.
- **OTA Updates** — Flash new firmware wirelessly, no USB cable needed.

---

## 📌 Hardware Pinout

| ESP32 Pin | Function | Description |
| :---: | :---: | :---: |
| **GPIO 27** | `ZERO_CROSS_PIN` | Zero-crossing interrupt signal input |
| **GPIO 26** | `TRIAC_GATE_PIN` | TRIAC gate control output |
| **GPIO 2**  | `LED_PIN` | Status indicator LED |
| **GPIO 18** | OLED SCK | SPI clock |
| **GPIO 23** | OLED SDA/MOSI | SPI master output data |
| **GPIO 5**  | OLED DC | Data/command selection |
| **GPIO 4**  | OLED RST | Display reset |

---

## 📚 Libraries Required

Install these via Arduino IDE or PlatformIO:

- [`WiFiManager`](https://github.com/tzapu/WiFiManager)
- [`U8g2`](https://github.com/olikraus/u8g2) — for the SH1106 display
- `WebServer`, `Preferences`, `ArduinoOTA`, `ESPmDNS` — bundled with the ESP32 core

---

## 🛠️ Usage & Setup

1. **Flash the firmware** in `firmware/` to your ESP32.
2. **Configure WiFi** — on first boot, connect to the `ESP32-DIMMER` access point (password: `12345678`) and enter your local network credentials.
3. **Open the control panel** at `http://esp32-dimmer.local` (or the assigned IP) to start controlling brightness.

---

## ⚠️ High Voltage Safety Warning

> **DANGER:** This project interfaces directly with mains AC voltage (110–240V). Exposed contacts are dangerous. Fully disconnect power before touching any hardware components.

---

## 📄 License

Licensed under the MIT License — see [LICENSE](LICENSE) for details.
