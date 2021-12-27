#pragma once

#include <vector>
#include <string>
#include <map>

#include "Ledger.h"

using namespace std;

// enum class for valid commands
enum class commands {
    invalid,
    help,
    prod,
    min,
    max,
    avg,
    predict,
    time,
    step,
    exit
};

// Map inputs to commands enum
static map<string, commands> cmdMap {
    {" ", commands::invalid},
    {"help", commands::help},
    {"prod", commands::prod},
    {"min", commands::min},
    {"max", commands::max},
    {"avg", commands::avg},
    {"predict", commands::predict},
    {"time", commands::time},
    {"step", commands::step},
    {"exit", commands::exit}
};

class AdvisorBot {
    public:
     AdvisorBot();
     void init();
    
    private:
     bool isRunning;
     Ledger ledger{"20200601.csv"};
     string currentTime;
     vector<string> products;

     void advisorPrint(vector<string> strings);
     void printMenu();
     vector<string> getUserInput();
     void processCommand(vector<string> command);
     void printHelp(vector<string> command);
     void printProd(vector<string> command);
     void printMin(vector<string> command);
     void printMax(vector<string> command);
     void exit(vector<string> command);

};