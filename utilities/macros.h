#pragma once

#include <cstring>
#include <iostream>

/// \brief Asserts a condition and prints an error message if the condition is
/// false.
///
/// If the condition is false, prints the provided message to std::cerr and
/// terminates the program.
/// \param cond The condition to check.
/// \param msg The message to display if the assertion fails.
inline auto ASSERT(bool cond, const std::string &msg) noexcept {
    if (!cond) [[unlikely]] {
        std::cerr << "ASSERT : " << msg << std::endl;

        exit(EXIT_FAILURE);
    }
}

/// \brief Prints a fatal error message and terminates the program.
///
/// Prints the provided message to std::cerr and terminates the program.
/// \param msg The fatal error message to display.
inline auto FATAL(const std::string &msg) noexcept {
    std::cerr << "FATAL : " << msg << std::endl;

    exit(EXIT_FAILURE);
}