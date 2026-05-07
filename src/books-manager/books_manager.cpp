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

#include "books-manager/books_manager.h"

// system includes
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <variant>

// local includes
#include "itch-parser/messages/itch_messages.h"
#include "thread/thread.h"
#include "utilities/types.h"

BooksManager::BooksManager(
    std::shared_ptr<LockFreeQueue<ITCH::Message>>& input_queue)
    : m_InputQueue(input_queue), m_OrderContexts(ME_MAX_ORDER_IDS) {
    m_OrderBooks.fill(nullptr);
}

BooksManager::~BooksManager() {
    for (auto& book : m_OrderBooks) {
        delete book;
        book = nullptr;
    }
}

std::shared_ptr<std::thread> BooksManager::Start(int core_id) {
    return CreateAndStartThread(core_id, "Books Manager",
                                [this]() { Execute(); });
}

auto BooksManager::Execute() -> void {
    while (true) {
        const auto* message = m_InputQueue->GetNextRead();
        if (message) {
            // A std::monostate variant signals termination
            if (std::holds_alternative<std::monostate>(*message)) [[unlikely]] {
                m_InputQueue->UpdateReadIndex();
                break;
            }

            std::cout << "-------------------------\n" << *message << "\n";
            ProcessMessage(*message);
            m_InputQueue->UpdateReadIndex();
        } else {
            // Yield to avoid busy spinning at 100% CPU when starved
            std::this_thread::yield();
        }
    }

    std::cerr << "BooksManager: finished processing. "
              << m_SymbolToTicker.size() << " symbols tracked across "
              << m_NextTickerId << " order books.\n";
}

auto BooksManager::TrimStockId(const ITCH::StockID& stock_id) -> std::string {
    std::string symbol(stock_id.begin(), stock_id.end());
    // ITCH StockIDs are right-padded with spaces
    auto end = symbol.find_last_not_of(' ');
    if (end != std::string::npos) {
        symbol.erase(end + 1);
    } else {
        symbol.clear();
    }
    return symbol;
}

auto BooksManager::ResolveTickerId(const ITCH::StockID& stock_id) -> TickerId {
    const auto symbol = TrimStockId(stock_id);
    if (symbol.empty()) return TickerId_INVALID;

    auto it = m_SymbolToTicker.find(symbol);
    if (it != m_SymbolToTicker.end()) {
        return it->second;
    }

    // Capacity check
    if (m_NextTickerId >= ME_MAX_TICKERS) [[unlikely]] {
        return TickerId_INVALID;
    }

    // Allocate a new order book for this symbol
    const TickerId ticker_id = m_NextTickerId++;
    m_SymbolToTicker[symbol] = ticker_id;
    m_OrderBooks[ticker_id] = new MarketOrderBook(ticker_id);

    return ticker_id;
}

auto BooksManager::ProcessMessage(const ITCH::Message& message) -> void {
    std::visit(
        [this](const auto& msg) {
            using T = std::decay_t<decltype(msg)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                return;
            } else if constexpr (std::is_same_v<T, ITCH::AddOrderMessage> ||
                                 std::is_same_v<
                                     T, ITCH::AddOrderMPIDAttributionMessage>) {
                const auto ticker_id = ResolveTickerId(msg.stock);

                if (ticker_id == TickerId_INVALID) [[unlikely]] {
                    return;
                }

                OrderContext ctx{};
                ctx.ticker_id = ticker_id;
                ctx.side =
                    (msg.buy_sell_indicator == 'B') ? Side::BUY : Side::SELL;
                ctx.price = static_cast<Price>(msg.price);
                ctx.qty = msg.shares;
                auto idx = OrderIdToIndex(msg.order_reference_number);
                m_OrderContexts[idx] = ctx;

                MEMarketUpdate update{};
                update.type = MarketUpdateType::ADD;
                update.order_id = msg.order_reference_number;
                update.ticker_id = ticker_id;
                update.side = ctx.side;
                update.price = ctx.price;
                update.qty = ctx.qty;
                update.priority = msg.order_reference_number;

                m_OrderBooks[ticker_id]->OnMarketUpdate(&update);

            } else if constexpr (std::is_same_v<T,
                                                ITCH::OrderExecutedMessage> ||
                                 std::is_same_v<
                                     T, ITCH::OrderExecutedWithPriceMessage>) {
                auto idx = OrderIdToIndex(msg.order_reference_number);
                auto& ctx = m_OrderContexts[idx];
                if (ctx.ticker_id == TickerId_INVALID) return;

                ctx.qty -= msg.executed_shares;

                MEMarketUpdate trade{};
                trade.type = MarketUpdateType::TRADE;
                trade.order_id = msg.order_reference_number;
                trade.ticker_id = ctx.ticker_id;
                trade.side = ctx.side;
                if constexpr (std::is_same_v<
                                  T, ITCH::OrderExecutedWithPriceMessage>) {
                    trade.price = static_cast<Price>(msg.execution_price);
                } else {
                    trade.price = ctx.price;
                }
                trade.qty = msg.executed_shares;
                m_OrderBooks[ctx.ticker_id]->OnMarketUpdate(&trade);

                MEMarketUpdate modify{};
                modify.order_id = msg.order_reference_number;
                modify.ticker_id = ctx.ticker_id;
                if (ctx.qty == 0) {
                    modify.type = MarketUpdateType::CANCEL;
                    ctx.ticker_id = TickerId_INVALID;
                } else {
                    modify.type = MarketUpdateType::MODIFY;
                    modify.qty = ctx.qty;
                }
                m_OrderBooks[ctx.ticker_id]->OnMarketUpdate(&modify);

            } else if constexpr (std::is_same_v<T, ITCH::OrderCancelMessage>) {
                auto idx = OrderIdToIndex(msg.order_reference_number);
                auto& ctx = m_OrderContexts[idx];
                if (ctx.ticker_id == TickerId_INVALID) return;

                ctx.qty -= msg.cancelled_shares;

                MEMarketUpdate modify{};
                modify.order_id = msg.order_reference_number;
                modify.ticker_id = ctx.ticker_id;
                if (ctx.qty == 0) {
                    modify.type = MarketUpdateType::CANCEL;
                    ctx.ticker_id = TickerId_INVALID;
                } else {
                    modify.type = MarketUpdateType::MODIFY;
                    modify.qty = ctx.qty;
                }
                m_OrderBooks[ctx.ticker_id]->OnMarketUpdate(&modify);

            } else if constexpr (std::is_same_v<T, ITCH::OrderDeleteMessage>) {
                auto idx = OrderIdToIndex(msg.order_reference_number);
                auto& ctx = m_OrderContexts[idx];
                if (ctx.ticker_id == TickerId_INVALID) return;

                MEMarketUpdate cancel{};
                cancel.type = MarketUpdateType::CANCEL;
                cancel.order_id = msg.order_reference_number;
                cancel.ticker_id = ctx.ticker_id;

                m_OrderBooks[ctx.ticker_id]->OnMarketUpdate(&cancel);
                ctx.ticker_id = TickerId_INVALID;

            } else if constexpr (std::is_same_v<T, ITCH::OrderReplaceMessage>) {
                auto old_idx =
                    OrderIdToIndex(msg.original_order_reference_number);
                auto& old_ctx = m_OrderContexts[old_idx];
                if (old_ctx.ticker_id == TickerId_INVALID) return;

                const auto ticker_id = old_ctx.ticker_id;
                const auto side = old_ctx.side;

                MEMarketUpdate cancel{};
                cancel.type = MarketUpdateType::CANCEL;
                cancel.order_id = msg.original_order_reference_number;
                cancel.ticker_id = ticker_id;
                m_OrderBooks[ticker_id]->OnMarketUpdate(&cancel);
                old_ctx.ticker_id = TickerId_INVALID;

                OrderContext new_ctx{};
                new_ctx.ticker_id = ticker_id;
                new_ctx.side = side;
                new_ctx.price = static_cast<Price>(msg.price);
                new_ctx.qty = msg.shares;
                auto new_idx = OrderIdToIndex(msg.new_order_reference_number);
                m_OrderContexts[new_idx] = new_ctx;

                MEMarketUpdate add{};
                add.type = MarketUpdateType::ADD;
                add.order_id = msg.new_order_reference_number;
                add.ticker_id = ticker_id;
                add.side = side;
                add.price = new_ctx.price;
                add.qty = new_ctx.qty;
                add.priority = msg.new_order_reference_number;
                m_OrderBooks[ticker_id]->OnMarketUpdate(&add);

            } else {
                return;
            }
        },
        message);
}
