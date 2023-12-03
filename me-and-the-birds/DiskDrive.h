/* DiskDrive.h
 * John Lawler | JML190001 | CS4348.501
 *
 * Disk class to read and write data.
 * Uses block structure from ProjectThree.h to create sections of stored data. 
*/

class DiskDrive {
    private:
        block *data;
        int allocationMethod;

    public:
        DiskDrive();
        block readFromDisk(int);
        void writeToDisk(block, int);
};