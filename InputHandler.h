#pragma once
#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <string>

/**
 * InputHandler - Handles user input operations
 * Responsibility: Read user input from console
 */
class InputHandler {
public:
    // Get string input from user
    static std::string ask(const char* prompt);
    
    // Get time input from user and convert to minutes
    static int askTime(const char* prompt);
};

#endif
