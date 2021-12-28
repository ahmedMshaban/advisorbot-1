#include "Ledger.h"

#include <algorithm>
#include <map>
#include <string>
#include <iostream>

#include "CSVReader.h"
#include "AdvisorBot.h"

using namespace std;

/** Generate ledger from csv file */
Ledger::Ledger(string filename) {
    entries = CSVReader::readCSV(filename);
    // products = getProducts();
    // timesteps = getTimesteps();
    storeProdAndTimesteps();
}

/** Retrieve all orders according to parameters set */
vector<Entry> Ledger::getOrders(EntryType type,
                                string product,
                                string timestamp) {
    vector<Entry> orders;

    for (Entry& e : entries) {
        // Check that all params match
        if (e.orderType == type && e.product == product &&
            e.timestamp == timestamp) {
            orders.push_back(e);
        }
    }

    return orders;
}

string Ledger::getEarliestTime() { return entries[0].timestamp; }

string Ledger::getNextTime(string timestamp) {
    // Search for next timestamp
    for (Entry& e : entries) {
        if (e.timestamp > timestamp) {
            return e.timestamp;
        }
    }

    // Loop back to start if on the last timestep
    return entries[0].timestamp;
}

void Ledger::inputEntry(Entry order) {
    int pos;

    // Search for the next timestamp
    for (int i = 0; i < entries.size(); i++) {
        if (entries[i].timestamp > order.timestamp) {
            pos = i;
            break;
        }
    }

    // Insert order at the next timestamp
    entries.insert(entries.begin() + pos, order);
}

vector<Entry> Ledger::matchEntries(string timestamp) {
    // For each product
    for (string& product : products) {
        vector<Entry> sales;

        // Get bids and asks for the product
        vector<Entry> bids = getOrders(EntryType::bid, product, timestamp);
        vector<Entry> asks = getOrders(EntryType::ask, product, timestamp);

        // Immediately return of no bids or asks since no sale can be made
        if (bids.size() == 0 || asks.size() == 0) {
            return sales;
        }

        // Sort asks by lowest first
        std::sort(asks.begin(), asks.end(), Entry::sortByPriceAsc);
        // Sort bids by highest first
        std::sort(bids.begin(), bids.end(), Entry::sortByPriceDesc);

        // Loop over asks
        for (int i = 0; i < asks.size(); i++) {
            // Loop over bids
            for (int j = 0; j < bids.size(); j++) {
                // If bid >= ask price we match
                if (bids[j].price >= asks[i].price) {
                    Entry sale = transactionHandler(bids[j], asks[i]);
                    sales.push_back(sale);

                    // Calculate remaining bid amount left
                    bids[j].amount = bids[j].amount - sale.amount;
                    // If bid amount was exhausted
                    if (bids[j].amount == 0) {
                        // Remove bid entry from bids vector
                        bids.erase(bids.begin() + j);
                        // Decrement bids iterator
                        j--;
                    }

                    // Calculate remaining ask amount left
                    asks[i].amount = asks[i].amount - sale.amount;
                    // If ask was exhausted
                    if (asks[i].amount == 0) {
                        // Remove ask entry from asks vector
                        asks.erase(asks.begin() + i);
                        // Decrement asks iterator
                        i--;
                        break;
                    }
                } else {
                    // Otherwise, we break the bids loop
                    // Since bids are sorted by highest first, if the first bid
                    // does not match, subsequent bids will definitely not match
                    break;
                }
            }
        }
    }
}

Entry Ledger::transactionHandler(Entry bid, Entry ask) {
    Entry sale{ask.price,
               0,
               ask.timestamp,
               ask.product,
               EntryType::sale,
               false};

    if (bid.isUser == true) {
        sale.isUser = true;
        // Deduct transaction amount from user's wallet
    } else if (ask.isUser == true) {
        sale.isUser = true;
        // Add transaction amount to user's wallet
    }

    if (ask.amount >= bid.amount) {
        sale.amount = bid.amount;
    } else if (ask.amount < bid.amount) {
        sale.amount = ask.amount;
    }

    return sale;
}

double Ledger::getMaxPrice(string product,
                           string timestamp,
                           EntryType type) {
    double price;
    // Retrieve orders according to filters
    vector<Entry> orders = getOrders(type, product, timestamp);
    // Set price to first order in orders vector
    price = orders[0].price;

    // Loop over orders
    for (Entry const& order : orders) {
        // Compare price
        if (order.price > price) {
            price = order.price;
        }
    }

    return price;
}

double Ledger::getMinPrice(string product,
                           string timestamp,
                           EntryType type) {
    double price;
    // Retrieve orders according to filters
    vector<Entry> orders = getOrders(type, product, timestamp);
    // Set price to first order in orders vector
    price = orders[0].price;

    // Loop over orders
    for (Entry const& order : orders) {
        // Compare price
        if (order.price < price) {
            price = order.price;
        }
    }

    return price;
}

double Ledger::getAvgPrice(string product,
                           string startTime,
                           string endTime,
                           EntryType type) {
        // Find the starting index in the entries vector
        int startIndex = distance(entries.begin(), find_if(entries.begin(), entries.end(), [&] (const Entry& e) { return e.timestamp == startTime; }));

        double sum = 0;
        int elems = 0;

        // Loop over the entries vector, beginning at the starting index, and ending when the timestamp of the entry exceeds the current time
        for (int i = startIndex; entries[i].timestamp <= endTime; i++) {
            if (entries[i].orderType == type && entries[i].product == product) {
                sum += entries[i].price;
                elems++;
            }
        }

        double avg = sum/elems;
        
        return avg;
    }

// Find index of first entry in entries


double Ledger::predictPrice(string product,
                            string timestamp,
                            int steps,
                            EntryType type) {}

/** Retrieves a vector of all existing timesteps */
void Ledger::storeProdAndTimesteps() {
    map<string, bool> timeMap;
    map<string, bool> prodMap;

    // Generate map for both
    for (Entry& e : entries) {
        timeMap[e.timestamp] = true;
        prodMap[e.product] = true;
    }

    // Push timesteps
    for (auto const& e : timeMap) {
        timesteps.push_back(e.first);
    }

    // Push products
    for (auto const& e : prodMap) {
        products.push_back(e.first);
    }
}
