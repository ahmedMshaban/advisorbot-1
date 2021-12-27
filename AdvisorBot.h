#pragma once

#include <vector>
#include <string>

#include "Ledger.h"

using namespace std;

class AdvisorBot {
    public:
     AdvisorBot();
     void init();
    
    private:
     bool isRunning;
     Ledger ledger{"20200601.csv"};

     void advisorPrint(string s);
     void printMenu();
     vector<string> getUserInput();
     void processCommand(vector<string> command);
     void printProd();
     void exit();

};