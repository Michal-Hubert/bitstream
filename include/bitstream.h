#pragma once

#include "utils.h"
#include <cstdint>
#include <deque>
#include <gtest/gtest_prod.h>

template<typename CHUNKTYPE = uint8_t>
class Bitstream {
public:
  size_t getSizeInBits() const {
    return m_write_index - m_read_index;
  }

  /**
   * Put lsb_bits from val without checking if there is enough space in the current chunk
   */
  template<typename T>
  void putUnsafe(T val, size_t lsb_bits) {
    val = val & Utils::getMask<T>(lsb_bits, Utils::LSB);
    while (lsb_bits)
    {
      size_t write_byte_index = m_write_index / Utils::sizeInBits<CHUNKTYPE>();
      size_t write_bit_index = m_write_index % Utils::sizeInBits<CHUNKTYPE>();
      CHUNKTYPE &chunk = m_stream[write_byte_index];
      size_t chunk_capacity = Utils::sizeInBits<CHUNKTYPE>() - write_bit_index;
      size_t can_write_bits = std::min(chunk_capacity, lsb_bits);
      auto current_chunk_val = Utils::getSubsequenceBits(val, lsb_bits, lsb_bits - can_write_bits);
      current_chunk_val <<= (chunk_capacity - can_write_bits);
      chunk |= current_chunk_val;
      m_write_index += can_write_bits;
      lsb_bits -= can_write_bits;
    }
  }

  /**
   * Get num of bits from stream. Does not reduce size of stream after getting.
   */
  template<typename T>
  T getUnsafe(size_t bits) {
    T result;
    while (bits)
    {
      result <<= bits;
      size_t read_byte_index = m_read_index / Utils::sizeInBits<CHUNKTYPE>();
      size_t read_bit_index = m_read_index % Utils::sizeInBits<CHUNKTYPE>();
      CHUNKTYPE chunk = m_stream[read_byte_index];
      size_t chunk_capacity = Utils::sizeInBits<CHUNKTYPE>() - read_bit_index;
      size_t can_read_bits = std::min(chunk_capacity, bits);
      CHUNKTYPE mask = Utils::getMask<CHUNKTYPE>(can_read_bits, Utils::LSB) << (Utils::sizeInBits<CHUNKTYPE>() - can_read_bits - read_bit_index);
      CHUNKTYPE read_chunk_val = chunk & mask;
      read_chunk_val >>= (Utils::sizeInBits<CHUNKTYPE>() - can_read_bits - read_bit_index);//read value shall be as of LSB position
      result |= read_chunk_val;
      //result <<= can_read_bits;
      m_read_index += can_read_bits;
      bits -= can_read_bits;
    }
    return result;
  }

  /**
   * Put num of the least significant bits from val into stream
   */
  template<typename T>
  void put(T val, size_t lsb_bits) {
    grow(lsb_bits);
    putUnsafe(val, lsb_bits);
  }

  /**
   * Get num of bits from stream. Reduce size of stream if possible.
   */
  template<typename T>
  T get(size_t bits) {
    auto res = getUnsafe<T>(bits);
    shrink();
    return res;
  }

  /**
   * Put single bit.
   */
  void putBit(bool bit) {
    if (bit)
    {
      put(0x1, 1);
    }
    else
    {
      put(0x0, 1);
    }
  }

  /**
   *
   * Get single bit
   */
  bool getBit() {
    if (get<uint8_t>(1))
    {
      return true;
    }
    return false;
  }

  /**
   * Get given chunk from stream.
   */
  CHUNKTYPE getChunk(size_t i) const {
    return m_stream[i];
  }

  FRIEND_TEST(BitstreamTest, PutUnsafe);
  FRIEND_TEST(BitstreamTest, PutUnsafe1);
  FRIEND_TEST(BitstreamTest, GetUnsafe);
  FRIEND_TEST(BitstreamTest, GetUnsafe1);
  FRIEND_TEST(BitstreamTest, GetUnsafeMultiByte);

private:
  /**
   * Grow stream by extra_size
   */
  void grow(size_t extra_size) {
    auto size_bits = getSizeInBits() + extra_size;
    if (size_bits > m_stream.size() * Utils::sizeInBits<CHUNKTYPE>())
    {
      auto newsize = size_bits / Utils::sizeInBits<CHUNKTYPE>() + 1;
      m_stream.resize(newsize);
    }
  }
  /**
   * Shrink stream if possible
   */
  void shrink() {
    if (m_read_index / Utils::sizeInBits<CHUNKTYPE>() > sizeof(CHUNKTYPE))
    {
      m_stream.pop_front();
    }
    if (getSizeInBits() == 0)
    {
      m_read_index = m_write_index = 0;
    }
  }
  size_t m_read_index{0};
  size_t m_write_index{0};
  std::deque<CHUNKTYPE> m_stream;
};
