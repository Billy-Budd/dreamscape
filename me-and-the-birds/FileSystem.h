/* FileSystem.h
 * John Lawler | JML190001 | CS4348.501
 *
 * Header file that contains the FileSystem. 
 * For more information about each function, please find their respective function in FileSystem.cpp
*/

class FileSystem {
    private:
        DiskDrive diskDrive;
        std::string fileSystemType;
        std::vector<int> freeSpace;

        void addFile(std::string, int, int);
        void deleteBlock(int);
        void flipBit(int);
        int getFileSize(const std::string&);
        pair freeContiguousPair(int);
        int getBlocks(int);
        pair findFile(std::string);
        void getFileAllocationData(std::string, int&, int&);
        void writeRandomBlock(block);
        void deleteChainedIndexedBlock(int);
        int getFreeSpace();

    public:
        FileSystem(DiskDrive&, std::string);
        void deleteFile(std::string);
        block readBlock(int);
        void writeBlock(block, int);
        std::string getFile(std::string);
        void writeRealFile(std::string);
        int writeFile(std::string, std::string);
        
};