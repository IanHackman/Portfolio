/*
 *  HuffmanCoder.cpp
 *  Ian Hackman (ihackm01)
 *  10/12
 *
 *  CS 15 proj3 - zap
 *
 *  Function definitions for HuffmanCoder class. Funciton definitions 
 * are ordered by when the are called in each of the public funcitons, 
 * first encoder() and then decoder(). The best way to start the class 
 * is to call encoder() or decoder() to initiate the encoding or decoding 
 * path, as every other function only works to support either of the two 
 * funcitons. 
 *
 */

#include "HuffmanCoder.h"
#include "HuffmanTreeNode.h"
#include "BinaryIO.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

/*
 * name:      encoder
 * purpose:   read in text from a file and call funcitons to encode the text
 * arguments: string with name of file to be read from and string with name 
 of file to write output to
 * returns:   none
 * effects:   the serialized tree and encoded text and written to an outfile
 */
void HuffmanCoder::encoder(std::string input_file, std::string output_file) {
    freqsSize = 0;
    std::ifstream infile;
    std::ofstream outfile;
    open_or_die(infile, input_file);
    open_or_die(outfile, output_file);
    count_freqs(infile);
    infile.close();
    pushQueue();
    HuffmanTreeNode *tree = buildTree();
    infile.open(input_file);
    std::string textCode = encodeText(tree, infile);
    std::string serialized = serialize_tree(tree);
    BinaryIO write;
    write.writeFile(output_file, serialized, textCode);
    std::cout << "Success! Encoded given text using " << textCode.size();
    std::cout << " bits." << std::endl;
    postDelete(tree);
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
void HuffmanCoder::open_or_die(streamtype &stream, std::string file_name) {
    stream.open(file_name);
    if (not stream.is_open()) {
        throw std::runtime_error( "Unable to open file: " + file_name);
    }
}
/*
 * name:      count_freqs
 * purpose:   read in the characters of a file and count the frequency of 
 each character
 * arguments: istream with adress of file to read from
 * returns:   none
 * effects:   stores frequency of each character to array private data variable
 */
void HuffmanCoder::count_freqs(std::istream &text) {
    checkEmpty(text);
    char next;
    while(text.get(next)) {
        int size = encodeArr.size();
        if (size == 0) {
            charFreq newChar;
            newChar.chars = next;
            newChar.freq = 1;
            encodeArr.push_back(newChar);
        } else {
            bool match = false;
            for (int i = 0; i < size; i++) {
                if (next == encodeArr[i].chars) {
                    encodeArr[i].freq++;
                    match = true;
                }
            }
            if (not match) {
                charFreq newChar;
                newChar.chars = next;
                newChar.freq = 1;
                encodeArr.push_back(newChar);
            }
        }
    }
    freqsSize = encodeArr.size();
}

/*
 * name:      checkEmpty
 * purpose:   check if the file is empty
 * arguments: istream with reference to file
 * returns:   none
 * effects:   checks if the file if empty, if so throw runtime error
 */
void HuffmanCoder::checkEmpty(std::istream &file) {
    if (file.peek() == EOF) {
        throw std::runtime_error("Cannot zap an empty file!");
    }
}

/*
 * name:      pushQueue
 * purpose:   order frequencies of charcaters in arr into a priority queue
 * arguments: none
 * returns:   none
 * effects:   stores frequencies into priority queue private member variable
 */
void HuffmanCoder::pushQueue() {
    for (int i = 0; i < freqsSize; i++) {
        HuffmanTreeNode *newNode = new HuffmanTreeNode(encodeArr[i].chars, 
                                                        encodeArr[i].freq);
        pq.push(newNode);                          //to prevent over 80 columns
    }
}

/*
 * name:      buildTree
 * purpose:   construct Huffman tree by combining lowest frequenct nodes
 * arguments: none
 * returns:   pointer to root of tree
 * effects:   constructs Huffman tree and return pointer to root of tree
 */
HuffmanTreeNode *HuffmanCoder::buildTree() {
    HuffmanTreeNode *node1 = pq.top();
    if (freqsSize == 1) {
        return node1;
    } else {
        pq.pop();
        freqsSize--;
        HuffmanTreeNode *node2 = pq.top();
        pq.pop();
        freqsSize--;
        HuffmanTreeNode *combo = new HuffmanTreeNode('\0', node1->get_freq() + 
                                                        node2->get_freq());
                                                    //to prevent over 80 columns
        combo->set_left(node2);
        combo->set_right(node1);
        pq.push(combo);
        freqsSize++;
        return(buildTree());
    }
    return nullptr;
}

/*
 * name:      encodingText
 * purpose:   encodes Huffman tree into a string
 * arguments: pointer to root of tree
 * returns:   string with encoded text
 * effects:   encodes Huffman tree and returns a string
 */
std::string HuffmanCoder::encodeText(HuffmanTreeNode *root, std::istream &text){
    std::stringstream encoded;
    char next;
    while(text.get(next)) {
        encodeHelper(next, root);
    }
    std::string word = encodedWord;
    return word;
}

/*
 * name:      encodeHelper
 * purpose:   traverse HuffmanTree and add directions fo traversal, 
 0s and 1s, to string
 * arguments: character of node to find, string of encoded word, 
 pointer to current node
 * returns:   string with encoded directions for single leaf
 * effects:   adds characters to string with encoded text
 */
void HuffmanCoder::encodeHelper(char target, HuffmanTreeNode *curr) {
    if (curr->isLeaf()) {
        if (curr->get_val() == target) {
            encodedWord.append(encodedWordHelper);
        }
        return;
    } else {
        encodedWordHelper.push_back('0');
        encodeHelper(target, curr->get_left());
        encodedWordHelper.pop_back();
        encodedWordHelper.push_back('1');
        encodeHelper(target, curr->get_right());
        encodedWordHelper.pop_back();
    }
}

/*
 * name:      serialize_tree
 * purpose:   serialize Huffman tree into string with the sequence of nodes
 * arguments: pointer to root of tree
 * returns:   string with serialized tree
 * effects:   writes node sequence into string and returns it
 */
std::string HuffmanCoder::serialize_tree(HuffmanTreeNode *root) {
    if (root == nullptr) {
        return "";
    }
    std::string tree;
    serializeTreeHelp(root, tree);
    return tree;
}

/*
 * name:      serialize_helper_tree
 * purpose:   traverse binary tree and add node types and value to string
 * arguments: pointer to current node and adress of string
 * returns:   none
 * effects:   creates string with serialized tree
 */
void HuffmanCoder::serializeTreeHelp(HuffmanTreeNode *curr, std::string &tree) {
    if (curr == nullptr) {
        return;
    }
    if (not curr->isLeaf()) {
        tree.push_back('I');
    } else {
        tree.push_back('L');
        tree.push_back(curr->get_val());
    }
    serializeTreeHelp(curr->get_left(), tree);
    serializeTreeHelp(curr->get_right(), tree);
}

/*
 * name:      postDelete
 * purpose:   recycle heap-allocated data from nodes used to create tree
 * arguments: pointer to current node
 * returns:   none
 * effects:   deletes Huffman tree, frees heap allocated memory
 */
void HuffmanCoder::postDelete(HuffmanTreeNode *curr) {
    if (curr == nullptr) {
        return;
    }
    postDelete(curr->get_left());
    postDelete(curr->get_right());
    delete curr;
}

/*
 * name:      decoder
 * purpose:   open file with encoded tree, call fucntions to decode the tree 
 and print the result to an output file
 * arguments: stirng with name of input file, string with name of output file
 * returns:   none
 * effects:   opens file, call fucntions and passes returned strings to output
 */
void HuffmanCoder::decoder(std::string input_file, std::string output_file) {
    BinaryIO decode;
    decodeArr = decode.readFile(input_file);
    HuffmanTreeNode *tree = deserialize_tree(decodeArr[0]);
    createdTree = tree;
    std::string decodedText = decString(tree, decodeArr[1]);
    if (decodedText == "") {
        throw std::runtime_error("Encoding did not match Huffman tree.");
    }
    std::ofstream output;
    open_or_die(output, output_file);
    output << decodedText;
    postDelete(tree);
}

/*
 * name:      deserialize_tree
 * purpose:   open file with encoded tree, call fucntions to decode the tree 
 and print the result to an output file
 * arguments: stirng with name of input file, string with name of output file
 * returns:   none
 * effects:   opens file, call fucntions and passes returned strings to output
 */
HuffmanTreeNode *HuffmanCoder::deserialize_tree(std::string serial_tree) {
    std::istringstream serial(serial_tree);
    return(dTreeH(serial));
}

/*
 * name:      deserializeTreeHelp
 * purpose:   construct Huffman tree from string stream of serialized tree
 * arguments: stringstream with serialized tree
 * returns:   pointer to root of constructed tree
 * effects:   recreates Huffman tree
 */
HuffmanTreeNode *HuffmanCoder::dTreeH(std::istream &enStream) {
    char node = enStream.get();
    if (node == 'L') {
        return new HuffmanTreeNode(enStream.get(), 1);
    } else if (node == 'I') {
        return new HuffmanTreeNode('\0', 1, dTreeH(enStream), dTreeH(enStream));
    } else {
        return nullptr;
    }
}

/*
 * name:      decString
 * purpose:   process encoded string into string with text and 
 return result in string
 * arguments: pointer to root of tree, string with encoded bits
 * returns:   none
 * effects:   creates string with serialized tree
 */
std::string HuffmanCoder::decString(HuffmanTreeNode *curr, std::string encoded){
    int size = encoded.size();
    int index = 0;
    while (index != size + 1) {
        if (curr->isLeaf()) {
            decodeHelper.push_back(curr->get_val());
            if (index == size) {
                std::string decoded = decodeHelper;
                return decoded;
            } else {
                curr = createdTree;
            }
        } else {
            if (encoded[index] == '0') {
                curr = curr->get_left();
            } else if (encoded[index] == '1') {
                curr = curr->get_right();
            }
            index++;
        }
    }
    return "";
}
