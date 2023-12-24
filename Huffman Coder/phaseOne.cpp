#include "phaseOne.h"
#include "HuffmanTreeNode.h"

#include <iostream>
#include <string>
#include <istream>
#include <vector>
#include <sstream>

/*
 * name:      count_freqs
 * purpose:   read in the characters of a file and count the frequency of eahc character
 * arguments: istream with adress of file to read from
 * returns:   none
 * effects:   prints charcater frequency of file to std::cout
 */
void count_freqs(std::istream &text) {
    std::vector<charFreq> arr;
    char next;
    while(text.get(next)) {
        int size = arr.size();
        if (size == 0) {
            charFreq newChar;
            newChar.chars = next;
            newChar.freq = 1;
            arr.push_back(newChar);
        } else {
            bool match = false;
            for (int i = 0; i < size; i++) {
                if (next == arr[i].chars) {
                    arr[i].freq++;
                    match = true;
                }
            }
            if (not match) {
                charFreq newChar;
                newChar.chars = next;
                newChar.freq = 1;
                arr.push_back(newChar);
            }
        }
    }
    int size2 = arr.size();
    for (int i = 0; i < size2; i++) {
        std::cout << arr[i].chars << ": " << arr[i].freq << std::endl;
    }
}

/*
 * name:      serialize_tree
 * purpose:   call recursive funciton to serialize binary tree
 * arguments: pointer to root of tree
 * returns:   string with serialized binary tree
 * effects:   creates string with serialized binary tree and returns it
 */
std::string serialize_tree(HuffmanTreeNode *root) {
    if (root == nullptr) {
        return ("");
    }
    std::string tree = serialize_tree_helper(root, tree);
    return(tree);
}

/*
 * name:      serialize_helper_tree
 * purpose:   traverse binary tree and add node types and value to string
 * arguments: pointer to current node and adress of string
 * returns:   none
 * effects:   creates string with serialized tree
 */
void serialize_tree_helper(HuffmanTreeNode *curr, std::string &tree) {
    if (curr == nullptr) {
        return;
    }
    if (not curr->isLeaf()) {
        tree.push_back('I');
    } else {
        tree.push_back('L');
        tree.push_back(curr->get_val());
    }
    serialize_tree_helper(curr->get_left(), tree);
    serialize_tree_helper(curr->get_right(), tree);
}

/*
 * name:      deserialize_tree
 * purpose:   call recursive function to deserialize binary tree
 * arguments: string with serialized tree
 * returns:   pointer to root of constructed tree
 * effects:   calls funciton to create Huffman tree
 */
HuffmanTreeNode *deserialize_tree(std::string serial_tree) {
    int size = serial_tree.size();
    int index = 0;
    return (deserialize_tree_helper(serial_tree, index, size));
}

/*
 * name:      deserialize_tree_helper
 * purpose:   construct Huffman tree from serialized tree in string
 * arguments: string with serialized tree, adress of index of string, size of string
 * returns:   pointer to root of constructed tree
 * effects:   creates Huffman tree
 */
HuffmanTreeNode *deserialize_tree_helper(std::string serial_tree, int &index, int size) {
    HuffmanTreeNode *root = nullptr;
    if (index == size) {
        return root;
    }
    if (serial_tree[index] == 'I') {
        root = new HuffmanTreeNode('\0', 1);
        index++;
        root->set_left(deserialize_tree_helper(serial_tree, index, size));
        index++;
        root->set_right(deserialize_tree_helper(serial_tree, index, size));
        return root;
    } else if (serial_tree[index] == 'L') {
        index++;
        root = deserialize_tree_helper(serial_tree, index, size);
        return root;
    } else {
        root = new HuffmanTreeNode(serial_tree[index], 1);
        return root;
    }
}
