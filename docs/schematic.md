# Motorized Blinds Schematic

## System Block Diagram

```mermaid
flowchart TB
    subgraph USB_INPUT["USB-C PD Input"]
        USBC[USB-C Connector]
        PD[USB PD Controller<br/>IP2721 or STUSB4500<br/>Negotiate 15V]
        CC1_R[R1: 5.1k<br/>CC1 Pulldown]
        CC2_R[R2: 5.1k<br/>CC2 Pulldown]
    end

    subgraph CHARGING["Battery Charging"]
        BUCK_CHG[Buck Converter<br/>MP1584 set to 12.6V<br/>Simple charging]
        BMS[3S BMS<br/>10-15A<br/>Balance + Protection]
    end

    subgraph BATTERY["3S2P Battery Pack"]
        BAT["6x Samsung 50E 21700<br/>11.1V nom / 12.6V max<br/>10,000mAh"]
    end

    subgraph POWER_REG["Power Regulation"]
        BUCK1[Buck Converter<br/>LM2596 or MP1584<br/>12V → 5V @ 2A]
        LDO[LDO 3.3V<br/>AMS1117-3.3<br/>or ESP32 onboard]
    end

    subgraph MCU["Microcontroller"]
        ESP32[ESP32-C6<br/>WiFi/BLE<br/>3.3V Logic]
    end

    subgraph MOTOR_DRIVE["Motor Driver"]
        DRIVER[H-Bridge Driver<br/>TB6612FNG or DRV8833<br/>12V / 2A]
        MOTOR[DC Worm Gear Motor<br/>5840-31ZY<br/>12V 40RPM]
    end

    subgraph ENCODER["Position Feedback"]
        MAG_ENC[Magnetic Encoder<br/>WWZMDiB<br/>3.3V]
    end

    %% Connections
    USBC --> PD
    USBC -.-> CC1_R
    USBC -.-> CC2_R
    PD -->|15V| BUCK_CHG
    BUCK_CHG -->|12.6V| BMS
    BMS <--> BAT
    BAT -->|11-12.6V| BUCK1
    BAT -->|11-12.6V| DRIVER
    BUCK1 -->|5V| ESP32
    BUCK1 -->|5V| LDO
    LDO -->|3.3V| MAG_ENC
    ESP32 -->|PWM + DIR| DRIVER
    DRIVER --> MOTOR
    MAG_ENC -->|Position| ESP32
```

## Detailed Wiring Schematic

```mermaid
flowchart LR
    subgraph USB_C["J1: USB-C Connector"]
        VBUS[VBUS]
        CC1[CC1]
        CC2[CC2]
        DN1[D-]
        DP1[D+]
        GND_USB[GND]
    end

    subgraph PD_CTRL["U1: IP2721 USB PD Sink"]
        VIN_PD[VIN]
        VOUT_PD[VOUT 15V]
        CC1_PD[CC1]
        CC2_PD[CC2]
        GND_PD[GND]
    end

    VBUS --> VIN_PD
    CC1 --> CC1_PD
    CC2 --> CC2_PD
    GND_USB --> GND_PD

    CC1 -.->|R1 5.1k| GND_USB
    CC2 -.->|R2 5.1k| GND_USB
```

## Power Section Detail

```mermaid
flowchart TB
    subgraph BUCK["U2: LM2596-ADJ Buck Converter"]
        VIN[VIN<br/>7-40V]
        VOUT[VOUT<br/>5V]
        FB[FB]
        ON_OFF[ON/OFF]
        GND_B[GND]
    end

    subgraph BUCK_EXTERNAL["External Components"]
        CIN[C1: 100uF<br/>Input Cap]
        COUT[C2: 220uF<br/>Output Cap]
        L1[L1: 33uH<br/>Inductor]
        D1[D1: SS34<br/>Schottky]
        R_FB1[R3: 1k<br/>FB Divider]
        R_FB2[R4: 3.3k<br/>FB Divider]
    end

    BAT_POS[VBAT+<br/>12V] --> CIN --> VIN
    VOUT --> L1 --> NODE_OUT[5V OUT]
    NODE_OUT --> COUT --> GND
    VOUT --> D1 --> GND
    NODE_OUT --> R_FB1 --> FB
    FB --> R_FB2 --> GND
    ON_OFF --> GND
    GND_B --> GND[GND]
```

## Motor Driver Section

```mermaid
flowchart TB
    subgraph TB6612["U3: TB6612FNG Motor Driver"]
        VM[VM<br/>Motor Power]
        VCC[VCC<br/>Logic 3.3V]
        AIN1[AIN1]
        AIN2[AIN2]
        PWMA[PWMA]
        AO1[AO1]
        AO2[AO2]
        STBY[STBY]
        GND_M[GND]
    end

    VBAT[VBAT 12V] --> VM
    V33[3.3V] --> VCC
    V33 --> STBY

    ESP_GPIO1[ESP32 GPIO1] --> AIN1
    ESP_GPIO2[ESP32 GPIO2] --> AIN2
    ESP_PWM[ESP32 PWM GPIO] --> PWMA

    AO1 --> MOTOR_P[Motor +]
    AO2 --> MOTOR_N[Motor -]
```

## ESP32-C6 Connections

```mermaid
flowchart TB
    subgraph ESP32["U4: ESP32-C6-WROOM Module"]
        direction TB
        VIN_ESP[5V IN]
        V3_ESP[3.3V OUT]
        GND_ESP[GND]

        GPIO0[GPIO0 - BOOT]
        GPIO1[GPIO1 - AIN1]
        GPIO2[GPIO2 - AIN2]
        GPIO3[GPIO3 - PWMA]
        GPIO4[GPIO4 - ENC_A]
        GPIO5[GPIO5 - ENC_B]
        GPIO6[GPIO6 - LED]
        EN[EN - Reset]
        TX[TX]
        RX[RX]
    end

    5V_RAIL[5V Rail] --> VIN_ESP
    GND_RAIL[GND] --> GND_ESP

    GPIO1 -->|Direction| MOTOR_DRIVER
    GPIO2 -->|Direction| MOTOR_DRIVER
    GPIO3 -->|Speed PWM| MOTOR_DRIVER

    GPIO4 -->|Encoder A| ENCODER
    GPIO5 -->|Encoder B| ENCODER

    V3_ESP -->|3.3V| ENCODER_VCC[Encoder VCC]
```

## Bill of Materials

| Ref | Component | Value/Part | Purpose |
|-----|-----------|------------|---------|
| J1 | USB-C Connector | USB4110-GF-A | Power input |
| U1 | USB PD Controller | IP2721 or STUSB4500 | Negotiate 15V from USB-C PD |
| R1, R2 | Resistor | 5.1k 0603 | CC line pulldowns |
| - | 3S BMS | 10A 3S BMS | Battery protection + balance |
| - | Buck Converter (charging) | MP1584 set to 12.6V | Simple charging (BMS handles cutoff) |
| U2 | Buck Converter | LM2596-ADJ or MP1584 | 12V to 5V |
| C1 | Capacitor | 100uF 25V | Buck input |
| C2 | Capacitor | 220uF 10V | Buck output |
| L1 | Inductor | 33uH 3A | Buck inductor |
| D1 | Schottky Diode | SS34 | Buck freewheeling |
| R3 | Resistor | 1k | FB divider |
| R4 | Resistor | 3.3k | FB divider |
| U3 | Motor Driver | TB6612FNG | H-bridge driver |
| U4 | MCU | ESP32-C6-WROOM-1 | Main controller |
| C3, C4 | Capacitor | 10uF | Decoupling |
| C5, C6 | Capacitor | 100nF | Decoupling |
| - | Encoder | WWZMDiB Magnetic | Position feedback |
| M1 | Motor | 5840-31ZY 40RPM | Worm gear motor |

## Simplified Module-Based Build

If you want to use off-the-shelf modules instead of discrete components:

```mermaid
flowchart LR
    subgraph MODULES["Pre-built Modules"]
        USB_MOD[USB-C PD Trigger<br/>15V Output<br/>~$3]
        BUCK_CHG_MOD[MP1584 Buck Module<br/>Adjust to 12.6V<br/>~$2]
        BMS_MOD[3S 10A BMS<br/>~$3]
        BUCK_MOD[MP1584 Buck Module<br/>Adjust to 5V<br/>~$2]
        DRIVER_MOD[TB6612 Breakout<br/>~$5]
        ESP_MOD[ESP32-C6 DevKit<br/>~$8]
    end

    USB_MOD -->|15V| BUCK_CHG_MOD
    BUCK_CHG_MOD -->|12.6V| BMS_MOD
    BMS_MOD <-->|12V| BATTERY
    BATTERY[3S2P Pack] --> BUCK_MOD
    BATTERY --> DRIVER_MOD
    BUCK_MOD -->|5V| ESP_MOD
    ESP_MOD -->|Control| DRIVER_MOD
    DRIVER_MOD --> MOTOR[Motor]
```

## GPIO Pin Assignment (ESP32-C6)

| GPIO | Function | Notes |
|------|----------|-------|
| GPIO0 | BOOT | Hold LOW during reset to enter bootloader |
| GPIO1 | Motor AIN1 | Direction control |
| GPIO2 | Motor AIN2 | Direction control |
| GPIO3 | Motor PWM | Speed control (LEDC) |
| GPIO4 | Encoder A | Quadrature input |
| GPIO5 | Encoder B | Quadrature input |
| GPIO6 | Status LED | Optional |
| GPIO7 | Limit Switch 1 | Optional - top limit |
| GPIO8 | Limit Switch 2 | Optional - bottom limit |
| GPIO21 | TX | Debug UART |
| GPIO20 | RX | Debug UART |

## Power Budget

| Component | Voltage | Current (typ) | Current (max) |
|-----------|---------|---------------|---------------|
| ESP32-C6 | 5V | 80mA | 500mA (WiFi TX) |
| Motor | 12V | 200mA | 600mA |
| Encoder | 3.3V | 10mA | 20mA |
| TB6612 | 3.3V | 1mA | 2mA |
| **Total from 12V** | - | ~300mA | ~700mA |
| **Total from 5V** | - | ~90mA | ~520mA |
