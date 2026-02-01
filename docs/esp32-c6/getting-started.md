# Seeed Studio XIAO ESP32C6 - Getting Started Guide

> Source: https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/
> Downloaded: 2026-01-31

## Product Overview

The XIAO ESP32C6 is a compact microcontroller featuring dual 32-bit RISC-V processors. The high-performance processor runs up to 160 MHz while the low-power variant operates at 20 MHz. The device includes 512KB SRAM and 4MB Flash memory.

## Key Specifications

| Specification | Details |
|---------------|---------|
| **Processor** | Dual RISC-V (160 MHz HP / 20 MHz LP) |
| **Memory** | 512KB SRAM, 4MB Flash |
| **Wireless** | Wi-Fi 6, Bluetooth 5.3, Zigbee, Thread |
| **Interfaces** | 1 UART, 1 I2C, 1 SPI, 11 GPIO, 7 ADC |
| **Dimensions** | 21 x 17.8 mm |
| **Power Input** | USB Type-C (5V) or Battery (4.2V) |
| **Sleep Mode** | Deep sleep at ~15 μA |

## Core Features

- Matter-native support for smart home interoperability
- "Enhanced Connectivity" with 2.4GHz Wi-Fi 6 and IEEE 802.15.4 protocol support
- Onboard antenna with up to 80m BLE/Wi-Fi range
- RF switch supporting external UFL antenna connection
- Built-in lithium battery charging management

## Getting Started Requirements

**Hardware:**
- XIAO ESP32C6 board
- Computer with USB connectivity
- USB Type-C data cable
- Pin headers (sold separately; requires soldering)

**Software:**
- Arduino IDE version 2.0.8 or later
- ESP32 board package (version 3.0.0+)
- Board manager URL: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`

## Hardware Setup

### BootLoader Mode Access
Press and hold the BOOT button, connect via USB, then release. This recovers unresponsive devices.

### Pin Configuration Example
To enable external antenna functionality:

```cpp
void setup() {
  pinMode(3, OUTPUT);   // RF Switch power
  digitalWrite(3, LOW);
  delay(100);
  pinMode(14, OUTPUT);  // RF Switch control
  digitalWrite(14, HIGH);
}
```

## Battery Voltage Monitoring

Monitor battery voltage using a 1:2 voltage divider connected to analog pin A0. The sample code takes 16 measurements, averages them, and compensates for the divider ratio to provide accurate voltage readings in volts.

## Power Management Modes

- **Modem-sleep:** ~30 mA
- **Light-sleep:** ~2.5 mA
- **Deep-sleep:** ~15 μA

## Deep Sleep Implementation

Two primary wake-up methods are demonstrated:
1. External trigger via GPIO with RTC memory persistence
2. Timer-based wake-up at configurable intervals

Both implementations maintain boot count data across sleep cycles using RTC memory attributes.

## Resources Available

- ESP32-C6 datasheet (PDF)
- KiCAD schematic libraries
- Pinout reference spreadsheet
- 3D STEP model files
- "XIAO: Big Power, Small Board" eBook on Arduino and TinyML

## Notes for This Project

This board is being used in the motorized blinds project for its:
- Low power deep sleep capabilities (~15 μA)
- Built-in battery charging management
- Wi-Fi 6 and BLE 5.3 connectivity
- Compact form factor (21 x 17.8 mm)
