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

#include "order_book.h"

#include "market-orders/market_order.h"
#include "market-orders/market_update.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "utilities/types.h"

OrderBook::OrderBook(TickerId ticker_id)
    : m_tickerId(ticker_id),
      m_ordersAtPricePool(ME_MAX_PRICE_LEVELS),
      m_orderPool(ME_MAX_ORDER_IDS) {
    auto* logger = quill::Frontend::get_logger("root");
    if (static_cast<bool>(logger)) {
        LOG_INFO(logger, "Created Book: {} at: {}", m_tickerId,
                 static_cast<void*>(this));
    }
}

OrderBook::~OrderBook() {
    // reset the internal data members
    auto* logger = quill::Frontend::get_logger("root");
    if (static_cast<bool>(logger)) {
        LOG_INFO(logger, "Destroy Book: {} at: {}", m_tickerId,
                 static_cast<void*>(this));
    }
    m_bidsByPrice = nullptr;
    m_asksByPrice = nullptr;
    m_orderIdToOrder.fill(nullptr);
}

auto OrderBook::GetBestBidOffer() const noexcept -> const BestBidOffer* {
    return &m_bestBidOffer;
}

auto OrderBook::UpdateBestBidOffer(bool update_bid, bool update_ask) noexcept {
    if (update_bid) {
        if (static_cast<bool>(m_bidsByPrice)) {
            m_bestBidOffer.m_bidPrice = m_bidsByPrice->m_price;
            m_bestBidOffer.m_bidQty = m_bidsByPrice->m_firstMarketOrder->m_qty;
            for (auto* order = m_bidsByPrice->m_firstMarketOrder->m_nextOrder;
                 order != m_bidsByPrice->m_firstMarketOrder;
                 order = order->m_nextOrder) {
                m_bestBidOffer.m_bidQty += order->m_qty;
            }
        } else {
            // There is no head the the m_bidsByPrice is nullptr
            m_bestBidOffer.m_bidPrice = Price_INVALID;
            m_bestBidOffer.m_askQty = Qty_INVALID;
        }
    }

    if (update_ask) {
        if (static_cast<bool>(m_asksByPrice)) {
            m_bestBidOffer.m_askPrice = m_asksByPrice->m_price;
            m_bestBidOffer.m_askQty = m_asksByPrice->m_firstMarketOrder->m_qty;
            for (auto* order = m_asksByPrice->m_firstMarketOrder->m_nextOrder;
                 order != m_asksByPrice->m_firstMarketOrder;
                 order = order->m_nextOrder) {
                m_bestBidOffer.m_askQty += order->m_qty;
            }
        } else {
            // There is no head the the m_asksByPrice is nullptr
            m_bestBidOffer.m_askPrice = Price_INVALID;
            m_bestBidOffer.m_askQty = Qty_INVALID;
        }
    }
}

auto OrderBook::AddOrdersAtPrice(
    MarketOrderAtPrice* new_orders_at_price) noexcept {
    m_priceOrdersAtPrice.at(PriceToIndex(new_orders_at_price->m_price)) =
        new_orders_at_price;

    MarketOrderAtPrice*& best_orders_by_price =
        (new_orders_at_price->m_side == Side::BUY ? m_bidsByPrice
                                                  : m_asksByPrice);

    if (!best_orders_by_price) [[unlikely]] {
        best_orders_by_price = new_orders_at_price;
        new_orders_at_price->m_prevEntry = new_orders_at_price;
        new_orders_at_price->m_nextEntry = new_orders_at_price;
        return;
    }

    auto* target = best_orders_by_price;
    const Price price = new_orders_at_price->m_price;
    bool is_worse_than_best;

    if (new_orders_at_price->m_side == Side::BUY) {
        is_worse_than_best = (price < target->m_price);
        if (is_worse_than_best) {
            target = target->m_nextEntry;
            while (target != best_orders_by_price && target->m_price > price) {
                target = target->m_nextEntry;
            }
        }
    } else {
        is_worse_than_best = (price > target->m_price);
        if (is_worse_than_best) {
            target = target->m_nextEntry;
            while (target != best_orders_by_price && target->m_price < price) {
                target = target->m_nextEntry;
            }
        }
    }

    // Insert new_orders_at_price BEFORE target
    new_orders_at_price->m_prevEntry = target->m_prevEntry;
    new_orders_at_price->m_nextEntry = target;
    target->m_prevEntry->m_nextEntry = new_orders_at_price;
    target->m_prevEntry = new_orders_at_price;

    // Update the head pointer if the new price is better than the best
    if (!is_worse_than_best) {
        best_orders_by_price = new_orders_at_price;
    }
}

auto OrderBook::AddOrder(MarketOrder* order) noexcept -> void {
    auto* const orders_at_price = GetOrdersAtPrice(order->m_price);

    if (!static_cast<bool>(orders_at_price)) {
        order->m_nextOrder = order->m_prevOrder = order;

        auto* new_orders_at_price = m_ordersAtPricePool.Allocate(
            order->m_side, order->m_price, order, nullptr, nullptr);
        AddOrdersAtPrice(new_orders_at_price);
    } else {
        auto* first_order = (static_cast<bool>(orders_at_price)
                                 ? orders_at_price->m_firstMarketOrder
                                 : nullptr);
        first_order->m_prevOrder->m_nextOrder = order;
        order->m_prevOrder = first_order->m_prevOrder;
        order->m_nextOrder = first_order;
        first_order->m_prevOrder = order;
    }

    m_orderIdToOrder.at(order->m_orderId) = order;
}

auto OrderBook::OnMarketUpdate(const MarketUpdate* market_update) noexcept {
    // Check if the bid price level was updated by comparing side and price
    const auto bid_updated =
        (static_cast<bool>(m_bidsByPrice) && market_update->side == Side::BUY &&
         market_update->price >= m_bidsByPrice->m_price);
    // Check if the ask price level was updated by comparing side and price
    const auto ask_updated = (static_cast<bool>(m_asksByPrice) &&
                              market_update->side == Side::SELL &&
                              market_update->price <= m_asksByPrice->m_price);

    // Process the market update based on its type
    switch (market_update->type) {
        case MarketUpdateType::ADD: {
            // Allocate a new order from the memory pool with update details
            auto* order = m_orderPool.Allocate(
                market_update->order_id, market_update->side,
                market_update->price, market_update->qty,
                market_update->priority, nullptr, nullptr);
            // Add the newly created order to the order book
            AddOrder(order);
        } break;
        case MarketUpdateType::MODIFY: {
            auto* order = m_orderIdToOrder.at(market_update->order_id);
            order->m_qty = market_update->qty;
        } break;
        case MarketUpdateType::CANCEL: {
            auto* order = m_orderIdToOrder.at(market_update->order_id);
            order->m_qty -= market_update->qty;
        } break;
        case MarketUpdateType::DELETED: {
            auto* order = m_orderIdToOrder.at(market_update->order_id);
            RemoveOrder(order);
        } break;
        case MarketUpdateType::CLEAR: {
            // Clear the full limit order book and Deallocate all resources
            for (auto& order : m_orderIdToOrder) {
                if (static_cast<bool>(order)) {
                    m_orderPool.Deallocate(order);
                }
            }
            // Reset the order ID mapping
            m_orderIdToOrder.fill(nullptr);

            // Deallocate all bid price levels
            if (static_cast<bool>(m_bidsByPrice)) {
                for (auto* bid = m_bidsByPrice->m_nextEntry;
                     bid != m_bidsByPrice; bid = bid->m_nextEntry) {
                    m_ordersAtPricePool.Deallocate(bid);
                }
                m_ordersAtPricePool.Deallocate(m_bidsByPrice);
            }

            // Deallocate all ask price levels
            if (static_cast<bool>(m_asksByPrice)) {
                for (auto* ask = m_asksByPrice->m_nextEntry;
                     ask != m_asksByPrice; ask = ask->m_nextEntry) {
                    m_ordersAtPricePool.Deallocate(ask);
                }
                m_ordersAtPricePool.Deallocate(m_asksByPrice);
            }

            // Reset bid and ask pointers
            m_bidsByPrice = m_asksByPrice = nullptr;
        } break;
        case MarketUpdateType::TRADE:
        case MarketUpdateType::INVALID:
        case MarketUpdateType::SNAPSHOT_START:
        case MarketUpdateType::SNAPSHOT_END:
            break;
    }

    UpdateBestBidOffer(bid_updated, ask_updated);
}

auto OrderBook::RemoveOrdersAtPrice(Side side, Price price) noexcept {
    auto* const best_orders_by_price =
        (side == Side::BUY ? m_bidsByPrice : m_asksByPrice);
    auto* orders_at_price = GetOrdersAtPrice(price);

    if (orders_at_price->m_nextEntry == orders_at_price)
        [[unlikely]] {  // empty side of book.
        (side == Side::BUY ? m_bidsByPrice : m_asksByPrice) = nullptr;
    } else {
        orders_at_price->m_prevEntry->m_nextEntry =
            orders_at_price->m_nextEntry;
        orders_at_price->m_nextEntry->m_prevEntry =
            orders_at_price->m_prevEntry;

        if (orders_at_price == best_orders_by_price) {
            (side == Side::BUY ? m_bidsByPrice : m_asksByPrice) =
                orders_at_price->m_nextEntry;
        }

        orders_at_price->m_prevEntry = orders_at_price->m_nextEntry = nullptr;
    }

    m_priceOrdersAtPrice.at(PriceToIndex(price)) = nullptr;

    m_ordersAtPricePool.Deallocate(orders_at_price);
}

auto OrderBook::RemoveOrder(MarketOrder* order) noexcept -> void {
    auto* orders_at_price = GetOrdersAtPrice(order->m_price);

    if (order->m_prevOrder == order) {  // only one element.
        RemoveOrdersAtPrice(order->m_side, order->m_price);
    } else {  // remove the link.
        auto* const order_before = order->m_prevOrder;
        auto* const order_after = order->m_nextOrder;
        order_before->m_nextOrder = order_after;
        order_after->m_prevOrder = order_before;

        if (orders_at_price->m_firstMarketOrder == order) {
            orders_at_price->m_firstMarketOrder = order_after;
        }

        order->m_prevOrder = order->m_nextOrder = nullptr;
    }

    m_orderIdToOrder.at(order->m_orderId) = nullptr;
    m_orderPool.Deallocate(order);
}