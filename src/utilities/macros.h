/*
 * ------------------------------------------------------------------------------
 * Author:      Harrison Farrell
 * Project:     Solo-Strategy Trading System
 * Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
 *
 * Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
 * This program is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
 * ------------------------------------------------------------------------------
 */

/**
 * @file macros.h
 * @brief Utility macros for error handling and assertions.
 */

#ifndef SOLO_STRATEGY_SRC_UTILITIES_MACROS_H_
#define SOLO_STRATEGY_SRC_UTILITIES_MACROS_H_

#include <cstring>
#include <iostream>

/**
 * @brief Asserts a condition and terminates the program if it's false.
 * @param cond The condition to evaluate.
 * @param msg The message to print to stderr if the assertion fails.
 */
inline auto ASSERT(bool cond, const std::string& msg) noexcept {
    if (!cond) [[unlikely]] {
        std::cerr << "ASSERT : " << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Prints a fatal error message and terminates the program.
 * @param msg The descriptive fatal error message.
 */
inline auto FATAL(const std::string& msg) noexcept {
    std::cerr << "FATAL : " << msg << std::endl;

    exit(EXIT_FAILURE);
}

#endif  // SOLO_STRATEGY_SRC_UTILITIES_MACROS_H_