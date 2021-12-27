#pragma once

#include <string>

using namespace std;

enum class EntryType {
    bid,
    ask,
    sale,
    invalid
};

class Entry {
   public:
    Entry(double _price,
          double _amount,
          string _timestamp,
          string _product,
          EntryType _orderType,
          bool _isUser);
    
    static EntryType stringToEntryType(string s);

    static bool sortByPriceAsc(Entry& e1, Entry& e2) {
        return e1.price < e2.price;
    }
    static bool sortByPriceDesc(Entry& e1, Entry& e2) {
        return e1.price > e2.price;
    }


    double price;
    double amount;
    string timestamp;
    string product;
    EntryType orderType;
    bool isUser;

    private:
};