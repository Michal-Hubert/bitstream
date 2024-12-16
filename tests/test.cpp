#include "bitstream.h"
#include "utils.h"
#include "gtest/gtest.h"

TEST(BitstreamTest, Initial) {
  Bitstream<uint8_t> bs;
  ASSERT_EQ(bs.getSizeInBits(), 0);
}

TEST(BitstreamTest, Mask) {
  Bitstream<> bs;
  ASSERT_EQ(Utils::getMask<uint8_t>(1, Utils::BIT::MSB), 0b10000000);
  ASSERT_EQ(Utils::getMask<uint8_t>(1, Utils::BIT::LSB), 0b00000001);

  ASSERT_EQ(Utils::getMask<uint8_t>(2, Utils::BIT::MSB), 0b11000000);
  ASSERT_EQ(Utils::getMask<uint8_t>(2, Utils::BIT::LSB), 0b00000011);

  ASSERT_EQ(Utils::getMask<uint8_t>(8, Utils::BIT::MSB), 0b11111111);
  ASSERT_EQ(Utils::getMask<uint8_t>(8, Utils::BIT::LSB), 0b11111111);

  ASSERT_EQ(Utils::getMask<uint32_t>(10, Utils::BIT::MSB), 0xFFC00000);
  ASSERT_EQ(Utils::getMask<uint32_t>(10, Utils::BIT::LSB), 0x000003FF);
}

TEST(BitstreamTest, GetBits) {
  ASSERT_EQ(Utils::getBits(uint16_t(0b1100100000000000), 5, Utils::MSB), 0b11001);
  ASSERT_EQ(Utils::getBits(uint16_t(0b11011001), 5, Utils::LSB), 0b11001);
  ASSERT_EQ(Utils::getSubsequenceBits(uint16_t(0b1100100),5,2), 0b1001);
}

TEST(BitstreamTest, PutUnsafe) {
  Bitstream<uint16_t> bs;

  std::array<uint16_t, 8> expected = {
    0b1100000000000000,
    0b1111000000000000,
    0b1111110000000000,
    0b1111111100000000,
    0b1111111111000000,
    0b1111111111110000,
    0b1111111111111100,
    0b1111111111111111
  };

  for (size_t i = 0; i < 8; ++i)
  {
    bs.grow(2);
    bs.putUnsafe(0b11, 2);
    ASSERT_EQ(bs.m_stream.size(), 1);
    ASSERT_EQ(bs.getSizeInBits(), 2*(i+1));
    ASSERT_EQ(bs.m_stream[0], expected[i]);
  }
}

TEST(BitstreamTest, PutUnsafe1) {
  Bitstream<uint16_t> bs;
  bs.m_write_index = 6;
  bs.grow(6);
  bs.putUnsafe(0b111111, 6);
  ASSERT_EQ(bs.m_stream[0], 0b0000001111110000);
}

TEST(BitstreamTest, GetUnsafe) {
  Bitstream<uint8_t> bs;
  bs.grow(5);
  bs.putUnsafe(0b11111, 5);
  ASSERT_EQ(bs.m_stream[0], 0b11111000);
  auto res = bs.getUnsafe<uint8_t>(5);
  ASSERT_EQ(res, 0b00011111);
}

TEST(BitstreamTest, GetUnsafe1) {
  Bitstream<uint16_t> bs;
  bs.grow(16);
  bs.putUnsafe(0b0000011111000000, 16);
  ASSERT_EQ(bs.m_stream[0], 0b0000011111000000);
  auto res = bs.getUnsafe<uint8_t>(3);
  ASSERT_EQ(res, 0b000);
  res = bs.getUnsafe<uint8_t>(3);
  ASSERT_EQ(res, 0b001);
  res = bs.getUnsafe<uint8_t>(3);
  ASSERT_EQ(res, 0b111);
  res = bs.getUnsafe<uint8_t>(3);
  ASSERT_EQ(res, 0b100);
  res = bs.getUnsafe<uint8_t>(3);
  ASSERT_EQ(res, 0b0);
  ASSERT_EQ(bs.m_read_index, 15);
}

TEST(BitstreamTest, GetUnsafeMultiByte) {
  Bitstream<uint16_t> bs;
  bs.grow(16);
  bs.putUnsafe(0b0000001111001100, 16);
  auto res = bs.getUnsafe<uint16_t>(10);
  ASSERT_EQ(res, 0b0000001111);
}

TEST(BitstreamTest, PutBit) {
  Bitstream<uint16_t> bs;
  for (size_t i = 0; i < 24; i++)
  {
    bs.putBit(i % 2);
    ASSERT_EQ(bs.getSizeInBits(), i + 1);
  }
  ASSERT_EQ(bs.getSizeInBits(), 24);
  for (size_t i = 0; i < 24; i++)
  {
    auto res = bs.getBit();
    ASSERT_EQ(res, i % 2);
  }
}

TEST(BitstreamTest, Put) {
  {
    Bitstream<uint16_t> bs;
    bs.put(0b000111000111001, 12);
    ASSERT_EQ(bs.getSizeInBits(), 12);
    auto r = bs.get<uint16_t>(12);
    ASSERT_EQ(r, 0b111000111001);
    ASSERT_EQ(bs.getSizeInBits(), 0);
  }
  {
    Bitstream<uint8_t> bs;
    bs.put(0b000111000111001, 12);
    ASSERT_EQ(bs.getSizeInBits(), 12);
    auto r = bs.get<uint16_t>(12);
    ASSERT_EQ(r, 0b111000111001);
    ASSERT_EQ(bs.getSizeInBits(), 0);
  }
  {
    Bitstream<uint8_t> bs;
    bs.put(0b11001100110011, 7);
    bs.put(0b101010101010, 9);
    ASSERT_EQ(bs.getSizeInBits(), 16);
  }
}
