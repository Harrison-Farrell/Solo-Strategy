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

#ifndef SOLO_STRATEGY_SRC_THREAD_AFFINITY_THREAD_AFFINITY_H_
#define SOLO_STRATEGY_SRC_THREAD_AFFINITY_THREAD_AFFINITY_H_

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/backend/ThreadUtilities.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elifdef defined(__linux__)
    #include <pthread.h>  // Explicitly include for pthread_setaffinity_np
    #include <sys/syscall.h>
    #include <unistd.h>
#endif

enum ThreadState : uint8_t { Run = 0, Pause = 1, Stop = 2 };

/// \brief Set affinity for current thread to be pinned to the provided core_id.
inline auto SetThreadCore(int core_id) noexcept {
#if defined(_WIN32) || defined(_WIN64)
    // Windows uses a bitmask where bit 0 is core 0, bit 1 is core 1, etc.
    HANDLE thread = GetCurrentThread();
    const DWORD_PTR mask = (1ULL << core_id);
    return (SetThreadAffinityMask(thread, mask) != 0);
#elifdef defined(__linux__)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    return (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t),
                                   &cpuset) == 0);
#else
    return false;  // Unsupported OS
#endif
}

/// \brief Creates a thread instance, sets affinity on it, assigns it a name and
/// passes the function to be run on that thread as well as the arguments to the
/// function.
template <typename T, typename... A>
inline auto CreateAndStartThread(int core_id, const std::string& name, T&& func,
                                 A&&... args) noexcept {
    // Captured by value [=] or specific copy/forwarding is safer here to
    // prevent dangling references if the parent scope finishes execution before
    // the thread spins up.
    auto object_thread = std::make_shared<std::thread>(
        [core_id, name, func = std::forward<T>(func),
         ... args = std::forward<A>(args)]() mutable {
            if (core_id >= 0 && !SetThreadCore(core_id)) {
                auto* logger = quill::Frontend::get_logger("root");
                if (logger)
                    LOG_CRITICAL(
                        logger, "Failed to set core affinity for {} to core {}",
                        name, core_id);
                std::quick_exit(EXIT_FAILURE);
            }
            auto* logger = quill::Frontend::get_logger("root");
            if (logger)
                LOG_INFO(logger,
                         "Successfully set core affinity for {} to core {}",
                         name, core_id);

            std::invoke(std::move(func), std::move(args)...);
        });

    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(1s);

    return object_thread;
}

#endif  // SOLO_STRATEGY_SRC_THREAD_AFFINITY_THREAD_AFFINITY_H_