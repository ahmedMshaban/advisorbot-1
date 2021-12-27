#pragma once

#include <string>
#include <vector>

#include "Entry.h"

using namespace std;

class CSVReader {
   public:
    CSVReader();
    static vector<Entry> readCSV(string fileName);
    static vector<string> tokenise(string s, char separator);
    static Entry stringsToOBE(string price,
                              string amount,
                              string timestamp,
                              string product,
                              EntryType type);

   private:
    static Entry stringsToOBE(vector<string> s);
};