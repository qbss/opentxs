/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#include "CmdShowMarkets.hpp"

#include <opentxs/client/ot_otapi_ot.hpp>

#include <opentxs/core/Log.hpp>

using namespace opentxs;
using namespace std;

CmdShowMarkets::CmdShowMarkets()
{
    command = "showmarkets";
    args[0] = "--server <server>";
    category = catMarkets;
    help = "Show the list of markets.";
}

CmdShowMarkets::~CmdShowMarkets()
{
}

int32_t CmdShowMarkets::runWithOptions()
{
    return run(getOption("server"));
}

int32_t CmdShowMarkets::run(string server)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    OTDB::MarketList* marketList = loadMarketList(server);
    if (nullptr == marketList) {
        otOut << "Error: cannot load market list.\n";
        return -1;
    }

    int32_t items = marketList->GetMarketDataCount();
    if (0 > items) {
        otOut << "Error: cannot load market list item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The market list is empty.\n";
        return 0;
    }

    cout << "Index\tScale\tMarket\t\t\t\t\t\tAsset\t\t\t\t\t\tCurrency\n";

    for (int32_t i = 0; i < items; i++) {
        OTDB::MarketData* marketData = marketList->GetMarketData(i);
        if (nullptr == marketData) {
            otOut << "Error: cannot load market data at index " << i << ".\n";
            return -1;
        }

        cout << i << "\t" << marketData->scale << "\tM "
             << marketData->market_id << "\tA "
             << marketData->instrument_definition_id << "\tC "
             << marketData->currency_type_id << "\n";
    }

    return 1;
}

OTDB::MarketList* CmdShowMarkets::loadMarketList(const string& server)
{
    if (!OTDB::Exists("markets", server, "market_data.bin", "")) {
        otOut << "The market list file doesn't exist.\n";
        return nullptr;
    }

    otWarn << "Markets file exists...Querying list of markets...\n";

    OTDB::Storable* storable = OTDB::QueryObject(
        OTDB::STORED_OBJ_MARKET_LIST, "markets", server, "market_data.bin", "");
    if (nullptr == storable) {
        otOut << "Failed to verify storable object. Probably doesn't exist.\n";
        return nullptr;
    }

    otWarn << "QueryObject worked. Now dynamic casting from storable to "
              "marketlist...\n";

    OTDB::MarketList* marketList = dynamic_cast<OTDB::MarketList*>(storable);
    if (nullptr == marketList) {
        otOut << "Unable to dynamic cast a storable to a marketlist.\n";
        return nullptr;
    }

    return marketList;

    // This WAS a "load or create" sort of function, but I commented out the
    // "create" part because you will literally NEVER need to create this list.
}
