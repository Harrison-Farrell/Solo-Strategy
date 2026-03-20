// -----------------------------------------------------------------------------
// Author:      Harrison Farrell
// Project:     Solo-Strategy Trading System
// Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
//
// Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
// This program is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
// -----------------------------------------------------------------------------
// NOLINTBEGIN
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "lock-free-queue/lock_free_queue.h"

/**
 * @brief Test fixture for LockFreeQueue tests.
 */
class LockFreeQueueTest : public ::testing::Test {
   protected:
    const size_t kCapacity = 5;
    LockFreeQueue<int> queue{kCapacity};
};

TEST_F(LockFreeQueueTest, InitialState) {
    EXPECT_EQ(queue.size(), 0);
    EXPECT_EQ(queue.getNextRead(), nullptr);
}

TEST_F(LockFreeQueueTest, PushPopSingleElement) {
    EXPECT_TRUE(queue.push(10));
    EXPECT_EQ(queue.size(), 1);

    int value = 0;
    EXPECT_TRUE(queue.pop(value));
    EXPECT_EQ(value, 10);
    EXPECT_EQ(queue.size(), 0);
}

TEST_F(LockFreeQueueTest, PushToFull) {
    for (size_t i = 0; i < kCapacity; ++i) {
        EXPECT_TRUE(queue.push(static_cast<int>(i)));
    }
    EXPECT_EQ(queue.size(), kCapacity);
    EXPECT_FALSE(queue.push(100));  // Should be full
}

TEST_F(LockFreeQueueTest, PopFromEmpty) {
    int value = -1;
    EXPECT_FALSE(queue.pop(value));
    EXPECT_EQ(value, -1);
}

TEST_F(LockFreeQueueTest, CircularBufferWrapAround) {
    // Fill the queue
    for (size_t i = 0; i < kCapacity; ++i) {
        queue.push(static_cast<int>(i));
    }

    // Pop 3 elements
    int val;
    for (int i = 0; i < 3; ++i) {
        EXPECT_TRUE(queue.pop(val));
        EXPECT_EQ(val, i);
    }
    EXPECT_EQ(queue.size(), kCapacity - 3);

    // Push 3 more elements to trigger wrap-around
    EXPECT_TRUE(queue.push(100));
    EXPECT_TRUE(queue.push(101));
    EXPECT_TRUE(queue.push(102));
    EXPECT_EQ(queue.size(), kCapacity);

    // Verify all remaining elements
    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 3);
    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 4);
    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 100);
    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 101);
    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 102);
    EXPECT_EQ(queue.size(), 0);
}

TEST_F(LockFreeQueueTest, MoveSemantics) {
    LockFreeQueue<std::string> strQueue(2);
    std::string s1 = "hello";
    EXPECT_TRUE(strQueue.push(std::move(s1)));
    EXPECT_TRUE(s1.empty());  // Check if moved

    std::string s2;
    EXPECT_TRUE(strQueue.pop(s2));
    EXPECT_EQ(s2, "hello");
}

TEST_F(LockFreeQueueTest, PointerInterface) {
    int* ptr = queue.getNextWrite();
    ASSERT_NE(ptr, nullptr);
    *ptr = 42;
    queue.updateWriteIndex();
    EXPECT_EQ(queue.size(), 1);

    const int* readPtr = queue.getNextRead();
    ASSERT_NE(readPtr, nullptr);
    EXPECT_EQ(*readPtr, 42);
    queue.updateReadIndex();
    EXPECT_EQ(queue.size(), 0);
}
// NOLINTEND