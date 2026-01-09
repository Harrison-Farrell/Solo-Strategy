#pragma once

#include <string>

#include "utilities/macros.h"
#include "utilities/types.h"

/// \struct MarketOrder
/// \brief Represents a single market order in the order book.
struct MarketOrder {
    /// Order identifier.
    OrderId mOrder_id = OrderId_INVALID;
    /// Side of the order (buy/sell).
    Side mSide = Side::INVALID;
    /// Price of the order.
    Price mPrice = Price_INVALID;
    /// Quantity of the order.
    Qty mQty = Qty_INVALID;
    /// Priority of the order.
    Priority mPriority = Priority_INVALID;

    /// Pointer to the previous order in the linked list.
    MarketOrder *mPrev_order = nullptr;
    /// Pointer to the next order in the linked list.
    MarketOrder *mNext_order = nullptr;

    /// \brief Default constructor. Only needed for use with the Memory Pool.
    MarketOrder() = default;

    /// \brief Constructs a MarketOrder with all fields specified.
    /// \param order_id Order identifier.
    /// \param side Side of the order.
    /// \param price Price of the order.
    /// \param qty Quantity of the order.
    /// \param priority Priority of the order.
    /// \param prev_order Pointer to previous order.
    /// \param next_order Pointer to next order.
    MarketOrder(OrderId order_id, Side side, Price price, Qty qty,
                Priority priority, MarketOrder *prev_order,
                MarketOrder *next_order) noexcept;

    /// \brief Returns a string representation of the MarketOrder.
    /// \return String representation.
    auto toString() const -> std::string;
};

/// \typedef OrderArray
/// \brief Hash map from OrderId to MarketOrder.
typedef std::array<MarketOrder *, ME_MAX_ORDER_IDS> OrderArray;

/// \struct MarketOrderAtPrice
/// \brief Represents all market orders at a specific price level.
struct MarketOrderAtPrice {
    /// Side of the orders at this price.
    Side mSide = Side::INVALID;
    /// Price level.
    Price mPrice = Price_INVALID;

    /// Pointer to the first market order at this price.
    MarketOrder *mFirst_market_order = nullptr;

    /// Pointer to the previous price entry in the linked list.
    MarketOrderAtPrice *mPrev_entry = nullptr;
    /// Pointer to the next price entry in the linked list.
    MarketOrderAtPrice *mNext_entry = nullptr;

    /// \brief Default constructor.
    MarketOrderAtPrice() = default;

    /// \brief Constructs a MarketOrderAtPrice with all fields specified.
    /// \param side Side of the orders.
    /// \param price Price level.
    /// \param first_market_order Pointer to first market order.
    /// \param prev_entry Pointer to previous price entry.
    /// \param next_entry Pointer to next price entry.
    MarketOrderAtPrice(Side side, Price price, MarketOrder *first_market_order,
                       MarketOrderAtPrice *prev_entry,
                       MarketOrderAtPrice *next_entry) noexcept;
    /// \brief Returns a string representation of the MarketOrderAtPrice.
    /// \return String representation.
    auto toString() const -> std::string;
};

/// \typedef OrdersAtPriceArray
/// \brief Hash map from Price to MarketOrderAtPrice.
typedef std::array<MarketOrderAtPrice *, ME_MAX_PRICE_LEVELS>
    OrdersAtPriceArray;

/// \struct BestBidOffer
/// \brief Represents the best bid and offer in the order book.
struct BestBidOffer {
    /// Best bid price.
    Price mBid_price = Price_INVALID;
    /// Best ask price.
    Price mAsk_price = Price_INVALID;

    /// Quantity at best bid.
    Qty mBid_qty = Qty_INVALID;
    /// Quantity at best ask.
    Qty mAsk_qty = Qty_INVALID;

    /// \brief Returns a string representation of the BestBidOffer.
    /// \return String representation.
    auto toString() const -> std::string;
};