// SPDX-License-Identifier: Apache-2.0
#include <unity.h>

#include "filter.h"

void test_single_spike_is_rejected(void) {
  MedianFilter filter(0);
  filter.push(100);
  filter.push(100);
  filter.push(100);
  filter.push(100);
  filter.push(1000);
  TEST_ASSERT_EQUAL_UINT16(100, filter.value());
}

void test_median_of_five(void) {
  MedianFilter filter(0);
  filter.push(30);
  filter.push(10);
  filter.push(40);
  filter.push(20);
  filter.push(50);
  TEST_ASSERT_EQUAL_UINT16(30, filter.value());
}

void test_rolling_window(void) {
  MedianFilter filter(0);
  for (int i = 0; i < 5; ++i) {
    filter.push(100);
  }
  TEST_ASSERT_EQUAL_UINT16(100, filter.value());

  filter.push(0);
  TEST_ASSERT_EQUAL_UINT16(100, filter.value());
}

void test_constant_input_stays_constant(void) {
  MedianFilter filter(0);
  for (int i = 0; i < 20; ++i) {
    filter.push(423);
  }
  TEST_ASSERT_EQUAL_UINT16(423, filter.value());
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_single_spike_is_rejected);
  RUN_TEST(test_median_of_five);
  RUN_TEST(test_rolling_window);
  RUN_TEST(test_constant_input_stays_constant);
  return UNITY_END();
}
