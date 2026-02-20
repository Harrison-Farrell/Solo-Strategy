/*
 * --------------------------------------------------------------------------
 * Author:      Harrison Farrell
 * Project:     Solo-Strategy Trading System
 * Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
 *
 * Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
 * This program is distributed WITHOUT ANY WARRANTY; without even the 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
 * --------------------------------------------------------------------------
 */

/**
 * @file market_update.h
 * @brief Messaging structures for publishing and consuming market data updates.
 */

#ifndef SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_UPDATE_H_
#define SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_UPDATE_H_

#include <sstream>

#include "lock-free-queue/lock_free_queue.h"
#include "utilities/types.h"

/**
 * @brief Represents the type of action in a market update message.
 */
enum class MarketUpdateType : uint8_t {
    INVALID = 0,
    CLEAR = 1,
    ADD = 2,
    MODIFY = 3,
    CANCEL = 4,
    TRADE = 5,
    SNAPSHOT_START = 6,
    SNAPSHOT_END = 7
};

/**
 * @brief Converts a MarketUpdateType enum to its string representation.
 * @param type The MarketUpdateType to convert.
 * @return String representation of the type.
 */
inline std::string marketUpdateTypeToString(MarketUpdateType type) {
    switch (type) {
        case MarketUpdateType::CLEAR:
            return "CLEAR";
        case MarketUpdateType::ADD:
            return "ADD";
        case MarketUpdateType::MODIFY:
            return "MODIFY";
        case MarketUpdateType::CANCEL:
            return "CANCEL";
        case MarketUpdateType::TRADE:
            return "TRADE";
        case MarketUpdateType::SNAPSHOT_START:
            return "SNAPSHOT_START";
        case MarketUpdateType::SNAPSHOT_END:
            return "SNAPSHOT_END";
        case MarketUpdateType::INVALID:
            return "INVALID";
    }
    return "UNKNOWN";
}
/// \brief These structures go over the wire / network, so the binary structures
/// are packed to remove system dependent extra padding.
#pragma pack(push, 1)

/**
 * @brief Market update structure used internally by the matching engine.
 * These structures are packed to ensure consistent binary layout across systems.
 */
struct MEMarketUpdate {
    MarketUpdateType type = MarketUpdateType::INVALID;
    OrderId order_id = OrderId_INVALID;
    TickerId ticker_id = TickerId_INVALID;
    Side side = Side::INVALID;
    Price price = Price_INVALID;
    Qty qty = Qty_INVALID;
    Priority priority = Priority_INVALID;

    /**
     * @brief Converts the MEMarketUpdate to a string representation.
     * @return String representation of the update.
     */
    auto toString() const {
        std::stringstream ss;
        ss << "MEMarketUpdate"
           << " ["
           << " type:" << marketUpdateTypeToString(type)
           << " ticker:" << tickerIdToString(ticker_id)
           << " oid:" << orderIdToString(order_id)
           << " side:" << sideToString(side) << " qty:" << qtyToString(qty)
           << " price:" << priceToString(price)
           << " priority:" << priorityToString(priority) << "]";
        return ss.str();
    }
};

/**
 * @brief Market update structure published over the network.
 * Includes a sequence number for tracking and ordering.
 */
struct MDPMarketUpdate {
    size_t seq_num_ = 0;
    MEMarketUpdate me_market_update_;

    /**
     * @brief Converts the MDPMarketUpdate to a string representation.
     * @return String representation of the update.
     */
    auto toString() const {
        std::stringstream ss;
        ss << "MDPMarketUpdate"
           << " ["
           << " seq:" << seq_num_ << " " << me_market_update_.toString() << "]";
        return ss.str();
    }
};

/// \brief Undo the packed binary structure directive moving forward.
#pragma pack(pop)

/// \typedef MEMarketUpdateLFQueue
/// \brief Lock free queue of matching engine market update messages.
typedef LockFreeQueue<MEMarketUpdate> MEMarketUpdateLFQueue;

/// \typedef MDPMarketUpdateLFQueue
/// \brief Lock free queue of market data publisher market update messages.
typedef LockFreeQueue<MDPMarketUpdate> MDPMarketUpdateLFQueue;

#endif  // SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_UPDATE_H_
