// -----------------------------------------------------------------------------
#ifndef SOLO_STRATEGY_SRC_BOOK_BUILDER_UI_BOOK_BUILDER_UI_H_
#define SOLO_STRATEGY_SRC_BOOK_BUILDER_UI_BOOK_BUILDER_UI_H_

#include "book-builder/book_builder.h"
#include "quill/Logger.h"
#include "utilities/types.h"

/// @brief Handles the UI rendering for the BookBuilder.
class BookBuilderUI final {
   public:
    /// @brief Constructs a BookBuilderUI.
    /// @param book_builder Pointer to the BookBuilder instance to observe.
    explicit BookBuilderUI(const BookBuilder* book_builder);

    /// @brief Renders the UI for selecting a book and displaying its depth.
    auto RenderUI() -> void;

   private:
    const BookBuilder* m_bookBuilder;
    quill::Logger* m_logger = nullptr;

    /// @brief Ticker ID selected in the UI.
    TickerId m_selectedTickerId = TickerId_INVALID;

    /// @brief Renders the ticker selection combo box.
    auto RenderTickerSelector() -> void;

    /// @brief Renders the market depth plot for a specific order book.
    /// @param book Pointer to the OrderBook to render.
    auto RenderMarketDepthPlot(OrderBook* book) -> void;
};

#endif  // SOLO_STRATEGY_SRC_BOOK_BUILDER_UI_BOOK_BUILDER_UI_H_
