<div align="center">

```
 ██████╗██╗   ██╗██████╗ ██╗  ██╗███████╗██████╗
██╔════╝╚██╗ ██╔╝██╔══██╗██║  ██║██╔════╝██╔══██╗
██║      ╚████╔╝ ██████╔╝███████║█████╗  ██████╔╝
██║       ╚██╔╝  ██╔═══╝ ██╔══██║██╔══╝  ██╔══██╗
╚██████╗   ██║   ██║     ██║  ██║███████╗██║  ██║
 ╚═════╝   ╚═╝   ╚═╝     ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
```

### STM32F446RE Bare Metal Secure IoT Firmware
#### `No HAL` &nbsp;•&nbsp; `No CubeMX` &nbsp;•&nbsp; `No Shortcuts` &nbsp;•&nbsp; `Just Registers`

<br>

![Language](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c&logoColor=white)
![MCU](https://img.shields.io/badge/MCU-STM32F446RE-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![HAL](https://img.shields.io/badge/HAL-NONE-red?style=for-the-badge)
![Bootloader](https://img.shields.io/badge/Bootloader-Custom-blueviolet?style=for-the-badge)
![Security](https://img.shields.io/badge/Security-CRC%20%2B%20Encrypted%20Telemetry-darkgreen?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-In%20Progress-orange?style=for-the-badge)

<br>

> *"No HAL. No CubeMX. No shortcuts.*
> *Custom bootloader, encrypted telemetry and OTA firmware updates*
> *on STM32F446RE — built register by register."*

</div>

---

<br>

## 📌 &nbsp; What Is CYPHER?

**CYPHER** is a bare metal secure IoT firmware for the **STM32F446RE Nucleo-64**,
built entirely from scratch as a student project to understand embedded systems
at the deepest level possible.

Every peripheral is driven by directly writing to hardware registers after reading
the **RM0390 Reference Manual**. No HAL. No auto-generated code. No CubeMX.

CYPHER solves three real problems every IoT product faces:

```
Problem 1 → How do you update firmware on deployed devices safely?
Solution  → Custom bootloader with UART-based OTA firmware update

Problem 2 → What if firmware gets corrupted during an update?
Solution  → CRC32 verification — corrupted firmware never runs

Problem 3 → How do you protect sensor data being transmitted?
Solution  → Encrypted telemetry — XOR cipher with CRC integrity check
```

<br>

---

## ⚙️ &nbsp; System Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                         CYPHER SYSTEM                               │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   POWER ON                                                          │
│       │                                                             │
│       ▼                                                             │
│  ┌────────────────────────────────┐                                 │
│  │         BOOTLOADER             │  ← 0x08000000                  │
│  │                                │                                 │
│  │  1. Compute CRC32 of app       │                                 │
│  │  2. Compare with stored CRC    │                                 │
│  │  3a. PASS → jump to app        │                                 │
│  │  3b. FAIL → stay, wait for OTA │                                 │
│  │  4. Receive firmware over UART │                                 │
│  │  5. Write to Flash             │                                 │
│  │  6. Verify → jump              │                                 │
│  └────────────┬───────────────────┘                                 │
│               │ verified ✅                                          │
│               ▼                                                     │
│  ┌────────────────────────────────┐                                 │
│  │         APPLICATION            │  ← 0x08010000                  │
│  │                                │                                 │
│  │  ┌──────────┐  I2C             │                                 │
│  │  │  BMP280  │──────▶ Temp      │                                 │
│  │  │ Sensor   │       Pressure   │                                 │
│  │  └──────────┘                  │                                 │
│  │                                │                                 │
│  │  ┌──────────┐  ADC+DMA         │                                 │
│  │  │  Light   │──────▶ Lux       │                                 │
│  │  │  Sensor  │                  │                                 │
│  │  └──────────┘                  │                                 │
│  │                                │                                 │
│  │  Data → Encrypt → UART+DMA ────┼──────▶ PC (Python decrypt)     │
│  │                                │                                 │
│  │  RTC Timestamps                │                                 │
│  │  IWDG Watchdog                 │                                 │
│  │  Low Power Standby             │                                 │
│  │  PWM Health Indicator          │                                 │
│  └────────────────────────────────┘                                 │
└─────────────────────────────────────────────────────────────────────┘
```

**Bootloader Decision Flow:**

```
Boot
 │
 ├──▶ CRC32 check
 │         │
 │    ┌────┴────┐
 │   PASS      FAIL
 │    │         │
 │    ▼         ▼
 │  Jump      Stay in bootloader
 │  to app    Wait for UART OTA
 │            Receive firmware.bin
 │            Write to Flash
 │            Verify CRC
 │            Jump to app
```

<br>

---

## 🔐 &nbsp; Security Features

### Custom Bootloader
```c
/* Bootloader lives at 0x08000000
 * App lives at 0x08010000
 * CRC stored in last Flash sector */

if(crc32_verify(APP_START, APP_SIZE) == STORED_CRC)
{
    jump_to_application(APP_START);   /* verified — run it */
}
else
{
    uart_send("CYPHER: FIRMWARE VERIFICATION FAILED\r\n");
    enter_ota_mode();                 /* corrupted — refuse to run */
}
```

### Encrypted Telemetry
```c
/* Raw sensor data */
/* TEMP:27.4 PRESS:1013 LUX:847 TIME:09:32:11 */

/* After encryption */
/* X7#92@!kL$3&mP9@2#Lq */

/* Only a device with the key can read it */
/* Python script on PC decrypts live */
```

### OTA Firmware Update
```
Laptop                          STM32
  │                               │
  │─── firmware.bin over UART ───▶│
  │                               │ write to Flash
  │                               │ verify CRC32
  │◀─── "OTA SUCCESS" ────────────│
  │                               │ reboot → jump to new app
```

<br>

---

## 🔧 &nbsp; Bare Metal Depth

Most STM32 projects look like this:

```c
/* HAL — hides everything */
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
```

CYPHER looks like this:

```c
/*──────────────────────────────────────────────────────
 * GPIO Initialization — PA5 output
 * Reference: RM0390 Section 8.4
 *──────────────────────────────────────────────────────*/

/* Enable GPIOA clock — RCC_AHB1ENR bit 0 = GPIOAEN */
RCC_AHB1ENR |= (1U << 0);

/* PA5 as output — MODER bits 11:10 = 01
 * Clear both bits first — avoid hitting analog mode briefly */
GPIOA_MODER &= ~(3U << 10);
GPIOA_MODER |=  (1U << 10);

/* Drive PA5 high — ODR bit 5 */
GPIOA_ODR |= (1U << 5);
```

Every register address comes from the RM0390 memory map.
Every bit operation is intentional and documented.
Nothing is written without understanding it.

<br>

---

## ✨ &nbsp; Full Feature Set

| # | Feature | Implementation |
|---|---|---|
| 1 | **Custom Bootloader** | Lives at 0x08000000 — CRC verify before jump |
| 2 | **CRC32 Verification** | Firmware integrity check — bad code never runs |
| 3 | **OTA Firmware Update** | Bare metal UART receive → Flash write → verify |
| 4 | **Encrypted Telemetry** | XOR cipher + CRC — Python script decrypts on PC |
| 5 | **GPIO** | MODER, ODR, BSRR, IDR — boot signal + tamper alert |
| 6 | **UART** | BRR, CR1, SR, DR — bare metal TX/RX + printf redirect |
| 7 | **Timers + PWM** | TIMx PSC/ARR/CCR — breathing health indicator |
| 8 | **ADC + DMA** | Continuous light sampling — CPU never polls |
| 9 | **SPI Flash** | W25Q32 — encrypted log storage |
| 10 | **I2C BMP280** | CR1/CR2/SR1/CCR — temp + pressure sensor |
| 11 | **DMA Pipeline** | SxCR/NDTR/PAR/MAR — CPU-free data flow |
| 12 | **RTC** | TR/DR/ALRMAR — timestamped encrypted logs |
| 13 | **IWDG Watchdog** | KR/RLR — hardware fault recovery |
| 14 | **Low Power** | PWR_CR/CSR — standby + RTC alarm wakeup |
| 15 | **Custom Startup** | startup.s + linker.ld — written by hand |
| 16 | **MISRA C** | Static analysis clean — cppcheck verified |

<br>

---

## 🗂️ &nbsp; Project Structure

```
CYPHER-STM32/
│
├── 🔐 bootloader/                  lives at Flash 0x08000000
│   ├── boot_main.c                 bootloader entry point
│   ├── boot_uart.c / .h            UART driver for OTA receive
│   ├── boot_flash.c / .h           Flash erase + write for OTA
│   ├── boot_crc.c / .h             CRC32 firmware verification
│   ├── startup_boot.s              bootloader vector table
│   └── linker_boot.ld              bootloader memory regions
│
├── 📁 drivers/                     bare metal peripheral drivers
│   ├── gpio_driver.c / .h          RCC_AHB1ENR · MODER · ODR · IDR
│   ├── uart_driver.c / .h          USART_BRR · CR1 · SR · DR
│   ├── timer_driver.c / .h         TIMx_PSC · ARR · CCR
│   ├── adc_driver.c / .h           ADC_CR1 · CR2 · SQR · DR
│   ├── spi_driver.c / .h           SPI_CR1 · CR2 · SR · DR
│   ├── i2c_driver.c / .h           I2C_CR1 · CR2 · SR1 · CCR
│   ├── dma_driver.c / .h           DMA_SxCR · NDTR · PAR · MAR
│   ├── rtc_driver.c / .h           RTC_TR · DR · ALRMAR
│   └── watchdog_driver.c / .h      IWDG_KR · PR · RLR
│
├── 📁 devices/                     sensor drivers
│   ├── bmp280.c / .h               I2C + compensation formula
│   └── w25q_flash.c / .h           SPI flash read/write/erase
│
├── 📁 app/                         application logic
│   ├── telemetry.c / .h            sensor data + encryption + streaming
│   ├── alerts.c / .h               tamper detection + SOS signal
│   ├── crypto.c / .h               XOR cipher + CRC integrity
│   └── power_manager.c / .h        standby + duty cycle control
│
├── 📁 startup/                     written entirely by hand
│   ├── startup_app.s               app vector table + .data/.bss init
│   └── linker_app.ld               Flash + SRAM memory layout
│
├── 📁 tools/                       PC side Python scripts
│   ├── decrypt_telemetry.py        decrypts UART data live on PC
│   ├── firmware_update.py          sends firmware.bin over UART
│   └── crc_generator.py            generates CRC for firmware binary
│
├── 📁 docs/
│   ├── boot_flow.png               bootloader decision diagram
│   ├── memory_map.png              Flash layout — boot + app regions
│   ├── i2c_waveform.png            logic analyzer capture
│   ├── spi_waveform.png            logic analyzer capture
│   └── uart_ota_capture.png        OTA update waveform
│
├── 📁 tests/                       Unity unit tests
├── main.c                          clean entry point
├── Makefile                        hand-written build system
├── ARCHITECTURE.md                 every design decision explained
└── MISRA_NOTES.md                  MISRA C 2012 compliance notes
```

<br>

---

## 🖥️ &nbsp; Hardware

```
┌──────────────────────────────────────────────┐
│              NUCLEO-F446RE                   │
│                                              │
│  STM32F446RE                                 │
│  ├── Core    : Cortex-M4F @ 180MHz           │
│  ├── Flash   : 512 KB                        │
│  ├── RAM     : 128 KB                        │
│  └── FPU     : Yes                           │
│                                              │
│  Pin Map                                     │
│  ├── PA5  ──▶ Onboard LED  (boot signal)    │
│  ├── PC13 ──▶ User Button  (tamper input)   │
│  ├── PA2  ──▶ UART2 TX     (USB debug)      │
│  ├── PA3  ──▶ UART2 RX     (OTA receive)    │
│  ├── PB8  ──▶ I2C1 SCL     (BMP280)        │
│  ├── PB9  ──▶ I2C1 SDA     (BMP280)        │
│  ├── PA5  ──▶ SPI1 SCK     (W25Q32)        │
│  ├── PA6  ──▶ SPI1 MISO    (W25Q32)        │
│  └── PA7  ──▶ SPI1 MOSI    (W25Q32)        │
└──────────────────────────────────────────────┘
```

<br>

---

## 🔨 &nbsp; Build

### Requirements
```bash
arm-none-eabi-gcc
make
```

### Commands
```bash
make          # compile application
make boot     # compile bootloader
make clean    # remove build artifacts
make flash    # flash via ST-Link
```

### Output
```
firmware.elf  →  ELF binary    — GDB debugging
firmware.bin  →  raw binary    — ST-Link flashing
firmware.hex  →  Intel HEX     — STM32CubeProgrammer
```

<br>

---

## 📈 &nbsp; Build Progress

| Week | Module | Key Registers | Status |
|:---:|---|---|:---:|
| **1** | GPIO — boot signal + tamper alert | `RCC_AHB1ENR` `GPIOA_MODER` `GPIOA_ODR` `GPIOC_IDR` | ✅ |
| **2** | UART — TX/RX, printf redirect | `USART_BRR` `USART_CR1` `USART_SR` | ⏳ |
| **3** | Timers + PWM — breathing LED | `TIMx_PSC` `TIMx_ARR` `TIMx_CCR` | ⏳ |
| **4** | ADC — live light sensor | `ADC_CR1` `ADC_CR2` `ADC_SQR` `ADC_DR` | ⏳ |
| **5** | Bootloader scaffold — Flash R/W | `FLASH_CR` `FLASH_SR` `FLASH_KEYR` | ⏳ |
| **6** | I2C — BMP280 temp + pressure | `I2C_CR1` `I2C_SR1` `I2C_CCR` | ⏳ |
| **7** | EXTI — tamper detection | `EXTI_IMR` `EXTI_RTSR` `NVIC` | ⏳ |
| **8** | DMA — non-blocking pipeline | `DMA_SxCR` `DMA_NDTR` `DMA_PAR` | ⏳ |
| **9** | DMA deep dive — full CPU-free flow | DMA + ADC + SPI | ⏳ |
| **10** | RTC + Watchdog | `RTC_TR` `IWDG_KR` `IWDG_RLR` | ⏳ |
| **11** | Low power — standby + wakeup | `PWR_CR` `PWR_CSR` `RTC_ALRMAR` | ⏳ |
| **12** | MISRA C — static analysis | cppcheck clean | ⏳ |
| **13** | Bootloader + App integration | CRC verify + OTA update | ⏳ |
| **14** | Security testing — corrupt firmware | verify rejection + recovery | ⏳ |
| **15** | Docs — boot flow + memory map | logic analyzer captures | ⏳ |
| **16** | Demo video + v1.0 release | 🎯 | ⏳ |

<br>

---

## 🎬 &nbsp; The Demo — Week 16

At week 16 this is what I will demonstrate:

```
Step 1 — Normal boot
  Power on → bootloader verifies CRC → jumps to app
  Encrypted sensor data streams to PC
  Python script decrypts live on screen

Step 2 — Corrupt firmware rejection
  Intentionally corrupt firmware binary
  Bootloader detects bad CRC
  Refuses to jump — stays in update mode
  UART prints: "CYPHER: FIRMWARE VERIFICATION FAILED"

Step 3 — OTA firmware update
  Send new firmware.bin from laptop over UART
  Bootloader receives → writes to Flash → verifies CRC
  Reboots → jumps to new app → system running again
```

No other intern candidate has this demo.

<br>

---

## 💡 &nbsp; Things I Learned Building This

**`volatile` is not optional.**
Without it the compiler caches register values and never re-reads hardware.
Your code looks correct and silently does nothing.

**Always clear before you set.**
MODER uses 2 bits per pin. Setting without clearing first briefly hits
mode `11` (analog). On fast hardware that glitch is real.

**The bootloader changes how you think about memory.**
You stop seeing Flash as one big space and start seeing it as carefully
planned regions — bootloader here, application there, CRC stored here.
Every address is intentional.

**The watchdog is the most underrated peripheral.**
Every deployed IoT device needs one. If firmware hangs,
IWDG forces a hardware reset automatically.
You don't get second chances in the field.

**Encryption is not magic.**
Even a simple XOR cipher with a rolling key and CRC integrity check
demonstrates that you understand data security in embedded systems.
Most embedded engineers never think about this.

<br>

---

## 📚 &nbsp; References

| Document | Description |
|---|---|
| **RM0390** | STM32F446xx Reference Manual |
| **DS10693** | STM32F446RE Datasheet |
| **PM0214** | Cortex-M4 Programming Manual |
| **AN2606** | STM32 Bootloader Application Note |
| **Course** | STM32 Bare Metal Programming — Israel Gbati (Udemy) |

<br>

---

## 👤 &nbsp; About Me

<div align="center">

**Siddhesh Kavitkar**

*Electronics and Communication Engineering Student*

*Targeting Embedded Software Internship — June 2025*

*IoT Startups &nbsp;•&nbsp; Defense / Aerospace &nbsp;•&nbsp; Consumer Electronics*

<br>

I built CYPHER because I wanted to understand embedded systems
the way they actually work — not through library abstractions,
but through the reference manual, one register at a time.

Every line of code in this repository was written by hand.
Every register was looked up in RM0390.
Nothing was auto-generated.

<br>

[![Email](https://img.shields.io/badge/Email-siddheshkavitkar07%40gmail.com-D14836?style=for-the-badge&logo=gmail&logoColor=white)](mailto:siddheshkavitkar07@gmail.com)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Siddhesh%20Kavitkar-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](https://linkedin.com/in/siddhesh-kavitkar-1a3469339)

</div>

<br>

---

<div align="center">

*Built register by register &nbsp;•&nbsp; RM0390 open on every commit &nbsp;•&nbsp; No HAL. Ever.*

</div>
