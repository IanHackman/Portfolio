/*
 *  unit_tests.h
 *  YOUR NAME HERE
 *  DATE CREATED
 *
 *  CS 15 HW 2 Cheshire ConCATenation - Time to linked up in Fur
 *
 *  Test functions for CharLinkedList. Functions were tested using 
 *  assert() function and the funcitons size(), empty(), elementAt() 
 *  and toString(). In addition error messages were cought using throw 
 *  and catch.
 *
 */

#include "CharLinkedList.h"
#include <cassert>
#include <sstream>
#include <iostream>

//test create empty linked list and test if it's empty
void create_empty_list() {
    CharLinkedList test;
    assert(test.isEmpty());
}

//test create list with 1 node
void create_node() {
    CharLinkedList test('c');
    assert(test.size() == 1);
}

//test creating list with multiple nodes from an array of chars
void test_create_array() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    assert(test.size() == 3);
}

//test copy constructor
void test_copy_constructor() {
    char array[3] = { 'c', 'a', 't' };
    CharLinkedList testList(array, 3);

    CharLinkedList empty(testList);

    assert(empty.size() == 3);
    assert(empty.toString() == "[CharLinkedList of size 3 <<cat>>]");
}

//test setting two list equal to each other
void test_assignmentOperator() {
    char array[3] = { 'c', 'a', 't' };
    CharLinkedList testArray1(array, 3);

    char array2[4] = { 'C', 'U', 'T', 'E' };
    CharLinkedList testArray2(array2, 4);

    testArray1 = testArray2;
    
    assert(testArray1.size() == 4);
    std::cout << testArray1.toString() << std::endl;
    assert(testArray1.toString() == "[CharLinkedList of size 4 <<CUTE>>]");
}

//test deleting a linked list
void test_clear() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    test.clear();
    assert(test.isEmpty());
}

//test acessing first node of linked list
void test_first() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    assert(test.first() == 'c');
}

//test acessing last node of linked list
void test_last() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    assert(test.last() == 't');
}

//test acessing user-given node of linked list
void test_elementAt() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    assert(test.elementAt(1) == 'a');
}

//test acessing user-given node of linked list
void test_elementAt2() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    assert(test.elementAt(0) == 'c');
}

//test turning char element of linked list into a string
void test_toString() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    assert(test.toString() == "[CharLinkedList of size 3 <<cat>>]");
}

//test turning char element of linked list into a string but reversed
void test_toReverseString() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    assert(test.toReverseString() == "[CharLinkedList of size 3 <<tac>>]");
}

//test inserting in middle of array
void test_insertAt() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    test.insertAt('w', 1);
    assert(test.size() == 4);
    assert(test.elementAt(1) == 'w');
}

//test inserting into empty array
void insertAt_empty_correct() { 
    CharLinkedList test_list;
    test_list.insertAt('a', 0);
    assert(test_list.size() == 1);
    assert(test_list.elementAt(0) == 'a');      
}

//test inserting node in end of list
void test_pushAtBack() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    test.pushAtBack('w');
    assert(test.size() == 4);
    assert(test.elementAt(3) == 'w');
}

//test inserting node in end of list
void test_pushAtFront() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    test.pushAtFront('w');
    assert(test.size() == 4);
    assert(test.elementAt(0) == 'w');
}

//test inserting new char in alphabetical order
void test_insertInOrder() {
    char array[5] = { 'a', 'b', 'd', 'e', 'f'};
    CharLinkedList testArray(array, 5);

    testArray.insertInOrder('c');

    assert(testArray.size() == 6);
    assert(testArray.elementAt(2) == 'c');
}

//test inserting new char in alphabetical order
void test_insertInOrder1() {
    char array[3] = { 'D', 'E', 'D' };
    CharLinkedList testArray(array, 3);

    testArray.insertInOrder('A');

    assert(testArray.size() == 4);
    assert(testArray.elementAt(0) == 'A');
}

//test deleting first item in array
void test_popFromFront() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharLinkedList testArray(array, 8);

    testArray.popFromFront();

    assert(testArray.size() == 7);
    assert(testArray.elementAt(0) == 'b');
}

//test deleting last item in array
void test_popFromBack() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharLinkedList testArray(array, 8);

    testArray.popFromBack();

    assert(testArray.size() == 7);
    assert(testArray.elementAt(6) == 'g');
}

//test deleting an item in array at specified index
void test_removeAt() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharLinkedList testArray(array, 8);

    testArray.removeAt(3);

    assert(testArray.size() == 7);
    assert(testArray.elementAt(3) == 'e');
}

//test out-of-range index for error message
void test_removeAt_errorMessage() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharLinkedList testArray(array, 8);

    bool range_error_thrown = false;

    std::string error_message = "";

    try {
         testArray.removeAt(9);
    }
    catch (const std::range_error &e) {
        range_error_thrown = true;
        error_message = e.what();
    }

    assert(range_error_thrown);
    assert(error_message == "index (9) not in range [0..8)");
}

//test deleting an item in array at specified index
void test_removeAt1() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharLinkedList testArray(array, 8);

    testArray.removeAt(3);

    assert(testArray.size() == 7);
    assert(testArray.elementAt(3) == 'e');
}

//test deleting an item in array at front specified index
void test_removeAt2() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharLinkedList testArray(array, 8);

    testArray.removeAt(0);

    assert(testArray.size() == 7);
    assert(testArray.elementAt(0) == 'b');
}

//test deleting an item from back of array
void test_removeAt3() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharLinkedList testArray(array, 8);

    testArray.removeAt(7);

    assert(testArray.size() == 7);
    assert(testArray.elementAt(6) == 'g');
}

//test deleting multiple items
void test_removeAt4() {
    char test_arr[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
        CharLinkedList test_list(test_arr, 8);

        test_list.removeAt(3);

        assert(test_list.size() == 7);
        assert(test_list.elementAt(3) == 'e');
        assert(test_list.toString() == 
        "[CharLinkedList of size 7 <<abcefgh>>]");

        test_list.removeAt(0);

        assert(test_list.size() == 6);
        assert(test_list.elementAt(0) == 'b');
        assert(test_list.toString() == 
        "[CharLinkedList of size 6 <<bcefgh>>]");

        test_list.removeAt(5);

        assert(test_list.size() == 5);
        assert(test_list.elementAt(4) == 'g');
        assert(test_list.toString() == 
        "[CharLinkedList of size 5 <<bcefg>>]");
}

//test replacing an a char in array with a new char
void test_replaceAt() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharLinkedList testArray(array, 8);

    testArray.replaceAt('z', 3);

    assert(testArray.elementAt(3) == 'z');
}

//test replacing an item in array at front specified index
void test_replaceAt2() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharLinkedList testArray(array, 8);

    testArray.replaceAt('z', 0);

    assert(testArray.elementAt(0) == 'z');
}

//test replacing an item from back of array
void test_replaceAt3() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharLinkedList testArray(array, 8);

    testArray.replaceAt('z', 7);

    assert(testArray.elementAt(7) == 'z');
}

//test combining 2 arrays
void test_concatenate() {
    char array1[3] = { 'c', 'a', 't' };
    CharLinkedList testArray1(array1, 3);

    char array2[4] = { 'C', 'U', 'T', 'E' };
    CharLinkedList testArray2(array2, 4);
    CharLinkedList *other = &testArray2;

    testArray1.concatenate(other);
    assert(testArray1.size() == 7);

    assert(testArray1.toString() == "[CharLinkedList of size 7 <<catCUTE>>]");
}

//tets combining emptry array with full one
void test_Emptyconcatenate() {
    char array1[3] = { 'c', 'a', 't' };
    CharLinkedList testArray1(array1, 3);

    CharLinkedList testArray2;
    CharLinkedList *other = &testArray2;

    testArray1.concatenate(other);
    assert(testArray1.size() == 3);

    assert(testArray1.toString() == "[CharLinkedList of size 3 <<cat>>]");
}

//test concatenating 2 empty arrays
void test_double_empty_concatenate() {
    CharLinkedList testArray1;

    CharLinkedList testArray2;
    CharLinkedList *other = &testArray2;

    testArray1.concatenate(other);
    assert(testArray1.size() == 0);

    assert(testArray1.toString() == "[CharLinkedList of size 0 <<>>]");
}

//test combining an array with itself
void test_concatenate_with_self() {
    char array1[3] = { 'c', 'a', 't' };
    CharLinkedList testArray1(array1, 3);

    CharLinkedList *other = &testArray1;

    testArray1.concatenate(other);

    assert(testArray1.toString() == "[CharLinkedList of size 6 <<catcat>>]");
}

//tets error message for insertAt
void insertAt_empty_incorrect() {
    bool range_error_thrown = false;
    std::string error_message = "";
    CharLinkedList test_list;
    try {
        test_list.insertAt('a', 42);
    }
    catch (const std::range_error &e) {
        range_error_thrown = true;
        error_message = e.what();
    }
    assert(range_error_thrown);
    assert(error_message == "index (42) not in range [0..0]");
}

// Tests correct insertAt for front of 1-element list.
void insertAt_front_singleton_list() {
    CharLinkedList test_list('a');
    test_list.insertAt('b', 0);
    assert(test_list.size() == 2);
    assert(test_list.elementAt(0) == 'b');
    assert(test_list.elementAt(1) == 'a');
    
}

// Tests correct insertAt for back of 1-element list.
void insertAt_back_singleton_list() {
    CharLinkedList test_list('a');
    test_list.insertAt('b', 1);
    assert(test_list.size() == 2);
    assert(test_list.elementAt(0) == 'a');
    assert(test_list.elementAt(1) == 'b');
    
}

// Tests calling insertAt for a large number of elements.
// Not only does this test insertAt, it also checks that
// array expansion works correctly.
void insertAt_many_elements() {
    CharLinkedList test_list;
    for (int i = 0; i < 1000; i++) {
        test_list.insertAt('a', i);
    }
    assert(test_list.size() == 1000);
    for (int i = 0; i < 1000; i++) {
        assert(test_list.elementAt(i) == 'a');
    }
}

// Tests insertion into front, back, and middle of a larger list.
void insertAt_front_back_middle() {
        char test_arr[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
        CharLinkedList test_list(test_arr, 8);
        
        test_list.insertAt('z', 3);
        assert(test_list.size() == 9);
        assert(test_list.elementAt(3) == 'z');
        assert(test_list.toString() == 
        "[CharLinkedList of size 9 <<abczdefgh>>]");
        test_list.insertAt('y', 0);

        assert(test_list.size() == 10);
        assert(test_list.elementAt(0) == 'y');
        assert(test_list.toString() == 
        "[CharLinkedList of size 10 <<yabczdefgh>>]");

        test_list.insertAt('x', 10);

        assert(test_list.size() == 11);
        assert(test_list.elementAt(10) == 'x');
        assert(test_list.toString() == 
        "[CharLinkedList of size 11 <<yabczdefghx>>]");
        
}

// Tests out-of-range insertion for a non-empty list.
void insertAt_nonempty_incorrect() {
    
    char test_arr[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharLinkedList test_list(test_arr, 8);

    bool range_error_thrown = false;

    std::string error_message = "";

    try {
        test_list.insertAt('a', 42);
    }
    catch (const std::range_error &e) {
        range_error_thrown = true;
        error_message = e.what();
    }

    assert(range_error_thrown);
    assert(error_message == "index (42) not in range [0..8]");
    
}

//test element in front of list
void test_elementAt76() {
    char testArray[3] = { 'c', 'a', 't'};
    CharLinkedList test(testArray, 3);
    assert(test.elementAt(0) == 'c');
}

//test for one char list
void test_insertInOrder_OneChar() {
    CharLinkedList test('c');
    test.insertInOrder('d');
    assert(test.elementAt(1) == 'd');
}

//test inseting char at end
void test_insertInOrder_end() {
    char array[4] = { 'a', 'b', 'c', 'd' };
    CharLinkedList testArray(array, 4);

    testArray.insertInOrder('z');

    assert(testArray.size() == 5);
    assert(testArray.elementAt(4) == 'z');
}