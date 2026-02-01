# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

Always use `todos` cli (run `todos tutorial` to learn more for creating project tasks, and persisting to git repo, if the user ask "add todo/task/whatever" then do that with `todos add` (run `todos tutorial` first!))

## Project Overview

Battery-powered motorized blinds controlled via ESP32-C6 with Matter/HomeKit integration. This is a hardware project currently in the prototyping phase with complete documentation and specifications.

## Hardware Platform

- **MCU:** Seeed XIAO ESP32-C6 (pre-soldered)
- **Motor:** BRINGSMART 5840-31ZY 12V 40RPM self-locking worm gear
- **Battery:** 3S2P Samsung 50E 21700 cells (11.1V, 10,000mAh)
- **Target runtime:** 2-6 months between charges

## Development Environment

**Arduino CLI:**

```bash
# Compile firmware
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 .

# Upload to board (find port with: ls /dev/cu.usb*)
arduino-cli upload --fqbn esp32:esp32:XIAO_ESP32C6 -p /dev/cu.usbmodem* .

# Monitor serial output
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=115200
```

**Arduino IDE 2.x:**

- Board: "XIAO_ESP32C6"
- USB CDC On Boot: Enabled (for serial debugging)

## GPIO Pin Assignments (ESP32-C6)

Critical mappings documented in PROJECT-OVERVIEW.md and schematic.md:

| GPIO | Function         | Component                 |
| ---- | ---------------- | ------------------------- |
| 1    | AIN1 (direction) | TB6612 motor driver       |
| 2    | AIN2 (direction) | TB6612 motor driver       |
| 3    | PWMA (speed)     | TB6612 motor driver       |
| 4    | I2C SDA          | AS5600 encoder            |
| 5    | I2C SCL          | AS5600 encoder            |
| 6    | I2C SDA          | INA219 current sensor     |
| 7    | I2C SCL          | INA219 current sensor     |
| 8    | Gate             | 2N7000 MOSFET (LED power) |
| 9    | Button Up        | Pull-up input             |
| 10   | Button Down      | Pull-up input             |

## Power Management

- Active (motor running): ~600mA @ 12V
- Deep sleep target: ~60µA
- Two I2C buses (encoder + current sensor on separate buses)
- WS2812B LED powered via MOSFET to achieve 0µA when off

## Key Design Decisions

1. **Simplified charging:** USB-C PD → Buck converter (12.6V) → BMS. No dedicated 3S charger IC. BMS handles overcharge protection.
2. **Self-locking motor:** Worm gear holds position with zero power consumption
3. **Dual buck converters:** One @ 12.6V (charging), one @ 5V (ESP32 power)
4. **Magnetic encoder:** AS5600 provides 0.023mm resolution over 380cm travel

## Documentation Structure

- `PROJECT-OVERVIEW.md` - Complete system specification, BOM, power budget
- `schematic.md` - Circuit diagrams (Mermaid format), detailed component specs
- `wiring-diagram.md` - Physical wiring instructions, module connections
- `docs/` - Reference documentation for ESP32-C6 and Arduino setup

## Firmware Features (To Be Implemented)

Per PROJECT-OVERVIEW.md, the firmware should include:

- Matter protocol support (native on ESP32-C6)
- Soft start/stop (PWM ramping)
- Position memory (flash storage)
- Auto-calibration via stall detection (using INA219)
- Obstruction detection
- Battery voltage reporting to Home app

## Hardware Assembly Notes

- **Critical:** Set buck converters BEFORE connecting (12.6V for charging, 5.0V for ESP32)
- **Safety:** Remove jewelry when working with battery pack - short circuit risk
- 3S2P battery configuration with balance taps to BMS (B+, B1, B2, B-)
- Encoder must use 3.3V (NOT 5V)
