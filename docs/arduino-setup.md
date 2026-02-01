# Arduino Setup Guide

> Setup completed: 2026-01-31

## Installed Software

### Arduino CLI (v1.4.1)
Command-line tool for compiling and uploading Arduino sketches.

### Arduino IDE (v2.3.7)
Graphical IDE for writing and managing Arduino projects.
Location: `/Applications/Arduino IDE.app`

### ESP32 Board Support (v3.3.6)
Includes support for XIAO ESP32C6 and all other ESP32 variants.

## Quick Start - Arduino CLI

### Verify Your Board
When you connect your XIAO ESP32C6 via USB:
```bash
arduino-cli board list
```

### Create a New Sketch
```bash
arduino-cli sketch new MyProject
cd MyProject
```

### Compile for XIAO ESP32C6
```bash
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 MyProject
```

### Upload to Board
```bash
arduino-cli upload -p /dev/cu.usbmodem* --fqbn esp32:esp32:XIAO_ESP32C6 MyProject
```

### Install a Library
```bash
# Search for a library
arduino-cli lib search "library name"

# Install a library
arduino-cli lib install "library name"
```

### Common Libraries for ESP32C6
```bash
# For WiFi functionality
arduino-cli lib install WiFi

# For BLE
arduino-cli lib install "ESP32 BLE Arduino"

# For servo control (for motorized blinds)
arduino-cli lib install Servo
```

## Quick Start - Arduino IDE

1. Open Arduino IDE from Applications
2. Go to **Tools > Board > esp32 > XIAO_ESP32C6**
3. Connect your board via USB
4. Go to **Tools > Port** and select the appropriate port (usually `/dev/cu.usbmodem*`)
5. Write your sketch and click Upload

## XIAO ESP32C6 Board Settings

**Board:** XIAO_ESP32C6
**FQBN:** esp32:esp32:XIAO_ESP32C6

Default settings are usually fine, but you can customize:
- Upload Speed: 921600 (faster) or 115200 (more reliable)
- Flash Mode: QIO
- Flash Size: 4MB
- Partition Scheme: Default or choose based on your needs

## Entering Bootloader Mode

If the board is unresponsive:
1. Press and hold the **BOOT** button
2. Connect via USB (or press RESET while holding BOOT)
3. Release BOOT button
4. Now you can upload firmware

## Serial Monitor

### CLI
```bash
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=115200
```

### IDE
Click the Serial Monitor icon in the top-right corner, then select baud rate (usually 115200).

## Configuration Files

- Arduino CLI config: `~/Library/Arduino15/arduino-cli.yaml`
- Installed libraries: `~/Documents/Arduino/libraries/`
- Board packages: `~/Library/Arduino15/packages/`

## Next Steps

1. Connect your XIAO ESP32C6 via USB
2. Try the built-in Blink example
3. Refer to `docs/esp32-c6/getting-started.md` for ESP32C6-specific features
