#pragma once

#include <string>

#include "utilities/macros.h"
#include "utilities/types.h"


struct MarketOrder {
    OrderId mOrder_id = OrderId_INVALID;
    Side mSide = Side::INVALID;
    Price mPrice = Price_INVALID;
    Qty mQty = Qty_INVALID;
    Priority mPriority = Priority_INVALID;

    MarketOrder *mPrev_order = nullptr;
    MarketOrder *mNext_order = nullptr;

    // only needed for use with the Memory Pool
    MarketOrder() = default;

    MarketOrder(OrderId order_id, Side side, Price price, Qty qty,
                Priority priority, MarketOrder *prev_order,
                MarketOrder *next_order) noexcept
        : mOrder_id(order_id),
          mSide(side),
          mPrice(price),
          mQty(qty),
          mPriority(priority),
          mPrev_order(prev_order),
          mNext_order(next_order) {}

    auto toString() const -> std::string;
};

// Hash map from OrderId -> MarketOrder.
typedef std::array<MarketOrder *, ME_MAX_ORDER_IDS> OrderHashMap;

struct MarketOrderAtPrice {
    Side mSide = Side::INVALID;
    Price mPrice = Price_INVALID;

    MarketOrder *mFirst_market_order = nullptr;

    MarketOrderAtPrice *mPrev_entry = nullptr;
    MarketOrderAtPrice *mNext_entry = nullptr;

    MarketOrderAtPrice() = default;

    MarketOrderAtPrice(Side side, Price price, MarketOrder *first_market_order,
                       MarketOrderAtPrice *prev_entry,
                       MarketOrderAtPrice *next_entry) noexcept
        : mSide(side),
          mPrice(price),
          mFirst_market_order(first_market_order),
          mPrev_entry(prev_entry),
          mNext_entry(next_entry) {}

    auto toString() const -> std::string;
};

// Hash map from Price -> MarketOrdersAtPrice.
typedef std::array<MarketOrderAtPrice *, ME_MAX_PRICE_LEVELS>
    OrdersAtPriceHashMap;

struct BestBidOffer {
    Price mBid_price = Price_INVALID;
    Price mAsk_price = Price_INVALID;

    Qty mBid_qty = Qty_INVALID;
    Qty mAsk_qty = Qty_INVALID;

    auto toString() const -> std::string;
};