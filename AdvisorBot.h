#pragma once

#include <map>
#include <string>
#include <vector>

#include "Ledger.h"

using namespace std;

class AdvisorBot {
   public:
    AdvisorBot();
    void init();

   private:
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
        change,
        exit
    };

    // Map inputs to commands enum
    map<string, commands> cmdMap{{" ", commands::invalid},
                                 {"help", commands::help},
                                 {"prod", commands::prod},
                                 {"min", commands::min},
                                 {"max", commands::max},
                                 {"avg", commands::avg},
                                 {"predict", commands::predict},
                                 {"time", commands::time},
                                 {"step", commands::step},
                                 {"exit", commands::exit},
                                 {"change", commands::change}};

    bool isRunning;
    Ledger ledger{"20200601.csv"};
    // Ledger ledger{"test.csv"};
    string currentTime;
    int currentTimeIndex;

    void advisorPrint(vector<string> strings);
    void printMenu();
    vector<string> getUserInput();
    void processCommand(vector<string> command);
    void printHelp(vector<string> command);
    void printProd(vector<string> command);
    void printMin(vector<string> command);
    void printMax(vector<string> command);
    void printAvg(vector<string> command);
    void printPred(vector<string> command);
    void printTime(vector<string> command);
    void printStep(vector<string> command);
    void printChange(vector<string> command);
    void exit(vector<string> command);

    bool validProd(string prod);
    bool validType(EntryType type);
    bool validInt(string numString);
};