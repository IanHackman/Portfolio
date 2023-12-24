/*  MetroSim.cpp
 *  Ian Hackman (ihackm01)
 *  10/12
 *
 *  CS 15 proj1 - MetroSim
 *
 *  Function definitions for the MetroSim class. The constructor reads 
 * in the file names and number of arguments and creates streams that 
 * are passed to every other funciton that prints out data either to 
 * std::cout or to an outfile. The constructor checks if the files can 
 * be opened, passed the address of the stations file to be read line 
 * by line, gettin gthe total number of lines and then the contents of 
 * the lines themselves, in and turned into an array of Station structs 
 * and then, depending on if the user gives a commands text file or 
 * will inputt using std::cin and std::cout, the construcotr will pass 
 * either as an outstream type to the function that runs the command 
 * loop. In the command function, the loop will continue as long as the 
 * user doesn't input the QUIT command, f. The command loop checks if 
 * the command is p or m. If p the loop inputts the arrival and departure 
 * station and then calls a function to insert a passenger. If the first 
 * command if m, the second input is checked. If it is m, the function 
 * to move the train, which also calls funcitons that put the passengers 
 * waiting at each station on the train and removes the passenger structs 
 * on the train, is called. 
 *
 */

#include "MetroSim.h"
#include "Passenger.h"
#include "PassengerQueue.h"

#include <iostream>
#include <string>
#include <fstream>

/*
 * name:      MetroSim constructor
 * purpose:   open the files to be used in MetroSim, determine if a 
 text file with commands was inputted, if not the input will be from 
 cin and cout
 * arguments: int for numbe of arguments, char array pointer to argument 
 names
 * returns:   none
 * effects:   pass files to be read 
 */
MetroSim::MetroSim(int argc, char *argv[]) {
    std::ifstream instream;
    std::ofstream outstream; //declare streams for in and out files
    if (argc < 3) {
        abort("Usage: ./MetroSim stationsFile outputFile [commandsFile]");
    }   //if not enough arguments given
    open_or_die(instream, argv[1]);
    readStationsFile(instream, argv[1]);
    if (argc == 3) {
        open_or_die(outstream, argv[2]);
        commands(std::cin, std::cout, outstream); //if no command text file given
    } else {
        open_or_die(instream, argv[3]);
        open_or_die(outstream, argv[2]);
        commands(instream, std::cout, outstream); //if command text file
    }
    instream.close();
    outstream.close();
}

/*
 * name:      abort 
 * purpose:   construct error messagages
 * arguments: string containing the wording of the error message
 * returns:   none
 * effects:   terminates program
 */
void MetroSim::abort(std::string error_message) {
    std::cerr << error_message << std::endl;
    exit(EXIT_FAILURE); //terminate program
}

/*
 * name:      open or die
 * purpose:   to open the file in the file stream and determine if 
 it can be opened or not. If it cant call abort funciton for error 
 message and to quit program
 * arguments: adress of the type of stream the file to be opened is, 
 infile or outfile, and a string with the name of the file
 * returns:   none
 * effects:   if file can't be opened, program quits
 */
template<typename streamtype> //to check opening files with in and outstream
void MetroSim::open_or_die(streamtype &stream, std::string file_name) {
    stream.open(file_name);
    if (not stream.is_open()) {
        abort("Error: could not open file" + file_name);
    }
}

/*
 * name:      readStationsFile
 * purpose:   read in the file of stations. First read line by line to get 
 the number of stations. Then re-read the file and store the name and 
 station number (through incrementing the numStations int) in a heap-
 allocated array which stations will point to.
 * arguments: address of the instream file and a string with the name of 
 the file
 * returns:   none
 * effects:   creates stations array and stores the names of the stations in it
 */
void MetroSim::readStationsFile(std::ifstream &input, std::string stations_file)
{
    int stationsInFile = 0;
    std::string line;
    while(std::getline(input, line)) {
        stationsInFile++; //read in each line to get total number of stations
    }
    numStations = stationsInFile;
    input.close();
    stations = new Station[numStations];
    initStations();
    input.open(stations_file);
    std::string stationName;
    int stationNum = 0;
    for (int i = 0; i < numStations; i++) {
        getline(input, line); //read in entire line for each station name
        stations[i].stationNum = stationNum;
        stations[i].stationName = line;
        stationNum++;
    }
    input.close();
}

/*
 * name:      MetroSim destructor
 * purpose:   remove heap allocated data from stations struct that was declared 
 in readStationsFile
 * arguments: none
 * returns:   none
 * effects:   the dynamic array of station structs is deleted
 */
MetroSim::~MetroSim() {
    delete [] stations;
}

/*
 * name:      initstations
 * purpose:   initialize the all elements of the Station struct in 
 every index of the stations array to empty strings, ints and queues. 
 Also initialize private member ints passengerId to 1, and trainIsAt, 
 numPassWaiting and numPassInTrainCar to 0.
 * arguments: none
 * returns:   none
 * effects:   sets all elemets of structs in stations array to 0, 
 empty and empty queue
 */
void MetroSim::initStations() {
    PassengerQueue emptyQueue;
    for (int i = 0; i < numStations; i++) {
        stations[i].stationNum = 0;
        stations[i].stationName = "";
        stations[i].passengersWaiting = emptyQueue;
        stations[i].passengersOnTrain = emptyQueue;
        stations[i].numPassWaiting = 0;
        stations[i].numPassInTrainCar = 0;
    }   //initialize each element with empty values
    passengerId = 1;
    trainIsAt = 0;
}

/*
 * name:      printStations
 * purpose:   output the names of the stations, the number of each 
 station, the passengers waiting at each stations and which stations 
 the train is at to the terminal. 
 * arguments: address of the output stream
 * returns:   none
 * effects:   none
 */
void MetroSim::printStations(std::ostream &output) {
    for (int i = 0; i < numStations; i++) {
        if (stations[i].stationNum == trainIsAt) {
            output << "TRAIN: ["; //to indicate which stop train is at
        } else {
            output << "       [";
        }
        output << stations[i].stationNum << "] " << stations[i].stationName;
        output << " {";
        stations[i].passengersWaiting.print(output); //call to print queue
        output << "}" << std::endl;
    }
}

/*
 * name:      commands
 * purpose:   run the commands loop for metro sim by printing the 
 passengers ont the train, the stations and passengers waiting, 
 then asking for an input and, depending on the input to either quit, 
 move the train or add a passenger, call a function associated with each. 
 * arguments: addresses of the instreama nd outstream files to be 
 inputted from or outputted to
 * returns:   none
 * effects:   none
 */
void MetroSim::commands(std::istream &input, std::ostream &output1, std::ostream &output2) {
    char command;
    printPassengersOnTheTrain(output1);
    printStations(output1);
    output1 << "Command? ";
    while (input >> command) {
        if (command == MOVE_TRAIN) {
            input >> command;
            if (command == QUIT) {
                output1 << "Thanks for playing MetroSim. Have a nice day!";
                std::cout << std::endl; //exit program
                return;
            } else {
                moveTrain(output2);
            }
        } else {
            int from, to;  //if 'p', input values for to and from
            input >> from >> to;
            if (from > numStations) {
                abort("Not a real station!");
            } //if values for to, from not in range of number of stations
            if (to > numStations) {
                abort("Not a real station!");
            }
            addPassenger(from, to);
        }
        printPassengersOnTheTrain(output1);
        printStations(output1);
        output1 << "Command? ";
    }
    output1 << "Thanks for playing MetroSim. Have a nice day!" << std::endl;
}

/*
 * name:      move train
 * purpose:   increment the trainIs at int to the next station or the 0th 
 station, dequeue the passengers on the train, than add each passenger 
 waiting in the tsation to the train by dequeueing them from the station 
 and enqueueing them t the train.
 * arguments: address of the outstream 
 * returns:   none
 * effects:   the queue elements of the stations structs are altered
 */
void MetroSim::moveTrain(std::ostream &output) {
    if (stations[trainIsAt].numPassWaiting != 0) {
        while (stations[trainIsAt].numPassWaiting != 0) {
            Passenger PassBoard = stations[trainIsAt].passengersWaiting.front();
            stations[trainIsAt].passengersWaiting.dequeue();
            stations[trainIsAt].numPassWaiting--;
            stations[PassBoard.to].passengersOnTrain.enqueue(PassBoard);
            stations[PassBoard.to].numPassInTrainCar++;
        } //get first passenger from waiting list, store in temporary Passenger
    }     //struct, dequeue, and add temp passenger to on train queue
    if (trainIsAt == numStations - 1) {
        trainIsAt = 0;
    } else {
        trainIsAt++; //updat eposition of train
    }
    removePassengersFromTrain(output);
}

/*
 * name:      print passengers on the train 
 * purpose:   output the passengers on the train queue in numerical 
 order for each station struct in the array
 * arguments: address of outstream
 * returns:   none
 * effects:   none
 */
void MetroSim::printPassengersOnTheTrain(std::ostream &output) {
    output << "Passengers on the train: {";
    for (int i = 0; i < numStations; i++) {
        stations[i].passengersOnTrain.print(output);
    }   //for each station in array, call to PassengerQueue print funciton
    output << "}" << std::endl;
}

/*
 * name:      add passenger
 * purpose:   create a new passenger struct and enqueue it to the 
 stations array at the index of the from element the user inputs
 * arguments: int of where the passenger boards and int of where 
 the passenger will get off
 * returns:   none
 * effects:   a passenger struct is added to the passenegrs waiting at 
 station queue for 1 
 element of the array
 */
void MetroSim::addPassenger(int from, int to) {
    Passenger newPassenger(passengerId, from, to);
    stations[from].passengersWaiting.enqueue(newPassenger);
    stations[from].numPassWaiting++;
    passengerId++; //add new passenger to waiting queue
}

/*
 * name:      remove passengers from train
 * purpose:   for each passenger in the passengers on train queue at 
 the index equal to where the train is at, dequeue all and output to 
 the outfile that each has gotten off
 * arguments: address of outstream
 * returns:   none
 * effects:   removes Passenegr structs from the stations 
 passengers on train queue at index of trainIsAt
 */
void MetroSim::removePassengersFromTrain(std::ostream &output) {
    while(stations[trainIsAt].numPassInTrainCar != 0) {
        Passenger temp = stations[trainIsAt].passengersOnTrain.front();
        stations[trainIsAt].passengersOnTrain.dequeue();
        stations[trainIsAt].numPassInTrainCar--;
        output << "Passenger " << temp.id << " left train at station "; 
        output << stations[trainIsAt].stationName << std::endl;
    } //remove each Passenger in on train queue and output name and 
}     //station number to output file