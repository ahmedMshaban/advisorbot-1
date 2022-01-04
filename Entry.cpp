#include "Entry.h"


map<string, EntryType> typeMap{{" ", EntryType::invalid},
                                {"ask", EntryType::ask},
                                {"bid", EntryType::bid}};

Entry::Entry(double _price,
             double _amount,
             string _timestamp,
             string _product,
             EntryType _orderType)
    : price(_price),
      amount(_amount),
      timestamp(_timestamp),
      product(_product),
      orderType(_orderType) {}

EntryType Entry::stringToEntryType(string s) {
    if (s == "ask") {
        return EntryType::ask;
    }
    else if (s == "bid") {
        return EntryType::bid;
    }
    else {
        return EntryType::invalid;
    }

    // return typeMap[s];
}
