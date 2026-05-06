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

MarketOrderBook::MarketOrderBook(TickerId ticker_id)
    : m_tickerId(ticker_id),
      m_ordersAtPricePool(ME_MAX_PRICE_LEVELS),
      m_orderPool(ME_MAX_ORDER_IDS) {};

MarketOrderBook::~MarketOrderBook() {
    // reset the internal data members
    m_bidsByPrice = nullptr;
    m_asksByPrice = nullptr;
    m_orderIdToOrder.fill(nullptr);
}

auto MarketOrderBook::OnMarketUpdate(
    const MEMarketUpdate* market_update) noexcept -> void {
    // Check if the bid price level was updated by comparing side and price
    const auto bid_updated =
        (m_bidsByPrice && market_update->side == Side::BUY &&
         market_update->price >= m_bidsByPrice->m_price);
    // Check if the ask price level was updated by comparing side and price
    const auto ask_updated =
        (m_asksByPrice && market_update->side == Side::SELL &&
         market_update->price <= m_asksByPrice->m_price);

    // Process the market update based on its type
    switch (market_update->type) {
        case MarketUpdateType::ADD: {
            // Allocate a new order from the memory pool with update details
            auto order = m_orderPool.Allocate(
                market_update->order_id, market_update->side,
                market_update->price, market_update->qty,
                market_update->priority, nullptr, nullptr);
            // Add the newly created order to the order book
            addOrder(order);
        } break;
        case MarketUpdateType::MODIFY: {
            // Retrieve the existing order by its ID
            auto order = m_orderIdToOrder.at(market_update->order_id);
            // Update the order quantity with the new quantity from the update
            order->m_qty = market_update->qty;
        } break;
        case MarketUpdateType::CANCEL: {
            // Retrieve the order to be cancelled by its ID
            auto order = m_orderIdToOrder.at(market_update->order_id);
            // Remove the order from the order book
            removeOrder(order);
        } break;
        case MarketUpdateType::TRADE: {
            // Trade updates are not processed; return early
            return;
        } break;
        case MarketUpdateType::CLEAR: {
            // Clear the full limit order book and Deallocate all resources

            // Deallocate all individual orders from the memory pool
            for (auto& order : m_orderIdToOrder) {
                if (order) m_orderPool.Deallocate(order);
            }
            // Reset the order ID mapping
            m_orderIdToOrder.fill(nullptr);

            // Deallocate all bid price levels
            if (m_bidsByPrice) {
                for (auto bid = m_bidsByPrice->m_nextEntry;
                     bid != m_bidsByPrice; bid = bid->m_nextEntry)
                    m_ordersAtPricePool.Deallocate(bid);
                m_ordersAtPricePool.Deallocate(m_bidsByPrice);
            }

            // Deallocate all ask price levels
            if (m_asksByPrice) {
                for (auto ask = m_asksByPrice->m_nextEntry;
                     ask != m_asksByPrice; ask = ask->m_nextEntry)
                    m_ordersAtPricePool.Deallocate(ask);
                m_ordersAtPricePool.Deallocate(m_asksByPrice);
            }

            // Reset bid and ask pointers
            m_bidsByPrice = m_asksByPrice = nullptr;
        } break;
        case MarketUpdateType::INVALID:
        case MarketUpdateType::SNAPSHOT_START:
        case MarketUpdateType::SNAPSHOT_END:
            // These update types require no processing
            break;
    }

    UpdateBestBidOffer(bid_updated, ask_updated);
}