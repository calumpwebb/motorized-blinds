# PlatformIO commands for ESP32-C6

# Build and upload firmware to connected board
upload:
    pio run --target upload

# Build firmware without uploading
build:
    pio run

# Monitor serial output (115200 baud)
monitor:
    pio device monitor

# Upload and immediately start monitoring
upload-monitor:
    pio run --target upload && pio device monitor

# Clean build files
clean:
    pio run --target clean

# List available serial ports
ports:
    pio device list
