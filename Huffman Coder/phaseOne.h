#ifndef __PHASEONE_H__
#define __PHASEONE_H__

#include "HuffmanTreeNode.h"

#include <iostream>
#include <string>
#include <istream>

void count_freqs(std::istream &text);

std::string serialize_tree(HuffmanTreeNode *root);

HuffmanTreeNode *deserialize_tree(std::string serial_tree);

void serialize_tree_helper(HuffmanTreeNode *curr, std::string &tree);

HuffmanTreeNode *deserialize_tree_helper(std::string serial_tree, int &index, int size);

struct charFreq {
    int freq;
    char chars;
};

#endif
