/* FileSystem.cpp
 * John Lawler | JML190001 | CS4348.501
 *
 * File contains all of the main methods for menu functions.
 * Functions here mostly deal with copying files from outside the simulated
 * disk and viewing how different storage solutions are implemented. 
*/
#include "ProjectThree.h"

// add file to the file system
void FileSystem::addFile(std::string fileName, int start, int length) {
    block file = readBlock(File_Allocation);
    file.bytes += fileName + "\t" + std::to_string(start) + "\t" + std::to_string(length) + "\n";
    writeBlock(file, File_Allocation);
}

// constructor for FileSystem to initialize disk and storage type
FileSystem::FileSystem(DiskDrive& disk, std::string type){
    this->diskDrive = disk;
    this->fileSystemType = type;

    // create a blank block
    block b;
    b.bytes = std::string(MAXBLOCKS, '0');

    // initalize header with index and bitmap
    b.bytes[0] = '1';
    b.bytes[1] = '1';
    disk.writeToDisk(b, Bitmap);

    // shuffling for chained and indexed implementations
    if (type == "CHAINED" || type == "INDEXED") {
        for (int i = 2; i < MAXBLOCKS; i++) {
            freeSpace.push_back(i);
        }
        std::random_shuffle(freeSpace.begin(), freeSpace.end());
    }
}

// calculate number of digits in a number for chained pointers
int getDigits(int x){
    int digits = 0;
    while (x > 0) {
        x /= 10;
        digits++;
    }
    return digits;
}

// deleting a file from the filesystem given a file name
void FileSystem::deleteFile(std::string fileName) {

    block file = readBlock(File_Allocation);

    // no file to delete, return
    if (file.bytes == "") {
        std::cerr << "Error: No file to delete.\n";
        return;
    }

    // find pair associated with filename
    pair p = findFile(fileName);

    // pair not found, return
    if (p.beginning == -1 || p.end == -1) {
        std::cerr << "Error: File not found for deletion.\n";
        return;
    }

    // create a substring of the bytes in the file from pair
    std::string fileData = file.bytes.substr(p.beginning, p.end);
    int start, fileSize;

    // get filesize to identify blocks
    getFileAllocationData(fileData, start, fileSize);

    // identify blocks
    int blocks = getBlocks(fileSize);

    // contiguous, delete blocks in order
    if (fileSystemType == "CONTIGUOUS") {
        for (int i = start; i < start + blocks; i++) {
            deleteBlock(i);
        }
    }

    // chained
    else if (fileSystemType == "CHAINED") {
        int pointerSize = getDigits(MAXBLOCKS);
        int address = start;

        // delete blocks according to associated addresses
        while (address != 0) {
            int temp = std::stoi(readBlock(address).bytes.substr(0, pointerSize));
            deleteChainedIndexedBlock(address);
            address = temp;
        }
    }

    // indexed
    else if (fileSystemType == "INDEXED") {

        // create necessary variables for finding indexed blocks
        std::vector<int> blockIndex;
        std::string line;
        std::stringstream stream(readBlock(start).bytes);

        // find indexed blocks
        while (getline(stream, line)) {
            blockIndex.push_back(std::stoi(line));
        }

        // delete indexed blocks
        for (int i = 0; i < static_cast<int>(blockIndex.size()); i++) {
            deleteChainedIndexedBlock(blockIndex[i]);
        }

        // delete start of index
        deleteChainedIndexedBlock(start);
    }

    // could not find file
    else {
        std::cerr << "Error: Could not delete file. Type not found.\n";
        return;
    }

    // return to initial state
    file.bytes.erase(p.beginning, fileData.length());
    writeBlock(file, File_Allocation);
}

/* simple functions to write, read, and delete specified blocks */
// read block
block FileSystem::readBlock(int i) {

    // read if block is within parameters
    if (i >= 0 && i < MAXBLOCKS) {
        return diskDrive.readFromDisk(i);
    }

    // otherwise block is out of range
    else {
        std::cerr << "Error: Read index out of range.\n";
        return *(new block);
    }
}

// delete block
void FileSystem::deleteBlock(int i) {

    // create a new blank block to write to disk
    diskDrive.writeToDisk(*(new block), i);

    // change all 1 bit in bitmap to 0
    if (readBlock(i).bytes[i] != '0') {
        flipBit(i);
    }
}

// write block
void FileSystem::writeBlock(block b, int i) {

    // write to disk
    diskDrive.writeToDisk(b, i);
    
    // change byte to 1 when written
    if (readBlock(Bitmap).bytes[i] == '0') {
        flipBit(i);
    }
}

// function to change a specified bit in bitmap
void FileSystem::flipBit(int i) {
    block b = readBlock(Bitmap);
    if (b.bytes[i] == '1') {
        b.bytes[i] = '0';
    }
    else if (b.bytes[i] == '0') {
        b.bytes[i] = '1';
    }
    else {
        std::cerr << "Error: Bit in bitmap not in binary.\n";
    }

    // write changes
    diskDrive.writeToDisk(b, Bitmap);
}

// get a filesize from a filename
int FileSystem::getFileSize(const std::string& fileName) {

    // open file
    std::ifstream file(fileName, std::ifstream::in | std::ifstream::ate);

    // if file did not open, return error
    if (!file.is_open()) {
        return -1;
    }

    // get file size
    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    file.close();

    return fileSize;
}

// free pair for contiguous implementation
pair FileSystem::freeContiguousPair(int fileSize) {
    pair contiguousPair;
    contiguousPair.beginning = -1;
    contiguousPair.end = -1;

    int allocationSize = fileSize;
    int i = 2;

    // loop through blocks starting from the third block (i=2)
    for (i = 2; i < MAXBLOCKSIZE && allocationSize != 0; i++) {

        // if current block is free and no beginning of contiguous block is found yet
        if (readBlock(1).bytes[i] == '0' && contiguousPair.beginning == -1) {
            contiguousPair.beginning = i;
        }

        // if the current block is free, decrement allocationSize
        if (readBlock(1).bytes[i] == '0') {
            allocationSize--;
        }

        // else, block is not free, reset at beginning 
        else {
            contiguousPair.beginning = -1;
            allocationSize = fileSize;
        }
    }

    // set the end of contiguous block (last checked - 1)
    contiguousPair.end = i - 1;

    // if allocationSize is not fully satisfied, reset pair to initial value
    if (allocationSize != 0) {
        contiguousPair.beginning = -1;
        contiguousPair.end = -1;
    }

    return contiguousPair;
}

// simple function to receive the filesize integer
int FileSystem::getBlocks(int fileSize) {
    return ceil((double)fileSize / MAXBLOCKSIZE);
}

// find a file in the filesystem given a filename
pair FileSystem::findFile(std::string fileName) {
    pair position;
    position.beginning = -1;
    position.end = -1;

    // read the block containing file information
    std::string str;
    std::stringstream stream(readBlock(File_Allocation).bytes);

    int length = 0;

    // iterate through each line in the block
    while (getline(stream, str)) {

        // check if current line starts with specified filename
        if (str.substr(0, str.find("\t")) == fileName) {

            // set the beginning and end positions of block
            position.beginning = length;
            position.end = length + str.length() + 1; // add the newline character
            break; // stop searching
        }

        // move to new line and update length accordingly
        length += str.length() + 1; // add the newline character
    }
    
    return position;
}

// get information about allocated data
void FileSystem::getFileAllocationData(std::string fileData, int& start, int& length) {
    char *token; // initialize c style string
    token = strtok((char*)fileData.c_str(), "\t"); // extract first token using tab character as delimiter
    token = strtok(NULL, "\t"); // move to start position (next token)
    start = stoi(std::string(token)); // convert token to integer and assign start to it
    token = strtok(NULL, "\n"); // move to the next newline token
    length = stoi(std::string(token)); // convert token to integer that shows length of line
}

// retrieve a file from filesystem
std::string FileSystem::getFile(std::string fileName) {

    std::string fileData = "";
    int start, length;

    // initialize pair given a filename
    pair position = findFile(fileName);

    // check if file was found in file
    if (position.beginning != -1) {

        // extract information from metadata in file allocation block
        std::string fileMetadata = readBlock(File_Allocation).bytes.substr(position.beginning, position.end);

        // parse information
        getFileAllocationData(fileMetadata, start, length);

        // calculate blocks for file
        int blocks = getBlocks(length);

        // contiguous implementation
        if (fileSystemType == "CONTIGUOUS") {

            // for contiguous file system, concatenate blocks in sequence
            for (int i = start; i < start + blocks; i++) {
                fileData += readBlock(i).bytes;
            }
        }

        // chained implementation
        else if (fileSystemType == "CHAINED") {
            // for chained file system, follow pointers to linked blocks

            int pointerSize = getDigits(MAXBLOCKS);
            int address = start;

            // for chained file keep looking while address isnt 0
            while (address !=0) {
                block b = readBlock(address);
                fileData += b.bytes.substr(pointerSize, b.bytes.length());
                address = std::stoi(readBlock(address).bytes.substr(0, pointerSize));
            }
        }

        // indexed implementation
        else if (fileSystemType == "INDEXED") {

            // initialize indices vector
            std::vector<int> blockIndex;
            std::string line;
            std::stringstream stream(readBlock(start).bytes);

            // use block indices to retrieve data
            while (getline(stream, line)) {
                blockIndex.push_back(std::stoi(line));
            }

            for (int i = 0; i < static_cast<int>(blockIndex.size()); i++) {
                fileData += readBlock(blockIndex[i]).bytes;
            }
        }

        // should never execute because type was already checked
        else {
            std::cerr << "Error: Type was incorrect for getFile.\n";
            return fileData;
        }
    } 

    // if returned invalid value, file was not found
    else {
        std::cerr << "Error: File not found.\n";
        return fileData;
    }

    return fileData;
}

// write contents of a simulation file to the system
void FileSystem::writeRealFile(std::string fileName) {

    // retrieve the contents of the file from the file system
    std::string fileData = getFile(fileName);

    // check if file is empty
    if (fileData != "") {

        // open real file
        std::fstream file(fileName, std::ios::out);

        // write real file
        file << fileData;
    }

    // error message if the file is empty or if it does not exist
    else {
        std::cerr << "Error: File " << fileName << " is either empty or does not exist.\n";
    }
}

// write a block to randomly selected free space in file system
void FileSystem::writeRandomBlock(block b) {

    // get index of freespace from front of freespace vector
    int index = freeSpace.front();

    // remove selected index from freespace vector 
    freeSpace.erase(freeSpace.begin());

    // write block
    writeBlock(b, index);
}

// deletion for chained or indexed blocks
void FileSystem::deleteChainedIndexedBlock(int i) {

    // add index i to freespace vector
    freeSpace.push_back(i);

    // delete block at i in file system
    deleteBlock(i);
}
// get amount of freespace in freespace vector
int FileSystem::getFreeSpace() {

    // chained and indexed track this, so just return freespace vector
    if (fileSystemType == "CHAINED" || fileSystemType == "INDEXED") {
        return freeSpace.size();
    }

    int space = 0;

    // read bitmap block of the file system
    std::string bitmap = readBlock(Bitmap).bytes;

    // iterate through bitmap incrementing space for every 0
    for (int i = 0; i < static_cast<int>(bitmap.length()); i++) {
        if (bitmap[i] == '0') {
            space++;
        }
    }

    return space;
}

// write a file to the file system
int FileSystem::writeFile(std::string fromFile, std::string toFile) {

    // ensure both source and destination have been provided
    if (fromFile == "" || toFile == "") {
        std::cerr << "Cannot write file without two file names.\n";
        return -1;
    }

    // get size of source file
    int fileSize = getFileSize(fromFile.c_str());

    // calculate blocks required to store file
    int blocks = getBlocks(fileSize);

    // open the source file for reading
    std::fstream file(fromFile, std::ifstream::in);

    // check if file opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: File " << fromFile << " failed to open.\n";
        return -2;
    }

    // check if file is empty
    if (blocks <= 0) {
        std::cerr << "Error: File is empty.\n";
        return -3;
    }

    // check if file exceeds maximum block size
    if (blocks > 10) {
        std::cerr << "Error: File is too large (10 block maximum).\n";
        return -4;
    }

    // contiguous
    if (fileSystemType == "CONTIGUOUS") {
        
        // get a contiguous pair for the required block size
        pair contiguousPair = freeContiguousPair(blocks);

        // check if there is enough contiguous space
        if (contiguousPair.beginning == -1) {
            std::cerr << "Error: Not enough space available.\n";
            return -5;
        }

        int allocationSpace = fileSize;

        // iterate through the contiguous pair and write the file data
        for (int i = contiguousPair.beginning; i <= contiguousPair.end; i++) {
            int space;

            // adjust space properly
            if (allocationSpace > MAXBLOCKSIZE) {
                space = MAXBLOCKSIZE;
            }
            else {
                space = allocationSpace;
            }

            // update allocation space
            allocationSpace -= MAXBLOCKSIZE;

            // read data, add it to a block, and add that block to filesystem
            block b;
            char* cstring = new char[space];
            file.read(cstring, space);
            b.bytes = std::string(cstring);
            writeBlock(b, i);
        }

        // add file data to the file allocation block
        addFile(toFile, contiguousPair.beginning, fileSize);
    }

    // chained
    else if (fileSystemType == "CHAINED") {

        const int pointerSize = getDigits(MAXBLOCKS); // pointerSize for fileSizePointers and space calculation
        int allocationSpace = fileSize; // required space for file
        int start = freeSpace.front(); // get start block for chained file
        int fileSizePointers = fileSize + (blocks * pointerSize); // calculate total size of chained file
        int position = 0;

        // recalculate blocks required according for new fileSizePointers
        blocks = getBlocks(fileSizePointers);

        // check if there is enough space for the chained file
        if (getFreeSpace() < blocks) {
            std::cerr << "Error: Not enough space available.\n";
            return -6;
        }

        // read the file data into a string
        char* cstring = new char[fileSize];
        file.read(cstring, fileSize);
        std::string str = std::string(cstring);

        // iterate through blocks and write chained file data
        for (int i = 0; i < blocks; i++) {
            int space;

            //adjust space accordingly
            if (allocationSpace > (MAXBLOCKSIZE - pointerSize)) {
                space = (MAXBLOCKSIZE - pointerSize);
            }
            else {
                space = allocationSpace;
            }

            // update allocation space
            allocationSpace -= (MAXBLOCKSIZE - pointerSize);

            block b;
            std::stringstream stream;

            // for the last block, add an empty pointer to indicate end of chain
            if (i == blocks - 1) {
                stream << std::setfill('0') << std::setw(pointerSize) << "";
            }

            // for all other blocks, add a pointer to the next free block
            else {
                stream << std::setfill('0') << std::setw(pointerSize) << freeSpace.at(1);            
            }

            // copy data to block and randomly write
            b.bytes = stream.str();
            b.bytes += str.substr(position, space);
            position += space;

            writeRandomBlock(b);
        }

        // add file data to the file allocation block
        addFile(toFile, start, fileSizePointers);
    }

    // indexed
    else if (fileSystemType == "INDEXED") {

        int fileSizeIndex = fileSize + MAXBLOCKSIZE; // calculate total size of indexed file
        int allocationSpace = fileSize; // required space for file

        // vector to store block indices
        std::vector<int> blockIndex;

        // iterate through blocks and write indexed file data
        for (int i = 0; i < blocks; i++) {
            int space;

            // adjust space accordingly
            if (allocationSpace > MAXBLOCKSIZE) {
                space = MAXBLOCKSIZE;
            }

            else {
                space = allocationSpace;
            }

            // update allocation space
            allocationSpace -= MAXBLOCKSIZE;

            // create a block and read data from the file into it
            block b;
            char* cstring = new char[space];
            file.read(cstring, space);
            b.bytes = std::string(cstring);

            // add the index of block to vector for tracking and mark freespace
            blockIndex.push_back(freeSpace.front());

            // write the block to randomly selected free space
            writeRandomBlock(b);
        }

        block indexFile;

        // populate index  file block with indices
        for (int i = 0; i < static_cast<int>(blockIndex.size()); i++) {
            indexFile.bytes += std::to_string(blockIndex[i]) + "\n";
        }

        // get the start block
        int start = freeSpace.front();

        // write index file to random free space
        writeRandomBlock(indexFile);

        // add file data to the file allocation block
        addFile(toFile, start, fileSizeIndex);
    }

    // close the file
    file.close();

    // return 0, successful
    return 0;
}