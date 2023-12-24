/*
 *  Passenger.h
 *  Ian Hackman (ihackm01)
 *  10/5
 *
 *  CS 15 proj1 - MetroSim
 *
 * Passenger struct definition and declaration of print function. 
 * The passenger struct contains three integers, id, from and to. 
 * It also contains a constructor to initialize the ints as -1 and 
 * another constructor to assign the 3 ints values. The Passenger 
 * structs make up the queues in PassengerQueue. In the MetroSim class, 
 * they are used when a new passenger is created and assigned info by 
 * the user or from a file. 
 *
 */

#ifndef __PASSENGER_H__
#define __PASSENGER_H__

#include <iostream>

struct Passenger
{

        int id, from, to;
        
        Passenger()
        {
                id   = -1;
                from = -1;
                to   = -1;
        }

        Passenger(int newId, int arrivalStation, int departureStation)
        {
                id   = newId;
                from = arrivalStation;
                to   = departureStation;
        }

        // TODO: implement the print function in Passenger.cpp
        void print(std::ostream &output) {
                output << "[" << id << ", " << from << "->" << to << "]";

        }

};

#endif
