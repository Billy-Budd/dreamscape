/* ProjectThree.h
 * John Lawler | JML190001 | CS4348.501
 *
 * Header file for the main file to create prototypes and necessary structures
 * for proper execution and initialization of the simulated file system and disk.
*/
#ifndef PROJECTTHREE_H
#define PROJECTTHREE_H

// required libraries
#include <iostream>
#include <fstream> 
#include <sstream>
#include <iomanip>
#include <math.h>
#include <random>
#include <vector>
#include <algorithm>
#include <string>
#include <string.h>
#include <cstdlib>

// given maxes from design document
#define MAXBLOCKSIZE 512
#define MAXBLOCKS 256

// main function prototypes
int checkValid(std::string argv);
std::string outputType(int type);
void printHeader();
void printMenu();
int getPrompt();
std::string getString();

// pair structure for file size tracking
struct pair {
    int beginning, end;
    pair() {
        beginning = -1;
        end = -1;
    }
};

// block structure for saving data to blocks of bytes
struct block {
    std::string bytes;
    block() {
        bytes = "";
    }
};

// include other two headers for constructors
#include "DiskDrive.h"
#include "FileSystem.h"

enum Resource {File_Allocation, Bitmap};

#endif //PROJECTTHREE_H