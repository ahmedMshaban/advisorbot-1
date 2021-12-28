#pragma once

#include <string>
#include <vector>

#include "Entry.h"

using namespace std;

class Ledger {
   public:
    Ledger(string filename);
    /** Vector of available products */
    vector<string> products;
    vector<string> timesteps;
    vector<Entry> getOrders(EntryType type, string product, string timestamp);
    string getEarliestTime();
    string getNextTime(string timestamp);
    void inputEntry(Entry order);
    vector<Entry> matchEntries(string timestamp);
    Entry transactionHandler(Entry bid, Entry ask);
    double getMaxPrice(string product,
                       string timestamp,
                       EntryType type);
    double getMinPrice(string product,
                       string timestamp,
                       EntryType type);
    double getAvgPrice(string product,
                       string startTime,
                       string endTime,
                       EntryType type);
    double predictPrice(string product,
                        string timestamp,
                        int steps,
                        EntryType type);

   private:
    vector<Entry> entries;
    vector<string> getProducts();
    vector<string> getTimesteps();
    void storeProdAndTimesteps();
};