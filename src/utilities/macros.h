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

/// \file macros.h
/// \brief Utility macros for error handling and assertions.

#ifndef SOLO_STRATEGY_SRC_UTILITIES_MACROS_H_
#define SOLO_STRATEGY_SRC_UTILITIES_MACROS_H_

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/backend/ThreadUtilities.h"

/// \brief Asserts a condition and terminates the program if it's false.
/// \param cond The condition to evaluate.
/// \param msg The message to print to stderr if the assertion fails.
inline auto ASSERT(bool cond, const std::string& msg) noexcept {
    if (!cond) [[unlikely]] {
        auto* logger = quill::Frontend::get_logger("root");
        if (logger) LOG_CRITICAL(logger, "ASSERT : {}", msg);
        std::cerr << "ASSERT : " << msg << "\n";
        std::quick_exit(EXIT_FAILURE);
    }
}

/// \brief Prints a fatal error message and terminates the program.
/// \param msg The descriptive fatal error message.
inline auto FATAL(const std::string& msg) noexcept {
    auto* logger = quill::Frontend::get_logger("root");
    if (logger) LOG_CRITICAL(logger, "FATAL : {}", msg);
    std::cerr << "FATAL : " << msg << "\n";

    std::quick_exit(EXIT_FAILURE);
}

#endif  // SOLO_STRATEGY_SRC_UTILITIES_MACROS_H_