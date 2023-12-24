//
// Title:           Gerp.cpp
//
// Authors:         Miles Mamigonian, Ian Hackman
//
// Created:         2022.12.05
//
// Last Modified:   2022.12.13
//
// Purpose:         Implementation for the Gerp class:
//                  When a client creates a Gerp object and gives it a name
//                  of the root of a file system and the name of an output
//                  file, gerp.cpp will create a gerpHash object, then
//                  insert() each word in the file system into the hash.
//                  Gerp will then output to terminal a call for a prompt
//                  and will print to the output file all the instances of
//                  the word that the user inputs.
//

#include "DirNode.h"
#include "FSTree.h"
#include "gerpHash.h"
#include "Gerp.h"

#include <iostream>
#include <string>
#include <cctype>
#include <fstream>
#include <istream>
#include <sstream>

/*
 * name:      Gerp constructor
 * purpose:   check if the correct number of arguments has been provided and 
              send input directory and output file to helper funcitons to be 
              processed.
 * arguments: number of command line arguments and poiner to array with 
              arguments
 * returns:   none
 * effects:   constructs an instance of Gerp and passes command line argumnts 
              to be processed
 */
Gerp::Gerp(int argc, char *argv[]) {
    if (argc != 3) {
        abort("Usage: ./gerp inputDirectory outputFile");
    }
    buildHash(argv[1]);
    commands(argv[2]);
}

Gerp::~Gerp() {}

/*
 * name:      abort 
 * purpose:   construct error messagages
 * arguments: string containing the wording of the error message
 * returns:   none
 * effects:   terminates program
 */
void Gerp::abort(std::string error_message) {
    std::cerr << error_message << std::endl;
    exit(EXIT_FAILURE);
}

/*
 * name:      buildHash
 * purpose:   constructs tree with directories, subdirectories and files
 * arguments: string with name of directory to be processed
 * returns:   none
 * effects:   constructs tree with directory, passes root to be read by helper 
              function
 */
void Gerp::buildHash(std::string directory) {
    FSTree newTree(directory);
    DirNode *root = newTree.getRoot();
    addTreeToHash(root, directory + "/");
}

/*
 * name:      addTreetoHash
 * purpose:   traverse the directory tree to reach the files
 * arguments: pointer to root of tree, string with pathway
 * returns:   none
 * effects:   stores the pathway to each file in a string, passes information 
              to helper function that will use the pathway to open the file
 */
void Gerp::addTreeToHash(DirNode *node, std::string path) {
    for (int i = 0; i < node->numFiles(); i++) {
        addFileToHash(path, node->getFile(i));
    }
    
    if (not node->hasSubDir()) {
        return;
    } else {
        for (int i = 0; i < node->numSubDirs(); i++) {
            addTreeToHash(node->getSubDir(i), 
                          path + node->getSubDir(i)->getName() + "/");
        }
    }
}

/*
 * name:      commands
 * purpose:   runs command loop for gerp
 * arguments: string with name of output file
 * returns:   none
 * effects:   runs command loop, calls helper funcitons to carry out each 
              user-inputted command
 */
void Gerp::commands(std::string outputFile) {
    std::ofstream output;
    open_or_die(output, outputFile);
    
    std::string query;
    std::cout << "Query? ";
    
    while (std::cin >> query) {
        if (query == "@i" or query == "@insensitive") {
            std::cin >> query;
            caseInSen(stripNonAlphaNum(query), output);
        } else if (query == "@f") {
            output.close();
            std::cin >> query;
            open_or_die(output, query);
        } else if (query == "@q" or query == "@quit") {
            output.close();
            std::cout << "Goodbye! Thank you and have a nice day." << std::endl;
            return;
        } else {
            caseSen(stripNonAlphaNum(query), output);
        }
        std::cout << "Query? ";
    }
    
    std::cout << "Goodbye! Thank you and have a nice day." << std::endl;
}

/*
 * name:      addFileToHash
 * purpose:   opens files, reads file line by line and word by word to pass 
              words to Hash table
 * arguments: string with pathway, string with file name
 * returns:   none
 * effects:   opens file, reads contents, passes words to hash table, closes 
              file
 */
void Gerp::addFileToHash(std::string pathway, std::string fileName) {
    std::ifstream inputFile;
    std::string pathFile = pathway + fileName;
    open_or_die(inputFile, pathFile);
    
    int lineNum = 0;
    std::string line;
    
    while(std::getline(inputFile, line)) {
        lineNum++;
        if (lineNum == 1) {
            std::vector<std::string> newFile;
            newFile.push_back(pathFile + ":" + std::to_string(lineNum) 
                              + ": " + line);
            content.push_back(newFile);
        } else {
            content.back().push_back(pathFile + ":" + std::to_string(lineNum) 
                                     + ": " + line);

        }
        
        std::string word;
        std::istringstream ss(line);
        
        while (ss >> word) {
            hash.insert(stripNonAlphaNum(word), lineNum, content.size() - 1);
        }
    }
    
    inputFile.close();
}

/*
 * name:      open or die
 * purpose:   to open the file in the file stream and determine if 
              it can be opened or not. If it cant call abort funciton for error 
              message and to quit program
 * arguments: adress of the type of stream the file to be opened is, 
              infile or outfile, and a string with the name of the file
 * returns:   none
 * effects:   if file can't be opened, program quits
 */
template<typename streamtype>
void Gerp::open_or_die(streamtype &stream, std::string pathAndFile) {
    stream.open(pathAndFile);
    
    if (not stream.is_open()) {
        throw std::runtime_error("Unable to open file: " + pathAndFile);
    }
}

/*
 * name:      caseInSen
 * purpose:   handle case insensitive commands, traverse through all variations 
              and instances of a word and print to output file.
 * arguments: string with word to be found, reference to output file
 * returns:   none
 * effects:   prints every instance and variation of a word to an output file
 */
void Gerp::caseInSen(std::string word, std::ostream &output) {
    std::vector<Hash::versionsInfo> versions = hash.caseInSensitive(word);
    if (versions.front().instances.empty()) {
        output << word << " Not Found." << std::endl;
        return;
    }
    for (unsigned long i = 0; i < versions.size(); i++) {
        for (unsigned long j = 0; j < versions[i].instances.size(); j++) {
            printLine(versions[i].instances[j].fileIndex, 
                      versions[i].instances[j].lineNum, output);
        }
    }
}

/*
 * name:      caseSen
 * purpose:   handle case sensitive commands, traverse through all variations 
              and instances of a word and print to output file.
 * arguments: string with word to be found, reference to output file
 * returns:   none
 * effects:   prints every instance of a word to an output file
 */
void Gerp::caseSen(std::string word, std::ostream &output) {
    std::vector<Hash::wordInfo> instances = hash.caseSensitive(word);
    if (instances.empty()) {
        output << word << " Not Found. Try with @insensitive or @i." 
               << std::endl;
        return;
    }
    for (unsigned long i = 0; i < instances.size(); i++) {
        printLine(instances[i].fileIndex, instances[i].lineNum, output);
    }
}

/*
 * name:      stripNonAlphaNum
 * purpose:   remove non alpha numeric characters from the string the user 
              inputts to search for in the directory
 * arguments: string with word to be stripped
 * returns:   string with stripped word
 * effects:   removes non alpha numeric characters from a string
 */
std::string Gerp::stripNonAlphaNum(std::string input) {
    bool strip = false;
    for (unsigned long i = 0; i < input.size(); i++) {
        if (isalnum(input[i])) {
            strip = true;
        }
    }

    if (not strip) {
        return "";
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

/*
 * name:      printLine
 * purpose:   print pathway, line number, and line contents of word 
 * arguments: int with index of vector that stores file names, int with line 
              number and reference to output
 * returns:   none
 * effects:   prints word information to output file
 */ 
void Gerp::printLine(int fileIndex, int lineNum, std::ostream &output) {
    output << content[fileIndex][lineNum - 1] << std::endl;
}