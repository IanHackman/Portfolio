/*  main.cpp
 *  Ian Hackman (ihackm01)
 *  10/12
 *
 *  CS 15 proj1 - MetroSim
 *
 *  main begins the program by constructing a MetroSim instance 
 * and passing the number of arguments and the char pointer to 
 * those argumments to the constructor which will read in the files 
 * and execute the command loop in MetroSim.
 *
 */

#include <iostream>
#include <fstream>
#include <string>

#include "MetroSim.h"

int main(int argc, char *argv[]) {
    MetroSim newTrainSim(argc, argv);
}
