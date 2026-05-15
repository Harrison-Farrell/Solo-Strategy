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

/// @file book_builder.h
/// @brief Definitions for the BookBuilder class managing multiple order books.

#ifndef SOLO_STRATEGY_SRC_BOOK_BUILDER_BOOK_BUILDER_H_
#define SOLO_STRATEGY_SRC_BOOK_BUILDER_BOOK_BUILDER_H_

#include <memory>
#include <thread>

#include "lock-free-queue/lock_free_queue.h"
#include "market-orders/market_update.h"
#include "order-book/order_book.h"
#include "quill/Logger.h"

/// @brief Consumes market updates, manages books for each unique ticker.
class BookBuilder final {
   public:
    /// @brief Constructs a BookBuilder.
    /// @param input_queue Queue from which to read market updates.
    explicit BookBuilder(std::shared_ptr<LockFreeQueue<MarketUpdate>> input_queue);

    /// @brief Destructor.
    ~BookBuilder();

    /// Deleted copy & move constructors and assignment-operators.
    BookBuilder(const BookBuilder&) = delete;
    BookBuilder(const BookBuilder&&) = delete;
    BookBuilder& operator=(const BookBuilder&) = delete;
    BookBuilder& operator=(const BookBuilder&&) = delete;

    /// @brief Executes the Execute() function on the newly created thread
    /// @param core_id Core id to pin the thread affinity onto
    auto Start(int core_id) -> std::shared_ptr<std::thread>;

    /// @brief Continuously pulls market updates and dispatches them.
    auto Execute() -> void;

    /// @brief Processes a market update and dispatches it to the
    /// corresponding OrderBook.
    /// @param market_update Pointer to the market update message.
    auto ProcessMarketUpdate(const MarketUpdate* market_update) noexcept
        -> void;

   private:
    std::shared_ptr<LockFreeQueue<MarketUpdate>> m_InputQueue;

    /// @brief Hash map from TickerId to OrderBook pointers.
    OrderBookHashMap m_tickerOrderBook;

    /// @brief Logger for this BookBuilder.
    quill::Logger* m_logger = nullptr;
};

#endif  // SOLO_STRATEGY_SRC_BOOK_BUILDER_BOOK_BUILDER_H_
