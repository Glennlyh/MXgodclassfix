#include "InputHandler.h"
#include "Utils.h"
#include <iostream>

using namespace std;

// ============================================================================
// Get string input from user
// ============================================================================

string InputHandler::ask(const char* prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

// ============================================================================
// Get time input from user and convert to minutes
// ============================================================================

int InputHandler::askTime(const char* prompt) {
    string t = ask(prompt);
    return parseTimeToMinutes(t);
}
