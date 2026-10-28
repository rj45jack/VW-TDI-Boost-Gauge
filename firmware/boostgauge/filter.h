// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <stdint.h>

// Rolling median of five ADC samples. A median rejects the single-sample
// spikes that a moving average would smear into the reading, which matters on
// a car electrical system.
class MedianFilter {
 public:
  explicit MedianFilter(uint16_t initial = 0) {
    for (uint8_t i = 0; i < kSize; ++i) {
      samples_[i] = initial;
    }
  }

  void push(uint16_t value) {
    samples_[index_] = value;
    index_ = static_cast<uint8_t>((index_ + 1) % kSize);
  }

  uint16_t value() const {
    uint16_t sorted[kSize];
    for (uint8_t i = 0; i < kSize; ++i) {
      sorted[i] = samples_[i];
    }

    // Five elements: insertion sort is smaller and faster than pulling in a
    // general-purpose sort.
    for (uint8_t i = 1; i < kSize; ++i) {
      uint16_t key = sorted[i];
      int8_t j = static_cast<int8_t>(i - 1);
      while (j >= 0 && sorted[j] > key) {
        sorted[j + 1] = sorted[j];
        --j;
      }
      sorted[j + 1] = key;
    }

    return sorted[kSize / 2];
  }

 private:
  static const uint8_t kSize = 5;
  uint16_t samples_[kSize];
  uint8_t index_ = 0;
};
