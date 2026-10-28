// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <stdint.h>

namespace pressure {

// NXP MPX4250A transfer function, normalised to the supply voltage:
//
//   Vout / Vs = 0.04 + 0.00369 * P[kPa]
//
// The sensor is ratiometric and the Arduino ADC uses the same 5 V rail as its
// reference, so the supply voltage cancels out. Only the ratio matters.
constexpr float kRatioAtZeroKpa = 0.04f;
constexpr float kRatioPerKpa = 0.00369f;
constexpr float kAdcFullScale = 1023.0f;

constexpr float kKpaPerPsi = 6.894757f;
constexpr float kKpaPerMbar = 0.1f;
constexpr float kStandardAtmosphereKpa = 101.325f;

constexpr float kpaFromRatio(float ratio) {
  return (ratio - kRatioAtZeroKpa) / kRatioPerKpa;
}

constexpr float kpaFromRaw(uint16_t raw) {
  return kpaFromRatio(static_cast<float>(raw) / kAdcFullScale);
}

constexpr float gaugeKpa(float absoluteKpa) {
  return absoluteKpa - kStandardAtmosphereKpa;
}

constexpr float psiFromKpa(float kpa) { return kpa / kKpaPerPsi; }

constexpr float mbarFromKpa(float kpa) { return kpa / kKpaPerMbar; }

}  // namespace pressure
