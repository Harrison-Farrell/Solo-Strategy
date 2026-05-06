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

#include "market_order.h"

#include <sstream>
#include <string>

#include "utilities/types.h"

auto MarketOrder::toString() const -> std::string {
    std::stringstream ss;

    ss << "MarketOrder" << "["
       << "oid:" << OrderIdToString(m_orderId) << " "
       << "side:" << SideToString(m_side) << " "
       << "price:" << PriceToString(m_price) << " "
       << "qty:" << QtyToString(m_qty) << " "
       << "prio:" << PriorityToString(m_priority) << " "
       << "prev:"
       << OrderIdToString(m_prevOrder ? m_prevOrder->m_orderId
                                      : OrderId_INVALID)
       << " "
       << "next:"
       << OrderIdToString(m_nextOrder ? m_nextOrder->m_orderId
                                      : OrderId_INVALID)
       << "]";
    return ss.str();
}

auto MarketOrderAtPrice::toString() const -> std::string {
    std::stringstream ss;
    ss << "MarketOrdersAtPrice["
       << "side:" << SideToString(m_side) << " "
       << "price:" << PriceToString(m_price) << " "
       << "first_mkt_order:"
       << (m_firstMarketOrder ? m_firstMarketOrder->toString() : "null")
       << " "
       << "prev:"
       << PriceToString(m_prevEntry ? m_prevEntry->m_price : Price_INVALID)
       << " "
       << "next:"
       << PriceToString(m_nextEntry ? m_nextEntry->m_price : Price_INVALID)
       << "]";

    return ss.str();
}

auto BestBidOffer::toString() const -> std::string {
    std::stringstream ss;
    ss << "Best Bid Offer\t{" << QtyToString(m_bidQty) << "@"
       << PriceToString(m_bidPrice) << "X" << PriceToString(m_askPrice) << "@"
       << QtyToString(m_askQty) << "}";

    return ss.str();
}