#include "Ledger.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

#include "AdvisorBot.h"
#include "CSVReader.h"

using namespace std;

/** Generate ledger from csv file */
Ledger::Ledger(string filename) {
    currentLedger = CSVReader::readCSV(filename, products, timesteps);
    original = currentLedger;
}

/** Retrieve current entries according to parameters set */
vector<pair<Entry, int>> Ledger::getCurrentEntries(EntryType type,
                                           string product,
                                           string timestamp) {
    vector<pair<Entry, int>> orders;

    for (int i = 0; i < currentLedger.size(); i++) {
        // Check that all params match
        if (currentLedger[i].orderType == type && currentLedger[i].product == product &&
            currentLedger[i].timestamp == timestamp) {
            orders.push_back({currentLedger[i], i});
        }
    }

    return orders;
}

/** Retrieve all entries according to parameters set */
vector<Entry> Ledger::getAllEntries(EntryType type,
                                    string product,
                                    string timestamp) {
    vector<Entry> orders;

    for (int i = 0; i < currentLedger.size(); i++) {
        // Check that all params match
        if (currentLedger[i].orderType == type && currentLedger[i].product == product &&
            currentLedger[i].timestamp == timestamp) {
            orders.push_back(currentLedger[i]);
        }
    }

    return orders;
}

/** Returns the earliest timestamp of the ledger */
string Ledger::getEarliestTime() { return currentLedger[0].timestamp; }

/** Returns the next timestamp in the program */
string Ledger::getNextTime(string timestamp) {
    // Search for next timestamp
    for (Entry& e : currentLedger) {
        if (e.timestamp > timestamp) {
            return e.timestamp;
        }
    }

    // Loop back to start if on the last timestep
    return currentLedger[0].timestamp;
}

/** Matches bids and asks when moving to the next timestep */
vector<Entry> Ledger::matchEntries(string timestamp) {
    // Output
    vector<Entry> sales;

    // Loop over all products
    for (const string& prod : products) {
        // Vectors to store bids and asks we are comparing
        vector<pair<Entry, int>> bids;
        vector<pair<Entry, int>> asks;

        // Vector to store indexes of entries to delete
        vector<int> markedForDeletion;

        // Loop through all timesteps
        for (int i = 0; i < timesteps.size(); i++) {
            // If we went past the current time, break the loop
            if (timesteps[i] > timestamp) {
                break;
            }
            // Retrieve all bids and asks at each timestep
            vector<pair<Entry, int>> currentBids =
                getCurrentEntries(EntryType::bid, prod, timesteps[i]);
            bids.insert(bids.end(), currentBids.begin(), currentBids.end());

            vector<pair<Entry, int>> currentAsks =
                getCurrentEntries(EntryType::ask, prod, timesteps[i]);
            asks.insert(asks.end(), currentAsks.begin(), currentAsks.end());
        }

        // Immediately return if no bids or asks since no sale can be made
        if (bids.size() == 0 || asks.size() == 0) {
            cout << "There are no bids or no asks" << endl;
            return sales;
        }

        // Sort bids and asks such that highest bid and lowest ask is first
        std::sort(bids.begin(), bids.end(), Entry::sortByPriceDesc);
        std::sort(asks.begin(), asks.end(), Entry::sortByPriceAsc);

        // Loop over asks
        for (int i = 0; i < asks.size(); i++) {
            // Loop over bids
            for (int j = 0; j < bids.size(); j++) {
                // If bid price >= ask price we match
                if (bids[j].first.price >= asks[i].first.price) {
                    Entry sale =
                        transactionHandler(bids[j].first, asks[i].first);
                    sales.push_back(sale);

                    // Calculate remaining bid and ask amount left
                    currentLedger[bids[j].second].amount -= sale.amount;
                    currentLedger[asks[i].second].amount -= sale.amount;

                    // If bid amount was exhausted
                    if (currentLedger[bids[j].second].amount == 0) {
                        // Remove bid entry from bids vector and mark it for
                        // deletion
                        markedForDeletion.push_back(bids[j].second);
                        bids.erase(bids.begin() + j);
                        // Decrement bids iterator
                        j--;
                    }

                    // If ask amount was exhausted
                    if (currentLedger[asks[i].second].amount == 0) {
                        // Remove ask entry from asks vector and mark it for
                        // deletion
                        markedForDeletion.push_back(asks[i].second);
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

        // Sort markedForDeletion in descending order
        // If we do not sort, the range-for loop may delete earlier indexes
        // before later ones, causing the vector to shrink This will cause the
        // indexes of the entries we want to delete to shift, which we want to
        // avoid to keep it simple
        sort(markedForDeletion.begin(),
             markedForDeletion.end(),
             greater<int>());
        // Delete empty entries
        for (const int& index : markedForDeletion) {
            currentLedger.erase(currentLedger.begin() + index);
        }
    }

    return sales;
}

/** Abstracted function to generate a sale entry object */
Entry Ledger::transactionHandler(Entry bid, Entry ask) {
    Entry sale{ask.price,
               0,
               ask.timestamp,
               ask.product,
               EntryType::sale};

    if (ask.amount >= bid.amount) {
        sale.amount = bid.amount;
    } else if (ask.amount < bid.amount) {
        sale.amount = ask.amount;
    }

    return sale;
}

/** Returns max bid/ask price of a product in the current timestamp */
double Ledger::getMaxPrice(string product, string timestamp, EntryType type) {
    double price;
    // Retrieve orders according to filters
    vector<Entry> orders = getAllEntries(type, product, timestamp);
    // Set price to first order in orders vector
    price = orders[0].price;

    // Loop over orders
    for (Entry const& e : orders) {
        // Compare price
        if (e.price > price) {
            price = e.price;
        }
    }

    return price;
}

/** Returns min bid/ask price of a product in the current timestamp */
double Ledger::getMinPrice(string product, string timestamp, EntryType type) {
    double price;
    // Retrieve orders according to filters
    vector<Entry> orders = getAllEntries(type, product, timestamp);
    // Set price to first order in orders vector
    price = orders[0].price;

    // Loop over orders
    for (Entry const& e : orders) {
        // Compare price
        if (e.price < price) {
            price = e.price;
        }
    }

    return price;
}

/** Returns the average price of a product over a period of time */
double Ledger::getAvgPrice(string product,
                           string startTime,
                           string endTime,
                           EntryType type) {
    // Find the starting index in the currentLedger vector
    int startIndex =
        distance(currentLedger.begin(),
                 find_if(currentLedger.begin(), currentLedger.end(), [&](const Entry& e) {
                     return e.timestamp == startTime;
                 }));

    double sum = 0;
    int elems = 0;

    // Loop over the currentLedger vector, beginning at the starting index, and ending
    // when the timestamp of the entry exceeds the current time
    for (int i = startIndex; currentLedger[i].timestamp <= endTime; i++) {
        if (currentLedger[i].orderType == type && currentLedger[i].product == product) {
            sum += currentLedger[i].price;
            elems++;
        }
    }

    double avg = sum / elems;

    return avg;
}

/** Predicts the min price of a product in the next timestep */
double Ledger::predictMin(string product, int timestampIndex, EntryType type) {
    double minPred;
    int currentTimeIndex = timestampIndex;

    while (currentTimeIndex >= 0) {
        vector<Entry> orders = getAllEntries(type, product, timesteps[currentTimeIndex]);
        double localMin = orders[0].price;
        for (const Entry& e : orders) {
            if (e.price < localMin) {
                localMin = e.price;
            }
        }

        minPred += localMin;

        currentTimeIndex--;
    }

    minPred /= timestampIndex + 1;

    return minPred;
}

/** Predicts the max price of a product in the next timestep */
double Ledger::predictMax(string product, int timestampIndex, EntryType type) {
    double maxPred;
    int currentTimeIndex = timestampIndex;

    while (currentTimeIndex >= 0) {
        vector<Entry> orders = getAllEntries(type, product, timesteps[currentTimeIndex]);
        double localMax = orders[0].price;
        for (const Entry& e : orders) {
            if (e.price > localMax) {
                localMax = e.price;
            }
        }

        maxPred += localMax;

        currentTimeIndex--;
    }

    maxPred /= timestampIndex + 1;

    return maxPred;
}

/** Returns the percentage change in the min/max price of a product over a period of time */
double Ledger::getChange(string product,
                         int timestampIndex,
                         int steps,
                         EntryType type,
                         string valueType) {
    double pChange;
    double startPrice;
    double currPrice;

    // Get min prices
    if (valueType == "min") {
        startPrice = getMinPrice(product, timesteps[timestampIndex - steps], type);
        currPrice = getMinPrice(product, timesteps[timestampIndex], type);
    }
    // Get max prices
    else if (valueType == "max") {
        startPrice = getMaxPrice(product, timesteps[timestampIndex - steps], type);
        currPrice = getMaxPrice(product, timesteps[timestampIndex], type);
    }
    // Get avg prices
    else if (valueType == "avg") {
        startPrice = getAvgPrice(product, timesteps[timestampIndex - steps], timesteps[timestampIndex - steps], type);
        currPrice = getAvgPrice(product, timesteps[timestampIndex], timesteps[timestampIndex], type);
    }

    pChange = (currPrice - startPrice/startPrice)*100;

    return pChange;
}
