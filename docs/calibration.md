# Calibration

## Transfer function

The NXP MPX4250A is a ratiometric absolute pressure sensor with a 0–250 kPa
range. Its datasheet transfer function, normalised to the supply voltage, is:

```
Vout / Vs = 0.04 + 0.00369 * P[kPa]
```

Solving for pressure:

```
P[kPa] = (Vout / Vs - 0.04) / 0.00369
```

The Arduino ADC measures `Vout` against the same 5 V rail that powers the
sensor, so `Vs` cancels out. The firmware therefore uses the ADC ratio
directly:

```
P[kPa] = (ADC / 1023 - 0.04) / 0.00369
```

There is no `5.1 V` or `4.8828 mV` constant to tune. That is why the rewrite
dropped the original `vs` and `a` variables.

## ADC values

| Absolute pressure | Vout / Vs | ADC (0–1023) | Gauge pressure |
|---|---|---|---|
| 0 kPa | 0.0400 | 41 | -14.70 psi |
| 100 kPa | 0.4090 | 418 | -0.19 psi |
| 101.325 kPa | 0.4139 | 423 | 0.00 psi |
| 150 kPa | 0.5935 | 607 | 7.06 psi |
| 200 kPa | 0.7780 | 796 | 14.31 psi |
| 250 kPa | 0.9625 | 985 | 21.56 psi |

The sensor tops out at 4.8 V, not 5.0 V, so the last usable ADC value is about
985, not 1023.

## Gauge vs absolute

`SHOW_GAUGE_PRESSURE` in `firmware/boostgauge/config.h` selects the mode:

- `true` (default): subtract 101.325 kPa, so the display reads 0 psi with the
  engine off and negative values under vacuum. This is what a boost gauge
  should do.
- `false`: show absolute pressure, which is what the original firmware did.

The original code added `1` **after** converting to the selected unit. That is
1 psi in PSI mode (6.895 kPa) but 1 mbar in mbar mode (0.1 kPa), so the zero
offset changed by a factor of about 69 when the button was pressed. The
rewrite applies `PRESSURE_TRIM_KPA` in kPa, before any unit conversion.

## Zero trim

1. Key on, engine off. The gauge should read `0.0 PSI` (or `0 mBar`).
2. Note the error. 1 psi is 6.895 kPa; 1 mbar is 0.1 kPa.
3. Set `PRESSURE_TRIM_KPA` in `firmware/boostgauge/config.h` to the error.
   A gauge reading 1.5 psi high needs `PRESSURE_TRIM_KPA = -10.34`.
4. Rebuild and flash.

The trim is a single additive offset. If the error is a scale error (correct
at 0 but wrong at 20 psi), the sensor or its 5 V supply is suspect; check the
supply and the sensor output before adding a slope correction.

## Altitude

`SHOW_GAUGE_PRESSURE` subtracts standard sea-level atmosphere (101.325 kPa).
At 1,500 m the true atmospheric pressure is roughly 84 kPa, so the gauge will
read about -2.5 psi with the engine off. Use the local pressure in
`pressure::kStandardAtmosphereKpa`, or set `PRESSURE_TRIM_KPA` to +17 kPa to
compensate.

## Range

The MPX4250A reads up to 250 kPa absolute, which is about 21.6 psi of boost at
sea level. A higher-boost engine needs a higher-range sensor; do not exceed
the sensor's rated pressure.
