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

#include "thread/thread.h"

// Test 1: Verify setThreadCore succeeds with a valid core (Core 0)
TEST(ThreadTest, SetAffinitySuccess) {
    // This should pass on almost any system as Core 0 always exists
    EXPECT_TRUE(setThreadCore(0));
}

// Test 2: Verify createAndStartThread successfully spins up a thread and runs a
// function
TEST(ThreadTest, CreateAndStartThreadExecutes) {
    std::atomic<bool> work_done{false};

    auto work_func = [&work_done](bool value) { work_done = value; };

    // Spin it on Core 0
    auto* t = createAndStartThread(0, "Test_Worker", work_func, true);

    ASSERT_NE(t, nullptr);  // Ensure pointer is valid
    t->join();
    delete t;

    EXPECT_TRUE(work_done.load());
}

// Test 3: Negative or out-of-bounds core edge-case handling
TEST(ThreadTest, IgnoreNegativeCore) {
    std::atomic<int> counter{0};

    // Passing -1 bypasses the affinity setter (per your header's if condition)
    auto* t = createAndStartThread(-1, "No_Affinity_Worker",
                                           [&counter]() { counter++; });

    t->join();
    delete t;

    EXPECT_EQ(counter.load(), 1);
}

// NOLINTBEGIN