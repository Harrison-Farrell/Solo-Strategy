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

#include "book_builder.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <utility>

// 3rd party includes
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "thread/thread.h"

BookBuilder::BookBuilder(
    std::shared_ptr<LockFreeQueue<MarketUpdate>> input_queue)
    : m_InputQueue(std::move(input_queue)) {
    m_tickerOrderBook.fill(nullptr);
    m_logger = quill::Frontend::create_or_get_logger("BookBuilder");
    if (static_cast<bool>(m_logger)) {
        LOG_INFO(m_logger, "BookBuilder created at: {}",
                 static_cast<void*>(this));
    }
}

BookBuilder::~BookBuilder() {
    if (static_cast<bool>(m_logger)) {
        LOG_INFO(m_logger, "BookBuilder destroyed at: {}",
                 static_cast<void*>(this));
    }
    for (auto* order_book : m_tickerOrderBook) {
        if (order_book != nullptr) {
            delete order_book;
        }
    }
    m_tickerOrderBook.fill(nullptr);
}

auto BookBuilder::Start(int core_id) -> std::shared_ptr<std::thread> {
    return CreateAndStartThread(core_id, "BookBuilder", [this] { Execute(); });
}

auto BookBuilder::Execute() -> void {
    if (static_cast<bool>(m_logger)) {
        LOG_INFO(m_logger, "BookBuilder Execute() started");
    }
    while (true) {
        const auto* update = m_InputQueue->GetNextRead();
        if (static_cast<bool>(update)) {
            if (update->type == MarketUpdateType::INVALID) [[unlikely]] {
                LOG_INFO(m_logger,
                         "BookBuilder received INVALID market_update");
                m_InputQueue->UpdateReadIndex();
                break;
            }
            ProcessMarketUpdate(update);
            m_InputQueue->UpdateReadIndex();
        } else {
            std::this_thread::yield();
        }
    }
    if (static_cast<bool>(m_logger)) {
        LOG_INFO(m_logger, "BookBuilder Execute() finished");
    }
}

auto BookBuilder::ProcessMarketUpdate(
    const MarketUpdate* market_update) noexcept -> void {
    if (market_update == nullptr || market_update->ticker_id >= ME_MAX_TICKERS)
        [[unlikely]] {
        return;
    }

    auto& order_book = m_tickerOrderBook.at(market_update->ticker_id);

    if (order_book == nullptr) [[unlikely]] {
        order_book = new OrderBook(market_update->ticker_id);
        {
            std::lock_guard<std::mutex> lock(m_activeTickersMutex);
            m_activeTickers.push_back(market_update->ticker_id);
        }
        if (static_cast<bool>(m_logger)) {
            LOG_INFO(m_logger, "New OrderBook created for ticker_id: {}",
                     market_update->ticker_id);
        }
    }

    order_book->OnMarketUpdate(market_update);
}

auto BookBuilder::GetActiveTickers() const -> std::vector<TickerId> {
    std::lock_guard<std::mutex> lock(m_activeTickersMutex);
    return m_activeTickers;
}

auto BookBuilder::GetOrderBook(TickerId ticker_id) const -> OrderBook* {
    if (ticker_id >= ME_MAX_TICKERS) {
        return nullptr;
    }
    return m_tickerOrderBook.at(ticker_id);
}
