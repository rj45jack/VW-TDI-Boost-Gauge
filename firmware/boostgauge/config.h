// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <Arduino.h>

// ---------------------------------------------------------------- pins
// These are the values from the original 2009-2010 sketch. They are Arduino
// digital pin numbers, not ATmega port names.
constexpr uint8_t PIN_LCD_RS = 12;
constexpr uint8_t PIN_LCD_EN = 11;
constexpr uint8_t PIN_LCD_D4 = 5;
constexpr uint8_t PIN_LCD_D5 = 4;
constexpr uint8_t PIN_LCD_D6 = 3;
constexpr uint8_t PIN_LCD_D7 = 2;
constexpr uint8_t PIN_SENSOR = A0;
constexpr uint8_t PIN_UNITS_BUTTON = 9;

// ---------------------------------------------------------------- display
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;
constexpr uint8_t BAR_SEGMENTS = 14;
constexpr float BAR_FULL_SCALE_KPA = 172.4f;  // 25 psi of boost

// ---------------------------------------------------------------- pressure
// true  = boost gauge: 0 psi at atmospheric pressure, negative under vacuum
// false = absolute pressure, which is what the original firmware displayed
constexpr bool SHOW_GAUGE_PRESSURE = true;

// Software zero trim in kPa, applied to the absolute reading. If the gauge
// does not read ~0.0 psi with the engine off, put the error here. 1 psi is
// 6.895 kPa, so a 1 psi error is +6.895 or -6.895.
constexpr float PRESSURE_TRIM_KPA = 0.0f;

// The original board reads the button as HIGH when pressed. If you rewire the
// button to ground, set this to true and the internal pull-up is used.
constexpr bool BUTTON_ACTIVE_LOW = false;

// ---------------------------------------------------------------- timing
constexpr uint16_t SAMPLE_INTERVAL_MS = 25;
constexpr uint16_t REDRAW_INTERVAL_MS = 150;
constexpr uint16_t BUTTON_DEBOUNCE_MS = 30;
constexpr uint16_t SPLASH_TIME_MS = 1000;

// Remember the selected unit across power cycles (ATmega EEPROM).
constexpr bool REMEMBER_UNITS = true;
constexpr uint8_t EEPROM_UNITS_ADDRESS = 0;
