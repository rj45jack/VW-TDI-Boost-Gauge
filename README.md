# VW TDI Boost Gauge

An Arduino-powered digital boost gauge for a 1999–2004.5 VW TDI. An NXP
MPX4250A pressure sensor reads intake pressure after the turbo; an
ATmega8/168/328P drives a 16×2 HD44780 LCD and a two-unit display (PSI / mbar).

The hardware and the first sketch date from 2009–2010. This branch is a
firmware and documentation refresh: the original pin map and splash text are
preserved, but the pressure math, display loop, button handling and build
system have been rebuilt.

[![firmware](https://github.com/rj45jack/VW-TDI-Boost-Gauge/actions/workflows/firmware.yml/badge.svg)](https://github.com/rj45jack/VW-TDI-Boost-Gauge/actions/workflows/firmware.yml)
[![license](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](LICENSE)

Original install video: <https://www.youtube.com/watch?v=Suixj9K7thE>

## What changed in this rewrite

- **Datasheet pressure math.** The sensor is ratiometric and the ADC uses the
  same 5 V rail, so the firmware uses the ADC ratio directly. The old `5.1 V`
  and `4.8828 mV` constants are gone.
- **No more unit-dependent zero offset.** The original added `+1` after
  converting to the selected unit: 1 psi in PSI mode, 1 mbar in mbar mode. The
  rewrite applies a trim in kPa before any conversion.
- **Gauge pressure by default.** The display reads 0 psi at atmospheric
  pressure and negative values under vacuum. Absolute mode is one config flag
  away.
- **Median-of-five filter.** A single electrical spike no longer moves the
  reading.
- **Non-blocking display loop.** No `lcd.clear()` every 200 ms; only rows that
  changed are rewritten, so the display does not flicker.
- **Debounced units button** with the selection stored in EEPROM.
- **Watchdog timer**, useful on a car electrical system.
- **PlatformIO builds** for the ATmega328P, ATmega168 and ATmega8, host-side
  unit tests for the math and filter, and GitHub Actions CI.
- **Cleaner repository**: hardware sources under `hardware/`, Eagle autosave
  backups removed, binary file types declared in `.gitattributes`.

## How it works

The MPX4250A is an **absolute** pressure sensor with a 0–250 kPa range. Its
transfer function, normalised to the supply, is:

```
Vout / Vs = 0.04 + 0.00369 * P[kPa]
```

The ADC reference and the sensor supply are the same 5 V rail, so `Vs`
cancels:

```
P[kPa] = (ADC / 1023 - 0.04) / 0.00369
```

The firmware samples A0 every 25 ms, takes the median of the last five
samples, converts to kPa, optionally subtracts standard atmosphere, then
formats PSI or mbar. The display redraws every 150 ms. The button toggles the
unit on a debounced press and writes the choice to EEPROM address 0.

The original design used two boards: a sensor/power board under the hood and a
display board in the cabin, joined by a Cat5 cable. The cable carries power
and signals; it is not Ethernet. See the hardware caveat below.

## Hardware

| Qty | Part | Notes |
|---|---|---|
| 1 | NXP MPX4250A | 0–250 kPa absolute, 5 V ratiometric |
| 1 | HD44780-compatible 16×2 LCD | 4-bit mode |
| 1 | ATmega8 / 168 / 328P | 16 MHz, Arduino bootloader or ISP |
| 1 | 16 MHz crystal + 2× 22 pF | |
| 1 | 5 V regulator | See schematic |
| 2 | RJ45 8P8C jacks | SparkFun RJ45-8 in the Eagle files |
| 1 | Momentary pushbutton | Units toggle |
| 1 | Trimmer | See schematic |
| 1 | 16-pin LCD header | |
| — | Passives, PCBs, Cat5 cable | See `hardware/` |

The original parts list also calls for a 1999–2004.5 VW TDI, which is the
expensive part.

### Pinout

| Function | Arduino pin | ATmega port |
|---|---|---|
| LCD RS / E / D4–D7 | D12, D11, D5, D4, D3, D2 | PB4, PB3, PD5, PD4, PD3, PD2 |
| Sensor output | A0 | PC0 |
| Units button | D9 | PB1 |

Full LCD header, button wiring and sensor notes:
[`docs/pinout.md`](docs/pinout.md).

## Build and flash

### PlatformIO

```bash
cd firmware
pio run                    # ATmega328P
pio run -e atmega168       # ATmega168
pio run -e atmega8         # ATmega8
pio test -e native         # host-side math and filter tests
pio run -t upload          # flash the default environment
```

PlatformIO installs the `LiquidCrystal` library automatically.

### Arduino IDE

Open `firmware/boostgauge/boostgauge.ino`, select the board and upload. The
IDE's bundled `LiquidCrystal` library works; if your IDE does not have it,
install "LiquidCrystal" by Arduino from the Library Manager.

## Calibration

With the engine off, the gauge should read `0.0 PSI`. If it does not, put the
error in `PRESSURE_TRIM_KPA` in `firmware/boostgauge/config.h` (1 psi =
6.895 kPa) and reflash.

The full transfer function, ADC table and altitude note are in
[`docs/calibration.md`](docs/calibration.md).

## Repository layout

```
firmware/
  platformio.ini
  boostgauge/
    boostgauge.ino
    config.h        pins, display, calibration and timing
    pressure.h      datasheet conversion and unit math
    filter.h        median-of-five filter
  test/
    test_pressure/  host tests for the conversion
    test_filter/    host tests for the filter
hardware/
  display_board/    Eagle 5 sources + Gerbers
  cpu_sensor_board/ Eagle 5 sources + Gerbers
docs/
  pinout.md
  calibration.md
.github/workflows/firmware.yml
```

## Verification

The firmware was compiled with PlatformIO 6.2.0 / Arduino AVR 5.4.0:

| Target | Flash | RAM |
|---|---|---|
| ATmega328P | 6,464 bytes (20.0%) | 142 bytes (6.9%) |
| ATmega168 | 6,464 bytes (45.1%) | 142 bytes (13.9%) |
| ATmega8 | 5,940 bytes (82.9% of 7 KB) | 142 bytes (13.9%) |

`pio test -e native` runs 10 host tests (pressure conversion, unit conversion,
gauge offset, median filter) and all pass.

This is compile-tested and unit-tested, not road-tested. The original install
is from 2010 and no hardware was available to re-flash.

## Hardware caveat

The Eagle projects are binary Eagle 5 files from 2010. Strings inside the
board files show the ATmega8 and MPX4250A library parts in `display_board`,
and the regulator, trimmer and connectors in `cpu_sensor_board` — the opposite
of what the folder names suggest. The original README also says the files may
not be correct.

Open the schematic before fabricating or rewiring. Details:
[`hardware/README.md`](hardware/README.md).

## History

- 2009–2010: original sketch, two PCB revisions, car install.
- 2017–2019: repository published, Eagle files added, README written.
- 2026: this firmware and documentation rewrite.

The original sketch is preserved in the git history before the `firmware/`
move.

## License

Apache-2.0. See [`LICENSE`](LICENSE).
