/*
 *  main.cpp
 *  Ian Hackman (ihackm01)
 *  10/12
 *
 *  CS 15 proj3 - zap
 *
 * Reads in the file names from the commands line, ensures that the correct 
 * amount of arguments is provided, and sorts the HuffmanCoder funciton calls 
 * based on the second command input, zap or unzap, calling either encode or 
 * decode. 
 * 
 */

#include "HuffmanTreeNode.h"
#include "BinaryIO.h"
#include "HuffmanCoder.h"

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    if (argc < 4 or argc > 4) {
        std::cerr << "Usage: ./zap [zap | unzap] inputFile outputFile";
        std::cerr << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string zapType = argv[1];
    HuffmanCoder start;
    if (zapType == "zap") {
        start.encoder(argv[2], argv[3]);
    } else if (zapType == "unzap") {
        start.decoder(argv[2], argv[3]);   
    } else {
        std::cerr << "Usage: ./zap [zap | unzap] inputFile outputFile";
        std::cerr << std::endl;
        exit(EXIT_FAILURE);
    }
}