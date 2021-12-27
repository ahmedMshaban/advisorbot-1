#include "CSVReader.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Entry.h"

using namespace std;

CSVReader::CSVReader() {}

vector<Entry> CSVReader::readCSV(string fileName) {
    vector<Entry> entries;

    ifstream csvFile{fileName};
    string line;
    if (csvFile.is_open()) {
        while (getline(csvFile, line)) {
            try {
                Entry obe = stringsToOBE(tokenise(line, ','));
                entries.push_back(obe);
            } catch (const exception& e) {
                cout << "CSVReader::readCSV bad data" << endl;
            }
        }
    }

    cout << "CSVReader::readCSV read " << entries.size() << " entries"
            << endl;
    return entries;
}

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

Entry CSVReader::stringsToOBE(string priceString,
                              string amountString,
                              string timestamp,
                              string product,
                              EntryType orderType) {
    double price, amount;
    try {
        price = stod(priceString);
        amount = stod(amountString);
    } catch (const exception& e) {
        cout << "CSVReader::stringsToOBE Bad float! " << priceString << endl;
        cout << "CSVReader::stringsToOBE Bad float! " << amountString << endl;
        throw;
    }
    Entry obe{price, amount, timestamp, product, orderType, true};

    return obe;
}

Entry CSVReader::stringsToOBE(vector<string> tokens) {
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
        cout << "CSVReader::stringsToOBE Bad float! " << tokens[3] << endl;
        cout << "CSVReader::stringsToOBE Bad float! " << tokens[4] << endl;
        throw;
    }

    Entry obe{price,
              amount,
              tokens[0],
              tokens[1],
              Entry::stringToEntryType(tokens[2]),
              false};

    return obe;
}
