#pragma once
#include <climits>
#include <cstddef>
#include <cstdint>

namespace Utils
{

enum BIT : uint8_t {
  MSB,
  LSB
};

template<typename T>
T getMask(size_t bits, BIT bit) {
  if (bit == BIT::MSB)
  {
    T res = ((T) -1) >> bits;
    return ~res;
  }
  if (bits == sizeof(T) * CHAR_BIT)
  {
    return static_cast<T>(-1);
  }
  return static_cast<T>((0x1u << bits) - 1);
}

template<typename T>
size_t sizeInBits() {
  return 8 * sizeof(T);
}

/**
 * Get num_of_bits MSB or LSB bits from val
 */
template<typename T>
T getBits(T val, size_t num_of_bits, BIT bit) {
  auto mask = getMask<T>(num_of_bits, bit);
  if (bit == MSB)
  {
    val = val & mask;
    val >>= (sizeInBits<T>() - num_of_bits);
    return val;
  }
  return val & mask;
}

/**
 * Get subsequnce of bits [from, to] counted from LSB. Counting from zero.
 */
template<typename T>
T getSubsequenceBits(T val, size_t to, size_t from) {
  auto res = getBits(val, to + 1, Utils::LSB);
  return res >> from;
}

}// namespace Utils