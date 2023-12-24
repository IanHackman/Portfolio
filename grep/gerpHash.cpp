//
// Title:           gerpHash.cpp
//
// Authors:         Miles Mamigonian, Ian Hackman
//
// Created:         2022.12.05
//
// Last Modified:   2022.12.13
//
// Purpose:         Implementation for the gerpHash class:
//                  The gerpHash class is a class that stores a collection of
//                  words in a hash table, allowing a client to add a word
//                  and to get a vector of all instances of a word, either
//                  with or without case sensitivity. This file contains all
//                  of the code that defines what each public and private
//                  member function in the gerpHash class does.
//
#include "gerpHash.h"

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cctype>

/*
 * name:       Hash constructor
 * purpose:    constructs an empty instance of Hash by declaring and 
               intitilizing versionsInfo and chainInfo structs and empty 
               hashTable
 * arguments: none
 * returns:   none
 * effects:   constructs an empty instance of Hash 
 */
Hash::Hash() {
    versionsInfo emptyVersion;
    emptyVersion.word = "";
    std::vector<wordInfo> emptyInstances;
    emptyVersion.instances = emptyInstances;

    chainInfo emptyChainLink;
    emptyChainLink.loweredWord = "";
    std::vector<versionsInfo> emptyVersions;
    emptyVersions.push_back(emptyVersion);
    emptyChainLink.versions = emptyVersions;

    std::vector<chainInfo> emptyChain;
    emptyChain.push_back(emptyChainLink);
    hashTable.push_back(emptyChain);
}

/*
 * name:      insert
 * purpose:   lowercase word, get index from hash function and compression and 
              determine if the word is already in the table or not and call 
              heplper funcitons to insert
 * arguments: string with word, int with line number and int with file index
 * returns:   none
 * effects:   if the word is new, push into hash and chain else call function 
              to push into versions vector in versionsInfo struct
 */
void Hash::insert(std::string word, int lineNum, int fileIndex) {
    if (word == "") {
        return;
    }
    std::string lowered = lowercase(word);
    wordInfo newWordInfo;
    newWordInfo.lineNum = lineNum;
    newWordInfo.fileIndex = fileIndex;
    int index = hashCompression(lowered);
    for (unsigned long i = 0; i < hashTable[index].size(); i++) {
        if (hashTable[index][i].loweredWord == lowered) {
            addExistingWord(word, newWordInfo, index, i);
            rehashIfNeed();
            return;
        }
    }
    hashTable[index].push_back(addNewWord(word, lowered, newWordInfo));
    rehashIfNeed();
}

/*
 * name:      lowercase
 * purpose:   convert uppercase characters of string to lowercase
 * arguments: string with word
 * returns:   string with lowercased word
 * effects:   converts string into all lowercase and returns it
 */
std::string Hash::lowercase(std::string word) {
    for (unsigned long i = 0; i < word.size(); i++) {
        word[i] = tolower(word[i]);
    }
    return word;
}

/*
 * name:      hashCompression
 * purpose:   run string through a hash function and compression to get the 
              index of where to insert it into hashTable
 * arguments: string with lowered word
 * returns:   int with index
 * effects:   generate index of the lowered word
 */
int Hash::hashCompression(std::string lowered) {
    int size = hashTable.size();
    if (size == 0) {
        return size;
    }
    int index = std::hash<std::string>{}(lowered) % size;
    return index;
}

/*
 * name:      addExistingWord
 * purpose:   search through the versions vector, if the word exists, add it to 
              instances. If the word doesn't exist than insert it into the 
              versions vector
 * arguments: string with word, wordInfo struct, int with index and int with 
              chain index
 * returns:   none
 * effects:   if word is already in versions vector add to instances, 
              if not, add to versions
 */
void Hash::addExistingWord(std::string word, wordInfo info, int index, int n) {
    for (unsigned long i = 0; i < hashTable[index][n].versions.size(); i++) {
        if (hashTable[index][n].versions[i].word == word) {
            if ((hashTable[index][n].versions[i].instances.back().lineNum 
                == info.lineNum) 
                and 
                (hashTable[index][n].versions[i].instances.back().fileIndex 
                == info.fileIndex)) {
                return;
            }
            hashTable[index][n].versions[i].instances.push_back(info); 
            return;
        }
    }
    versionsInfo newVersion;
    newVersion.word = word;
    newVersion.instances.push_back(info);
    hashTable[index][n].versions.push_back(newVersion);
    return;
}

/*
 * name:      addNewWord 
 * purpose:   add a new word to the chain
 * arguments: string with word, string with lowercased word, structs with 
              wordInfo
 * returns:   chainInfo struct to be inserted into a chain vector
 * effects:   constructs a chainInfo struct and return it
 */
Hash::chainInfo Hash::addNewWord(std::string word, std::string lowered, 
                                 wordInfo info) {
    chainInfo newChainLink;
    newChainLink.loweredWord = lowered;
    versionsInfo newVersion;
    newVersion.word = word;
    newVersion.instances.push_back(info);
    newChainLink.versions.push_back(newVersion);
    return newChainLink;
}

/*
 * name:      rehashIfNeed
 * purpose:   calculate load factor, call the function to hash the table if 
              the table becomes 3/4 filled
 * arguments: none
 * returns:   none
 * effects:   if the loadfactr is greater than .75, rehash. Else nothing.
 */
void Hash::rehashIfNeed() {
    fillnumber++;
    float f;
    f = fillnumber;
    float size;
    size = hashTable.size();
    float loadfactor = f / size;
    if (loadfactor > .75) {
        rehash(size);
    }
}

/*
 * name:      rehash
 * purpose:   copy contents of hashTable into a new hashTable with twice + 1 of 
              the amount of elements
 * arguments: int with size of table
 * returns:   none
 * effects:   increases size of hashtable and rehashes all elements
 */
void Hash::rehash(int size) {
    std::vector<std::vector<chainInfo>> oldTable = hashTable;
    hashTable.clear();
    hashTable.resize(size * 2 + 1);
    for (int i = 0; i < size; i++) {
        for (unsigned long j = 0; j < oldTable[i].size(); j++) {
            int newIndex = hashCompression(oldTable[i][j].loweredWord);
            hashTable[newIndex].push_back(oldTable[i][j]);
        }
    }
}

/*
 * name:      caseSensitive
 * purpose:   handle case sensitive search, to return a vector containing every 
              instance of the version of the word
 * arguments: strng with user-inputted word
 * returns:   vector of wordIfo structs containing instances of the word
 * effects:   returns vector, doesn't modify any information
 */
std::vector<Hash::wordInfo> Hash::caseSensitive(std::string word) {
    std::string lowered = lowercase(word);
    int index = hashCompression(lowered);
    for (unsigned long i = 0; i < hashTable[index].size(); i++) {
        if (hashTable[index][i].loweredWord == lowered) {
            for (unsigned long j = 0; 
                 j < hashTable[index][i].versions.size(); j++) {
                if (hashTable[index][i].versions[j].word == word) {
                    return hashTable[index][i].versions[j].instances;
                }
            }
        }
    }
    std::vector<Hash::wordInfo> empty;
    return empty;
}

/*
 * name:      caseInSensitive
 * purpose:   handle case insensitive search, to return a vector containing 
              every version of the word. Each version contains every instance 
              of the version of the word
 * arguments: strng with user-inputted word
 * returns:   vector of versionsInfo structs with versions of word
 * effects:   returns vector, doesn't modify any information
 */
std::vector<Hash::versionsInfo> Hash::caseInSensitive(std::string word) {
    std::string lowered = lowercase(word);
    int index = hashCompression(lowered);
    for (unsigned long i = 0; i < hashTable[index].size(); i++) {
        if (hashTable[index][i].loweredWord == lowered) {
            return hashTable[index][i].versions;
        }
    }
    std::vector<Hash::versionsInfo> empty;
    return empty;
}