/*  MetroSim.h
 *  Ian Hackman (ihackm01)
 *  10/12
 *
 *  CS 15 proj1 - MetroSim
 *
 *  Function declarations and interface for MetroSim class. In public 
 * funcitons there is one constructor and destructor, the funciton to 
 * initiate the command loop and the commands for the user to enter as 
 * constant characters. In private are every other funciton that interacts 
 * or changes the elements of the pirvate memeber array. There are two 
 * funcitons that print data, one error message funciton, two functions 
 * that check if the files inputted from the user can be opened and then 
 * the files are read in, a function to initialize the data structure used 
 * and three functions that execute the commands inputted from th euser or 
 * from a file. In addition there is a private member variable struct of 
 * stations with the name, number, passengers waiting queue, passengers 
 * to get off at station queue and the sizes of both queues. There is a 
 * pointer to an array of Station structs and three ints to keep track of 
 * the size of the stations, where th etrain is at and the passenger IDs.
 *
 */

#ifndef _METROSIM_H_
#define _METROSIM_H_

#include "Passenger.h"
#include "PassengerQueue.h"

// Put any other necessary includes here
#include <iostream>
#include <string>
#include <list>
#include <fstream>
// Put any other structs you need here

class MetroSim {
public:
    MetroSim(int argc, char *argv[]);
    ~MetroSim();
    void commands(std::istream &input, std::ostream &output1, std::ostream &output2);
    const char ADD_PASSENGER = 'p';
    const char MOVE_TRAIN = 'm';
    const char QUIT = 'f';
private:
    void printStations(std::ostream &output);
    void printPassengersOnTheTrain(std::ostream &output);
    void abort(std::string error_message);
    void readStationsFile(std::ifstream &input, std::string file_name);
    void initStations();
    void moveTrain(std::ostream &output);
    void addPassenger(int from, int to);
    void removePassengersFromTrain(std::ostream &output);

    template<typename streamtype>
    void open_or_die(streamtype &stream, std::string file_name); 

    struct Station {
        int stationNum;
        std::string stationName;
        PassengerQueue passengersWaiting;
        int numPassWaiting;
        PassengerQueue passengersOnTrain;
        int numPassInTrainCar;
    };
    Station *stations;
    int numStations;
    int passengerId;
    int trainIsAt;
};

#endif
