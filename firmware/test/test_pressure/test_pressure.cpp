// SPDX-License-Identifier: Apache-2.0
#include <unity.h>

#include "pressure.h"

void test_zero_kpa(void) {
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, pressure::kpaFromRatio(0.04f));
}

void test_atmosphere(void) {
  const float ratio = 0.04f + 0.00369f * 101.325f;
  TEST_ASSERT_FLOAT_WITHIN(0.1f, 101.325f, pressure::kpaFromRatio(ratio));
}

void test_sensor_full_scale(void) {
  // 250 kPa is the top of the MPX4250A range (4.8 V at a 5 V supply).
  const float ratio = 0.04f + 0.00369f * 250.0f;
  TEST_ASSERT_FLOAT_WITHIN(0.5f, 250.0f, pressure::kpaFromRatio(ratio));
}

void test_atmosphere_from_adc(void) {
  // 101.325 kPa is a ratio of 0.4139, which is ADC 423 out of 1023.
  TEST_ASSERT_FLOAT_WITHIN(0.5f, 101.325f, pressure::kpaFromRaw(423));
}

void test_unit_conversions(void) {
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 14.6959f, pressure::psiFromKpa(101.325f));
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 1013.25f, pressure::mbarFromKpa(101.325f));
}

void test_gauge_pressure(void) {
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, pressure::gaugeKpa(101.325f));
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 98.675f, pressure::gaugeKpa(200.0f));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_zero_kpa);
  RUN_TEST(test_atmosphere);
  RUN_TEST(test_sensor_full_scale);
  RUN_TEST(test_atmosphere_from_adc);
  RUN_TEST(test_unit_conversions);
  RUN_TEST(test_gauge_pressure);
  return UNITY_END();
}
