/*
 *  unit_tests.h
 *  Ian Hackman (ihackm01)
 *  10/12
 *
 *  CS 15 proj1 - MetroSim
 *
 *  PURPOSE
 *
 */

#include "Passenger.h"
#include "PassengerQueue.h"
#include "MetroSim.h"
#include <cassert>
#include <sstream>
#include <iostream>

//test print element of Person struct
void test_printPassenger() {
    Passenger person;
    person.id = 1;
    person.from = 3;
    person.to = 7;

    std::ostringstream ss;
    
    person.print(ss);

    assert(ss.str() == "[1, 3->7]");
}

//test adding Passengers to queue
void test_enqueue() {
    Passenger person1;
    person1.id = 1;
    person1.from = 3;
    person1.to = 7;

    Passenger person2;
    person2.id = 2;
    person2.from = 4;
    person2.to = 8;

    Passenger person3;
    person3.id = 3;
    person3.from = 5;
    person3.to = 9;
    
    PassengerQueue p;

    p.enqueue(person1);
    p.enqueue(person2);
    p.enqueue(person3);

    std::ostringstream ss;

    p.print(ss);
    assert(ss.str() == "[1, 3->7][2, 4->8][3, 5->9]");
}

//test removing Passengers to queue
void test_dequeue() {
    Passenger person1;
    person1.id = 1;
    person1.from = 3;
    person1.to = 7;

    Passenger person2;
    person2.id = 2;
    person2.from = 4;
    person2.to = 8;

    Passenger person3;
    person3.id = 3;
    person3.from = 5;
    person3.to = 9;
    
    PassengerQueue p;

    p.enqueue(person1);
    p.enqueue(person2);
    p.enqueue(person3);

    p.dequeue();

    std::ostringstream ss;

    p.print(ss);
    assert(ss.str() == "[2, 4->8][3, 5->9]");
}

//test how many Passengers in queue
void test_size() {
    Passenger person1;
    person1.id = 1;
    person1.from = 3;
    person1.to = 7;

    Passenger person2;
    person2.id = 2;
    person2.from = 4;
    person2.to = 8;

    Passenger person3;
    person3.id = 3;
    person3.from = 5;
    person3.to = 9;
    
    PassengerQueue p;

    p.enqueue(person1);
    p.enqueue(person2);
    p.enqueue(person3);

    assert(p.size() == 3);
}

//test to see first Person in queue
void test_front() {
    Passenger person1;
    person1.id = 1;
    person1.from = 3;
    person1.to = 7;

    Passenger person2;
    person2.id = 2;
    person2.from = 4;
    person2.to = 8;

    Passenger person3;
    person3.id = 3;
    person3.from = 5;
    person3.to = 9;
    
    PassengerQueue p;

    p.enqueue(person1);
    p.enqueue(person2);
    p.enqueue(person3);

    Passenger person4 = p.front();
    std::ostringstream ss;

    person4.print(ss);

    assert(ss.str() == "[1, 3->7]");
}
