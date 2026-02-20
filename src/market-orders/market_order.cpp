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

#include "market_order.h"

#include <string>

auto MarketOrder::toString() const -> std::string {
    std::stringstream ss;

    ss << "MarketOrder" << "["
       << "oid:" << orderIdToString(mOrder_id) << " "
       << "side:" << sideToString(mSide) << " "
       << "price:" << priceToString(mPrice) << " "
       << "qty:" << qtyToString(mQty) << " "
       << "prio:" << priorityToString(mPriority) << " "
       << "prev:"
       << orderIdToString(mPrev_order ? mPrev_order->mOrder_id
                                      : OrderId_INVALID)
       << " "
       << "next:"
       << orderIdToString(mNext_order ? mNext_order->mOrder_id
                                      : OrderId_INVALID)
       << "]";
    return ss.str();
}

auto MarketOrderAtPrice::toString() const -> std::string {
    std::stringstream ss;
    ss << "MarketOrdersAtPrice["
       << "side:" << sideToString(mSide) << " "
       << "price:" << priceToString(mPrice) << " "
       << "first_mkt_order:"
       << (mFirst_market_order ? mFirst_market_order->toString() : "null")
       << " "
       << "prev:"
       << priceToString(mPrev_entry ? mPrev_entry->mPrice : Price_INVALID)
       << " "
       << "next:"
       << priceToString(mNext_entry ? mNext_entry->mPrice : Price_INVALID)
       << "]";

    return ss.str();
}

auto BestBidOffer::toString() const -> std::string {
    std::stringstream ss;
    ss << "Best Bid Offer\t{" << qtyToString(mBid_qty) << "@"
       << priceToString(mBid_price) << "X" << priceToString(mAsk_price) << "@"
       << qtyToString(mAsk_qty) << "}";

    return ss.str();
}