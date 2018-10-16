#include <stdint.h>
#include <string.h>

#include "gtest/gtest.h"

#include "apps/libmc/sim/cows.h"

namespace libmc {
namespace sim {
namespace testing {
namespace {

// The size of the buffer to test with.
constexpr int kBufferSize = 1024;

// Checks that we don't have any extraneous packet separators within the packet.
// Args:
//  packet: The packet to check.
//  length: The length of the packet.
void CheckInternalSeparators(const uint16_t *packet, int length) {
  for (int i = 1; i < length; ++i) {
    ASSERT_NE(0, packet[i]);
  }
}

// Compares two packets and makes sure they're identical.
// Args:
//  packet1: The first packet.
//  packet2: The second packet.
//  length: The size of the packets.
void ComparePackets(const uint16_t *packet1, const uint16_t *packet2,
                    int length) {
  // The first two words are reserved by the COWS algorithm.
  for (int i = 2; i < length; ++i) {
    EXPECT_EQ(packet2[i], packet1[i]);
  }
}

}  // namespace

class CowsTest : public ::testing::Test {
 protected:
  CowsTest() : test_buffer_(new uint16_t[kBufferSize]) {}
  ~CowsTest() {
    delete[] test_buffer_;
  }

  virtual void SetUp() {
    // Fill it with a pattern.
    test_buffer_[2] = 0;
    uint16_t base = 3;
    for (int i = 3; i < kBufferSize; ++i) {
      if (!test_buffer_[i - 1]) {
        test_buffer_[i] = base;
        base *= 5;
      } else {
        test_buffer_[i] = test_buffer_[i - 1] << 1;
      }
    }
  }

  // Large buffer for testing purposes.
  uint16_t *test_buffer_;
};

// Make sure we can stuff and unstuff something.
TEST_F(CowsTest, StuffUnstuffTest) {
  // Copy the testing buffer so we have something to compare with.
  uint16_t *test_buffer_copy = new uint16_t[kBufferSize];
  memcpy(test_buffer_copy, test_buffer_, sizeof(uint16_t) * kBufferSize);

  // Stuff it.
  CowsStuff(test_buffer_, kBufferSize);

  // The packet separator should have been added.
  EXPECT_EQ(0, test_buffer_[0]);
  // Check that we don't have extraneous zeros.
  CheckInternalSeparators(test_buffer_, kBufferSize);

  // Unstuff it.
  CowsUnstuff(test_buffer_, kBufferSize);

  // Check that it's the same.
  ComparePackets(test_buffer_, test_buffer_copy, kBufferSize);

  delete[] test_buffer_copy;
}

// Make sure we can stuff and unstuff a packet with no zeros.
TEST_F(CowsTest, NoZerosTest) {
  // Stuff the packet, which by definition removes the zeros.
  CowsStuff(test_buffer_, kBufferSize);
  CheckInternalSeparators(test_buffer_, kBufferSize);

  // Copy the testing buffer so we have something to compare with.
  uint16_t *test_buffer_copy = new uint16_t[kBufferSize];
  memcpy(test_buffer_copy, test_buffer_, sizeof(uint16_t) * kBufferSize);

  // Stuff it again.
  CowsStuff(test_buffer_, kBufferSize);
  CheckInternalSeparators(test_buffer_, kBufferSize);

  // Make sure that the next zero word points to the start of the next packet.
  EXPECT_EQ(kBufferSize - 1, test_buffer_[1]);

  // Unstuff it.
  CowsUnstuff(test_buffer_, kBufferSize);
  // Check that it's the same.
  ComparePackets(test_buffer_, test_buffer_copy, kBufferSize);

  delete[] test_buffer_copy;
}

// Make sure we can stuff a packet that's all zeros.
TEST_F(CowsTest, ZeroPacketTest) {
  // Fill a packet with zeros.
  memset(test_buffer_, 0, sizeof(uint16_t) * kBufferSize);

  // Copy the testing buffer so we have something to compare with.
  uint16_t *test_buffer_copy = new uint16_t[kBufferSize];
  memcpy(test_buffer_copy, test_buffer_, sizeof(uint16_t) * kBufferSize);

  // Stuff it.
  CowsStuff(test_buffer_, kBufferSize);
  CheckInternalSeparators(test_buffer_, kBufferSize);

  // Unstuff it.
  CowsUnstuff(test_buffer_, kBufferSize);
  // Check that it's the same.
  ComparePackets(test_buffer_, test_buffer_copy, kBufferSize);

  delete[] test_buffer_copy;
}

}  // namespace testing
}  // namespace sim
}  // namespace libmc
