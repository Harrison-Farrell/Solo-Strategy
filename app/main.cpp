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

// system includes
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
// local includes
#include "buildinfo/buildinfo.h"
#include "itch-parser/itch_parser.h"
#include "itch-parser/messages/itch_messages.h"
#include "lock-free-queue/lock_free_queue.h"
#include "thread/thread.h"

namespace {
void ExitSignal(int exit_value) { std::quick_exit(exit_value); }

void PrintingPress(
    const std::shared_ptr<LockFreeQueue<ITCH::Message>>& msg_queue) {
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(1s);

    for (const auto* new_message = msg_queue->GetNextRead();
         static_cast<bool>(new_message);
         new_message = msg_queue->GetNextRead()) {
        std::cout << *new_message << "\n";
        msg_queue->UpdateReadIndex();
    }
}
}  // namespace

int main(int argc, char* argv[]) {
    std::signal(SIGINT, ExitSignal);

    std::cout << "Solo-Strategy Trading System - ITCH Parser" << "\n";
    std::cout << BuildInfo::PrintBuildInfo() << "\n";

    constexpr int buffer_size = 1024 * 1024;

    if (argc < 2 || argc > 3) {
        std::cout << "Invalid Arguments!" << "\n";
        std::cout << "Supply the binary file path to read" << "\n";
        std::quick_exit(1);
    }

    // disable cppcoreguidelines-pro-bounds-pointer-arithmetic
    const std::string input_file_path(argv[1]);  // NOLINT

    std::cout << "File Path to read: " << input_file_path << "\n";

    auto msg_queue =
        std::make_shared<LockFreeQueue<ITCH::Message>>(buffer_size);

    ITCH_Parser read_binary(input_file_path, msg_queue);
    auto writing_thread = read_binary.Start(1);

    auto printing_thread =
        CreateAndStartThread(2, "Printing Worker", PrintingPress, msg_queue);

    writing_thread->join();
    printing_thread->join();

    return 0;
}