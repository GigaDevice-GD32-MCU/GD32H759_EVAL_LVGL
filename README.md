# LVGL V8.3 ported to the GD32H759I-EVAL

This project ports `LVGL V8.3` to the `GD32H759I-EVAL` platform for GUI demonstrations.

## Hardware Information

The `GD32H759I-EVAL` is based on:

- `GD32H759IM` microcontroller (`ARM Cortex-M7` core, maximum frequency `600 MHz`)
- `3840 KB` on-chip Flash memory and `1 MB` on-chip SRAM
- `480 x 272` TFT display (`24-bit RGB via TLI`)
- `GT911` capacitive touch panel (`I2C`)
- `SDRAM / SD card / ADC / I2C / LED / KEY`

## Project Information

- GUI framework: `LVGL V8.3.11`
- Toolchain: `Keil MDK-ARM / IAR / GD32 Embedded Builder`
- Target board: `<GD32H759I-EVAL>`
- Display configuration: `480 x 272 / RGB565 (16-bit) / landscape orientation`


## Third-Party Components

| Category    | In use | Component | Version    | License |
| --------    | ------ | --------- | ---------- | ------- |
| GUI         | `Yes`  | `LVGL`    | `V8.3.11`  | `MIT`   |
| Filesystem  | `Yes`  | `FatFs`   | `R0.16`    | `FatFs license` |


> When introducing a new third-party library, update this table and retain its license text and copyright notices.
