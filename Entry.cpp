#include "Entry.h"

Entry::Entry(double _price,
             double _amount,
             string _timestamp,
             string _product,
             EntryType _orderType,
             bool _isUser)
    : price(_price),
      amount(_amount),
      timestamp(_timestamp),
      product(_product),
      orderType(_orderType),
      isUser(_isUser) {}

EntryType Entry::stringToEntryType(string s) {
    if (s == "ask") {
        return EntryType::ask;
    }
    if (s == "bid") {
        return EntryType::bid;
    }
}
