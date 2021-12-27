#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "AdvisorBot.h"
#include "CSVReader.h"

using namespace std;

AdvisorBot::AdvisorBot() {}

void AdvisorBot::init() {
    isRunning = true;
    vector<string> command;

    advisorPrint("Hello, how can I help you today?");

    while (isRunning == true) {
        command = getUserInput();
        processCommand(command);
    }
}

void AdvisorBot::advisorPrint(string s) {
    cout << "AdvisorBot: " << s << endl;
}

void AdvisorBot::printMenu() {

}

vector<string> AdvisorBot::getUserInput() {
    string input;
    getline(cin, input);
    vector<string> command = CSVReader::tokenise(input, ' ');

    return command;
}

void AdvisorBot::processCommand (vector<string> command) {
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
    
    switch (cmdMap[command[0]]) {
        case commands::help:
            if (command.size() == 1) {
                cout << command[0] << endl;
                advisorPrint("The available commmands are help, help <cmd>, prod, min, max, avg, predict, time, step, exit");
            } else if (command.size() == 2) {
                switch (cmdMap[command[1]]) {
                    case commands::prod:
                        advisorPrint("Lists all available products");
                        advisorPrint("Usage: prod");
                        break;
                    case commands::min:
                        advisorPrint("Lists current minimum bid/ask price of desired product");
                        advisorPrint("Usage: min <product> <bid/ask>");
                        break;
                    case commands::max:
                        advisorPrint("Lists current maximum bid/ask price of desired product");
                        advisorPrint("Usage: max <product> <bid/ask>");
                        break;
                    case commands::avg:
                        advisorPrint("Lists average bid/ask price of desired product over the desired number of timesteps");
                        advisorPrint("Usage: avg <product> <ask/bid> <timesteps>");
                        break;
                    case commands::predict:
                        advisorPrint("Predicts the maximum/minimum bid/ask of desired product");
                        advisorPrint("Usage: predict <max/min> <product> <ask/bid>");
                        break;
                    case commands::time:
                        advisorPrint("Tells you the current timeframe the program is in");
                        advisorPrint("Usage: time");
                        break;
                    case commands::step:
                        advisorPrint("Moves to the next timestep");
                        advisorPrint("Usage: step");
                        break;
                    case commands::exit:
                        advisorPrint("Exits program");
                        advisorPrint("Usage: exit");
                        break;
                }
            }
            break;

        case commands::prod:
            if (command.size() == 1) {
                printProd();
            }
            break;

        case commands::min:
            if (command.size() == 3) {
                
            }
            break;

        default:
            cout << endl;
            cout << "Please enter a valid command (case sensitive)" << endl;
    }
}

void AdvisorBot::printProd() {
    string s;
    vector<string> products = ledger.getProducts();
    // Append each product and a seperator to the string
    for (string& prod : products) {
        s.append(prod + ", ");
    }
    // Remove the last ", " at the end of the string
    s.erase(s.end() - 2, s.end());

    advisorPrint(s);
}