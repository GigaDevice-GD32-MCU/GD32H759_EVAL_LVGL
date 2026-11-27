# LVGL Ported to GD32H759I EVAL

## Overview

This project ports `LVGL` to the `GD32H759I EVAL` evaluation board for GUI demonstrations.

The board features a `GD32H759IM` microcontroller based on the `ARM Cortex-M7` core, along with a `480×272` RGB TFT display and a capacitive touch panel.
The project uses landscape orientation with the `RGB565` pixel format and 16-bit color depth.

Project toolchains include `Keil MDK-ARM (ARM Compiler 6)`, `IAR`, and `GD32EmbeddedBuilder`.

## Benchmark

The following benchmark results were obtained using `LVGL 9.6.0` with IPA acceleration and double buffering.

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
|:------------------------- | --------:| --------:| ---------:| -----------:| ----------:|
| Empty screen              | 9%       | 33       | 2         | 2           | <1         |
| Moving wallpaper          | 23%      | 33       | 7         | 7           | <1         |
| Single rectangle          | 2%       | 33       | <1        | <1          | <1         |
| Multiple rectangles       | 10%      | 33       | 3         | 3           | <1         |
| Multiple RGB images       | 8%       | 33       | 2         | 2           | <1         |
| Multiple ARGB images      | 16%      | 33       | 4         | 4           | <1         |
| Rotated ARGB images       | 20%      | 33       | 6         | 6           | <1         |
| Multiple labels           | 13%      | 33       | 3         | 3           | <1         |
| Screen sized text         | 44%      | 33       | 13        | 13          | <1         |
| Multiple arcs             | 6%       | 33       | 1         | 1           | <1         |
| Containers                | 10%      | 33       | 4         | 4           | <1         |
| Containers with overlay   | 36%      | 33       | 10        | 10          | <1         |
| Containers with opa       | 13%      | 33       | 4         | 4           | <1         |
| Containers with opa_layer | 19%      | 33       | 8         | 8           | <1         |
| Containers with scrolling | 45%      | 33       | 12        | 12          | <1         |
| Widgets demo              | 57%      | 33       | 15        | 15          | <1         |
| All scenes avg.           | 20%      | 33       | 5         | 5           | <1         |

The following benchmark results were obtained using `LVGL 9.6.0` without IPA acceleration and with double buffering.

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
|:------------------------- | --------:| --------:| ---------:| -----------:| ----------:|
| Empty screen              | 13%      | 33       | 2         | 2           | <1         |
| Moving wallpaper          | 28%      | 33       | 8         | 8           | <1         |
| Single rectangle          | 5%       | 33       | <1        | <1          | <1         |
| Multiple rectangles       | 12%      | 33       | 2         | 2           | <1         |
| Multiple RGB images       | 13%      | 33       | 3         | 3           | <1         |
| Multiple ARGB images      | 18%      | 33       | 5         | 5           | <1         |
| Rotated ARGB images       | 20%      | 33       | 5         | 5           | <1         |
| Multiple labels           | 15%      | 33       | 4         | 4           | <1         |
| Screen sized text         | 48%      | 33       | 14        | 14          | <1         |
| Multiple arcs             | 9%       | 33       | 1         | 1           | <1         |
| Containers                | 11%      | 33       | 4         | 4           | <1         |
| Containers with overlay   | 35%      | 33       | 10        | 10          | <1         |
| Containers with opa       | 14%      | 33       | 5         | 5           | <1         |
| Containers with opa_layer | 19%      | 33       | 8         | 8           | <1         |
| Containers with scrolling | 44%      | 33       | 12        | 12          | <1         |
| Widgets demo              | 57%      | 32       | 15        | 15          | <1         |
| All scenes avg.           | 22%      | 32       | 6         | 6           | <1         |

## Specification

- **MCU:** GD32H759IM (Cortex-M7, 600MHz)
- **RAM:** 1024KB internal SRAM, 32768KB external SDRAM
- **Flash:** 3840KB internal
- **GPU:** IPA
- **Resolution:** 480×272
- **Display Size:** 4.3 inch
- **Interface:** TLI
- **Color Depth:** 16-bit RGB565
- **Technology:** a-Si TFT
- **DPI:** 130 px/inch
- **Touch Pad:** Capacitive touch(I2C)

## Getting Started

The following steps describe a general workflow. Confirm the project paths, software versions,
power supply requirements, and jumper settings against the actual project and the board manual.
This project currently supports three integrated development environments (IDEs): Keil MDK, IAR, and GD32EmbeddedBuilder. This document uses GD32EmbeddedBuilder to demonstrate how to build, download, and debug the project.

### Hardware Setup

1. Confirm that the board model is `GD32H759I EVAL`.
2. Check the TFT LCD display connections according to the board manual.
3. Connect the on-board GD-Link debug interface to your PC, and configure the power supply and jumpers as required.

### Software Setup

1. Install `GD32EmbeddedBuilder` for your operating system. Refer to the GD32EmbeddedBuilder software requirements for details.
2. Install the appropriate GD-Link driver software and confirm that your PC recognizes the debugger.

### Build the Project

1. Clone this repository.
   
   ```shell
   git clone "Repository URL"
   ```

2. Open `GD32EmbeddedBuilder` and click **Browse...** to select a local directory as your workspace.

3. Import the project by selecting **File > Import...** and choosing the project directory (`./Template/GD32EBuilder_project`).

4. Build the project by right-clicking the project in the project explorer and selecting **Build Project**.

5. To download and debug the project, right-click the project in the project explorer and select
   **Debug As > Debug Configurations... > GDB General Debugging**. Configure the settings for your setup,
   then click **Debug** to download the firmware and start debugging.

### Debugging

Start a debug session using your IDE and GD-Link to set breakpoints, step through code, and inspect variables.
If display  issues occur, first check peripheral initialization, SPI/DMA transfer, and display refresh.
The available debug options depend on your IDE and project configuration.

## Third-Party Components

| Category   | In Use | Component | Version  | License         |
| ---------- | ------ | --------- | -------- | --------------- |
| GUI        | Yes    | `LVGL`    | `V9.6.0` | `MIT`           |
| Filesystem | Yes    | `FatFs`   | `R0.16`  | `FatFs license` |

When adding a third-party library, update this table and retain its license text and copyright notices.

## Contribution and Support

If you find any issues with the development board feel free to open an Issue in this repository. For LVGL related issues (features, bugs, etc) please use the main [lvgl repository](https://github.com/lvgl/lvgl).

If you have found a bug and a fix, please submit a pull request. If you are new to pull requests, refer to the [LVGL contribution guide](https://docs.lvgl.io/master/CONTRIBUTING.html#pull-request) to learn the basics.
