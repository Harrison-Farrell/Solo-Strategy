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

#include "market_data_adapter.h"

#include "thread/thread.h"

MarketDataAdapter::MarketDataAdapter(
    std::shared_ptr<LockFreeQueue<ITCH::Message>> input_queue,
    std::shared_ptr<LockFreeQueue<MarketUpdate>> output_queue)
    : m_InputQueue(std::move(input_queue)),
      m_OutputQueue(std::move(output_queue)) {}

auto MarketDataAdapter::Start(int core_id) -> std::shared_ptr<std::thread> {
    return CreateAndStartThread(core_id, "MarketDataAdapter",
                                &MarketDataAdapter::Execute, this);
}

auto MarketDataAdapter::PushUpdate(const MarketUpdate& update) -> void {
    while (!m_OutputQueue->Push(update)) {
        // Yield the thread to allow the consumer to drain the queue
        std::this_thread::yield();
    }
}

auto MarketDataAdapter::Execute() -> void {
    ITCH::Message msg;
    while (true) {
        if (m_InputQueue->Pop(msg)) {
            // Check for exit condition (e.g. monostate)
            if (std::holds_alternative<std::monostate>(msg)) {
                MarketUpdate update;
                update.type = MarketUpdateType::INVALID;
                PushUpdate(update);
                break;
            }
            ProcessMessage(msg);
        } else {
            // Spin and yield if the queue is empty
            std::this_thread::yield();
        }
    }
}

auto MarketDataAdapter::ProcessMessage(const ITCH::Message& msg) -> void {
    std::visit(
        [this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            MarketUpdate update;

            if constexpr (std::is_same_v<T, ITCH::AddOrderMessage> ||
                          std::is_same_v<
                              T, ITCH::AddOrderMPIDAttributionMessage>) {
                update.type = MarketUpdateType::ADD;
                update.order_id = arg.order_reference_number;
                update.ticker_id = arg.stock_locate;
                update.side =
                    (arg.buy_sell_indicator == 'B') ? Side::BUY : Side::SELL;
                update.qty = arg.shares;
                update.price = arg.price;
                PushUpdate(update);

            } else if constexpr (std::is_same_v<T,
                                                ITCH::OrderExecutedMessage>) {
                update.type = MarketUpdateType::TRADE;
                update.order_id = arg.order_reference_number;
                update.ticker_id = arg.stock_locate;
                update.qty = arg.executed_shares;
                PushUpdate(update);

            } else if constexpr (std::is_same_v<
                                     T, ITCH::OrderExecutedWithPriceMessage>) {
                update.type = MarketUpdateType::TRADE;
                update.order_id = arg.order_reference_number;
                update.ticker_id = arg.stock_locate;
                update.qty = arg.executed_shares;
                update.price = arg.execution_price;
                PushUpdate(update);

            } else if constexpr (std::is_same_v<T, ITCH::OrderCancelMessage>) {
                update.type = MarketUpdateType::CANCEL;
                update.order_id = arg.order_reference_number;
                update.ticker_id = arg.stock_locate;
                update.qty = arg.cancelled_shares;
                PushUpdate(update);

            } else if constexpr (std::is_same_v<T, ITCH::OrderDeleteMessage>) {
                update.type = MarketUpdateType::DELETED;
                update.order_id = arg.order_reference_number;
                update.ticker_id = arg.stock_locate;
                PushUpdate(update);

            } else if constexpr (std::is_same_v<T, ITCH::OrderReplaceMessage>) {
                // Emit a DELETE for the original order
                update.type = MarketUpdateType::DELETED;
                update.order_id = arg.original_order_reference_number;
                update.ticker_id = arg.stock_locate;
                PushUpdate(update);

                update.type = MarketUpdateType::ADD;
                update.order_id = arg.new_order_reference_number;
                update.qty = arg.shares;
                update.price = arg.price;
                update.side = Side::INVALID;  // OrderReplaceMessage does NOT
                                              // contain a side.
                PushUpdate(update);

            } else if constexpr (std::is_same_v<T, ITCH::SystemEventMessage>) {
            } else {
            }
        },
        msg);
}
