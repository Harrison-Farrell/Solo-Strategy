#ifndef SOLO_STRATEGY_SRC_UTILITIES_MACROS_H_
#define SOLO_STRATEGY_SRC_UTILITIES_MACROS_H_

#include <cstring>
#include <iostream>

/**
 * @brief Asserts a condition and terminates the program if it's false.
 * @param cond The condition to evaluate.
 * @param msg The message to print to stderr if the assertion fails.
 */
inline auto ASSERT(bool cond, const std::string &msg) noexcept {
    if (!cond) [[unlikely]] {
        std::cerr << "ASSERT : " << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Prints a fatal error message and terminates the program.
 * @param msg The descriptive fatal error message.
 */
inline auto FATAL(const std::string &msg) noexcept {
    std::cerr << "FATAL : " << msg << std::endl;

    exit(EXIT_FAILURE);
}

#endif  // SOLO_STRATEGY_SRC_UTILITIES_MACROS_H_