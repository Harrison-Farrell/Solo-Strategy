// -----------------------------------------------------------------------------
#include "book_builder_ui.h"

#include <algorithm>
#include <vector>

#include "imgui.h"
#include "implot.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"

BookBuilderUI::BookBuilderUI(const BookBuilder* book_builder)
    : m_bookBuilder(book_builder) {
    m_logger = quill::Frontend::get_logger("BookBuilder");
}

auto BookBuilderUI::RenderUI() -> void {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Market Depth Viewer")) {
        ImGui::End();
        return;
    }

    RenderTickerSelector();

    if (m_selectedTickerId != TickerId_INVALID) {
        if (auto* book = m_bookBuilder->GetOrderBook(m_selectedTickerId)) {
            RenderMarketDepthPlot(book);
        }
    }

    ImGui::End();
}

auto BookBuilderUI::RenderTickerSelector() -> void {
    const char* current_ticker =
        (m_selectedTickerId == TickerId_INVALID)
            ? "None"
            : TickerIdToString(m_selectedTickerId).c_str();

    if (ImGui::BeginCombo("Select Ticker", current_ticker)) {
        auto active_tickers = m_bookBuilder->GetActiveTickers();
        for (auto ticker : active_tickers) {
            bool is_selected = (m_selectedTickerId == ticker);
            if (ImGui::Selectable(TickerIdToString(ticker).c_str(),
                                  is_selected)) {
                m_selectedTickerId = ticker;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

auto BookBuilderUI::RenderMarketDepthPlot(OrderBook* book) -> void {
    std::vector<DepthLevel> bids, asks;
    book->GetDepth(bids, asks, 50);

    if (bids.empty() && asks.empty()) {
        ImGui::Text("No data available for this ticker.");
        return;
    }

    // Prepare data for ImPlot
    std::vector<double> buyX, buyY, sellX, sellY;
    buyX.reserve(bids.size() * 2);
    buyY.reserve(bids.size() * 2);
    sellX.reserve(asks.size() * 2);
    sellY.reserve(asks.size() * 2);

    double cumulative_vol = 0;
    for (const auto& level : bids) {
        double price = static_cast<double>(level.price) / 10000.0;
        buyX.push_back(price);
        buyY.push_back(cumulative_vol);
        cumulative_vol += static_cast<double>(level.qty);
        buyX.push_back(price);
        buyY.push_back(cumulative_vol);
    }
    std::reverse(buyX.begin(), buyX.end());
    std::reverse(buyY.begin(), buyY.end());

    cumulative_vol = 0;
    for (const auto& level : asks) {
        double price = static_cast<double>(level.price) / 10000.0;
        sellX.push_back(price);
        sellY.push_back(cumulative_vol);
        cumulative_vol += static_cast<double>(level.qty);
        sellX.push_back(price);
        sellY.push_back(cumulative_vol);
    }

    if (ImPlot::BeginPlot("##MarketDepthPlot", ImVec2(-1, -1))) {
        ImPlot::SetupAxes("Price ($)", "Cumulative Quantity",
                          ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisFormat(ImAxis_X1, "%.4f");

        if (!buyX.empty()) {
            ImPlot::PlotShaded("Bids", buyX.data(), buyY.data(),
                               static_cast<int>(buyX.size()), 0.0,
                               ImPlotSpec(ImPlotProp_FillColor,
                                          ImVec4(0.0f, 0.78f, 0.31f, 0.5f)));
            ImPlot::PlotLine("Bids", buyX.data(), buyY.data(),
                             static_cast<int>(buyX.size()),
                             ImPlotSpec(ImPlotProp_LineColor,
                                        ImVec4(0.0f, 0.78f, 0.31f, 1.0f)));
        }

        if (!sellX.empty()) {
            ImPlot::PlotShaded("Asks", sellX.data(), sellY.data(),
                               static_cast<int>(sellX.size()), 0.0,
                               ImPlotSpec(ImPlotProp_FillColor,
                                          ImVec4(0.9f, 0.2f, 0.2f, 0.5f)));
            ImPlot::PlotLine("Asks", sellX.data(), sellY.data(),
                             static_cast<int>(sellX.size()),
                             ImPlotSpec(ImPlotProp_LineColor,
                                        ImVec4(0.9f, 0.2f, 0.2f, 1.0f)));
        }

        ImPlot::EndPlot();
    }
}
