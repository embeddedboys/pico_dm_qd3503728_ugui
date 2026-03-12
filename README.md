# Pico_DM_QD3503728 的 uGUI 移植版本

[English](./README.en.md) | [中文](./README.md)

## 什么是 µGUI？

µGUI 是一个适用于嵌入式系统的免费开源图形库。它与平台无关，可以轻松移植到几乎任何微控制器系统。只要显示屏能够显示图形，µGUI 就不受特定显示技术的限制。因此，它支持 LCD、TFT、电子墨水屏、LED 或 OLED 等多种显示技术。整个模块由三个文件组成：ugui.c、ugui.h 和 ugui_config.h。

### µGUI 特性

- µGUI 支持任何彩色、灰度或单色显示屏
- µGUI 支持任何显示分辨率
- µGUI 支持多个不同的显示屏
- µGUI 支持任何触摸屏技术（例如电阻屏、电容屏）
- µGUI 支持窗口和控件（例如按钮、文本框）
- µGUI 支持特定平台的硬件加速
- 提供 16 种不同的字体
- 支持西里尔字体
- 提供 TrueType 字体转换器（[https://github.com/AriZuu](https://github.com/AriZuu)）
- 集成且可自由缩放的系统控制台
- 基本几何绘图功能（例如直线、圆形、边框等）
- 可以轻松移植到几乎任何微控制器系统
- 无需使用有风险的动态内存分配

### µGUI 要求

µGUI 与平台无关，因此不需要使用特定的嵌入式系统。要使用 µGUI，只需满足两个要求：

- 一个能够控制目标显示屏像素的 C 函数
- 必须在 ugui_config.h 中调整目标平台的整数类型

#### 内存使用情况

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

## 开始使用

编译固件

```bash
git clone https://github.com/embeddedboys/pico_dm_qd3503728_ugui.git
cd pico_dm_qd3503728_ugui
git submodule update --init

# 为 Pico 2 编译
mkdir build && cd build
cmake -DPICO_BOARD=pico2 .. -G Ninja
ninja
```

将固件烧录到设备

1. 使用 picotool

使用最新版本的 picotool，您不再需要按住 BOOTSEL 按钮再插电进入 UF2 烧录模式。只需保持 USB 线连接，在终端运行以下命令，设备会自动进入烧录模式。

```bash
# 先将 Pico 开发板连接到电脑
picotool load -fvx ./pico_dm_qd3503728_sgl.uf2
```

2. 使用 .uf2 文件

按住 BOOTSEL 按钮再插电进入 UF2 烧录模式。

```bash
mkdir -p /tmp/pico
sudo mount /dev/sda1 /tmp/pico
sudo cp pico_dm_qd3503728_sgl.uf2 /tmp/pico
```

3. 使用调试器

将调试器连接到 Pico。
运行此命令前需要先安装 `openocd`

```bash
ninja flash
```

### 技术规格

| 部件   | 型号                            |
| ------ | ------------------------------- |
| 核心板 | 树莓派 Pico                     |
| 显示屏 | 3.5 英寸 480x320 ILI9488 非 IPS |
|        | 16位 8080 接口 50MHz            |
| 触摸屏 | 3.5 英寸 FT6236 电容触控        |

### 引脚定义

| 左侧    | 右侧   |
| ------- | ------ |
| GP0/DB0 | VBUS   |
| GP1/DB1 | VSYS   |
| GND     | GND    |
| GP2/DB2 | 3V3_EN |
| ...     | ...    |

- GP0 ~ GP15 -> ILI9488 16位数据引脚 DB0-DB15
- GP18 -> ILI9488 CS（片选）
- GP19 -> ILI9488 WR（写信号）
- GP20 -> ILI9488 RS（寄存器选择，低电平有效，0：命令，1：数据）
- GP22 -> ILI9488 复位（低电平有效）
- GP28 -> ILI9488 背光（高电平有效）

## 链接

- [https://github.com/achimdoebler/UGUI](https://github.com/achimdoebler/UGUI)
- [https://github.com/embeddedboys/pico_dm_qd3503728_ugui](https://github.com/embeddedboys/pico_dm_qd3503728_ugui)
