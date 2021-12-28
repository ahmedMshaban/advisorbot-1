#include "AdvisorBot.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "CSVReader.h"
#include "Entry.h"

using namespace std;

AdvisorBot::AdvisorBot() {}

void AdvisorBot::init() {
    isRunning = true;
    vector<string> command;
    currentTime = ledger.getEarliestTime();
    currentTimeIndex = 0;

    advisorPrint({"Hello, how can I help you today?",
                  "Enter \"help\" to see a list of available commands"});

    while (isRunning == true) {
        command = getUserInput();
        processCommand(command);
    }
}

void AdvisorBot::advisorPrint(vector<string> strings) {
    cout << endl;

    for (const string& s : strings) {
        cout << "AdvisorBot: " << s << endl;
    }

    cout << endl;
}

void AdvisorBot::printMenu() {}

vector<string> AdvisorBot::getUserInput() {
    string input;
    cout << "User: ";
    getline(cin, input);
    vector<string> command = CSVReader::tokenise(input, ' ');

    return command;
}

void AdvisorBot::processCommand(vector<string> command) {
    switch (cmdMap[command[0]]) {
        case commands::help:
            printHelp(command);
            break;

        case commands::prod:
            printProd(command);
            break;

        case commands::min:
            printMin(command);
            break;

        case commands::max:
            printMax(command);
            break;

        case commands::avg:
            printAvg(command);
            break;

        case commands::exit:
            exit(command);
            break;

        default:
            advisorPrint({"Please enter a valid command (case sensitive)"});
    }
}

void AdvisorBot::printHelp(vector<string> command) {
    // Input command was "help"
    if (command.size() == 1) {
        advisorPrint(
            {"The available commmands are help, help <cmd>, prod, min, max, "
             "avg, predict, time, step, exit"});
    }
    // Input command was "help <cmd>"
    else if (command.size() == 2) {
        // Switch using the 2nd string in command
        switch (cmdMap[command[1]]) {
            case commands::help:
                advisorPrint({"Displays available commands", "Usage: help"});
                break;
            case commands::prod:
                advisorPrint({"Lists all available products", "Usage: prod"});
                break;
            case commands::min:
                advisorPrint(
                    {"Lists current minimum bid/ask price of desired product",
                     "Usage: min <product> <bid/ask>"});
                break;
            case commands::max:
                advisorPrint(
                    {"Lists current maximum bid/ask price of desired product",
                     "Usage: max <product> <bid/ask>"});
                break;
            case commands::avg:
                advisorPrint(
                    {"Lists average bid/ask price of desired product over the "
                     "desired number of timesteps",
                     "Usage: avg <product> <ask/bid> <timesteps>"});
                break;
            case commands::predict:
                advisorPrint(
                    {"Predicts the maximum/minimum bid/ask of desired product",
                     "Usage: predict <max/min> <product> <ask/bid>"});
                break;
            case commands::time:
                advisorPrint(
                    {"Tells you the current timeframe the program is in",
                     "Usage: time"});
                break;
            case commands::step:
                advisorPrint({"Moves to the next timestep", "Usage: step"});
                break;
            case commands::exit:
                advisorPrint({"Exits program", "Usage: exit"});
                break;
            // Invalid commands will default to commands::invalid
            default:
                advisorPrint({"Please enter a valid command",
                              "Valid help commands are help, prod, min, max, "
                              "avg, predict, time, step, exit"});
        }
    }
}

void AdvisorBot::printProd(vector<string> command) {
    // Input command was "prod"
    if (command.size() == 1) {
        // Initialise output string and retrieve products
        string s;
        // Append each product and a seperator to the output string
        for (string& prod : ledger.products) {
            s.append(prod + ", ");
        }
        // Remove the last seperator at the end of the string
        s.erase(s.end() - 2, s.end());

        advisorPrint({s});
    } else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help prod\" to see valid uses"});
    }
}

void AdvisorBot::printMin(vector<string> command) {
    double price;

    // Check that command contains 3 keywords
    if (command.size() == 3) {
        // Convert type string to entry type
        EntryType type = Entry::stringToEntryType(command[2]);
        // Validate command args
        if (validProd(command[1]) && validType(type)) {
            price = ledger.getMinPrice(command[1], currentTime, type);

            advisorPrint({"The minimum price of " + command[1] + " is " +
                          to_string(price)});
        }
        // If invalid product or entry type
        else {
            advisorPrint({"Please enter a valid command",
                          "Use \"help min\" to see valid uses"});
        }
    }
    // Command does not contain 3 keywords -> invalid input
    else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help min\" to see valid uses"});
    }
}

void AdvisorBot::printMax(vector<string> command) {
    double price;

    // Check that command contains 3 keywords
    if (command.size() == 3) {
        // Convert type string to entry type
        EntryType type = Entry::stringToEntryType(command[2]);
        // Validate command arguments
        if (validProd(command[1]) && validType(type)) {
            price = ledger.getMaxPrice(command[1], currentTime, type);

            advisorPrint({"The maximum price of " + command[1] + " is " +
                          to_string(price)});
        }
        // If invalid product or entry type
        else {
            advisorPrint({"Please enter a valid command",
                          "Use \"help max\" to see valid uses"});
        }
    }
    // Command does not contain 3 keywords -> invalid input
    else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help max\" to see valid uses"});
    }
}

void AdvisorBot::printAvg(vector<string> command) {
    // Check that command contains 4 keywords
    if (command.size() == 4) {
        double avg;
        EntryType type = Entry::stringToEntryType(command[2]);

        // Validate command args
        if (validProd(command[1]) && validType(type) && validInt(command[3])) {
            int steps = stoi(command[3]);


            if (steps > currentTimeIndex) {
                advisorPrint({"Invalid number of time steps entered", "You may only obtain the average up to the beginning of the ledger"});
            }
            else {
                // Get timestamp string of the starting time
                string startTime = ledger.timesteps[currentTimeIndex - steps];
                // Get avg
                avg = ledger.getAvgPrice(command[1], startTime, currentTime, type);

                advisorPrint({"The average " + command[2] + " price of " + command[1] + " over the last " + to_string(steps) + " steps is: " + to_string(avg)});
            }
        }
    }
    // Command does not contain 4 keywords -> invalid input
    else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help avg\" to see valid uses"});
    }
}

void AdvisorBot::exit(vector<string> command) {
    if (command.size() == 1) {
        advisorPrint({"Thank you for using AdvisorBot!"});
        isRunning = false;
    } else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help exit\" to see valid uses"});
    }
}

bool AdvisorBot::validProd(string prod) {
    // https://www.delftstack.com/howto/cpp/cpp-find-element-in-vector/
    return any_of(ledger.products.begin(),
                  ledger.products.end(),
                  [&](const string& s) { return s == prod; });
}

bool AdvisorBot::validType(EntryType type) {
    return  (type == EntryType::ask || type == EntryType::bid);
}

bool AdvisorBot::validInt(string numString) {
    // Loop over each char in numString
    for (char const& c : numString) {
        // If char in string is not digit, string is not a valid int
        if (isdigit(c) == 0) return false;
    }

    return true;
}
