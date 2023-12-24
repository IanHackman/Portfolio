/*
 *  CharArrayList.cpp
 *  Ian Hackman
 *  9/21
 *
 *  COMP 15 HW 1 Cheshire ConCATenation
 *
 *  Function definitions for the CharArrayList class
 *
 */

#include "CharArrayList.h"

#include <sstream>

using namespace std;

/*
 * name:      CharArrayList Empty Constructor
 * purpose:   initialize an empty CharArrayList
 * arguments: none
 * returns:   none
 * effects:   numItems to 0 (also updates capacity and data array)
 */
CharArrayList::CharArrayList() {
    chars = nullptr; //initialize chars with nullpointer
    numChars = 0;
    capacity = 0;
}

/*
 * name:      CharArrayList Single Constructor
 * purpose:   initialize an CharArrayList of size 1
 * arguments: a character to add to array
 * returns:   none
 * effects:   numItems to 1 (also updates capacity and data array)
 */
CharArrayList::CharArrayList(char c) {
    chars = new char[1]; //create array of size 1
    capacity = 1;
    chars[0] = c;
    numChars = 1;
}

/*
 * name:      CharArrayList Array Constructor
 * purpose:   initialize an CharArrayList of inputted size
 * arguments: character array to add to CharArrayList and the size of the array
 * returns:   none
 * effects:   numItems to to size (also updates capacity and data array)
 */
CharArrayList::CharArrayList(char arr[], int size) {
    chars = new char[size * 2]; //create array on heap
    capacity = size * 2;
    for (int i = 0; i < size; i++) { //fill parameter array into chars array
        chars[i] = arr[i];
    }
    numChars = size;
}

/*
 * name:      CharArrayList Array Copy Constructor
 * purpose:   to make a shallow copy of the given instance
 * arguments: reference to instance to be copied
 * returns:   none
 * effects:   inputted instance copied to new instance
 */
CharArrayList::CharArrayList(const CharArrayList &other) {
    chars = new char[other.capacity]; //create new array whose size is capacity of other
    *chars = *other.chars; //copy contents of other into chars
    numChars = other.numChars;
    capacity = other.capacity;
}

/*
 * name:      CharArrayList Array Destructor
 * purpose:   free memory associated with CharArrayList
 * arguments: none
 * returns:   none
 * effects:   frees memory allocated by CharArraylist instances
 */
CharArrayList::~CharArrayList() {
    delete [] chars; //delete memory associated with chars on heap
}

/*
 * name:      expand
 * purpose:   increase the capacity of the ArrayList
 * arguments: none
 * returns:   none
 * effects:   creates new, larger array on heap, copies over elements,
 *            and recycles the old array
 */
void CharArrayList::expand() {
    char *new_chars = new char[capacity * 2 + 2]; //create new array with extra size
    for (int i = 0; i < numChars; i++) {
        new_chars[i] = chars[i]; //copy chars into new array
    }
    delete [] chars;
    chars = new_chars;
    capacity = capacity * 2 + 2;
}

/*
 * name:      CharArrayList assignment Operator
 * purpose:   recycles storage of left instance and make deep copy of 
              other to the left instance
 * arguments: none
 * returns:   instance on left
 * effects:   other copied to CharArrayList instance
 */
CharArrayList &CharArrayList::operator=(const CharArrayList &other) {
    if (this == &other) { //if already same, return
        return *this;
    }
    for (int i = 0; i < numChars + 1; i++) { //copy each element of other into chars
        chars[i] = other.chars[i]; 
    }
    numChars = other.numChars;
    capacity = other.capacity;
    return *this; 
}

/*
 * name:      isEmpty
 * purpose:   determines if the CharArrayList is empty or not
 * arguments: none
 * returns:   true if CharArrayList contains no elements, false otherwise
 * effects:   none
 */
bool CharArrayList::isEmpty() const {
    return (numChars == 0);
}

/*
 * name:      clear
 * purpose:   turn the CharArrayList into an empty list
 * arguments: none
 * returns:   none
 * effects:   emptys array
 */
void CharArrayList::clear() {
    numChars = 0;
}

/*
 * name:      size
 * purpose:   determine the number of items in the CharArrayList
 * arguments: none
 * returns:   number of elements currently stored in the CharArrayList
 * effects:   none
 */
int CharArrayList::size() const {
    return numChars;
}

/*
 * name:      first
 * purpose:   return the first char in the CharArrayList
 * arguments: none
 * returns:   first char in CharArrayList
 * effects:   none
 */
char CharArrayList::first() const {
    if (isEmpty()) {
        throw runtime_error("cannot get first of empty ArrayList");
    }
    return chars[0];
}

/*
 * name:      last
 * purpose:   return the last char in the CharArrayList
 * arguments: none
 * returns:   last char in CharArrayList
 * effects:   none
 */
char CharArrayList::last() const {
    if (isEmpty()) {
        throw runtime_error("cannot get last of empty ArrayList");
    }
    return chars[numChars - 1];
}

/*
 * name:      elementAt
 * purpose:   return the char at the specified index in CharArrayList
 * arguments: none
 * returns:   char from index in CharArrayList
 * effects:   none
 */
char CharArrayList::elementAt(int index) const {
    if (index < 0 || index >= numChars) {
        throw range_error(errorMessage2(index));
    }
    return chars[index];
}

/*
 * name:      toString
 * purpose:   turns the array into a string, and returns it
 * arguments: none
 * returns:   a string representation of the array
 * effects:   none
 */
std::string CharArrayList::toString() const {
    std::stringstream ss;
    ss << "[CharArrayList of size " << numChars << " <<";
    if (isEmpty()) {
        ss << "";
    }
    else {
        for (int i = 0; i < numChars; i++) { //import each char in array into string
        ss << chars[i];
        }
    }
    ss << ">>]";
    return ss.str();
}

/*
 * name:      toReverseString
 * purpose:   turns the array into a string from end of array to start, 
              and returns it
 * arguments: none
 * returns:   a string representation of the array in reverse order
 * effects:   none
 */
std::string CharArrayList::toReverseString() const {
    std::stringstream ss;
    ss << "[CharArrayList of size " << numChars << " <<";
    if (isEmpty()) {
        ss << "";
    }
    else {
        for (int i = numChars - 1; i >= 0; i--) { //import each char in array into 
        ss << chars[i];                           //string starting from end of array 
        }
    }
    ss << ">>]";
    return ss.str();
}

/*
 * name:      pushAtBack
 * purpose:   push the provided integer into the back of the CharArrayList
 * arguments: an integer to add to the back of the list
 * returns:   none
 * effects:   increases num elements of CharArrayList by 1,
 *            adds element to list
 */
void CharArrayList::pushAtBack(char c) {
    if (numChars == capacity) {
        expand();
    }
    chars[numChars] = c;
    numChars++; //update number of items in array
}

/*
 * name:      pushAtFront
 * purpose:   push the provided integer into the front of the CharArrayList and 
              shift the elements back 
 * arguments: an integer to add to the front of the list
 * returns:   none
 * effects:   increases num elements of CharArrayList by 1,
 *            adds element to list
 */
void CharArrayList::pushAtFront(char c) {
    insertAt(c, 0); //call to insert with index 0
}

/*
 * name:      insertAt
 * purpose:   push the provided integer into the of the CharArrayList 
              at specified index
 * arguments: an integer to add to the the list
 * returns:   none
 * effects:   increases num elements of CharArrayList by 1,
 *            adds element to list
 */
void CharArrayList::insertAt(char c, int index) {
    if (index < 0 || index > numChars) {
        throw range_error(errorMessage1(index));
    }
    if (numChars == capacity) {
        expand(); //if amount of chars in array is same as capacity, reserve more space
    }
    char new_char = c;
    char old_char;
    for (int i = index; i < numChars + 1; i++) { //for each element after insertion, 
        old_char = chars[i];                     //shift 1 space backwards
        chars[i] = new_char;
        new_char = old_char;
    }
    numChars++;
}

/*
 * name:      insertInOrder
 * purpose:   push the provided integer into the of the CharArrayList 
              in alphabetical order
 * arguments: an integer to add to the the list
 * returns:   none
 * effects:   increases num elements of CharArrayList by 1,
 *            adds element to list
 */
void CharArrayList::insertInOrder(char c) {
    for (int i = 0; i < numChars; i++) {
        if (i == 0) {
            if (c < chars[i]) {
                insertAt(c, 0);
                return;
            }
        }
        else {
            if (c > chars[i] && c < chars[i + 1]) {  //loop through array find where char
                insertAt(c, i + 1); //call insertAt  //parameter would fit in alphabetical order
                return;
            }
        }
    }

}

/*
 * name:      popFromFront
 * purpose:   remove the first item from the CharArrayList and shift 
              the other elements down
 * arguments: none
 * returns:   none
 * effects:   decreases num items of CharArrayList by 1
 *            removes the first item from the list
 */
void CharArrayList::popFromFront() {
    if (isEmpty()) {
        throw runtime_error("cannot pop from empty ArrayList");
    }
    for (int i = 0; i < numChars - 1; i++) { //shift every element to left by 1 space
        chars[i] = chars[i + 1];
    }
    numChars--; //update number of items in array
}

/*
 * name:      popFromBack
 * purpose:   remove the last item from the CharArrayList
 * arguments: none
 * returns:   none
 * effects:   decreases num items of CharArrayList by 1
 *            removes the last item from the list
 */
void CharArrayList::popFromBack() {
    if (isEmpty()) {
        throw runtime_error("cannot pop from empty ArrayList");
    }
    numChars--; //decrement size by 1, last element goes under the hood
}

/*
 * name:      removeAt
 * purpose:   remove the item from the CharArrayList at specified index 
              and shift other elements down
 * arguments: index indicating where to remove item
 * returns:   none
 * effects:   decreases num items of CharArrayList by 1
 *            removes the item from the list
 */
void CharArrayList::removeAt(int index) {
    if (index < 0 || index >= numChars) {
        throw range_error(errorMessage2(index));
    }
    for (int i = index; i < numChars - 1; i++) {
        chars[i] = chars[i + 1]; //shift every element to left by 1 space
    }
    numChars--; 
}

/*
 * name:      replaceAt
 * purpose:   replace an item the item from the CharArrayList at specified 
              index with new item
 * arguments: char to replace and index indicating where to replace item
 * returns:   none
 * effects:   replaces an item from the list with another
 */
void CharArrayList::replaceAt(char c, int index) {
    if (index < 0 || index >= numChars) {
        throw range_error(errorMessage2(index));
    }
    chars[index] = c;
}

/*
 * name:      concatenate
 * purpose:   combine CharArrayList with another instance
 * arguments: instance to copy
 * effects:   combines two instances into one
 */
void CharArrayList::concatenate(CharArrayList *other) { 
    int size1 = size(); //aquire sizes of the 2 arrays
    int size2 = other->size();
    char *comb_arr = new char[size1 + size2]; //make new array to put chars from other 2 into
    for (int i = 0; i < size1; i++) {
        comb_arr[i] = chars[i];
    }
    for (int j = 0; j < size2; j++) {
        comb_arr[size1 + j] = other->chars[j];
    }
    delete [] chars;
    chars = comb_arr;
    numChars = size1 + size2; //update number of items
    capacity = numChars;
}

/*
 * name:      shrink
 * purpose:   decrease capacity of array to the size of the 
              amount of items in the list
 * arguments: none
 * returns:   none
 * effects:   decreases capacity to numItems
 */
void CharArrayList::shrink() {
    char *new_chars = new char[numChars]; //create new array 
    for (int i = 0; i < numChars; i++) {
        new_chars[i] = chars[i];
    }
    delete [] chars;
    chars = new_chars;
    capacity = numChars; //update capacity to be same as number of items in array
}

/*
 * name:      errorMessage1
 * purpose:   create string with error message
 * arguments: number to place into string
 * returns:   a string representation of the error message
 * effects:   none
 */
std::string CharArrayList::errorMessage1(int index) const {
    std::stringstream ss;
    ss << "index (" << index << ") not in range [0.." << numChars << "]";
    return ss.str();
}

/*
 * name:      errorMessage2
 * purpose:   create string with error message
 * arguments: number to place into string
 * returns:   a string representation of the error message
 * effects:   none
 */
std::string CharArrayList::errorMessage2(int index) const {
    std::stringstream ss;
    ss << "index (" << index << ") not in range [0.." << numChars << ")";
    return ss.str();
}