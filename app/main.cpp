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
#include <memory>
#include <sstream>
#include <string>
#include <thread>
// local includes
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

void PrintingPress(
    const std::shared_ptr<LockFreeQueue<MarketUpdate>>& market_update_queue) {
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(1s);

    auto market_sink = quill::Frontend::create_or_get_sink<quill::FileSink>(
        "market_updates.log", []() {
            quill::FileSinkConfig cfg;
            cfg.set_open_mode('w');
            return cfg;
        }());
    auto* market_logger =
        quill::Frontend::create_or_get_logger("market", std::move(market_sink));

    while (true) {
        const auto* new_message = market_update_queue->GetNextRead();
        if (new_message) {
            if (new_message->type == MarketUpdateType::INVALID) {
                market_update_queue->UpdateReadIndex();
                break;
            }
            LOG_INFO(market_logger, "{}", new_message->ToString());
            market_update_queue->UpdateReadIndex();
        } else {
            std::this_thread::yield();
        }
    }

    LOG_INFO(market_logger, "PrintingPress Finishing");
}

void ITCHPrinterWorker(
    const std::shared_ptr<LockFreeQueue<ITCH::Message>>& input_queue,
    const std::shared_ptr<LockFreeQueue<ITCH::Message>>& output_queue) {
    auto itch_sink =
        quill::Frontend::create_or_get_sink<quill::FileSink>("itch.log", []() {
            quill::FileSinkConfig cfg;
            cfg.set_open_mode('w');
            return cfg;
        }());
    auto* itch_logger =
        quill::Frontend::create_or_get_logger("itch", std::move(itch_sink));

    ITCH::Message msg;
    while (true) {
        if (input_queue->Pop(msg)) {
            if (!std::holds_alternative<std::monostate>(msg)) {
                std::stringstream ss;
                ss << msg;
                LOG_INFO(itch_logger, "{}", ss.str());
            }
            while (!output_queue->Push(msg)) {
                std::this_thread::yield();
            }
            if (std::holds_alternative<std::monostate>(msg)) {
                LOG_INFO(itch_logger, "ITCH Printer Worker Finishing");
                break;
            }
        } else {
            std::this_thread::yield();
        }
    }
}

void ReadingWorker(
    const std::string& input_file_path,
    const std::shared_ptr<LockFreeQueue<RawItchPacket>>& input_queue) {
    MemoryMappedFile m_File;
    if (!m_File.Open(input_file_path)) {
        auto* logger = quill::Frontend::get_logger("root");
        if (logger)
            LOG_CRITICAL(logger, "Failed to open file: {}", input_file_path);
        return;
    }

    while (m_File.Tell() < m_File.Size()) {
        // Read 2-byte length (moldupd64)
        // Nasdaq uses big-endian for this length header
        auto length = m_File.ReadBE<uint16_t>();

        while (input_queue->Size() >= input_queue->Capacity()) {
            std::this_thread::yield();
        }
        RawItchPacket* packet = input_queue->GetNextWrite();

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
    while (input_queue->Size() >= input_queue->Capacity()) {
        std::this_thread::yield();
    }
    RawItchPacket* packet = input_queue->GetNextWrite();
    packet->length = 0;
    input_queue->UpdateWriteIndex();

    auto* logger = quill::Frontend::get_logger("root");
    if (logger) LOG_INFO(logger, "ReadingWorker Finishing");
}

}  // namespace

int main(int argc, char* argv[]) {
    std::signal(SIGINT, ExitSignal);

    quill::BackendOptions backend_options;
    quill::Backend::start(backend_options);

    auto main_sink =
        quill::Frontend::create_or_get_sink<quill::FileSink>("main.log", []() {
            quill::FileSinkConfig cfg;
            cfg.set_open_mode('w');
            return cfg;
        }());
    auto* root_logger =
        quill::Frontend::create_or_get_logger("root", std::move(main_sink));

    LOG_INFO(root_logger, "Solo-Strategy Trading System - ITCH Parser");
    LOG_INFO(root_logger, "{}", BuildInfo::PrintBuildInfo());

    constexpr int buffer_size = 1024 * 1024 * 8;

    if (argc != 2) {
        LOG_CRITICAL(root_logger, "Invalid Arguments!");
        LOG_CRITICAL(root_logger, "Usage: main_application <binary_file_path>");
        std::quick_exit(1);
    }

    // disable cppcoreguidelines-pro-bounds-pointer-arithmetic
    const std::string input_file_path(argv[1]);  // NOLINT
    LOG_INFO(root_logger, "File Path to read: {}", input_file_path);

    // clang-format off
    auto input_queue = std::make_shared<LockFreeQueue<RawItchPacket>>(buffer_size);
    auto msg_queue = std::make_shared<LockFreeQueue<ITCH::Message>>(buffer_size);
    auto engine_msg_queue = std::make_shared<LockFreeQueue<ITCH::Message>>(buffer_size);
    auto market_update_queue = std::make_shared<LockFreeQueue<MarketUpdate>>(buffer_size);
    // clang-format on

    auto reading_thread = CreateAndStartThread(0, "File Reader", ReadingWorker,
                                               input_file_path, input_queue);

    ITCH_Parser parser(input_queue, msg_queue);
    auto parser_thread = parser.Start(1);

    auto itch_printing_thread =
        CreateAndStartThread(2, "ITCH Printing Worker", ITCHPrinterWorker,
                             msg_queue, engine_msg_queue);

    MarketDataAdapter adapter(engine_msg_queue, market_update_queue);
    auto engine_thread = adapter.Start(3);

    auto printing_thread = CreateAndStartThread(
        4, "Printing Worker", PrintingPress, market_update_queue);

    // clang-format off
    reading_thread->join(); LOG_INFO(root_logger, "Reading Thread Joined");
    parser_thread->join();  LOG_INFO(root_logger, "Parser Thread Joined");
    engine_thread->join();  LOG_INFO(root_logger, "Engine Thread Joined");
    printing_thread->join();LOG_INFO(root_logger, "Printing Thread Joined");
    itch_printing_thread->join(); LOG_INFO(root_logger, "ITCH Printing Thread Joined");
    // clang-format on

    LOG_INFO(root_logger,
             "Solo-Strategy Trading System - ITCH Parser Finished");
    return 0;
}