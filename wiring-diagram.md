# Motorized Blinds - Module Wiring Diagram

## Parts List

| # | Module | Example Part | Buy From |
|---|--------|--------------|----------|
| 1 | USB-C PD Trigger Board (15V) | IP2721 or ZY12PDN | AliExpress/Amazon |
| 2 | 3S BMS (10A) | Generic 3S 10A BMS | AliExpress/Amazon |
| 3 | Buck Converter (×2) | MP1584 or Mini-360 | AliExpress/Amazon |
| 4 | Motor Driver | TB6612FNG Breakout | AliExpress/Amazon |
| 5 | ESP32-C6 DevKit | ESP32-C6-DevKitC-1 | AliExpress/Amazon |
| 6 | Magnetic Encoder | AS5600 | Amazon |
| 7 | Motor | 5840-31ZY 12V 40RPM | Amazon |
| 8 | 6x 21700 Cells | Samsung 50E | 18650batterystore |

**Note:** We use 2 buck converters - one set to 12.6V for charging (BMS handles cutoff), one set to 5V for ESP32.

## Physical Wiring Diagram

```
╔═══════════════════════════════════════════════════════════════════════════════════════╗
║                           MOTORIZED BLINDS WIRING DIAGRAM                              ║
╚═══════════════════════════════════════════════════════════════════════════════════════╝

┌─────────────────┐
│   USB-C PD      │
│   CHARGER       │
│   (e.g. 65W)    │
└────────┬────────┘
         │ USB-C Cable
         ▼
┌─────────────────┐         ┌─────────────────────────────────────┐
│  USB-C PD       │         │     BUCK CONVERTER (CHARGING)       │
│  TRIGGER (15V)  │         │     MP1584 set to 12.6V             │
│                 │         │                                     │
│  VOUT+ ●────────┼────────►│ VIN+                     VOUT+ ●────┼──┐
│  GND   ●────────┼────────►│ GND                      GND   ●────┼──┼──┐
└─────────────────┘         │                                     │  │  │
                            │  [Adjust pot to 12.6V]              │  │  │
                            └─────────────────────────────────────┘  │  │
                                                                     │  │
┌────────────────────────────────────────────────────────────────────┼──┼──┐
│                           3S BMS (10A)                             │  │  │
│  ┌─────────────────────────────────────────────────────────────┐   │  │  │
│  │  B+    B1    B2    B-    P+    P-    C-                     │   │  │  │
│  │  ●     ●     ●     ●     ●     ●     ●                      │   │  │  │
│  └──┼─────┼─────┼─────┼─────┼─────┼─────┼──────────────────────┘   │  │  │
│     │     │     │     │     │     │     │                          │  │  │
│     │     │     │     │     │     │     │◄─────────────────────────┘  │  │
│     │     │     │     │     │     │◄────┼─────────────────────────────┘  │
│     │     │     │     │     │     │     │◄───────────────────────────────┘
└─────┼─────┼─────┼─────┼─────┼─────┼─────┼────────────────────────────────┘
      │     │     │     │     │     │
      ▼     ▼     ▼     ▼     │     │
┌─────────────────────────────┐     │
│    3S2P BATTERY PACK        │     │
│                             │     │
│  ┌─────┐ ┌─────┐ ┌─────┐    │     │
│  │Cell1│ │Cell2│ │Cell3│    │     │
│  │ 50E │ │ 50E │ │ 50E │    │     │
│  └──┬──┘ └──┬──┘ └──┬──┘    │     │
│     │       │       │       │     │
│  ┌──┴──┐ ┌──┴──┐ ┌──┴──┐    │     │
│  │Cell4│ │Cell5│ │Cell6│    │     │
│  │ 50E │ │ 50E │ │ 50E │    │     │
│  └─────┘ └─────┘ └─────┘    │     │
│                             │     │
│  Wiring: 3S2P               │     │
│  Voltage: 11.1V nom         │     │
│  Capacity: 10,000mAh        │     │
└─────────────────────────────┘     │
                                    │
         POWER OUTPUT (P+ and P-)   │
         ┌──────────────────────────┘
         │
         ▼
     ════╪════════════════════════════════════════════  12V POWER BUS
         │
         ├─────────────────────────────┐
         │                             │
         ▼                             ▼
┌─────────────────┐          ┌─────────────────────┐
│  BUCK CONVERTER │          │   TB6612FNG         │
│   (MP1584)      │          │   MOTOR DRIVER      │
│                 │          │                     │
│  VIN+ ●◄────────┤          │  VM ●◄──────────────┤
│  GND  ●◄────────┼──┐       │  GND ●◄─────────────┼──┐
│                 │  │       │                     │  │
│  VOUT ●─────────┼──┼──┐    │  VCC ●◄─────────┐   │  │
│  GND  ●─────────┼──┼──┼─┐  │                 │   │  │
│                 │  │  │ │  │  STBY ●◄────────┤   │  │
│  [ADJ: Set 5V]  │  │  │ │  │                 │   │  │
└─────────────────┘  │  │ │  │  AIN1 ●◄────────┼───┼──┼──── GPIO1 (ESP32)
                     │  │ │  │  AIN2 ●◄────────┼───┼──┼──── GPIO2 (ESP32)
                     │  │ │  │  PWMA ●◄────────┼───┼──┼──── GPIO3 (ESP32)
     ════════════════╪══╪═╪══╪═════════════════╪═══╪══╪════  GND BUS
                     │  │ │  │                 │   │  │
                     │  │ │  │  AO1 ●──────────┼───┼──┼──────┐
                     │  │ │  │  AO2 ●──────────┼───┼──┼────┐ │
                     │  │ │  └─────────────────┼───┼──┼──┘ │ │
                     │  │ │                    │   │  │    │ │
                     │  │ │                    │   │  │    │ │
                     │  │ └────────────────────┼───┘  │    │ │
                     │  │                      │      │    │ │
     ════════════════╪══╪══════════════════════╪══════╪════╪═╪═  5V POWER BUS
                     │  │                      │      │    │ │
                     │  │                      │      │    │ │
                     ▼  ▼                      ▼      ▼    │ │
              ┌─────────────────────────────────────────┐  │ │
              │           ESP32-C6 DEVKIT              │  │ │
              │                                        │  │ │
              │    5V ●◄───────────────────────────────┤  │ │
              │    GND ●◄──────────────────────────────┤  │ │
              │    3V3 ●───────────────────────────┐   │  │ │
              │                                    │   │  │ │
              │    GPIO1 ●─────────────────────────┼───┼──┼─┼──► AIN1
              │    GPIO2 ●─────────────────────────┼───┼──┼─┼──► AIN2
              │    GPIO3 ●─────────────────────────┼───┼──┼─┼──► PWMA
              │    GPIO4 ●─────────────────────────┼───┼──┼─┼──► ENCODER A
              │    GPIO5 ●─────────────────────────┼───┼──┼─┼──► ENCODER B
              │                                    │   │  │ │
              └────────────────────────────────────┼───┘  │ │
                                                   │      │ │
     ══════════════════════════════════════════════╪══════╪═╪═  3.3V POWER BUS
                                                   │      │ │
                                                   ▼      │ │
              ┌─────────────────────────────────────┐     │ │
              │      MAGNETIC ENCODER              │     │ │
              │        (WWZMDiB)                   │     │ │
              │                                    │     │ │
              │    VCC ●◄──────────────────────────┤     │ │
              │    GND ●◄──────────────────────────┤     │ │
              │    OUT_A ●─────────────────────────┼─────┼─┼──► GPIO4
              │    OUT_B ●─────────────────────────┼─────┼─┼──► GPIO5
              └────────────────────────────────────┘     │ │
                                                         │ │
                                                         │ │
              ┌─────────────────────────────────────┐     │ │
              │      DC WORM GEAR MOTOR            │     │ │
              │      5840-31ZY 12V 40RPM           │     │ │
              │                                    │     │ │
              │    M+ (RED) ●◄──────────────────────┼─────┘ │
              │    M- (BLK) ●◄──────────────────────┼───────┘
              └────────────────────────────────────┘
```

## Wire-by-Wire Connection List

### Charging Section (Simplified - no dedicated charger IC)
| From | To | Wire Color | Notes |
|------|----|------------|-------|
| USB-C PD Trigger VOUT+ | Charging Buck VIN+ | Red | 15V from PD |
| USB-C PD Trigger GND | Charging Buck GND | Black | |
| Charging Buck VOUT+ | BMS C- or P- | Red | 12.6V to BMS (check your BMS for charge input) |
| Charging Buck GND | BMS P- | Black | Common ground |

**Important:** Set the charging buck converter to exactly 12.6V using a multimeter before connecting. The BMS will handle overcharge protection and cell balancing.

### BMS to Battery
| From | To | Wire Color | Notes |
|------|----|------------|-------|
| BMS B+ | Cell 1+/4+ (parallel group 1 positive) | Red | |
| BMS B1 | Junction Cell 1-/4- to Cell 2+/5+ | - | Balance tap 1 |
| BMS B2 | Junction Cell 2-/5- to Cell 3+/6+ | - | Balance tap 2 |
| BMS B- | Cell 3-/6- (parallel group 3 negative) | Black | |

### Power Distribution
| From | To | Wire Color | Notes |
|------|----|------------|-------|
| BMS P+ | Buck Converter VIN+ | Red | 12V |
| BMS P+ | TB6612 VM | Red | 12V Motor power |
| BMS P- | Buck Converter GND | Black | |
| BMS P- | TB6612 GND | Black | |
| Buck Converter VOUT (5V) | ESP32-C6 5V | Red | Adjust pot to 5.0V |
| Buck Converter GND | ESP32-C6 GND | Black | |

### Motor Driver Connections
| From | To | Wire Color | Notes |
|------|----|------------|-------|
| ESP32 3.3V | TB6612 VCC | Orange | Logic power |
| ESP32 3.3V | TB6612 STBY | Orange | Enable driver |
| ESP32 GPIO1 | TB6612 AIN1 | Yellow | Direction |
| ESP32 GPIO2 | TB6612 AIN2 | Green | Direction |
| ESP32 GPIO3 | TB6612 PWMA | Blue | PWM speed |
| TB6612 AO1 | Motor + (Red) | Red | Motor output |
| TB6612 AO2 | Motor - (Black) | Black | Motor output |

### Encoder Connections
| From | To | Wire Color | Notes |
|------|----|------------|-------|
| ESP32 3.3V | Encoder VCC | Red | 3.3V only! |
| ESP32 GND | Encoder GND | Black | |
| Encoder OUT_A | ESP32 GPIO4 | Yellow | Quadrature A |
| Encoder OUT_B | ESP32 GPIO5 | Green | Quadrature B |

## Battery Pack Internal Wiring (3S2P)

```
                    B+ (to BMS B+)
                     │
    ┌────────────────┴────────────────┐
    │                                 │
┌───┴───┐                         ┌───┴───┐
│ Cell1 │  ──┐              ┌──   │ Cell4 │
│  (+)  │    │   PARALLEL   │     │  (+)  │
│  50E  │    └──────────────┘     │  50E  │
│  (-)  │                         │  (-)  │
└───┬───┘                         └───┬───┘
    │                                 │
    └────────────────┬────────────────┘
                     │
                    B1 (to BMS B1) ◄── Balance tap between groups
                     │
    ┌────────────────┴────────────────┐
    │                                 │
┌───┴───┐                         ┌───┴───┐
│ Cell2 │  ──┐              ┌──   │ Cell5 │
│  (+)  │    │   PARALLEL   │     │  (+)  │
│  50E  │    └──────────────┘     │  50E  │
│  (-)  │                         │  (-)  │
└───┬───┘                         └───┬───┘
    │                                 │
    └────────────────┬────────────────┘
                     │
                    B2 (to BMS B2) ◄── Balance tap between groups
                     │
    ┌────────────────┴────────────────┐
    │                                 │
┌───┴───┐                         ┌───┴───┐
│ Cell3 │  ──┐              ┌──   │ Cell6 │
│  (+)  │    │   PARALLEL   │     │  (+)  │
│  50E  │    └──────────────┘     │  50E  │
│  (-)  │                         │  (-)  │
└───┬───┘                         └───┬───┘
    │                                 │
    └────────────────┬────────────────┘
                     │
                    B- (to BMS B-)
```

## Motor Control Logic

| AIN1 | AIN2 | PWMA | Motor Action |
|------|------|------|--------------|
| HIGH | LOW  | PWM  | Forward (open blinds) |
| LOW  | HIGH | PWM  | Reverse (close blinds) |
| LOW  | LOW  | X    | Coast/stop |
| HIGH | HIGH | X    | Brake |

## Important Notes

1. **Set Buck Converters BEFORE connecting anything:**
   - Charging buck: Set to **12.6V** (for battery charging)
   - 5V buck: Set to **5.0V** (for ESP32)
   - Use multimeter to verify both!
2. **Double-check battery polarity** - Reverse polarity will destroy components
3. **Use appropriate wire gauges**:
   - Battery/motor power: 18-20 AWG
   - Signal wires: 24-26 AWG
4. **Add inline fuse** - 2A fuse on battery P+ output recommended
5. **Encoder must be 3.3V** - Do NOT connect to 5V
6. **Simplified charging approach** - We skip a dedicated 3S charger IC. The buck converter outputs 12.6V and the BMS handles overcharge protection/cutoff. This is simpler and works fine for occasional charging.

## Optional Additions

### Power Switch
Add a toggle switch between BMS P+ and the rest of the circuit to turn system on/off.

### Status LED
```
ESP32 GPIO6 ──── 330Ω resistor ──── LED (+) ──── LED (-) ──── GND
```

### Limit Switches (optional)
```
ESP32 GPIO7 ──┬── Limit Switch 1 ──── GND  (Top limit)
              │
              └── 10k resistor ──── 3.3V   (Pull-up)

ESP32 GPIO8 ──┬── Limit Switch 2 ──── GND  (Bottom limit)
              │
              └── 10k resistor ──── 3.3V   (Pull-up)
```
