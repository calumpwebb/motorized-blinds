# Motorized Blinds Project Overview

## Project Summary

Battery-powered motorized blinds controlled via ESP32-C6 with Matter/HomeKit integration. Designed for minimal power consumption and USB-C charging.

---

## Key Specifications

| Spec | Value |
|------|-------|
| Blind travel distance | 380cm |
| Travel time | ~1 minute (40 RPM motor) |
| Force required | 36.75N (3.75kg load) |
| Battery | 3S2P 21700 (11.1V nominal, 10,000mAh) |
| Charging | USB-C PD (15V or 20V) |
| Control | Matter protocol (works with Apple Home, Google, Alexa) |

---

## System Block Diagram

```
                              ┌─────────────────────────────────────┐
                              │         USB-C PD Charger            │
                              └─────────────────┬───────────────────┘
                                                │
                                                ▼
                              ┌─────────────────────────────────────┐
                              │      PD Decoy Module (15V/20V)      │
                              │      Negotiates voltage from        │
                              │      USB-C PD source                │
                              └─────────────────┬───────────────────┘
                                                │ 15V/20V
                                                ▼
                              ┌─────────────────────────────────────┐
                              │      Buck Converter (MP1584)        │
                              │      Steps down to 12.6V for        │
                              │      CC/CV-ish charging             │
                              └─────────────────┬───────────────────┘
                                                │ 12.6V
                                                ▼
┌──────────────────────────────────────────────────────────────────────────────┐
│                              3S BMS (10A)                                     │
│                    Protection + Cell Balancing                                │
│                    - Overcharge protection (12.6V cutoff)                     │
│                    - Overdischarge protection                                 │
│                    - Short circuit protection                                 │
└──────────────────────────────────┬───────────────────────────────────────────┘
                                   │
                                   ▼
┌──────────────────────────────────────────────────────────────────────────────┐
│                         3S2P Battery Pack                                     │
│                    6x Samsung 50E 21700 cells                                 │
│                    11.1V nominal / 12.6V max                                  │
│                    10,000mAh capacity                                         │
└──────────────────────────────────┬───────────────────────────────────────────┘
                                   │ 11-12.6V
                    ┌──────────────┴──────────────┐
                    │                             │
                    ▼                             ▼
┌───────────────────────────┐     ┌───────────────────────────────────────────┐
│   Buck Converter (5V)     │     │              INA219                        │
│   MP1584 adjusted to 5V   │     │        Current Sensor                      │
│   Powers ESP32            │     │   Monitors motor current for               │
└─────────────┬─────────────┘     │   stall/obstruction detection              │
              │                   └──────────────────┬────────────────────────┘
              │ 5V                                   │
              ▼                                      ▼
┌───────────────────────────────────────────────────────────────────────────────┐
│                           ESP32-C6 (XIAO)                                      │
│                                                                                │
│   - WiFi for Matter/HomeKit                                                   │
│   - Deep sleep: ~50µA                                                         │
│   - Controls motor via TB6612                                                 │
│   - Reads encoder position                                                    │
│   - Reads motor current via INA219 (I2C)                                      │
│   - Controls status LED power                                                 │
│                                                                                │
│   GPIO Assignments:                                                           │
│   - GPIO1: Motor AIN1 (direction)                                             │
│   - GPIO2: Motor AIN2 (direction)                                             │
│   - GPIO3: Motor PWM (speed)                                                  │
│   - GPIO4: Encoder A (I2C SDA for AS5600)                                     │
│   - GPIO5: Encoder B (I2C SCL for AS5600)                                     │
│   - GPIO6: INA219 SDA                                                         │
│   - GPIO7: INA219 SCL                                                         │
│   - GPIO8: LED power control (via MOSFET)                                     │
│   - GPIO9: Button 1 (Up)                                                      │
│   - GPIO10: Button 2 (Down)                                                   │
└───────────────────────────┬───────────────────────────────────────────────────┘
                            │
          ┌─────────────────┼─────────────────┬──────────────────┐
          │                 │                 │                  │
          ▼                 ▼                 ▼                  ▼
┌─────────────────┐ ┌─────────────┐ ┌─────────────────┐ ┌─────────────────┐
│    TB6612FNG    │ │   AS5600    │ │    WS2812B      │ │   Push Buttons  │
│  Motor Driver   │ │  Magnetic   │ │   Status LED    │ │   (Momentary)   │
│                 │ │  Encoder    │ │                 │ │                 │
│ VM: 12V direct  │ │  12-bit     │ │ Powered via     │ │ Up / Down       │
│ VCC: 3.3V logic │ │  4096 pos/  │ │ 2N7000 MOSFET   │ │ control         │
│                 │ │  revolution │ │ for 0µA when    │ │                 │
│ Controls motor  │ │             │ │ off             │ │                 │
│ direction + PWM │ │ ~0.023mm    │ │                 │ │                 │
│                 │ │ resolution  │ │ ~0.6mA when on  │ │                 │
└────────┬────────┘ └─────────────┘ └─────────────────┘ └─────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        DC Worm Gear Motor                                    │
│                        BRINGSMART 5840-31ZY                                  │
│                                                                              │
│   - 12V DC                                                                   │
│   - 40 RPM                                                                   │
│   - 40 kg.cm torque (we need ~5.5 kg.cm)                                    │
│   - Self-locking worm gear (holds position with 0 power!)                   │
│   - 90° shaft orientation (good for mounting)                               │
│   - ~0.5A running, ~0.6A stall                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Component Selection Rationale

### Motor: BRINGSMART 5840-31ZY 12V 40RPM

**Why this motor:**
- **Self-locking worm gear**: Blinds stay in position with zero power - no brake needed, huge battery savings
- **90° shaft orientation**: Allows flat mounting against wall/window frame
- **40 kg.cm torque**: 7x more than our 5.5 kg.cm requirement - plenty of margin
- **40 RPM**: Gives ~1 minute travel time for 380cm with 30mm sprocket

**Calculations:**
```
Required torque = Force × Radius = 36.75N × 0.015m = 0.55 Nm = 5.5 kg.cm
Motor provides = 40 kg.cm (7× safety margin)

Speed with 30mm sprocket:
Circumference = π × 30mm = 94.25mm
Linear speed = 40 RPM × 94.25mm = 3770 mm/min = 62.8 mm/s
Travel time for 380cm = 3800mm / 62.8mm/s ≈ 60 seconds
```

### Battery: 3S2P Samsung 50E 21700

**Why this configuration:**
- **3S (11.1V nominal)**: Matches 12V motor, common voltage for electronics
- **2P (10,000mAh)**: Provides weeks/months of runtime
- **Samsung 50E**: High capacity (5000mAh), reliable, 9.8A continuous discharge

**Runtime estimate:**
```
Motor usage: ~0.5A × 2 min/day = ~17mAh/day
ESP32 deep sleep: ~50µA × 24h = ~1.2mAh/day
Total: ~20mAh/day = ~500 days theoretical (real world: weeks to months)
```

### ESP32-C6 (Seeed XIAO)

**Why ESP32-C6:**
- **Matter support**: Native Thread radio for Matter protocol
- **WiFi**: For cloud connectivity and HomeKit
- **Ultra-low power**: ~50µA deep sleep, ~7µA with proper configuration
- **Small form factor**: XIAO is tiny for compact enclosure
- **Sufficient GPIO**: Enough pins for motor, encoder, LED, buttons

### Motor Driver: TB6612FNG

**Why TB6612:**
- **Dual H-bridge**: Can control motor direction and speed
- **3.3V logic compatible**: Works directly with ESP32
- **Low voltage drop**: Efficient, less heat
- **Small**: Compact breakout boards available
- **PWM capable**: Smooth speed control

### Encoder: AS5600 (12-bit magnetic)

**Why magnetic encoder:**
- **Non-contact**: No wear, long lifespan
- **12-bit resolution**: 4096 positions/revolution = 0.023mm precision
- **I2C interface**: Only 2 wires needed
- **Works through plastic**: Can mount inside enclosure

**Position accuracy:**
```
Steps per revolution: 4096
Sprocket circumference: 94.25mm
Resolution: 94.25mm / 4096 = 0.023mm per step

For 380cm travel: 40.3 rotations × 4096 = 165,000+ unique positions
```

### Current Sensor: INA219

**Why INA219:**
- **I2C interface**: Easy to integrate, 2 wires
- **High accuracy**: ~1% vs ~30% for DIY sense resistor
- **Built-in shunt**: No external components needed
- **Low power**: 350µA active, 1µA power-down

**Use cases:**
- Detect motor stall (obstruction)
- Auto-calibration (find end stops)
- Battery monitoring

### Status LED: WS2812B with MOSFET power control

**Why WS2812B:**
- **RGB**: Any color for different states
- **Single GPIO**: Easy control
- **Addressable**: Can chain multiple if needed

**Why MOSFET power control (2N7000):**
- WS2812B draws ~0.6mA even when "off"
- MOSFET allows complete power cut = 0µA
- Only power LED when needed (button press, charging)

### Charging: USB-C PD Decoy + Buck Converter

**Why this approach:**
- **USB-C PD**: Universal, high power available
- **15V/20V negotiation**: Provides headroom above 12.6V battery voltage
- **Buck converter to 12.6V**: Simple CC/CV-ish charging
- **BMS protection**: Handles cell balancing and safety cutoffs

**Simpler than dedicated 3S charger IC, good enough for this application.**

---

## Power Budget

### Active (motor running)

| Component | Current |
|-----------|---------|
| Motor | ~500mA |
| ESP32 (WiFi) | ~80mA |
| TB6612 | ~2mA |
| Encoder | ~10mA |
| LED (if on) | ~20mA |
| INA219 | ~0.35mA |
| **Total** | **~600mA** |

### Idle (deep sleep)

| Component | Current |
|-----------|---------|
| ESP32 deep sleep | ~50µA |
| BMS quiescent | ~10µA |
| INA219 (power-down) | ~1µA |
| LED (MOSFET off) | 0µA |
| **Total** | **~60µA** |

### Battery life estimate

```
Daily usage:
- 2 blind movements × 1 min × 600mA = 20mAh
- 23.97 hours sleep × 0.06mA = 1.4mAh
- Total: ~22mAh/day

Battery capacity: 10,000mAh
Theoretical runtime: 10,000 / 22 = 454 days

Real world (accounting for inefficiencies, WiFi wake-ups, etc.):
Estimate 2-6 months between charges
```

---

## Wiring Summary

### Power connections

```
Battery+ (12V) ──┬── INA219 VIN+ ── INA219 VIN- ── TB6612 VM
                 │
                 └── Buck Converter IN+ ── OUT (5V) ── ESP32 5V

Battery- ──────────── Common GND (all components)
```

### Signal connections

| ESP32 Pin | Connects To | Function |
|-----------|-------------|----------|
| GPIO1 | TB6612 AIN1 | Motor direction |
| GPIO2 | TB6612 AIN2 | Motor direction |
| GPIO3 | TB6612 PWMA | Motor speed |
| GPIO4 | AS5600 SDA | Encoder I2C |
| GPIO5 | AS5600 SCL | Encoder I2C |
| GPIO6 | INA219 SDA | Current sensor I2C |
| GPIO7 | INA219 SCL | Current sensor I2C |
| GPIO8 | 2N7000 Gate | LED power control |
| GPIO9 | Button 1 | Up button |
| GPIO10 | Button 2 | Down button |
| 3.3V | TB6612 VCC, STBY, AS5600, WS2812B (via MOSFET) | Logic power |

---

## Smart Home Integration

### Matter (Recommended)

ESP32-C6 has native Matter support. Benefits:
- Works with Apple Home, Google Home, Amazon Alexa, Samsung SmartThings
- Local control (no cloud dependency)
- Thread radio for mesh networking

Device type: Window Covering

Features exposed:
- Open/close
- Position percentage
- Battery level

### Software features to implement

- **Soft start/stop**: Ramp PWM for smooth, quiet operation
- **Position memory**: Save to flash, restore after power loss
- **Auto-calibration**: Hold button to find end stops via stall detection
- **Obstruction detection**: Stop if current spikes unexpectedly
- **Battery reporting**: Monitor voltage, report to Home app

---

## Bill of Materials

| Component | Part | Qty | ~Price |
|-----------|------|-----|--------|
| MCU | Seeed XIAO ESP32-C6 | 1 | $13 |
| Motor | BRINGSMART 5840-31ZY 12V 40RPM | 1 | $29 |
| Motor driver | TB6612FNG breakout | 1 | $10 (3pk) |
| Encoder | AS5600 magnetic encoder | 1 | $16 (5pk) |
| Current sensor | INA219 module | 1 | $19 (10pk) |
| Buck converter | MP1584EN | 2 | $18 (12pk) |
| BMS | 3S 10A BMS | 1 | $9 (5pk) |
| PD trigger | 15V/20V PD decoy | 1 | $9 (5pk) |
| Batteries | Samsung 50E 21700 | 6 | ~$35 |
| LED | WS2812B | 1 | $15 (100pk) |
| MOSFET | 2N7000 | 1 | $9 (100pk) |
| Buttons | Momentary push button | 2 | $7 (10pk) |
| Spot welder | 8000W handheld | 1 | $50 |
| Battery insulators | 21700 rings | 1pk | $10 |
| Resistor kit | 1/2W 1% metal film | 1 | $14 |
| Solder | 63/37 0.8mm | 1 | $10 |
| Wire | 22 AWG silicone | 1 | $16 |
| Heat shrink | Assortment | 1 | ~$8 |
| Helping hands | Soldering station | 1 | $26 |
| **Total** | | | **~$300** |

*Many items are multi-packs, so cost per unit is much lower. Actual project cost ~$100-150.*

---

## Future Enhancements

- **Light sensor**: Auto open at sunrise
- **Temperature sensor**: Close blinds when hot
- **E-paper display**: Show position/battery (0µA when not updating)
- **Solar charging**: Top-up from window light
- **Multiple blind sync**: Control multiple blinds together

---

## Files in this repository

- `PROJECT-OVERVIEW.md` - This document
- `schematic.md` - Detailed circuit diagrams (Mermaid)
- `wiring-diagram.md` - Physical wiring connections
