/*
*/

#include "stringProcessing.h"

#include <iostream>
#include <string>
#include <cctype>

std::string stripNonAlphaNum(std::string input) {
    if (input == "") {
        return input;
    }
    int count = 0;
    while(not isalnum(input[count])) {
        count++;
    }
    input.erase(0, count);
    int size = input.length();
    count = 0;
    while(not isalnum(input[size - count - 1])) {
        count++;
    }
    input.erase(size - count, size - 1);
    return input;
}