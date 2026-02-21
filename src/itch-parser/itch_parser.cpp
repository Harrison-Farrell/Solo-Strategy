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

#include "itch-parser/itch_parser.h"

#include "itch-parser/memory-map/memory_map_file.h"
#include "itch-parser/messages/itch_messages.h"

ITCH_Parser::ITCH_Parser() : mMessageQueue(1024) {}
ITCH_Parser::ITCH_Parser(const std::string& file_path) : mMessageQueue(1024) {}

auto ITCH_Parser::SystemEventMessage(const ITCH::MessageHeader& header) {
    ITCH::SystemEventMessage msg = {.header = header, .event_code = (char)mReader.readChar()};
}

auto ITCH_Parser::StockDirectoryMessage(const ITCH::MessageHeader& header) {
    char symbols[4];
    mReader.copyString(symbols, 4);
    char financial_status_indicator = mReader.readChar();
    uint32_t round_lot_size = mReader.read32();
    char round_lots_only = mReader.readChar();
    char issue_classification = mReader.readChar();
    char issue_sub_type[2];
    issue_sub_type[0] = mReader.readChar();
    issue_sub_type[1] = mReader.readChar();
    char authenticity = mReader.readChar();
    char short_sale_threshold_indicator = mReader.readChar();
    char ipo_flag = mReader.readChar();
    char luld_ref = mReader.readChar();
    char etp_flag = mReader.readChar();
    char etp_leverage_factor = mReader.read32();
    char inverse_indicator = mReader.readChar();
}

auto ITCH_Parser::StockTradingActionMessage(const ITCH::MessageHeader& header) {
    ITCH::StockTradingActionMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::RegSHOMessage(const ITCH::MessageHeader& header) {
    ITCH::RegSHOMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::MarketParticipantPositionMessage(const ITCH::MessageHeader& header) {
    ITCH::MarketParticipantPositionMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::MWCBDeclineLevelMessage(const ITCH::MessageHeader& header) {
    ITCH::MWCBDeclineLevelMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::MWCBStatusMessage(const ITCH::MessageHeader& header) {
    ITCH::MWCBStatusMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::IPOQuotingPeriodUpdateMessage(const ITCH::MessageHeader& header) {
    ITCH::IPOQuotingPeriodUpdateMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::LULDAuctionCollarMessage(const ITCH::MessageHeader& header) {
    ITCH::LULDAuctionCollarMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::OperationalHaltMessage(const ITCH::MessageHeader& header) {
    ITCH::OperationalHaltMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::AddOrderMessage(const ITCH::MessageHeader& header) {
    ITCH::AddOrderMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::AddOrderMPIDAttributionMessage(const ITCH::MessageHeader& header) {
    ITCH::AddOrderMPIDAttributionMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::OrderExecutedMessage(const ITCH::MessageHeader& header) {
    ITCH::OrderExecutedMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::OrderExecutedWithPriceMessage(const ITCH::MessageHeader& header) {
    ITCH::OrderExecutedWithPriceMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::OrderCancelMessage(const ITCH::MessageHeader& header) {
    ITCH::OrderCancelMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::OrderDeleteMessage(const ITCH::MessageHeader& header) {
    ITCH::OrderDeleteMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::OrderReplaceMessage(const ITCH::MessageHeader& header) {
    ITCH::OrderReplaceMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::NonCrossTradeMessage(const ITCH::MessageHeader& header) {
    ITCH::NonCrossTradeMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::CrossTradeMessage(const ITCH::MessageHeader& header) {
    ITCH::CrossTradeMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::BrokenTradeMessage(const ITCH::MessageHeader& header) {
    ITCH::BrokenTradeMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::NOIIMessage(const ITCH::MessageHeader& header) {
    ITCH::NOIIMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::DLCRMessage(const ITCH::MessageHeader& header) {
    ITCH::DLCRMessage msg = {};
    mMessageQueue.push(msg);
}

auto ITCH_Parser::RetailPriceImprovementIndicatorMessage(const ITCH::MessageHeader& header) {
    ITCH::RetailPriceImprovementIndicatorMessage msg = {};
    mMessageQueue.push(msg);
}

bool ITCH_Parser::execute() {
    bool success_flag = true;
    while (mReader.tell() < mReader.size() && success_flag) {
        mReader.seek(mReader.tell() + 2);

        ITCH::MessageHeader header = {.message_type = (ITCH::MessageType)mReader.readChar(),
                                      .stock_locate = mReader.read16(),
                                      .tracking_number = mReader.read16(),
                                      .timestamp = mReader.read48()};

        switch (header.message_type) {
            case ITCH::MessageType::SystemEventMessage: {
                SystemEventMessage(header);
                break;
            }
            case ITCH::MessageType::StockDirectoryMessage: {
                StockDirectoryMessage(header);
                break;
            }
            case ITCH::MessageType::StockTradingActionMessage: {
                StockTradingActionMessage(header);
                break;
            }
            case ITCH::MessageType::RegSHOMessage: {
                RegSHOMessage(header);
                break;
            }
            case ITCH::MessageType::MarketParticipantPositionMessage: {
                MarketParticipantPositionMessage(header);
                break;
            }
            case ITCH::MessageType::MWCBDeclineLevelMessage: {
                MWCBDeclineLevelMessage(header);
                break;
            }
            case ITCH::MessageType::MWCBStatusMessage: {
                MWCBStatusMessage(header);
                break;
            }
            case ITCH::MessageType::IPOQuotingPeriodUpdateMessage: {
                IPOQuotingPeriodUpdateMessage(header);
                break;
            }
            case ITCH::MessageType::LULDAuctionCollarMessage: {
                LULDAuctionCollarMessage(header);
                break;
            }
            case ITCH::MessageType::OperationalHaltMessage: {
                OperationalHaltMessage(header);
                break;
            }
            case ITCH::MessageType::AddOrderMessage: {
                AddOrderMessage(header);
                break;
            }
            case ITCH::MessageType::AddOrderMPIDAttributionMessage: {
                AddOrderMPIDAttributionMessage(header);
                break;
            }
            case ITCH::MessageType::OrderExecutedMessage: {
                OrderExecutedMessage(header);
                break;
            }
            case ITCH::MessageType::OrderExecutedWithPriceMessage: {
                OrderExecutedWithPriceMessage(header);
                break;
            }
            case ITCH::MessageType::OrderCancelMessage: {
                OrderCancelMessage(header);
                break;
            }
            case ITCH::MessageType::OrderDeleteMessage: {
                OrderDeleteMessage(header);
                break;
            }
            case ITCH::MessageType::OrderReplaceMessage: {
                OrderReplaceMessage(header);
                break;
            }
            case ITCH::MessageType::NonCrossTradeMessage: {
                NonCrossTradeMessage(header);
                break;
            }
            case ITCH::MessageType::CrossTradeMessage: {
                CrossTradeMessage(header);
                break;
            }
            case ITCH::MessageType::BrokenTradeMessage: {
                BrokenTradeMessage(header);
                break;
            }
            case ITCH::MessageType::RetailPriceImprovementIndicatorMessage: {
                RetailPriceImprovementIndicatorMessage(header);
                break;
            }
            default:
                success_flag = false;
        }
    }
    return success_flag;
}
