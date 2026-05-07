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

/// @file books_manager.h
/// @brief Consumes parsed ITCH messages and maintains per-symbol order books.

#ifndef SOLO_STRATEGY_SRC_BOOKS_MANAGER_BOOKS_MANAGER_H_
#define SOLO_STRATEGY_SRC_BOOKS_MANAGER_BOOKS_MANAGER_H_

// system includes
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

// local includes
#include "itch-parser/messages/itch_messages.h"
#include "lock-free-queue/lock_free_queue.h"
#include "market-orders/market_update.h"
#include "order-book/order_book.h"
#include "utilities/types.h"

/// @brief Manages a collection of MarketOrderBooks, one per traded symbol.
///
/// BooksManager consumes ITCH::Message variants from a lock-free queue produced
/// by the ITCH_Parser and translates them into MEMarketUpdate operations that
/// drive the individual MarketOrderBook instances.
class BooksManager {
   public:
    /// @brief Constructs a BooksManager that reads from the given message
    /// queue.
    /// @param input_queue Shared pointer to the ITCH message lock-free queue.
    explicit BooksManager(
        std::shared_ptr<LockFreeQueue<ITCH::Message>>& input_queue);

    /// Deleted default, copy & move constructors and assignment-operators.
    BooksManager() = delete;
    BooksManager(const BooksManager&) = delete;
    BooksManager(const BooksManager&&) = delete;
    BooksManager& operator=(const BooksManager&) = delete;
    BooksManager& operator=(const BooksManager&&) = delete;

    ~BooksManager();

    /// @brief Launches the Execute() loop on a dedicated thread pinned to the
    /// given core.
    /// @param core_id CPU core to pin the worker thread to.
    /// @return Shared pointer to the running thread.
    std::shared_ptr<std::thread> Start(int core_id);

    /// @brief Main processing loop. Reads ITCH messages from the input queue
    /// and applies them to the appropriate order book until a std::monostate
    /// sentinel is received.
    auto Execute() -> void;

    /// @brief Returns a pointer to the order book for the given ticker.
    /// @param ticker_id The internal ticker identifier.
    /// @return Pointer to the MarketOrderBook, or nullptr if not allocated.
    auto GetOrderBook(TickerId ticker_id) const noexcept -> MarketOrderBook* {
        if (ticker_id >= ME_MAX_TICKERS) return nullptr;
        return m_OrderBooks[ticker_id];
    }

    /// @brief Returns the TickerId assigned to a stock symbol string.
    /// @param stock The stock symbol string.
    /// @return The internal TickerId, or TickerId_INVALID if not found.
    auto GetTickerId(const std::string& stock) const -> TickerId {
        auto it = m_SymbolToTicker.find(stock);
        return (it != m_SymbolToTicker.end()) ? it->second : TickerId_INVALID;
    }

   private:
    /// @brief Input queue of parsed ITCH messages.
    std::shared_ptr<LockFreeQueue<ITCH::Message>> m_InputQueue;

    /// @brief Array of order book pointers indexed by TickerId.
    MarketOrderBookHashMap m_OrderBooks{};

    /// @brief Context of an active order for O(1) state lookup.
    struct OrderContext {
        TickerId ticker_id = TickerId_INVALID;
        Side side = Side::INVALID;
        Price price = Price_INVALID;
        Qty qty = Qty_INVALID;
    };

    /// @brief Tracks order state for O(1) lookups and quantity updates.
    std::vector<OrderContext> m_OrderContexts;

    /// @brief Maps an OrderId to a vector index.
    /// @param order_id The order identifier.
    /// @return The index into the m_OrderContexts vector.
    inline static auto OrderIdToIndex(OrderId order_id) noexcept -> size_t {
        return order_id % ME_MAX_ORDER_IDS;
    }

    /// @brief Maps a trimmed stock symbol string to an internal TickerId.
    std::unordered_map<std::string, TickerId> m_SymbolToTicker;

    /// @brief Counter for assigning sequential TickerIds.
    TickerId m_NextTickerId = 0;

    /// @brief Resolves (or creates) an internal TickerId for the given stock
    /// symbol. If the symbol has not been seen before and capacity remains, a
    /// new MarketOrderBook is allocated.
    /// @param stock_id The raw ITCH StockID array.
    /// @return The TickerId, or TickerId_INVALID if capacity is exhausted.
    inline auto ResolveTickerId(const ITCH::StockID& stock_id) -> TickerId;

    /// @brief Trims trailing spaces from an ITCH StockID.
    /// @param stock_id The raw ITCH StockID array.
    /// @return Trimmed string.
    inline static auto TrimStockId(const ITCH::StockID& stock_id)
        -> std::string;

    /// @brief Visitor that processes a single ITCH message variant and forwards
    /// it to the correct order book as an MEMarketUpdate.
    /// @param message The ITCH message variant to process.
    auto ProcessMessage(const ITCH::Message& message) -> void;
};

#endif  // SOLO_STRATEGY_SRC_BOOKS_MANAGER_BOOKS_MANAGER_H_
