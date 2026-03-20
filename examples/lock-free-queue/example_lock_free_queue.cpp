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

#include <chrono>
#include <thread>

#include "lock-free-queue/lock_free_queue.h"

struct MyStruct {
    int data[3];
};

auto consumeFunction(LockFreeQueue<MyStruct>* data_queue) {
    using namespace std::literals::chrono_literals;

    std::this_thread::sleep_for(5s);

    while (data_queue->size()) {
        const auto d = data_queue->getNextRead();

        data_queue->updateReadIndex();

        std::cout << "consumeFunction read elem:" << d->data[0] << "," << d->data[1] << ","
                  << d->data[2] << " size:" << data_queue->size() << std::endl;

        std::this_thread::sleep_for(1s);
    }

    std::cout << "consumeFunction exiting." << std::endl;
}

int main(int, char**) {
    LockFreeQueue<MyStruct> data_queue(20);

    std::jthread ct(consumeFunction, &data_queue);

    for (auto i = 0; i < 50; ++i) {
        const MyStruct d{i, i * 10, i * 100};
        *data_queue.getNextWrite() = d;
        data_queue.updateWriteIndex();

        std::cout << "main constructed elem:" << d.data[0] << "," << d.data[1] << "," << d.data[2]
                  << " size:" << data_queue.size() << std::endl;

        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

    ct.join();

    std::cout << "main exiting." << std::endl;

    return 0;
}
