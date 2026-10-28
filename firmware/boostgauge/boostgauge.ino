// SPDX-License-Identifier: Apache-2.0
//
// VW TDI boost gauge firmware.
//
// Original sketch: rj45jack, 2009-2010.
// Modern rewrite: 2026.
//
// Target: ATmega8/168/328P at 16 MHz, HD44780 16x2 LCD in 4-bit mode,
// NXP MPX4250A pressure sensor, one momentary units button.

#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "filter.h"
#include "pressure.h"

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6,
                  PIN_LCD_D7);
MedianFilter sensorFilter;

bool usePsi = true;
bool forceRedraw = true;
bool lastButtonReading = false;
bool stableButtonState = false;
uint32_t lastSampleMs = 0;
uint32_t lastRedrawMs = 0;
uint32_t lastButtonMs = 0;
char row0[LCD_COLUMNS + 1] = "";
char row1[LCD_COLUMNS + 1] = "";

int16_t roundToInt(float value) {
  return static_cast<int16_t>(value >= 0.0f ? value + 0.5f : value - 0.5f);
}

void padToWidth(char* text, size_t size, uint8_t width) {
  size_t length = strlen(text);
  while (length < width && length + 1 < size) {
    text[length++] = ' ';
  }
  text[length] = '\0';
}

void pollButton(uint32_t now) {
  bool pressed =
      digitalRead(PIN_UNITS_BUTTON) == (BUTTON_ACTIVE_LOW ? LOW : HIGH);

  if (pressed != lastButtonReading) {
    lastButtonReading = pressed;
    lastButtonMs = now;
  }

  if ((uint32_t)(now - lastButtonMs) >= BUTTON_DEBOUNCE_MS &&
      pressed != stableButtonState) {
    stableButtonState = pressed;
    if (stableButtonState) {
      usePsi = !usePsi;
      if (REMEMBER_UNITS) {
        EEPROM.update(EEPROM_UNITS_ADDRESS, usePsi ? 0 : 1);
      }
      forceRedraw = true;
    }
  }
}

void formatPressureRow(float kpa, char* out, size_t size) {
  const char* label = SHOW_GAUGE_PRESSURE ? "BOOST" : "MAP";

  if (usePsi) {
    int16_t tenths = roundToInt(pressure::psiFromKpa(kpa) * 10.0f);
    bool negative = tenths < 0;
    int16_t magnitude = negative ? static_cast<int16_t>(-tenths) : tenths;
    snprintf(out, size, "%s %s%d.%d PSI", label, negative ? "-" : "",
             magnitude / 10, magnitude % 10);
  } else {
    int16_t mbar = roundToInt(pressure::mbarFromKpa(kpa));
    bool negative = mbar < 0;
    int16_t magnitude = negative ? static_cast<int16_t>(-mbar) : mbar;
    snprintf(out, size, "%s %s%d mBar", label, negative ? "-" : "", magnitude);
  }

  padToWidth(out, size, LCD_COLUMNS);
}

void formatBarRow(float kpa, char* out, size_t size) {
  uint8_t segments = 0;
  if (kpa > 0.0f) {
    int16_t scaled =
        roundToInt(kpa / BAR_FULL_SCALE_KPA * static_cast<float>(BAR_SEGMENTS));
    if (scaled > BAR_SEGMENTS) {
      scaled = BAR_SEGMENTS;
    }
    if (scaled > 0) {
      segments = static_cast<uint8_t>(scaled);
    }
  }

  if (size < 3) {
    if (size > 0) {
      out[0] = '\0';
    }
    return;
  }

  size_t i = 0;
  out[i++] = 'B';
  out[i++] = ':';
  for (uint8_t s = 0; s < segments && i + 1 < size; ++s) {
    out[i++] = '>';
  }
  while (i + 1 < size && i < LCD_COLUMNS) {
    out[i++] = ' ';
  }
  out[i] = '\0';
}

void draw() {
  float absoluteKpa =
      pressure::kpaFromRaw(sensorFilter.value()) + PRESSURE_TRIM_KPA;
  float shownKpa =
      SHOW_GAUGE_PRESSURE ? pressure::gaugeKpa(absoluteKpa) : absoluteKpa;

  char next0[LCD_COLUMNS + 1];
  char next1[LCD_COLUMNS + 1];
  formatPressureRow(shownKpa, next0, sizeof(next0));
  formatBarRow(shownKpa, next1, sizeof(next1));

  if (forceRedraw || strcmp(next0, row0) != 0) {
    lcd.setCursor(0, 0);
    lcd.print(next0);
    strcpy(row0, next0);
  }
  if (forceRedraw || strcmp(next1, row1) != 0) {
    lcd.setCursor(0, 1);
    lcd.print(next1);
    strcpy(row1, next1);
  }

  forceRedraw = false;
}

void setup() {
  wdt_disable();
  pinMode(PIN_UNITS_BUTTON, BUTTON_ACTIVE_LOW ? INPUT_PULLUP : INPUT);

  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Digital Gauge"));
  lcd.setCursor(0, 1);
  lcd.print(F("rj45jack"));
  delay(SPLASH_TIME_MS);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("VNT15 Boost"));
  lcd.setCursor(0, 1);
  lcd.print(F("Killabytes.net"));
  delay(SPLASH_TIME_MS);
  lcd.clear();

  if (REMEMBER_UNITS) {
    uint8_t stored = EEPROM.read(EEPROM_UNITS_ADDRESS);
    if (stored <= 1) {
      usePsi = (stored == 0);
    }
  }

  // Prime the filter so the first displayed value is not zero.
  uint16_t initial = analogRead(PIN_SENSOR);
  for (uint8_t i = 0; i < 5; ++i) {
    sensorFilter.push(initial);
  }

  lastSampleMs = millis();
  lastRedrawMs = millis();
  wdt_enable(WDTO_2S);
}

void loop() {
  wdt_reset();
  uint32_t now = millis();

  if ((uint32_t)(now - lastSampleMs) >= SAMPLE_INTERVAL_MS) {
    lastSampleMs = now;
    // The AVR ADC needs one conversion to settle after a channel change.
    // There is only one channel here, but the second read is still quieter.
    (void)analogRead(PIN_SENSOR);
    sensorFilter.push(analogRead(PIN_SENSOR));
  }

  pollButton(now);

  if ((uint32_t)(now - lastRedrawMs) >= REDRAW_INTERVAL_MS) {
    lastRedrawMs = now;
    draw();
  }
}
