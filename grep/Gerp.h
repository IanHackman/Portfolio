//
// Title:           Gerp.h
//
// Authors:         Miles Mamigonian, Ian Hackman
//
// Created:         2022.12.05
//
// Last Modified:   2022.12.11
//
// Purpose:         Interface for the Gerp class:
//                  Gives client access declarations of the public and private
//                  member functions and variables in the Gerp class without
//                  giving them access to the inner workings of Gerp. In fact,
//                  the client cannot explicitly call any of Gerp's member
//                  functions. The only public functions are the constructor
//                  and destructor, which are both called automatically when
//                  the client creates a new Gerp object.
//

#ifndef RUN_GERP_H
#define RUN_GERP_H

#include "DirNode.h"
#include "FSTree.h"
#include "gerpHash.h"

#include <iostream>
#include <string>
#include <fstream>

class Gerp {

public: 
    Gerp(int argc, char *argv[]);
    ~Gerp();

private:
    void abort(std::string error_message);
    template<typename streamtype>
    void open_or_die(streamtype &stream, std::string file_name);
    void buildHash(std::string directory);
    void addTreeToHash(DirNode *node, std::string path);
    void addFileToHash(std::string pathway, std::string pathAndFile);

    void commands(std::string outputFile);
    void caseInSen(std::string word, std::ostream &output);
    void caseSen(std::string word, std::ostream &output);
    std::string stripNonAlphaNum(std::string input);
    void printLine(int fileIndex, int lineNum, std::ostream &output);

    Hash hash;
    std::vector<std::vector<std::string>> content;
};

#endif