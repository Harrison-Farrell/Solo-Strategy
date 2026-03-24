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

#include "utilities/macros.h"

TEST(MarcoTest, assert_test) {
    EXPECT_EXIT(
        {
            ASSERT(true, "Shouldn't die");
            std::exit(0);
        },
        ::testing::ExitedWithCode(0), "");

    EXPECT_EXIT(ASSERT(false, "Death"), testing::ExitedWithCode(1), "Death");
}

TEST(MarcoTest, fatal_test) {
    EXPECT_EXIT(FATAL("Death"), testing::ExitedWithCode(1), "Death");
}
// NOLINTEND