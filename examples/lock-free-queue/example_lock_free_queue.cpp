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

#include <array>
#include <chrono>
#include <print>
#include <thread>

#include "lock-free-queue/lock_free_queue.h"

namespace {
constexpr std::chrono::seconds inital_sleep_in_seconds(5);
constexpr std::chrono::seconds loop_sleep_in_seconds(1);

struct MyStruct {
    std::array<int, 3> data;
};

auto ConsumeFunction(LockFreeQueue<MyStruct>* data_queue) {
    using namespace std::literals::chrono_literals;

    std::this_thread::sleep_for(inital_sleep_in_seconds);

    while (static_cast<bool>(data_queue->Size())) {
        const auto* const block = data_queue->GetNextRead();

        data_queue->UpdateReadIndex();

        std::println("ConsumeFunction read elem: {}, {}, {} size: {}", block->data.at(0),
                     block->data.at(1), block->data.at(2), data_queue->Size());

        std::this_thread::sleep_for(loop_sleep_in_seconds);
    }

    std::println("ConsumerFunction exiting");
}
}  // namespace

int main() {
    constexpr int data_queue_size = 20;
    constexpr int iteration_loop_size = 50;

    LockFreeQueue<MyStruct> data_queue(data_queue_size);

    std::jthread ctx(ConsumeFunction, &data_queue);

    for (auto i = 0; i < iteration_loop_size; ++i) {
        const MyStruct block{i, i * 10, i * 100};
        *data_queue.GetNextWrite() = block;
        data_queue.UpdateWriteIndex();

        std::println("main constructed elem: {}, {}, {} size: {}", block.data.at(0),
                     block.data.at(1), block.data.at(2), data_queue.Size());

        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(loop_sleep_in_seconds);
    }

    ctx.join();

    std::println("main thread exiting");

    return 0;
}
