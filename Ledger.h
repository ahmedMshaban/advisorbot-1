#pragma once

#include <string>
#include <vector>

#include "Entry.h"

using namespace std;

class Ledger {
   public:
    Ledger(string filename);
    vector<string> products;
    vector<string> timesteps;
    vector<pair<Entry, int>> getCurrentEntries(EntryType type,
                                               string product,
                                               string timestamp);
    vector<Entry> getAllEntries(EntryType type,
                                string product,
                                string timestamp);
    vector<int> getIndexes(EntryType type, string product, string timestamp);
    string getEarliestTime();
    string getNextTime(string timestamp);
    vector<Entry> matchEntries(string timestamp);
    Entry transactionHandler(Entry bid, Entry ask);
    double getMaxPrice(string product, string timestamp, EntryType type);
    double getMinPrice(string product, string timestamp, EntryType type);
    double getAvgPrice(string product,
                       string startTime,
                       string endTime,
                       EntryType type);
    double predictMin(string product, int timestampIndex, EntryType type);
    double predictMax(string product, int timestampIndex, EntryType type);
    double getChange(string product,
                     int timestampIndex,
                     int steps,
                     EntryType type,
                     string valueType);

   private:
    vector<Entry> currentLedger;
    vector<Entry> original;
    vector<string> getProducts();
    vector<string> getTimesteps();
    void storeProdAndTimesteps();
};