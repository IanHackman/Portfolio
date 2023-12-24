//
// Title:           gerpHash.h
//
// Authors:         Miles Mamigonian, Ian Hackman
//
// Created:         2022.12.05
//
// Last Modified:   2022.12.11
//
// Purpose:         Interface for the gerpHash class:
//                  The gerpHash class is a class that stores a collection of
//                  words in a hash table, allowing a client to add a word
//                  and to get a vector of all instances of a word, either
//                  with or without case sensitivity. This file provides
//                  the client with access to the function declarations 
//                  without giving them access to the the inner workings of
//                  gerpHash.
//

#ifndef GERP_HASH_H
#define GERP_HASH_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>

class Hash {

public: 
    struct wordInfo {
        int lineNum;
        int fileIndex;
    };

    struct versionsInfo {
        std::string word;
        std::vector<wordInfo> instances;
    };

    struct chainInfo {
        std::string loweredWord;
        std::vector<versionsInfo> versions;
    };
    
    Hash();
    void insert(std::string word, int lineNum, int fileIndex);
    std::vector<wordInfo> caseSensitive(std::string word);
    std::vector<versionsInfo> caseInSensitive(std::string word);

private:

    std::string lowercase(std::string word);
    int hashCompression(std::string lowered);
    void addExistingWord(std::string word, wordInfo info, int index, int n);
    chainInfo addNewWord(std::string word, std::string lowered, wordInfo info);
    void rehashIfNeed();
    void rehash(int size);
    
    int fillnumber = 0;
    std::vector<std::vector<chainInfo>> hashTable;
};

#endif