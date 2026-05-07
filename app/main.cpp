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
#include "books-manager/books_manager.h"
#include "buildinfo/buildinfo.h"
#include "itch-parser/itch_parser.h"
#include "itch-parser/messages/itch_messages.h"
#include "lock-free-queue/lock_free_queue.h"
#include "memory-map/memory_map_file.h"
#include "thread/thread.h"

namespace {
void ExitSignal(int exit_value) { std::quick_exit(exit_value); }

void ReadWorker(
    const std::string& input_file_path,
    const std::shared_ptr<LockFreeQueue<RawItchPacket>>& input_queue) {
    MemoryMappedFile m_File;
    if (!m_File.Open(input_file_path)) {
        std::cerr << "Failed to open file: " << input_file_path << "\n";
        return;
    }

    while (m_File.Tell() < m_File.Size()) {
        // Read 2-byte length (moldupd64)
        // Nasdaq uses big-endian for this length header
        uint16_t length = m_File.ReadBE<uint16_t>();

        RawItchPacket* packet = nullptr;
        while ((packet = input_queue->GetNextWrite()) == nullptr) {
            std::this_thread::yield();
        }

        packet->length = length;
        if (length > 0 && length <= packet->data.size()) {
            m_File.CopyString(reinterpret_cast<char*>(packet->data.data()),
                              length);
        } else {
            // Handle invalid length or skip
            m_File.Seek(m_File.Tell() + length);
        }
        input_queue->UpdateWriteIndex();
    }

    // Sentinel packet to signal the end of the stream to the ITCH_Parser
    RawItchPacket* packet = nullptr;
    while ((packet = input_queue->GetNextWrite()) == nullptr) {
        std::this_thread::yield();
    }
    packet->length = 0;
    input_queue->UpdateWriteIndex();
}

}  // namespace

int main(int argc, char* argv[]) {
    std::signal(SIGINT, ExitSignal);

    std::cout << "Solo-Strategy Trading System - ITCH Parser" << "\n";
    std::cout << BuildInfo::PrintBuildInfo() << "\n";

    constexpr size_t buffer_size = 1024 * 1024;

    if (argc < 2 || argc > 3) {
        std::cout << "Invalid Arguments!" << "\n";
        std::cout << "Supply the binary file path to read" << "\n";
        std::quick_exit(1);
    }

    // disable cppcoreguidelines-pro-bounds-pointer-arithmetic
    const std::string input_file_path(argv[1]);  // NOLINT

    std::cout << "File Path to read: " << input_file_path << "\n";

    // clang-format off
    // create the thread safe queues to pass data between threads
    auto input_queue = std::make_shared<LockFreeQueue<RawItchPacket>>(buffer_size);
    auto msg_queue = std::make_shared<LockFreeQueue<ITCH::Message>>(buffer_size);
    // clang-format on

    auto reading_thread = CreateAndStartThread(0, "File Reader", ReadWorker,
                                               input_file_path, input_queue);

    ITCH_Parser parser(input_queue, msg_queue);
    auto parsing_thread = parser.Start(1);

    BooksManager books(msg_queue);
    auto books_thread = books.Start(2);

    reading_thread->join();
    std::cout << "File reading completed" << "\n";
    parsing_thread->join();
    std::cout << "Parsing completed" << "\n";
    books_thread->join();
    std::cout << "Books manager completed" << "\n";

    return 0;
}