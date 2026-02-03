# Motorized Blinds - Wiring Guide

**Project:** ESP32-C6 Motorized Blinds Controller
**Date:** 2026-02-02
**Based on:** motorized-blinds.kicad_sch

---

## ⚠️ SAFETY FIRST
- **Remove all jewelry** before working with battery pack
- **Set buck converter voltages BEFORE connecting**: 12.6V and 5.0V
- **Use 3.3V for AS5600, NOT 5V** (critical!)
- **Double-check polarity** on all connections

---

## Module List

| Module | Description | Quantity |
|--------|-------------|----------|
| Battery Pack | 3S2P Samsung 50E (11.1V, 9800mAh) | 1 |
| BMS | ANMBEST 3S Battery Management System | 1 |
| Buck #1 | 12.6V Buck Converter (charging) | 1 |
| Buck #2 | 5V Buck Converter (ESP32 power) | 1 |
| ESP32-C6 | Seeed XIAO ESP32-C6 | 1 |
| TB6612 | Motor Driver Module | 1 |
| Motor | BRINGSMART 5840-31ZY 12V 40RPM | 1 |
| AS5600 | Magnetic Encoder (I2C) | 1 |
| INA219 | Current Sensor (I2C) | 1 |
| WS2812B | Addressable LED | 1 |
| 2N7000 | N-Channel MOSFET | 1 |
| Buttons | Tactile switches (x2) | 2 |

---

## 1. BATTERY TO BMS CONNECTIONS

**3S Battery Pack → BMS Input:**

| Battery Wire | BMS Input Pin | Voltage | Color (typical) |
|--------------|---------------|---------|-----------------|
| B+ (top)     | B+ or P+      | 12.6V   | Red             |
| B2 (balance) | B2            | ~8.4V   | Yellow/Orange   |
| B1 (balance) | B1            | ~4.2V   | Blue/Green      |
| B- (bottom)  | B- or GND     | 0V      | Black           |

**Notes:**
- BMS monitors each cell via balance taps
- Some BMS modules label these as P+, B2, B1, P-

---

## 2. BMS OUTPUT TO POWER DISTRIBUTION

**BMS Output → Power Rails:**

| BMS Output | Connects To | Wire Gauge | Notes |
|------------|-------------|------------|-------|
| P+ (VBAT)  | Buck #1 Input (+) <br> Buck #2 Input (+) | 18-20 AWG | Main power distribution |
| P- (GND)   | Buck #1 Input (-) <br> Buck #2 Input (-) <br> Common GND | 18-20 AWG | Ground distribution |

**Common GND connects to:**
- Both buck converters (-)
- ESP32-C6 GND
- TB6612 GND
- AS5600 GND
- INA219 GND
- WS2812B GND
- Button GND (via pull-ups)

---

## 3. BUCK CONVERTERS

### Buck Converter #1 (12.6V for Charging)

**Set output to 12.6V BEFORE connecting!**

| Buck #1 Pin | Connection | Notes |
|-------------|------------|-------|
| IN+         | VBAT (from BMS P+) | 11.1V nominal |
| IN-         | GND | Common ground |
| OUT+        | +12V6 rail | For USB-C PD charging (future) |
| OUT-        | GND | Common ground |

**Add capacitor:** 100µF electrolytic on output (C2)

### Buck Converter #2 (5V for ESP32)

**Set output to 5.0V BEFORE connecting!**

| Buck #2 Pin | Connection | Notes |
|-------------|------------|-------|
| IN+         | VBAT (from BMS P+) | 11.1V nominal |
| IN-         | GND | Common ground |
| OUT+        | +5V rail → ESP32-C6 5V pin | Powers ESP32 |
| OUT-        | GND | Common ground |

**Add capacitor:** 100µF electrolytic on output (C3)

---

## 4. ESP32-C6 (Seeed XIAO) CONNECTIONS

**Power:**
| ESP32 Pin | Connection | Notes |
|-----------|------------|-------|
| 5V        | Buck #2 output (+5V) | Main power input |
| 3V3       | → AS5600 VCC <br> → INA219 VCC | 3.3V output for sensors |
| GND       | Common GND | Multiple GND pins available |

**GPIO Assignments:**

| GPIO | Function | Connects To | Wire Color (suggest) | Notes |
|------|----------|-------------|----------------------|-------|
| GPIO1 | AIN1 (direction) | TB6612 Pin AIN1 | Orange | Motor direction control |
| GPIO2 | AIN2 (direction) | TB6612 Pin AIN2 | Yellow | Motor direction control |
| GPIO3 | PWMA (speed) | TB6612 Pin PWMA | Green | PWM motor speed |
| GPIO4 | I2C SDA (Bus #1) | AS5600 SDA | Blue | Encoder data |
| GPIO5 | I2C SCL (Bus #1) | AS5600 SCL | Purple | Encoder clock |
| GPIO6 | I2C SDA (Bus #2) | INA219 SDA | Brown | Current sensor data |
| GPIO7 | I2C SCL (Bus #2) | INA219 SCL | Gray | Current sensor clock |
| GPIO8 | LED Gate Control | 2N7000 MOSFET Gate | White | WS2812B power switch |
| GPIO9 | Button Up | SW1 (via pull-up) | Pink | User input |
| GPIO10 | Button Down | SW2 (via pull-up) | Cyan | User input |

---

## 5. TB6612 MOTOR DRIVER

**Power Connections:**

| TB6612 Pin | Connection | Notes |
|------------|------------|-------|
| VM         | VBAT (12V from BMS) | Motor power supply |
| VCC        | +5V (from Buck #2) | Logic power |
| GND        | Common GND | Ground |

**Control Inputs from ESP32:**

| TB6612 Pin | ESP32 GPIO | Function |
|------------|------------|----------|
| AIN1       | GPIO1 | Direction control bit 1 |
| AIN2       | GPIO2 | Direction control bit 2 |
| PWMA       | GPIO3 | Speed control (PWM) |
| STBY       | +5V | Standby (tie HIGH for active) |

**Motor Outputs:**

| TB6612 Pin | Motor Wire | Notes |
|------------|------------|-------|
| AO1        | Motor + | Polarity determines direction |
| AO2        | Motor - | Polarity determines direction |

**Add capacitor:** 0.1µF ceramic near VCC pin (C4)

---

## 6. AS5600 MAGNETIC ENCODER (I2C Bus #1)

**Critical: Use 3.3V, NOT 5V!**

| AS5600 Pin | Connection | Notes |
|------------|------------|-------|
| VCC (3-5V) | ESP32 3V3 output | **3.3V ONLY** |
| GND        | Common GND | Ground |
| SDA        | ESP32 GPIO4 | I2C data line |
| SCL        | ESP32 GPIO5 | I2C clock line |

**I2C Pull-up Resistors (required):**
- **R1:** 4.7kΩ between SDA and +3.3V
- **R2:** 4.7kΩ between SCL and +3.3V

**Add capacitor:** 0.1µF ceramic near VCC pin (C5)

**Magnet placement:** Diametrically magnetized magnet centered above sensor, 0.5-3mm gap

---

## 7. INA219 CURRENT SENSOR (I2C Bus #2)

**In-line with motor power to measure current:**

| INA219 Pin | Connection | Notes |
|------------|------------|-------|
| VCC        | ESP32 3V3 output | Sensor logic power |
| GND        | Common GND | Ground |
| SDA        | ESP32 GPIO6 | I2C data line |
| SCL        | ESP32 GPIO7 | I2C clock line |
| VIN+       | VBAT (before motor driver) | High-side measurement |
| VIN-       | TB6612 VM input | Measures motor current |

**I2C Pull-up Resistors (required):**
- **R3:** 4.7kΩ between SDA and +3.3V
- **R4:** 4.7kΩ between SCL and +3.3V

**Add capacitor:** 0.1µF ceramic near VCC pin (C6)

---

## 8. WS2812B LED + MOSFET POWER SWITCH

**Purpose:** MOSFET switches LED power to achieve 0µA when off

**WS2812B LED Connections:**

| WS2812B Pin | Connection | Notes |
|-------------|------------|-------|
| VCC (5V)    | 2N7000 Drain | Switched power |
| GND         | Common GND | Ground |
| DIN         | ESP32 GPIO8 | Data signal (through MOSFET gate) |

**2N7000 MOSFET Wiring:**

| MOSFET Pin | Connection | Function |
|------------|------------|----------|
| Gate (G)   | ESP32 GPIO8 | Control signal |
| Drain (D)  | +5V from Buck #2 | Switched power source |
| Source (S) | WS2812B VCC | Power to LED |

**Pull-down resistor:**
- **R5:** 10kΩ between Gate and GND (prevents floating)

**Notes:**
- When GPIO8 is HIGH, MOSFET turns ON, LED gets power
- When GPIO8 is LOW, MOSFET turns OFF, LED has 0µA consumption

---

## 9. USER BUTTONS

### Button Up (SW1)

| Connection | Pin | Notes |
|------------|-----|-------|
| One side   | ESP32 GPIO9 | Input with pull-up |
| Other side | GND | Pulls low when pressed |

**Components:**
- **R6:** 10kΩ pull-up resistor (GPIO9 to +3.3V)
- **C7:** 0.1µF debounce capacitor (GPIO9 to GND)

### Button Down (SW2)

| Connection | Pin | Notes |
|------------|-----|-------|
| One side   | ESP32 GPIO10 | Input with pull-up |
| Other side | GND | Pulls low when pressed |

**Components:**
- **R7:** 10kΩ pull-up resistor (GPIO10 to +3.3V)
- **C8:** 0.1µF debounce capacitor (GPIO10 to GND)

**Logic:**
- Idle (not pressed): HIGH (pulled up to 3.3V)
- Pressed: LOW (connected to GND)

---

## ASSEMBLY ORDER

### Step 1: Test Buck Converters
1. **Buck #1:** Set to 12.6V, verify with multimeter
2. **Buck #2:** Set to 5.0V, verify with multimeter

### Step 2: Battery + BMS
1. Connect battery to BMS (B+, B2, B1, B-)
2. Verify BMS output voltage (~11.1V)

### Step 3: Power Distribution
1. Connect BMS output to both buck converter inputs
2. Create common GND bus
3. Add output capacitors (C1, C2, C3)

### Step 4: ESP32-C6
1. Connect +5V from Buck #2 to ESP32 5V pin
2. Connect GND
3. **Test:** ESP32 should power on (LED blinks)

### Step 5: Motor Driver
1. Connect TB6612 VM to VBAT
2. Connect TB6612 VCC to +5V
3. Connect GND
4. Add decoupling cap (C4)
5. Connect control pins (GPIO1, 2, 3)
6. Connect motor (AO1, AO2)

### Step 6: Sensors
1. **AS5600:** Connect VCC (3.3V!), GND, SDA (GPIO4), SCL (GPIO5)
   - Add pull-ups (R1, R2) and cap (C5)
2. **INA219:** Connect VCC (3.3V), GND, SDA (GPIO6), SCL (GPIO7)
   - Add pull-ups (R3, R4) and cap (C6)
   - Wire in-line with motor power

### Step 7: LED
1. Connect MOSFET (Gate to GPIO8, Drain to +5V, Source to LED VCC)
2. Connect WS2812B (DIN to GPIO8, GND to common)
3. Add pull-down (R5)

### Step 8: Buttons
1. Wire SW1 (GPIO9) with pull-up (R6) and debounce cap (C7)
2. Wire SW2 (GPIO10) with pull-up (R7) and debounce cap (C8)

---

## VERIFICATION CHECKLIST

- [ ] Buck #1 outputs 12.6V (measured)
- [ ] Buck #2 outputs 5.0V (measured)
- [ ] BMS output voltage correct (~11.1V)
- [ ] ESP32 powers on (LED indicator)
- [ ] AS5600 powered with 3.3V (NOT 5V!)
- [ ] INA219 powered with 3.3V
- [ ] All GND connections to common ground
- [ ] I2C pull-ups installed on both buses
- [ ] All decoupling capacitors installed
- [ ] Motor polarity noted (for direction control)
- [ ] No short circuits (test with multimeter)

---

## TROUBLESHOOTING

**ESP32 won't power on:**
- Check Buck #2 output is 5.0V
- Verify GND connection
- Check USB-C connection (if programming)

**Motor doesn't run:**
- Verify TB6612 VM has 12V
- Check GPIO1, 2, 3 connections
- Test motor directly with battery

**AS5600 not detected:**
- Verify 3.3V power (measure at sensor)
- Check I2C pull-ups (R1, R2 = 4.7kΩ)
- Scan I2C bus (address: 0x36)

**INA219 not detected:**
- Verify 3.3V power
- Check I2C pull-ups (R3, R4 = 4.7kΩ)
- Scan I2C bus (address: 0x40 or 0x41)

**LED doesn't light:**
- Check GPIO8 signal (should go HIGH)
- Verify MOSFET orientation
- Test LED with direct 5V connection

**Buttons don't respond:**
- Check pull-up resistors (R6, R7 = 10kΩ)
- Verify GND connection
- Test with multimeter (should read ~3.3V idle, 0V pressed)

---

## WIRE COLORS RECOMMENDATION

| Signal Type | Suggested Color |
|-------------|-----------------|
| +12V (VBAT) | Red (thick) |
| +5V         | Red (thin) |
| +3.3V       | Orange |
| GND         | Black |
| Motor +     | Red/Yellow |
| Motor -     | Black/Blue |
| I2C SDA     | Blue |
| I2C SCL     | Purple/Violet |
| GPIO signals | Green/Yellow/White |

---

## NOTES

- **Current capacity:** Use 18-20 AWG wire for power lines (battery, motor)
- **Signal wires:** 22-24 AWG sufficient for GPIO, I2C
- **Cable management:** Keep I2C wires short (<30cm) and twisted pairs
- **Strain relief:** Secure wires to prevent stress on solder joints
- **Labeling:** Use wire labels or tape to mark connections

---

**Created by:** Claude Code
**Based on schematic:** motorized-blinds.kicad_sch
**Date:** 2026-02-02
