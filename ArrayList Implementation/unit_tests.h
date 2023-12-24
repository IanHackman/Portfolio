/*
 * unit_tests.h
 *
 * CS 15 homework 1
 * by Tyler Calabrese, January 2021
 *
 * edited by: Milod Kazerounian, January 2022
 *
 * Uses Matt Russell's unit_test framework to test the CharArrayList class.
 *
 * Instructions for using testing framework can be found at in CS 15's lab
 * 1 -- both in the spec and in the provided ArrayList_tests.h and Makefile.
 * More in-depth information for those who are curious as to how it works can
 * be found at http://www.github.com/mattrussell2/unit_test.
 */

#include "CharArrayList.h"
#include <cassert>

/********************************************************************\
*                       CHAR ARRAY LIST TESTS                        *
\********************************************************************/

/* To give an example of thorough testing, we are providing
 * the unit tests below which test the insertAt function. Notice: we have
 * tried to consider all of the different cases insertAt may be
 * called in, and we test insertAt in conjunction with other functions!
 *
 * You should emulate this approach for all functions you define.
 */


// Tests correct insertion into an empty AL.
// Afterwards, size should be 1 and element at index 0
// should be the element we inserted.
/*void insertAt_empty_correct() { 
    CharArrayList test_list;
    test_list.insertAt('a', 0);
    assert(test_list.size() == 1);
    assert(test_list.elementAt(0) == 'a');
                        
}

// Tests incorrect insertion into an empty AL.
// Attempts to call insertAt for index larger than 0.
// This should result in an std::range_error being raised.
void insertAt_empty_incorrect() {
    
    // var to track whether range_error is thrown
    bool range_error_thrown = false;

    // var to track any error messages raised
    std::string error_message = "";

    CharArrayList test_list;
    try {
        // insertAt for out-of-range index
        test_list.insertAt('a', 42);
    }
    catch (const std::range_error &e) {
        // if insertAt is correctly implemented, a range_error will be thrown,
        // and we will end up here
        range_error_thrown = true;
        error_message = e.what();
    }

    // out here, we make our assertions
    assert(range_error_thrown);
    assert(error_message == "index (42) not in range [0..0]");
    
}

// Tests correct insertAt for front of 1-element list.
void insertAt_front_singleton_list() {
    
    // initialize 1-element list
    CharArrayList test_list('a');

    // insert at front
    test_list.insertAt('b', 0);

    assert(test_list.size() == 2);
    assert(test_list.elementAt(0) == 'b');
    assert(test_list.elementAt(1) == 'a');
    
}

// Tests correct insertAt for back of 1-element list.
void insertAt_back_singleton_list() {
    
    // initialize 1-element list
    CharArrayList test_list('a');

    // insert at back
    test_list.insertAt('b', 1);

    assert(test_list.size() == 2);
    assert(test_list.elementAt(0) == 'a');
    assert(test_list.elementAt(1) == 'b');
    
}

// Tests calling insertAt for a large number of elements.
// Not only does this test insertAt, it also checks that
// array expansion works correctly.
void insertAt_many_elements() {
    
    CharArrayList test_list;

    // insert 1000 elements
    for (int i = 0; i < 1000; i++) {
        // always insert at the back of the list
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
        CharArrayList test_list(test_arr, 8);

        // insertAt middle
        test_list.insertAt('z', 3);

        assert(test_list.size() == 9);
        assert(test_list.elementAt(3) == 'z');
        assert(test_list.toString() == 
        "[CharArrayList of size 9 <<abczdefgh>>]");

        // insertAt front
        test_list.insertAt('y', 0);

        assert(test_list.size() == 10);
        assert(test_list.elementAt(0) == 'y');
        assert(test_list.toString() == 
        "[CharArrayList of size 10 <<yabczdefgh>>]");

        // insertAt back
        test_list.insertAt('x', 10);

        assert(test_list.size() == 11);
        assert(test_list.elementAt(10) == 'x');
        assert(test_list.toString() == 
        "[CharArrayList of size 11 <<yabczdefghx>>]");
        
}

// Tests out-of-range insertion for a non-empty list.
void insertAt_nonempty_incorrect() {
    
    char test_arr[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList test_list(test_arr, 8);

    // var to track whether range_error is thrown
    bool range_error_thrown = false;

    // var to track any error messages raised
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

//test construction of empty array list
void empty_constructor() {
    CharArrayList empty;
    assert(empty.isEmpty());
}

//test construction of array list with 1 char
void single_constructor() {
    CharArrayList single('z');
    assert(single.size() == 1);
    assert(single.elementAt(0) == 'z');
}

//test construction of array list with 8 chars
void array_constructor() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);
    
    assert(testArray.size() == 8);
    assert(testArray.toString() == "[CharArrayList of size 8 <<abcdefgh>>]");
}

//test clear function by clearing array of size 8
void clearTest() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);

    testArray.clear();

    assert(testArray.isEmpty());
}

//test if error message appears when trying to get first char of empty array list
void testFirstError() {
    CharArrayList test_list;

    bool runtime_error_thrown = false;

    std::string error_message = "";

    try {
        test_list.first();
    }
    catch (const std::runtime_error &e) {
        runtime_error_thrown = true;
        error_message = e.what();
    }

    assert(runtime_error_thrown);
    assert(error_message == "cannot get first of empty ArrayList");
}

//test if array printed in reverse correctly
void test_toReverseString() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);

    assert(testArray.toReverseString() == "[CharArrayList of size 8 <<hgfedcba>>]");
}

//test toString on empty array
void test_toString_empty() {
    CharArrayList testArray;

    assert(testArray.toReverseString() == "[CharArrayList of size 0 <<>>]");
}

//test adding new char to back of array
void test_pushAtBack() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);

    testArray.pushAtBack('z');

    assert(testArray.size() == 9);
    assert(testArray.elementAt(8) == 'z');
}

//test adding new char to front of array
void test_pushAtFront() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);
    
    testArray.insertAt('y', 0);

    assert(testArray.size() == 9);
    assert(testArray.elementAt(0) == 'y');
}

//test inserting new char in alphabetical order
void test_insertInOrder() {
    char array[5] = { 'a', 'b', 'd', 'e', 'f'};
    CharArrayList testArray(array, 5);

    testArray.insertInOrder('c');

    assert(testArray.size() == 6);
    assert(testArray.elementAt(2) == 'c');
}

//test deleting first item in array
void test_popFromFront() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);

    testArray.popFromFront();

    assert(testArray.size() == 7);
    assert(testArray.elementAt(0) == 'b');
}

//test deleting last item in array
void test_popFromBack() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);

    testArray.popFromBack();

    assert(testArray.size() == 7);
    assert(testArray.elementAt(6) == 'g');
}

//test deleting an item in array at specified index
void test_removeAt() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);

    testArray.removeAt(3);

    assert(testArray.size() == 7);
    assert(testArray.elementAt(3) == 'e');
}

//test out-of-range index for error message
void test_removeAt_errorMessage() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);

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

//test replacing an a char in array with a new char
void test_replaceAt() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);

    testArray.replaceAt('z', 3);

    assert(testArray.elementAt(3) == 'z');
}

//test combining 2 arrays
void test_concatenate() {
    char array1[3] = { 'c', 'a', 't' };
    CharArrayList testArray1(array1, 3);

    char array2[4] = { 'C', 'U', 'T', 'E' };
    CharArrayList testArray2(array2, 4);
    CharArrayList *other = &testArray2;

    testArray1.concatenate(other);
    assert(testArray1.size() == 7);

    assert(testArray1.toString() == "[CharArrayList of size 7 <<catCUTE>>]");
}

//tets combining emptry array with full one
void test_Emptyconcatenate() {
    char array1[3] = { 'c', 'a', 't' };
    CharArrayList testArray1(array1, 3);

    CharArrayList testArray2;
    CharArrayList *other = &testArray2;

    testArray1.concatenate(other);
    assert(testArray1.size() == 3);

    assert(testArray1.toString() == "[CharArrayList of size 3 <<cat>>]");
}

//test making shallow copy of a constructor
void test_copyConstructor() {
    char array[3] = { 'c', 'a', 't' };
    CharArrayList testArray(array, 3);

    CharArrayList empty(testArray);

    assert(empty.size() == 3);
}

//test setting two list equal to each other
void test_assignmentOperator() {
    char array[3] = { 'c', 'a', 't' };
    CharArrayList testArray1(array, 3);

    char array2[4] = { 'C', 'U', 'T', 'E' };
    CharArrayList testArray2(array2, 4);

    testArray1 = testArray2;
    
    assert(testArray1.size() == 4);
    assert(testArray1.toString() == "[CharArrayList of size 4 <<CUTE>>]");
}

//test increasing capacity
void test_expand() {
    char array[2] = { 'c', 'a' };
    CharArrayList testArray1(array, 2);

    testArray1.pushAtBack('t');
    testArray1.pushAtBack('t');
    testArray1.pushAtBack('t');

    assert(testArray1.toString() == "[CharArrayList of size 5 <<cattt>>]");
}

//test inserting new char in alphabetical order
void test_insertInOrder1() {
    char array[3] = { 'Z', 'E', 'D' };
    CharArrayList testArray(array, 3);

    testArray.insertInOrder('A');

    assert(testArray.size() == 4);
    assert(testArray.elementAt(0) == 'A');
}

//test deleting an item in array at specified index
void test_removeAt1() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);

    testArray.removeAt(3);

    assert(testArray.size() == 7);
    assert(testArray.elementAt(3) == 'e');
}

//test deleting an item in array at front specified index
void test_removeAt2() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);

    testArray.removeAt(0);

    assert(testArray.size() == 7);
    assert(testArray.elementAt(0) == 'b');
}

//test deleting an item from back of array
void test_removeAt3() {
    char array[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList testArray(array, 8);

    testArray.removeAt(7);

    assert(testArray.size() == 7);
    assert(testArray.elementAt(6) == 'g');
}

//test deleting multiple items
void test_removeAt4() {
    char test_arr[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
        CharArrayList test_list(test_arr, 8);

        test_list.removeAt(3);

        assert(test_list.size() == 7);
        assert(test_list.elementAt(3) == 'e');
        assert(test_list.toString() == 
        "[CharArrayList of size 7 <<abcefgh>>]");

        test_list.removeAt(0);

        assert(test_list.size() == 6);
        assert(test_list.elementAt(0) == 'b');
        assert(test_list.toString() == 
        "[CharArrayList of size 6 <<bcefgh>>]");

        test_list.removeAt(5);

        assert(test_list.size() == 5);
        assert(test_list.elementAt(4) == 'g');
        assert(test_list.toString() == 
        "[CharArrayList of size 5 <<bcefg>>]");
}

//test reducing capacity to numChars
void test_shrink() {
    char test_arr[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    CharArrayList test_list(test_arr, 8);

    test_list.shrink();
    assert(test_list.size() == 8);
}*/

//tets adding empty strings
void test_concatenate2() {
    CharArrayList testArray1;
    CharArrayList testArray2;
    
    CharArrayList *other = &testArray2;

    testArray1.concatenate(other);
    assert(testArray1.size() == 0);

    assert(testArray1.toString() == "[CharArrayList of size 0 <<>>]");
}

//test concatenating array with itself
void test_concatenate() {
    char array1[3] = { 'c', 'a', 't' };
    CharArrayList testArray1(array1, 3);

    CharArrayList *other = &testArray1;

    testArray1.concatenate(other);
    assert(testArray1.size() == 6);

    assert(testArray1.toString() == "[CharArrayList of size 6 <<catcat>>]");
}
