/*
 *  CharLinkedList.cpp
 *  Ian Hackman (ihackm01)
 *  9/27
 *
 *  CS 15 HW 2 Cheshire ConCATenation - Time to get linked up in Fur
 *
 *  Function definitions for the CharArrayList class. This file contains 
 *  the contents of the functions using the private member variables in 
 *  CharLinkedList.h. Fucntions that add nodes to the list allocate new nodes 
 * in which the client can store a character. Functions that remove nodes 
 * remove characters from the list. In addition, the client may use different 
 * funcitons to determine aspects of the list such as size, which element is 
 * at a specific index, etc. 
 *
 */

#include "CharLinkedList.h"
#include <sstream>
#include <iostream>

using namespace std;

/*
 * name:      CharLinkedList Empty Constructor
 * purpose:   initialize an empty CharLinkedList
 * arguments: none
 * returns:   none
 * effects:   front points to nullptr
 */
CharLinkedList::CharLinkedList() {
    front = nullptr;
}

/*
 * name:      CharLinkedList Single Constructor
 * purpose:   initialize an CharLinkedList with 1 node
 * arguments: a character to add to new node
 * returns:   none
 * effects:   creates new node
 */
CharLinkedList::CharLinkedList(char c) {
    Node *newNode = new Node;
    front = newNode;
    back = newNode; //create new node, point front and back to ir
    newNode->c = c;
    newNode->next = nullptr;
    newNode->prev = nullptr;
}

/*
 * name:      CharLinkedList Linked List Constructor
 * purpose:   initialize an CharLinkedList of inputted size
 * arguments: character Linked List to add to CharLinkedList and the size of the Linked List
 * returns:   none
 * effects:   creates linked list size of size parameter
 */
CharLinkedList::CharLinkedList(char arr[], int size) {
    for (int i = 0; i < size; i++) {
        if (i == 0) {
            Node *newNode = new Node;
            front = newNode;
            back = newNode; //if list is empty
            newNode->c = arr[i];
            newNode->next = nullptr;
            newNode->prev = nullptr;
        }
        else {
            Node *curr = back;
            Node *newNode = new Node;
            back = newNode;
            newNode->c = arr[i];
            newNode->next = nullptr;
            newNode->prev = curr;
            curr->next = newNode; //point new node to previous
        }
    }
}

/*
 * name:      CharLinkedList Linked List Copy Constructor
 * purpose:   to make a deep copy of the given instance
 * arguments: reference to instance to be copied
 * returns:   none
 * effects:   inputted instance copied to new instance
 */
CharLinkedList::CharLinkedList(const CharLinkedList &other) {
    if (other.front == nullptr) {
        front = nullptr;
    }
    Node *curr = other.front;
    front = nullptr;
    back = nullptr;
    while (curr != nullptr) {
        char other_char = curr->c;
        pushAtBack(other_char);
        curr = curr->next;
    }
}

/*
 * name:      CharLinkedList Linked List Destructor
 * purpose:   free memory associated with CharLinkedList
 * arguments: none
 * returns:   none
 * effects:   frees memory allocated by CharLinkedList instances
 */
CharLinkedList::~CharLinkedList() {
    Node *p = front;
    deleteRecursivly(p); //call recursive function
}

/*
 * name:      deleteRecursivly
 * purpose:   recurrsive function to free memory associated with CharLinkedList
 * arguments: none
 * returns:   none
 * effects:   frees memory allocated by CharLinkedList instances
 */
void CharLinkedList::deleteRecursivly(Node *p) {
    if (p == nullptr) {
        return; //base case if reach end of list
    }
    else {
        deleteRecursivly(p->next);
        delete p; //recursive case, return next node, delete curent
    }
}

/*
 * name:      CharLinkedList assignment Operator
 * purpose:   recycles storage of left instance and make deep copy of 
              other to the left instance
 * arguments: none
 * returns:   instance on left
 * effects:   other copied to CharLinkedList instance
 */
CharLinkedList &CharLinkedList::operator=(const CharLinkedList &other) {
    if (this == &other) {
        return *this; //if already same, return
    }
    else {
        Node *tmp = front;
        while(tmp != nullptr) {
            front = front->next; //delete node of this
            delete tmp;
            tmp = front;
        }
        tmp = other.front;
        while(tmp != nullptr) { //copy each element of other into chars
            pushAtBack(tmp->c);
            tmp = tmp->next;
        }
        return *this;
    }
}

/*
 * name:      isEmpty
 * purpose:   determines if the CharLinkedList is empty or not
 * arguments: none
 * returns:   true if front is a nullptr, false otherwise
 * effects:   none
 */
bool CharLinkedList::isEmpty() const {
    return (front == nullptr);
}

/*
 * name:      clear
 * purpose:   delete nodes of linked list
 * arguments: none
 * returns:   none
 * effects:   emptys Linked List
 */
void CharLinkedList::clear() {
    Node *p = front;
    deleteRecursivly(p); //call recursive function
    front = nullptr;
}

/*
 * name:      size
 * purpose:   determine the number of nodes in the CharLinkedList
 * arguments: none
 * returns:   number of nodes in the CharLinkedList
 * effects:   none
 */
int CharLinkedList::size() const {
    int count = 0;
    Node *curr = front;
    while (curr != nullptr) {
        curr = curr->next; //traverse ist, increment count
        count++;
    }
    return (count);
}

/*
 * name:      first
 * purpose:   return the first char of first node in the CharLinkedList
 * arguments: none
 * returns:   first char in CharLinkedList
 * effects:   none
 */
char CharLinkedList::first() const {
    if (front == nullptr) {
        throw runtime_error("cannot get first of empty LinkedList");
    }
    Node *curr = front;
    return(curr->c);
}

/*
 * name:      last
 * purpose:   return the last char of last node in the CharLinkedList
 * arguments: none
 * returns:   last char in CharLinkedList
 * effects:   none
 */
char CharLinkedList::last() const {
    if (front == nullptr) {
        throw runtime_error("cannot get last of empty LinkedList");
    }
    Node *curr = back;
    return (curr->c);
}

/*
 * name:      elementAt
 * purpose:   return the char at the specified index in CharLinkedList
 * arguments: none
 * returns:   char from index in CharLinkedList
 * effects:   none
 */
char CharLinkedList::elementAt(int index) const {
    if (index < 0 or index >= size()) {
        throw range_error(errorMessage2(index));
    }
    int count = 0;
    Node *curr = front;
    return(elementAtRecursivly(curr, index, count)); //call recursive fucntion
}

/*
 * name:      elementAtRecursivly
 * purpose:   recursive function return the char at the specified 
              index in CharLinkedList
 * arguments: none
 * returns:   char from index in CharLinkedList
 * effects:   none
 */
char CharLinkedList::elementAtRecursivly(Node *curr, int index, int count) const{
    if (count == index) {
        return (curr->c); //base case when reach index
    }
    else {
        return elementAtRecursivly(curr->next, index, count + 1);
    }                     //recursive case, return next node, increment count
}

/*
 * name:      toString
 * purpose:   turns the Linked List into a string, and returns it
 * arguments: none
 * returns:   a string representation of the Linked List
 * effects:   none
 */
std::string CharLinkedList::toString() const {
    std::stringstream ss;
    ss << "[CharLinkedList of size " << size() << " <<";
    if (isEmpty()) {
        ss << "";
    }
    else {
        Node *curr2 = front;
        while (curr2 != nullptr) {
            ss << curr2->c;  //traverse list add chars to string
            curr2 = curr2->next;
        }
    }
    ss << ">>]";
    return ss.str();
}

/*
 * name:      toReverseString
 * purpose:   turns the Linked List into a string from end of 
 *            Linked List to start, and returns it
 * arguments: none
 * returns:   a string representation of the Linked List in reverse order
 * effects:   none
 */
std::string CharLinkedList::toReverseString() const {
    std::stringstream ss;
    ss << "[CharLinkedList of size " << size() << " <<";
    if (isEmpty()) {
        ss << "";
    }
    else {
        Node *curr = back;
        while (curr != nullptr) {
            ss << curr->c; //travers elist backwards add chars to list
            curr = curr->prev;
        }
    }
    ss << ">>]";
    return ss.str();
}

/*
 * name:      pushAtBack
 * purpose:   push the provided integer into the back of the CharLinkedList
 * arguments: an integer to add to the back of the list
 * returns:   none
 * effects:   adds new noe to back of linked list
 *            
 */
void CharLinkedList::pushAtBack(char c) {
    if (front == nullptr) {
        Node *newNode = new Node;
        front = newNode;
        back = newNode;
        newNode->c = c; //if list empty
        newNode->next = nullptr;
        newNode->prev = nullptr;
    }
    else {
        Node *curr = back;
        Node *newNode = new Node;
        back = newNode; //if lst not empty, add node point it to previous
        newNode->c = c;
        newNode->next = nullptr;
        newNode->prev = curr;
        curr->next = newNode;
    }
}

/*
 * name:      pushAtFront
 * purpose:   push the provided integer into the front of the CharLinkedList 
 * arguments: an integer to add to the front of the list
 * returns:   none
 * effects:   adds new node to front of linked list
 *            
 */
void CharLinkedList::pushAtFront(char c) {
    if (front == nullptr) {
        Node *newNode = new Node;
        front = newNode;
        back = newNode; //if list empty
        newNode->c = c;
        newNode->next = nullptr;
        newNode->prev = nullptr;
    }
    else {
        Node *curr = front;
        Node *newNode = new Node;
        front = newNode;
        newNode->c = c; //if list not empty, add node to front,
        newNode->next = curr;  //point to next, point front to node
        newNode->prev = nullptr;
        curr->prev = newNode;
    }
}

/*
 * name:      insertAt
 * purpose:   add node to linked list at specified linked list
 * arguments: an integer to add to the the list
 * returns:   none
 * effects:   adds new node to linked list
 */
void CharLinkedList::insertAt(char c, int index) {
    if (index < 0 or index > size()) {
        throw range_error(errorMessage1(index));
    }
    if (index == 0) {
        if (front == nullptr) {
            Node *newNode = new Node;
            front = newNode;
            back = newNode; //if list empty
            newNode->c = c;
            newNode->next = nullptr;
            newNode->prev = nullptr;
        }
        else {
            pushAtFront(c); //if add element at front
        }
    }
    else {
        int count = 0;
        Node *curr = front;
        while (count != index - 1) {
            curr = curr->next;
            count++;  //traverse to find index
        }
        if (curr->next == nullptr) {
            pushAtBack(c); //if add at back
        }
        else {
            Node *newNode = new Node;
            newNode->c = c; //if add at middle
            newNode->next = curr->next;
            newNode->prev = curr;
            curr->next = newNode;
            newNode->next->prev = newNode;
        }
    }
}

/*
 * name:      insertInOrder
 * purpose:   adds new node to linked list in alphabetical order of c element
 * arguments: an integer to add to the the list
 * returns:   none
 * effects:   add snew node to linked list
 */
void CharLinkedList::insertInOrder(char c) {
    Node *curr = front;
    int count = 0;
    if (front == nullptr) {
        Node *newNode = new Node;
        front = newNode;
        newNode->c = c; //if list empty
        newNode->next = nullptr;
        newNode->prev = nullptr;
    }
    if (size() == 1) {
        if (c <= curr->c) {
            pushAtFront(c); //if add at front
            return;
        } else {
            pushAtBack(c);
        }
    }
    while (curr != nullptr) {
        if (count == 0) {
            if (c <= curr->c) {
                pushAtFront(c); //if add at front
                return;
            }
        } else {
            if (curr->next == nullptr) {
                pushAtBack(c); //if add at back
                return;
            } else {
                if (c >= curr->c and c <= curr->next->c) {
                    insertAt(c, count + 1); //if add in middle
                    return;
                }
            }

        }
        curr = curr->next;
        count++;
    }
}

/*
 * name:      popFromFront
 * purpose:   remove first node from linked list
 * arguments: none
 * returns:   none
 * effects:   removes node from list
 */
void CharLinkedList::popFromFront() {
    if (isEmpty()) {
        throw runtime_error("cannot pop from empty LinkedList");
    }
    Node *curr = front;
    Node *p;
    p = curr; //remove from front, point front to new first node
    curr = curr->next;
    delete p;
    front = curr;
    curr->prev = nullptr;
}

/*
 * name:      popFromBack
 * purpose:   remove the last node from the CharLinkedList
 * arguments: none
 * returns:   none
 * effects:   deletes last node from linked list
 */
void CharLinkedList::popFromBack() {
    if (isEmpty()) {
        throw runtime_error("cannot pop from empty LinkedList");
    }
    else {
        Node *curr = back; //set pointer to back of list
        Node *p;
        p = curr;
        curr->prev->next = nullptr;
        back = curr->prev;
        delete p;
    }
}

/*
 * name:      removeAt
 * purpose:   remove the node from the CharLinkedList at specified index 
 * arguments: index indicating where to remove node
 * returns:   none
 * effects:   removes a node from linked list
 */
void CharLinkedList::removeAt(int index) {
    if (index < 0 or index >= size()) {
        throw range_error(errorMessage2(index));
    }
    if (index == 0) {
        popFromFront(); //if remove from front
    } else {
        int count = 0;
        Node *curr = front;
        while (count != index) {
            curr = curr->next; //traverse to get to index
            count++;
        }
        if (curr->next == nullptr) {
            popFromBack(); //if remove from back
        } else {
            curr->prev->next = curr->next; //if remove from middle
            curr->next->prev = curr->prev;
            delete curr;
        }
    }
}

/*
 * name:      replaceAt
 * purpose:   replace an item the item from the CharLinkedList at specified 
              index with new item
 * arguments: char to replace and index indicating where to replace item
 * returns:   none
 * effects:   replaces an item from the list with another
 */
void CharLinkedList::replaceAt(char c, int index) {
    if (index < 0 or index >= size()) {
        throw range_error(errorMessage2(index));
    }
    int count = 0;
    Node *curr = front;
    replaceAtRecursivly(curr, index, count, c); //call to recursive fucntion
}

void CharLinkedList::replaceAtRecursivly(Node *curr, int index, int count, char c) {
    if (count == index) {
        curr->c = c; //base case reach index
    }
    else {
        replaceAtRecursivly(curr->next, index, count + 1, c);
    }            //recursive case, return next node, increment count
}

/*
 * name:      concatenate
 * purpose:   combine CharLinkedList with another instance
 * arguments: instance to copy
 * effects:   combines two instances into one
 */
void CharLinkedList::concatenate(CharLinkedList *other) {
    Node *other_curr = other->front;
    Node *curr = back;
    int size1 = other->size();
    int size2 = size();
    int sizeTotal = size1 + size2;
    while(other_curr != nullptr) {
        if (size() == sizeTotal) {
            return;
        }
        pushAtBack(other_curr->c); 
        curr = other_curr;  //add elements of other to back of list
        other_curr = other_curr->next;
    }
}

/*
 * name:      errorMessage1
 * purpose:   create string with error message
 * arguments: number to place into string
 * returns:   a string representation of the error message
 * effects:   none
 */
std::string CharLinkedList::errorMessage1(int index) const {
    std::stringstream ss;
    ss << "index (" << index << ") not in range [0.." << size() << "]";
    return ss.str();
}

/*
 * name:      errorMessage2
 * purpose:   create string with error message
 * arguments: number to place into string
 * returns:   a string representation of the error message
 * effects:   none
 */
std::string CharLinkedList::errorMessage2(int index) const {
    std::stringstream ss;
    ss << "index (" << index << ") not in range [0.." << size() << ")";
    return ss.str();
}
