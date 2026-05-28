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
#include <stdint.h>

#include <csignal>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>

// local includes
#include "book-builder/book_builder.h"
#include "buildinfo/buildinfo.h"
#include "itch-parser/itch_parser.h"
#include "itch-parser/messages/itch_messages.h"
#include "lock-free-queue/lock_free_queue.h"
#include "market-data-adapter/market_data_adapter.h"
#include "market-orders/market_update.h"
#include "memory-map/memory_map_file.h"
#include "thread/thread.h"

// 3rd party includes
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/FileSink.h"

namespace {
void ExitSignal(int exit_value) { std::quick_exit(exit_value); }

void ReadingWorker(
    const std::string& input_file_path,
    const std::shared_ptr<LockFreeQueue<RawItchPacket>>& input_queue) {
    MemoryMappedFile mapped_file;
    if (!mapped_file.Open(input_file_path)) {
        auto* logger = quill::Frontend::get_logger("root");
        if (static_cast<bool>(logger)) {
            LOG_CRITICAL(logger, "Failed to open file: {}", input_file_path);
        }
        return;
    }

    while (mapped_file.Tell() < mapped_file.Size()) {
        // Read 2-byte length (moldupd64)
        // Nasdaq uses big-endian for this length header
        auto length = mapped_file.ReadBE<uint16_t>();

        while (input_queue->Size() >= input_queue->Capacity()) {
            std::this_thread::yield();
        }
        RawItchPacket* packet = input_queue->GetNextWrite();

        packet->length = length;
        if (length > 0 && length <= packet->data.size()) {
            mapped_file.CopyString(reinterpret_cast<char*>(packet->data.data()),
                                   length);
        } else {
            // Handle invalid length or skip
            mapped_file.Seek(mapped_file.Tell() + length);
        }
        input_queue->UpdateWriteIndex();
    }

    // Sentinel packet to signal the end of the stream to the ITCH_Parser
    while (input_queue->Size() >= input_queue->Capacity()) {
        std::this_thread::yield();
    }
    RawItchPacket* packet = input_queue->GetNextWrite();
    packet->length = 0;
    input_queue->UpdateWriteIndex();

    auto* logger = quill::Frontend::get_logger("root");
    if (static_cast<bool>(logger)) {
        LOG_INFO(logger, "ReadingWorker Finishing");
    }
}

quill::Logger* SetupLogger() {
    const quill::BackendOptions backend_options;
    quill::Backend::start(backend_options);

    auto create_sink = [](const std::string& filename) {
        return quill::Frontend::create_or_get_sink<quill::FileSink>(
            filename, []() {
                quill::FileSinkConfig cfg;
                cfg.set_open_mode('w');
                return cfg;
            }());
    };

    auto order_sink = create_sink("order_book.log");
    auto builder_sink = create_sink("book_builder.log");
    auto market_sink = create_sink("market_data_adapter.log");
    auto root_sink = create_sink("root.log");

    quill::Frontend::create_or_get_logger("OrderBook", order_sink);
    quill::Frontend::create_or_get_logger("BookBuilder", builder_sink);
    quill::Frontend::create_or_get_logger("MarketDataAdapter", market_sink);

    return quill::Frontend::create_or_get_logger("root", root_sink);
}

void RunSystem(const std::string& input_file_path, quill::Logger* root_logger) {
    constexpr int buffer_size = 1024 * 1024 * 64;

    // clang-format off
    auto input_queue = std::make_shared<LockFreeQueue<RawItchPacket>>(buffer_size);
    auto msg_queue = std::make_shared<LockFreeQueue<ITCH::Message>>(buffer_size);
    auto market_update_queue = std::make_shared<LockFreeQueue<MarketUpdate>>(buffer_size);
    // clang-format on

    std::atomic<uint8_t> parser_flag{ThreadState::Run};
    std::atomic<uint8_t> adapter_flag{ThreadState::Run};
    std::atomic<uint8_t> builder_flag{ThreadState::Run};

    auto reading_thread = CreateAndStartThread(0, "File Reader", ReadingWorker,
                                               input_file_path, input_queue);

    ITCH_Parser parser(input_queue, msg_queue);
    auto parser_thread = parser.Start(1, parser_flag);

    MarketDataAdapter adapter(msg_queue, market_update_queue);
    auto market_adapter_thread = adapter.Start(3, adapter_flag);

    BookBuilder book_builder(market_update_queue);
    auto book_builder_thread = book_builder.Start(4, builder_flag);

    LOG_INFO(root_logger,
             "Backend threads started. Processing ITCH data feed...");

    reading_thread->join();
    LOG_INFO(root_logger, "Reading Thread Joined");

    parser_thread->join();
    LOG_INFO(root_logger, "Parser Thread Joined");

    market_adapter_thread->join();
    LOG_INFO(root_logger, "Market Adapter Thread Joined");

    book_builder_thread->join();
    LOG_INFO(root_logger, "Book Builder Thread Joined");
}

}  // namespace

int main(int argc, char* argv[]) {
    std::signal(SIGINT, ExitSignal);

    auto* root_logger = SetupLogger();

    LOG_INFO(root_logger, "Solo-Strategy Trading System - ITCH Parser");
    LOG_INFO(root_logger, "{}", BuildInfo::PrintBuildInfo());

    if (argc != 2) {
        LOG_CRITICAL(root_logger, "Invalid Arguments!");
        LOG_CRITICAL(root_logger, "Usage: main_application <binary_file_path>");
        std::quick_exit(1);
    }

    const std::string input_file_path(argv[1]);  // NOLINT
    LOG_INFO(root_logger, "File Path to read: {}", input_file_path);

    RunSystem(input_file_path, root_logger);

    LOG_INFO(root_logger,
             "Solo-Strategy Trading System - ITCH Parser Finished");
    return 0;
}