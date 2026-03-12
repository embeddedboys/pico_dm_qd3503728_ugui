# uGUI Port for Pico_DM_QD3503728

[English](./README.en.md) | [中文](./README.md)

## What is µGUI?

µGUI is a free and open-source graphics library for embedded systems. It is platform-independent and can be easily ported to almost any microcontroller system. As long as the display can show graphics, µGUI is not limited to specific display technologies. Therefore, it supports multiple display technologies such as LCD, TFT, e-ink displays, LED, or OLED. The entire module consists of three files: ugui.c, ugui.h, and ugui_config.h.

### µGUI Features

- µGUI supports any color, grayscale, or monochrome display
- µGUI supports any display resolution
- µGUI supports multiple different displays
- µGUI supports any touchscreen technology (such as resistive screens, capacitive screens)
- µGUI supports windows and controls (such as buttons, text boxes)
- µGUI supports hardware acceleration for specific platforms
- Provides 16 different fonts
- Supports Cyrillic fonts
- Provides TrueType font converter ([https://github.com/AriZuu](https://github.com/AriZuu))
- Integrated and freely scalable system console
- Basic geometric drawing functions (such as lines, circles, borders, etc.)
- Can be easily ported to almost any microcontroller system
- No need for risky dynamic memory allocation

### µGUI Requirements

µGUI is platform-independent, so it does not require a specific embedded system. To use µGUI, you only need to meet two requirements:

- A C function that can control the target display's pixels
- The integer types for the target platform must be adjusted in ugui_config.h

#### Memory Usage

```text
❯ ninja && picotool load -fvx ./pico_dm_qd3503728_ugui.uf2
[2/3] Linking CXX executable pico_dm_qd3503728_ugui.elf
Memory region         Used Size  Region Size  %age Used
           FLASH:       63396 B         4 MB      1.51%
             RAM:       22812 B       512 KB      4.35%
       SCRATCH_X:           0 B         4 KB      0.00%
       SCRATCH_Y:          2 KB         4 KB     50.00%
[3/3] Print target size info
      text       data        bss      total filename
     48040      15356      18988      82384 pico_dm_qd3503728_ugui.elf
```

## Getting Started

Building the firmware

```bash
git clone https://github.com/embeddedboys/pico_dm_qd3503728_ugui.git
cd pico_dm_qd3503728_ugui
git submodule update --init

# Build for Pico 2
mkdir build && cd build
cmake -DPICO_BOARD=pico2 .. -G Ninja
ninja
```

Flashing the firmware to the device

1. Using picotool

With the latest version of picotool, you no longer need to hold down the BOOTSEL button while plugging in to enter UF2 flashing mode. Just keep the USB cable connected and run the following command in the terminal - the device will automatically enter flashing mode.

```bash
# First connect the Pico development board to your computer
picotool load -fvx ./pico_dm_qd3503728_sgl.uf2
```

2. Using .uf2 file

Press and hold the BOOTSEL button while plugging in to enter UF2 flashing mode.

```bash
mkdir -p /tmp/pico
sudo mount /dev/sda1 /tmp/pico
sudo cp pico_dm_qd3503728_sgl.uf2 /tmp/pico
```

3. Using a debugger

Connect the debugger to the Pico.
You need to install `openocd` before running this command.

```bash
ninja flash
```

### Technical Specifications

| Component    | Model                            |
| ------------ | -------------------------------- |
| Main Board   | Raspberry Pi Pico                |
| Display      | 3.5 inch 480x320 ILI9488 non-IPS |
|              | 16-bit 8080 interface 50MHz      |
| Touch Screen | 3.5 inch FT6236 capacitive touch |

### Pin Definitions

| Left Side | Right Side |
| --------- | ---------- |
| GP0/DB0   | VBUS       |
| GP1/DB1   | VSYS       |
| GND       | GND        |
| GP2/DB2   | 3V3_EN     |
| ...       | ...        |

- GP0 ~ GP15 -> ILI9488 16-bit data pins DB0-DB15
- GP18 -> ILI9488 CS (chip select)
- GP19 -> ILI9488 WR (write signal)
- GP20 -> ILI9488 RS (register select, active low: 0 = command, 1 = data)
- GP22 -> ILI9488 Reset (active low)
- GP28 -> ILI9488 Backlight (active high)

## Links

- [https://github.com/achimdoebler/UGUI](https://github.com/achimdoebler/UGUI)
- [https://github.com/embeddedboys/pico_dm_qd3503728_ugui](https://github.com/embeddedboys/pico_dm_qd3503728_ugui)
