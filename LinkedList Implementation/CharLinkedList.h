/*
 *  CharLinkedList.h
 *  Ian Hackman (ihackm01)
 *  9/27
 *
 *  CS 15 HW 2 Cheshire ConCATenation - Time to get linked up in Fur
 *
 *  Function definitions for the CharArrayList class. In public are 
 *  3 constructors, a copy constructor, a destructor, an assignment operator 
 *  and fnctions that manipulate linked list or analyze its contents. In 
 *  private is the struct node, pointers to the front and back of the list and 
 *  recursive functions. The definitions also include which parameters are 
 *  needed for each function. 
 *
 */

#ifndef CHAR_LINKED_LIST_H
#define CHAR_LINKED_LIST_H

#include <iostream>
#include <string>

class CharLinkedList {
public: 

    CharLinkedList();
    CharLinkedList(char c);
    CharLinkedList(char arr[], int size);
    CharLinkedList(const CharLinkedList &other);
    ~CharLinkedList();

    CharLinkedList &operator=(const CharLinkedList &other);
    bool isEmpty() const;
    void clear();
    int size() const;
    char first() const;
    char last() const;
    char elementAt(int index) const;
    std::string toString() const;
    std::string toReverseString() const;
    void pushAtBack(char c);
    void pushAtFront(char c);
    void insertAt(char c, int index);
    void insertInOrder(char c);
    void popFromFront();
    void popFromBack();
    void removeAt(int index);
    void replaceAt(char c, int index);
    void concatenate(CharLinkedList *other);
private:
    struct Node {
        char c;
        Node *next;
        Node *prev;
    };
    Node *front;
    Node *back;
    void deleteRecursivly(Node *p);
    char elementAtRecursivly(Node *curr, int index, int count) const;
    void replaceAtRecursivly(Node *curr, int index, int count, char c);
    std::string errorMessage1(int index) const;
    std::string errorMessage2(int index) const;
};

#endif
