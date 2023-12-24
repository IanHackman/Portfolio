/*
 *  Passenger.h
 *  Ian Hackman (ihackm01)
 *  10/5
 *
 *  CS 15 proj1 - MetroSim
 *
 * Funciton definition for the print element in passenger struct. 
 * The print function uses an outstream to outstream the integer 
 * elements of the Passenger struct. 
 *
 */

#include <iostream>
#include <string>
#include "Passenger.h"

/*
 * name:      print
 * purpose:   print the elements of struct Passenger
 * arguments: output stream 
 * returns:   none
 * effects:   none
 */
void Passenger::print(std::ostream &output) {
    output << "[" << id << ", " << from << "->" << to << "]";
}
