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

/** Format output text for advisorbot output */
void AdvisorBot::advisorPrint(vector<string> strings) {
    cout << endl;

    for (const string& s : strings) {
        cout << "AdvisorBot: " << s << endl;
    }

    cout << endl;
}

/** Collect and return user input */
vector<string> AdvisorBot::getUserInput() {
    string input;
    cout << "User: ";
    getline(cin, input);
    vector<string> command = CSVReader::tokenise(input, ' ');

    return command;
}

/** User command parsing */
void AdvisorBot::processCommand(vector<string> command) {
    // Maps string to command enum class to enable the use of switch statement
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

        case commands::predict:
            printPred(command);
            break;

        case commands::time:
            printTime(command);
            break;

        case commands::step:
            printStep(command);
            break;

        case commands::change:
            printChange(command);
            break;

        case commands::exit:
            exit(command);
            break;

        // If none match -> invalid command
        default:
            advisorPrint({"Please enter a valid command (case sensitive)", "Enter \"help\" to see a list of available commands"});
    }
}

/** Print help menu */
void AdvisorBot::printHelp(vector<string> command) {
    // Input command was "help"
    if (command.size() == 1) {
        advisorPrint(
            {"The available commmands are help, help <cmd>, prod, min, max, "
             "avg, predict, time, step, change, exit"});
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

            case commands::change:
                advisorPrint({"Shows the percentage change in the price of a particular product over the desired number of timesteps", "Usage: change <min/max> <product> <type> <timesteps>"});
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

/** Print list of products */
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

        advisorPrint({"The available products are " + s});
    } else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help prod\" to see valid uses"});
    }
}

/** Prints current minimum price of a particular product */
void AdvisorBot::printMin(vector<string> command) {
    double price;

    // Check that command contains 3 keywords
    if (command.size() == 3) {
        // Convert type string to entry type
        EntryType type = Entry::stringToEntryType(command[2]);
        // Validate command args
        if (validProd(command[1]) && validType(type)) {
            price = ledger.getMinPrice(command[1], currentTime, type);

            advisorPrint({"The minimum " + command[2] + " price of " + command[1] + " is " +
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

/** Prints current maximum price of a particular product */
void AdvisorBot::printMax(vector<string> command) {
    double price;

    // Check that command contains 3 keywords
    if (command.size() == 3) {
        // Convert type string to entry type
        EntryType type = Entry::stringToEntryType(command[2]);
        // Validate command arguments
        if (validProd(command[1]) && validType(type)) {
            price = ledger.getMaxPrice(command[1], currentTime, type);

            advisorPrint({"The maximum " + command[2] + " price of " + command[1] + " is " +
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

/** Prints average price of a particular product over a certain number of timesteps */
void AdvisorBot::printAvg(vector<string> command) {
    // Check that command contains 4 keywords
    if (command.size() == 4) {
        double avg;
        EntryType type = Entry::stringToEntryType(command[2]);

        // Validate command args
        if (validProd(command[1]) && validType(type) && validInt(command[3])) {
            int steps = stoi(command[3]);

            // Check for valid time steps
            if (steps > currentTimeIndex) {
                advisorPrint({"Invalid number of time steps entered",
                              "You may only obtain the average up to the "
                              "beginning of the ledger"});
            } else {
                // Get timestamp string of the starting time
                string startTime = ledger.timesteps[currentTimeIndex - steps];
                // Get avg
                avg = ledger.getAvgPrice(command[1],
                                         startTime,
                                         currentTime,
                                         type);

                advisorPrint({"The average " + command[2] + " price of " +
                              command[1] + " over the last " +
                              to_string(steps) +
                              " steps is: " + to_string(avg)});
            }
        }
        // Invalid args
        else {
            advisorPrint({"Please enter a valid command",
                        "Use \"help avg\" to see valid uses"});
        }
    }
    // Command does not contain 4 keywords -> invalid input
    else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help avg\" to see valid uses"});
    }
}

/** Prints the predicted max/min price of a particular product in the next timestep */
void AdvisorBot::printPred(vector<string> command) {
    if (command.size() == 4) {
        EntryType type = Entry::stringToEntryType(command[3]);

        if (validProd(command[2]) && validType(type)) {
            if (command[1] == "min") {
                double minPred =
                    ledger.predictMin(command[2], currentTimeIndex, type);
                advisorPrint({"The predicted minimum " + command[3] + " price of " + command[2] + " is " + to_string(minPred)});
            } else if (command[1] == "max") {
                double maxPred =
                    ledger.predictMax(command[2], currentTimeIndex, type);
                advisorPrint({"The predicted maximum " + command[3] + " price of " + command[2] + " is " + to_string(maxPred)});
            } else {
                advisorPrint({"Please enter a valid command",
                              "Use \"help predict\" to see valid uses"});
            }
        }
    }
    // Command does not contain 4 keywords -> invalid input
    else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help predict\" to see valid uses"});
    }
}

/** Prints current time */
void AdvisorBot::printTime(vector<string> command) {
    if (command.size() == 1) {
        advisorPrint({"The current time is: " + currentTime});
    } else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help time\" to see valid uses"});
    }
}

/** Steps into the next timestep and prints sales (if any) */
void AdvisorBot::printStep(vector<string> command) {
    if (command.size() == 1) {
        // Match entries
        vector<Entry> sales = ledger.matchEntries(currentTime);

        // Enter next timestep
        if (currentTimeIndex <= ledger.timesteps.size() - 1) {
            currentTimeIndex++;
            currentTime = ledger.timesteps[currentTimeIndex];
        } else {
            advisorPrint({"Returning to first timestep"});
            currentTimeIndex = 0;
            currentTime = ledger.getEarliestTime();
        }

        vector<string> saleText;

        // Print sales if any were made
        if (sales.size() > 0) {
            for (Entry const& entry : sales) {
                saleText.push_back("Sold " + to_string(entry.amount) + " " +
                                entry.product + " at the price of " +
                                to_string(entry.price));
            }
            advisorPrint(saleText);
        }

        advisorPrint({"The current time is " + currentTime});
    }
    else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help step\" to see valid uses"});
    }
}

/** Prints the percentage change in the price of a particular product over a certain number of timesteps */
void AdvisorBot::printChange(vector<string> command) {
    if (command.size() == 5) {
        EntryType type = Entry::stringToEntryType(command[3]);
        // Validate args
        if (validProd(command[2]) && validType(type) && validInt(command[4]) && command[1] == "min" || command[1] == "max" || command[1] == "avg") {
            int steps = stoi(command[4]);

            // Check for valid time steps
            if (steps > currentTimeIndex) {
                advisorPrint({"Invalid number of time steps entered",
                              "You may only calculate up to the "
                              "beginning of the ledger"});
            } else {              
                double change = ledger.getChange(command[2], currentTimeIndex, steps, type, command[1]);
                advisorPrint({"The price of " + command[2] + " has changed by " + to_string(change) + "% over the last " + command[4] + " timesteps"});
            }
        }
    }
    else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help change\" to see valid uses"});
    }
}

/** Exits program */
void AdvisorBot::exit(vector<string> command) {
    if (command.size() == 1) {
        advisorPrint({"Thank you for using AdvisorBot!"});
        isRunning = false;
    } else {
        advisorPrint({"Please enter a valid command",
                      "Use \"help exit\" to see valid uses"});
    }
}

/** Validate if input string is a valid product */
bool AdvisorBot::validProd(string prod) {
    // https://www.delftstack.com/howto/cpp/cpp-find-element-in-vector/
    return any_of(ledger.products.begin(),
                  ledger.products.end(),
                  [&](const string& s) { return s == prod; });
}

/** Validate if input type is a valid type */
bool AdvisorBot::validType(EntryType type) {
    return (type == EntryType::ask || type == EntryType::bid);
}

/** Validate if input string is an integer */
bool AdvisorBot::validInt(string numString) {
    // Loop over each char in numString
    for (char const& c : numString) {
        // If char in string is not digit, string is not a valid int
        if (isdigit(c) == 0) return false;
    }

    return true;
}
