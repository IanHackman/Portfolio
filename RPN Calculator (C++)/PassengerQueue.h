/*
 *  PassengerQueue.h
 *  Ian Hackman (ihackm01)
 *  10/5
 *
 *  CS 15 proj1 - MetroSim
 *
 *  Funciton declarations for PassengerQueue class. PassengerQueue 
 * includes 5 functions, front, dequeue, enqueue, size and print. 
 * The queue list is initialized using default constructors and has 
 * the defualt big three. The queue list itself is initialized by 
 * std::list and is made of Passenger structs. The PassengerQueue 
 * class is used by two elements of the Station struct in MetroSim 
 * to store the queue of Passengers waiting for and on the train.
 *
 */

#ifndef PASSENGERQUEUE_H
#define PASSENGERQUEUE_H

#include <iostream>
#include <string>
#include <list>
#include "Passenger.h"

class PassengerQueue {
public:
    Passenger front();
    void dequeue();
    void enqueue(const Passenger &passenger);
    int size();
    void print(std::ostream &output);

private:
    std::list<Passenger> p;
};

#endif
