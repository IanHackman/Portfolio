/*
 *  PassengerQueue.cpp
 *  Ian Hackman (ihackm01)
 *  10/5
 *
 *  CS 15 proj1 - MetroSim
 *
 *  Function definitions for the PassengerQueue class. The functions 
 * enwueue and dequeue add or remove passsenger structs to the back and 
 * front of the list. Front accesses and returns the first passenger in 
 * the queue. Size returns the size of the queue and print accessed the 
 * print element of each Passenger struct. Each funciton uses fucntion 
 * calls from std::list. 
 *
 */

#include "PassengerQueue.h"
#include "Passenger.h"
#include <sstream>
#include <iostream>
#include <list>

using namespace std;

/*
 * name:      enque
 * purpose:   insert a new Passenger at the end of the queue
 * arguments: adress of a Passenger
 * returns:   none
 * effects:   adds new Passenger to queue        
 */
void PassengerQueue::enqueue(const Passenger &passenger) {
    p.push_back(passenger);
}

/*
 * name:      dequeue
 * purpose:   remove Passenger from front of queue
 * arguments: none
 * returns:   none
 * effects:   removes Passenger from queue
 */
void PassengerQueue::dequeue() {
    p.pop_front();
}

/*
 * name:      front
 * purpose:   return the first passenger of PassengerQueue queue
 * arguments: none
 * returns:   first char in CharLinkedList
 * effects:   none
 */
Passenger PassengerQueue::front() {
   return(p.front());
}

/*
 * name:      size
 * purpose:   determine the number of nodes in the CharLinkedList
 * arguments: none
 * returns:   number of nodes in the CharLinkedList
 * effects:   none
 */
int PassengerQueue::size() {
    return(p.size());
}

/*
 * name:      print
 * purpose:   access the print element of each Passenger node
 * arguments: output stream
 * returns:   none
 * effects:   none
 */
void PassengerQueue::print(ostream &output) {
    for (std::list<Passenger>::iterator i = p.begin(); i != p.end(); i++) {
        i->print(output);
    }
}