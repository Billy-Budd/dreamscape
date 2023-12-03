/* DiskDrive.h
 * John Lawler | JML190001 | CS4348.501
 *
 * Constructor for the DiskDrive object as well as the functions
 * to read and write data to that object.
*/

#include "ProjectThree.h"

// intialize the diskdrive
DiskDrive::DiskDrive() {
    data = new block[MAXBLOCKS];
}

// simple method for reading from disk array from a given location
block DiskDrive::readFromDisk(int location) { return data[location]; }

// write to disk whether or not it meets space requirements, but print error
void DiskDrive::writeToDisk(block newData, int location) {
    
    // deprecated, this is checked elsewhere
    /*if (newData.bytes.length() > MAXBLOCKSIZE) {
        std::cerr << "Error: data block is too big to write to disk.\n";
    }*/

    data[location] = newData;   
}