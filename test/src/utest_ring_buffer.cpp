#include <micro/container/ring_buffer.hpp>

#include "utils.hpp"

using namespace micro;

TEST(ring_buffer, constructor) {
    ring_buffer<float, 10> buffer;
    EXPECT_EQ(0, buffer.size());
    EXPECT_EQ(10, buffer.capacity());
}

TEST(ring_buffer, write_read) {
    ring_buffer<uint16_t, 2> buffer;
    
    EXPECT_TRUE(buffer.write(1));
    uint16_t result = 0;
    EXPECT_TRUE(buffer.read(result));
    EXPECT_EQ(1, result);
}

TEST(ring_buffer, multiple_write_read) {
    ring_buffer<uint16_t, 2> buffer;
    
    for (uint16_t i = 1; i < buffer.capacity() * 4; ++i) {
        EXPECT_TRUE(buffer.write(i));
        uint16_t result = 0;
        EXPECT_TRUE(buffer.read(result));
        EXPECT_EQ(i, result);
    }
}

TEST(ring_buffer, overflow_write_read) {
    ring_buffer<uint16_t, 2> buffer;
    
    for (uint16_t i = 0; i < buffer.capacity(); ++i) {
        EXPECT_TRUE(buffer.write(i));
    }

    EXPECT_EQ(buffer.capacity(), buffer.size());

    EXPECT_FALSE(buffer.write(1));

    for (uint16_t i = 0; i < buffer.capacity(); ++i) {
        uint16_t result = 0;
        EXPECT_TRUE(buffer.read(result));
        EXPECT_EQ(i, result);
    }

    EXPECT_EQ(0, buffer.size());
}

TEST(ring_buffer, start_finish_write_read) {
    ring_buffer<uint16_t, 2> buffer;

        uint16_t * const writePtr = buffer.startWrite();
        ASSERT_NE(nullptr, writePtr);
        *writePtr = 1;
        buffer.finishWrite();

        const uint16_t * const readPtr = buffer.startRead();
        ASSERT_NE(nullptr, readPtr);
        EXPECT_EQ(1, *readPtr);
}