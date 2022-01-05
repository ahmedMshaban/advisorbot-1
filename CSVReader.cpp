#include "CSVReader.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
// Benchmarking
#include <chrono>

#include "Entry.h"

using namespace std;
using namespace std::chrono;

CSVReader::CSVReader() {}

// Modified from Merklerex
/** Returns vector of entries and stores products and timesteps via reference */
vector<Entry> CSVReader::readCSV(string fileName,
                                 // Reference for products and timesteps vector so I can modify these directly
                                 vector<string>& products,
                                 vector<string>& timesteps) {
    // // Benchmarking
    // auto start = high_resolution_clock::now();
    
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

    // // Benchmarking
    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<milliseconds>(stop - start);
    // cout << "Execution time: " << duration.count() << endl;

    cout << entries.size() << " entries read" << endl;
    return entries;
}

// Copied from Merklerex
/** Returns a vector of tokens of a string */
vector<string> CSVReader::tokenise(string s, char separator) {
    vector<string> tokens;
    int start, end;
    string token;

    start = s.find_first_not_of(separator, 0);
    end = 1;
    do {
        end = s.find_first_of(separator, start);
        if (start == s.length() || start == end) break;
        if (end >= 0)
            token = s.substr(start, end - start);
        else
            token = s.substr(start, s.length() - start);
        tokens.push_back(token);
        start = end + 1;
    } while (end > 0);

    return tokens;
}

// Modified from Merklerex
/** Returns an entry object with the tokens passed in */
Entry CSVReader::stringsToEntry(vector<string> tokens) {
    double price, amount;

    // Invalid token input
    if (tokens.size() != 5) {
        cout << "Bad line " << endl;
        throw exception{};
    }
    try {
        // Changed stod to strtod
        price = strtod(tokens[3].c_str(), NULL);
        amount = strtod(tokens[4].c_str(), NULL);
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
