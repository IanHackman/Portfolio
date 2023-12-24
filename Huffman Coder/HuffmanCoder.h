/*
 *  HuffmanCoder.h
 *  Ian Hackman (ihackm01)
 *  10/12
 *
 *  CS 15 proj3 - zap
 *
 * Interface and funciton declarations for HuffmanCoder class. The class 
 * reads in the file, constructs a Huffman tree using the character 
 * frequencies, serializes the tree and then encodes it. The second path 
 * is decode, where the program deserializes the serialized tree and 
 * decodes the text. HuffmanCoder uses classes HuffmanTreeNode, to 
 * construct and connect the Nodes in the huffman tree, and BinaryIO, to 
 * write the serialized tree and encoded text to a file and take the file 
 * and read it back into strings. The class is used to support the 
 * functions encoder() and decoder().
 *
 */

#ifndef __HUFFMANCODER_H__
#define __HUFFMANCODER_H__

#include "HuffmanTreeNode.h"

#include <iostream>
#include <string>
#include <queue>
#include <vector>

class HuffmanCoder {
public:
    void encoder(std::string input_file, std::string output_file);
    void decoder(std::string input_file, std::string output_file);
private:
    template<typename streamtype>
    void open_or_die(streamtype &stream, std::string file_name);

    void count_freqs(std::istream &text);
    void checkEmpty(std::istream &file);
    void pushQueue();
    HuffmanTreeNode *buildTree();
    std::string serialize_tree(HuffmanTreeNode *root);
    void serializeTreeHelp(HuffmanTreeNode *curr, std::string &tree);
    void encodeHelper(char target, HuffmanTreeNode *curr);
    std::string encodeText(HuffmanTreeNode *root, std::istream &text);
    void message();
    void postDelete(HuffmanTreeNode *curr);
    HuffmanTreeNode *deserialize_tree(std::string serial_tree);
    HuffmanTreeNode *dTreeH(std::istream &enStream);
    std::string decString(HuffmanTreeNode *curr, std::string encoded);
    void decodeStringHelp(HuffmanTreeNode *curr, std::string encoded, 
                                                int &index, int size);
                                                //to prevent over 80 columns
    struct charFreq {
        int freq;
        char chars;
    };

    std::vector<charFreq> encodeArr;
    int freqsSize;
    std::priority_queue<HuffmanTreeNode*, std::vector<HuffmanTreeNode*>,
                   NodeComparator> pq;
    std::string encodedWordHelper;
    std::string encodedWord;
    std::vector<std::string> decodeArr;
    std::string decodeHelper;
    HuffmanTreeNode *createdTree;
};

#endif
