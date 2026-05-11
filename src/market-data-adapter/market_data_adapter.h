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

#ifndef SOLO_STRATEGY_SRC_MARKET_DATA_ADAPTER_H_
#define SOLO_STRATEGY_SRC_MARKET_DATA_ADAPTER_H_

#include <memory>
#include <thread>
#include <variant>

#include "itch-parser/messages/itch_messages.h"
#include "lock-free-queue/lock_free_queue.h"
#include "market-orders/market_update.h"

/// @brief Translates raw ITCH protocol messages into normalised MarketUpdate
/// objects that downstream order-book components can consume.
class MarketDataAdapter {
   public:
    MarketDataAdapter(std::shared_ptr<LockFreeQueue<ITCH::Message>>& input_queue,
                      std::shared_ptr<LockFreeQueue<MarketUpdate>>& output_queue);

    /// @brief Executes the Execute() function on the newly created thread
    /// @param core_id Core id to pin the thread affinity onto
    std::shared_ptr<std::thread> Start(int core_id);

    /// @brief Continuously pulls ITCH messages and converts them to
    /// MarketUpdates
    auto Execute() -> void;

   private:
    std::shared_ptr<LockFreeQueue<ITCH::Message>> m_InputQueue;
    std::shared_ptr<LockFreeQueue<MarketUpdate>> m_OutputQueue;

    /// @brief Maps a single ITCH Message to one or more MarketUpdate objects
    /// @param msg The ITCH Message to process
    auto ProcessMessage(const ITCH::Message& msg) -> void;

    /// @brief Helper to push an update to the lock-free output queue
    inline void PushUpdate(const MarketUpdate& update);
};

#endif  // SOLO_STRATEGY_SRC_MARKET_DATA_ADAPTER_H_
