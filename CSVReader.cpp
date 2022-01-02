#include "CSVReader.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Entry.h"

using namespace std;

CSVReader::CSVReader() {}

/** Returns vector of entries and stores products and timesteps via reference */
vector<Entry> CSVReader::readCSV(string fileName,
                                 vector<string>& products,
                                 vector<string>& timesteps) {
    vector<Entry> entries;
    // Map for timesteps and products
    map<string, bool> timeMap;
    map<string, bool> prodMap;

    ifstream csvFile{fileName};
    string line;
    if (csvFile.is_open()) {
        while (getline(csvFile, line)) {
            try {
                Entry entry = stringsToEntry(tokenise(line, ','));
                entries.push_back(entry);
                // Store timesteps and products into map
                timeMap[entry.timestamp] = true;
                prodMap[entry.product] = true;
            } catch (const exception& e) {
                cout << "CSVReader::readCSV bad data" << endl;
            }
        }
        // Push timesteps
        for (const auto& s : timeMap) {
            timesteps.push_back(s.first);
        }
        // Push products
        for (const auto& s : prodMap) {
            products.push_back(s.first);
        }
    }

    cout << "CSVReader::readCSV read " << entries.size() << " entries" << endl;
    return entries;
}

/** Returns a vector of tokens of a string */
vector<string> CSVReader::tokenise(string s, char separator) {
    vector<string> tokens;
    signed int start, end;
    string token;
    start = s.find_first_not_of(separator, 0);
    while (end > 0) {
        end = s.find_first_of(separator, start);
        if (start == s.length() || start == end) {
            break;
        }
        if (end >= 0) {
            token = s.substr(start, end - start);
        } else {
            token = s.substr(start, s.length() - start);
        }
        tokens.push_back(token);
        start = end + 1;
    }

    return tokens;
}

/** Returns an entry object with the tokens passed in */
Entry CSVReader::stringsToEntry(vector<string> tokens) {
    double price, amount;

    // Invalid token input
    if (tokens.size() != 5) {
        cout << "Bad line " << endl;
        throw exception{};
    }
    try {
        price = stod(tokens[3]);
        amount = stod(tokens[4]);
    } catch (const exception& e) {
        cout << "CSVReader::stringsToEntry Bad float! " << tokens[3] << endl;
        cout << "CSVReader::stringsToEntry Bad float! " << tokens[4] << endl;
        throw;
    }

    Entry entry{price,
                amount,
                tokens[0],
                tokens[1],
                Entry::stringToEntryType(tokens[2])};

    return entry;
}
