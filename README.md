# Port uGUI for Pico_DM_QD3503728

## What is µGUI?

µGUI is a free and open source graphic library for embedded systems. It is platform-independent and can be easily ported to almost any microcontroller system. As long as the display is capable of showing graphics, µGUI is not restricted to a certain display technology. Therefore, display technologies such as LCD, TFT, E-Paper, LED or OLED are supported. The whole module consists of three files: ugui.c, ugui.h and ugui_config.h.

### µGUI Features

- µGUI supports any color, grayscale or monochrome display
- µGUI supports any display resolution
- µGUI supports multiple different displays
- µGUI supports any touch screen technology (e.g. AR, PCAP)
- µGUI supports windows and objects (e.g. button, textbox)
- µGUI supports platform-specific hardware acceleration
- 16 different fonts available
- cyrillic fonts supported
- TrueType font converter available ([https://github.com/AriZuu](https://github.com/AriZuu))
- integrated and free scalable system console
- basic geometric functions (e.g. line, circle, frame etc.)
- can be easily ported to almost any microcontroller system
- no risky dynamic memory allocation required

### µGUI Requirements

µGUI is platform-independent, so there is no need to use a certain embedded system. In order to
use µGUI, only two requirements are necessary:

- a C-function which is able to control pixels of the target display.
- integer types for the target platform have to be adjusted in ugui_config.h.

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

## Getting started

Build the firmware

```bash
git clone https://github.com/embeddedboys/pico_dm_qd3503728_ugui.git
cd pico_dm_qd3503728_ugui
git submodule update --init

# Build for pico2
mkdir build && cd build
cmake -DPICO_BOARD=pico2 .. -G Ninja
ninja
```

Load it to your device

1. Use picotool

With the latest picotool, you no longer need to press the BOOTSEL button and plug in the power to enter UF2 burning mode. Simply keep the USB cable connected and run the following command from the terminal, and the device will automatically perform this process.

```bash
# connect the pico board to your computer first
picotool load -fvx ./pico_dm_qd3503728_sgl.uf2
```

2. Use .uf2 file

press the BOOTSEL button and plug in the power to enter UF2 burning mode.

```bash
mkdir -p /tmp/pico
sudo mount /dev/sda1 /tmp/pico
sudo cp pico_dm_qd3503728_sgl.uf2 /tmp/pico
```

3. Use debugger

Connect the debugger with pico.
You need to install `openocd` first to run this command

```bash
ninja flash
```

### Technical specifications

| Part        | Model                       |
| ----------- | --------------------------- |
| Core Board  | Rasberrypi Pico             |
| Display     | 3.5' 480x320 ILI9488 no IPS |
|             | 16-bit 8080 50MHz           |
| TouchScreen | 3.5' FT6236 capacity touch  |

### Pinout

| Left    | Right  |
| ------- | ------ |
| GP0/DB0 | VBUS   |
| GP1/DB1 | VSYS   |
| GND     | GND    |
| GP2/DB2 | 3V3_EN |
| ...     | ...    |

GP0 ~ GP15 -> ILI9488 16 DB0-DB15 pins
GP18 -> ILI9488 CS (Chip select)
GP19 -> ILI9488 WR (write signal)
GP20 -> ILI9488 RS (Register select, Active Low, 0: cmd, 1: data)
GP22 -> ILI9488 Reset (Active Low)
GP28 -> IlI9488 Backlight (Active High)

## Links

- [https://github.com/achimdoebler/UGUI](https://github.com/achimdoebler/UGUI)
- [https://github.com/embeddedboys/pico_dm_qd3503728_ugui](https://github.com/embeddedboys/pico_dm_qd3503728_ugui)
