#pragma once

#include <sstream>

#include "lock-free-queue/lockfreequeue.h"
#include "utilities/types.h"

/// \enum MarketUpdateType
/// \brief Represents the type / action in the market update message.
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

/// \brief Converts a MarketUpdateType enum to a string.
/// \param type The MarketUpdateType to convert.
/// \return String representation of the MarketUpdateType.
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

/// \struct MEMarketUpdate
/// \brief Market update structure used internally by the matching engine.
struct MEMarketUpdate {
    MarketUpdateType type_ = MarketUpdateType::INVALID;
    OrderId order_id_ = OrderId_INVALID;
    TickerId ticker_id_ = TickerId_INVALID;
    Side side_ = Side::INVALID;
    Price price_ = Price_INVALID;
    Qty qty_ = Qty_INVALID;
    Priority priority_ = Priority_INVALID;

    /// \brief Converts the MEMarketUpdate to a string representation.
    /// \return String representation of the MEMarketUpdate.
    auto toString() const {
        std::stringstream ss;
        ss << "MEMarketUpdate"
           << " ["
           << " type:" << marketUpdateTypeToString(type_)
           << " ticker:" << tickerIdToString(ticker_id_)
           << " oid:" << orderIdToString(order_id_)
           << " side:" << sideToString(side_) << " qty:" << qtyToString(qty_)
           << " price:" << priceToString(price_)
           << " priority:" << priorityToString(priority_) << "]";
        return ss.str();
    }
};

/// \struct MDPMarketUpdate
/// \brief Market update structure published over the network by the market data
/// publisher.
struct MDPMarketUpdate {
    size_t seq_num_ = 0;
    MEMarketUpdate me_market_update_;

    /// \brief Converts the MDPMarketUpdate to a string representation.
    /// \return String representation of the MDPMarketUpdate.
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
