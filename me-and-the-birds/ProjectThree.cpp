/* ProjectThree.cpp
 * John Lawler | JML190001 | CS4348.501
 *
 * Main file for creating a menu and controlling flow of file system.
 * Program creates a virtual file system using contiguous, indexed or chained
 * algorithms (you must choose one as an argument). 
*/
#include "ProjectThree.h"

int main(int argc, char** argv) {

    // usage error
    if (argc != 2) {
        std::cerr << "Usage: ./ProjectThree [CONTIGUOUS|INDEXED|CHAINED]\n"
            << "Please be sure to only use one option without including '[' or ']'.\n";
        return -1;
    }

    // check if argv[1] is valid
    int check = checkValid(argv[1]);
    if (check < 1 || check > 3) {
        std::cerr << "Error: Invalid Argument.\n" 
            << "Usage: ./ProjectThree [CONTIGUOUS|INDEXED|CHAINED]\n"
            << "Please be sure to only use one option without including '[' or ']'.\n";  
        return -2;
    }

    // else it was valid, print args back out to terminal
    else {
        std::cout << argv[0] << ": " << outputType(check) << std::endl;
    }

    // initialize the fileSystem
    DiskDrive diskDrive;
    FileSystem fileSystem(diskDrive, outputType(check));

    printHeader();

    // create menu loop
    int menuOption = 0;
    while (menuOption != 8) {

        // print menu, get prompt from user
        printMenu();
        menuOption = getPrompt();

        // handle menu options
        switch(menuOption) {
            // 1) Display a file
            case 1: {
                printHeader();
                std::cout << "Enter file name: ";
                std::string fileName = getString();
                std::cout << "Displaying file: " << fileName << std::endl;
                std::cout << fileSystem.getFile(fileName) << std::endl;
                printHeader();
                break;
            }
                
            
            // 2) Display the file table
            case 2: {
                printHeader();
                std::cout << "Displaying file table: \n";
                std::cout << fileSystem.readBlock(File_Allocation).bytes << std::endl;
                printHeader();
                break;
            }
                
            // 3) Display the free space bitmap
            case 3: {
                printHeader();
                std::cout << "Displaying free space bitmap: \n";
                block b = fileSystem.readBlock(Bitmap);
                for (int i = 0; i < MAXBLOCKS / 32; i++) {
                    for (int j = 0; j < 32; j++) {
                        std::cout << b.bytes[j + (i * 32)];
                    }
                    std::cout << std::endl;
                }
                printHeader();
                break;
            }
                
            // 4) Display a disk block
            case 4: {
                printHeader();
                std::cout << "Enter block index: ";
                int index;
                std::cin >> index;

                std::cout << "Displaying block index: \n";

                std::cout << fileSystem.readBlock(index).bytes << std::endl;
                printHeader();
                break;
            }
                

            // 5) Copy a file from the simulation to a file on the real system
            case 5: {
                printHeader();
                std::cout << "Enter a file to save from the simualtion: ";
                std::string fileName;
                std::getline(std::cin, fileName);
                
                if (fileName == "\n" || fileName == "" || fileName.length() > 8) {
                    std::cerr << "Error: Please enter a valid file name.\n";
                    printHeader();
                    break;
                }

                else {
                    fileSystem.writeRealFile(fileName);
                    std::cout << fileName << " was saved successfully.\n";
                    printHeader();
                }

                break;
            }
                

            // 6) Copy a file from the real system to a file in the simulation
            case 6: {
                std::string toFile;
                std::string fromFile;
                std::string buffer;
                printHeader();
                std::cout << "Copy from: ";
                std::getline(std::cin, buffer);
                fromFile = buffer; 
                if (fromFile == "\n" || fromFile == "") {
                    std::cerr << "Error: Please enter a valid file name.\n";
                    break;
                }

                std::cout << "Copy to: ";
                std::getline(std::cin, buffer);
                toFile = buffer;
                if (toFile == "\n" || toFile == "" || toFile.length() > 8) {
                    std::cerr << "Error: Please enter a valid file name to be copied.\n";
                    std::cerr << "A file name to be copied should be 8 or less characters, and it should not have a file extension.\n";
                    break;
                }

                std::cout << "Copying file to simulation.\n";
                if (fileSystem.writeFile(fromFile, toFile) == 0) {
                    std::cout << "Copied file from " << fromFile << " to " << toFile << " successfully.\n";
                }
                printHeader();
                break;
            }

            // 7) Delete a file
            case 7: {
                printHeader();
                std::cout << "Enter file to remove: ";
                std::string fileName;
                std::getline(std::cin, fileName);

                if (fileName == "\n" || fileName == "" || fileName.length() > 8) {
                    std::cerr << "Error: Please enter a valid file name.\n";
                    printHeader();
                    break;
                }

                else {
                    std::cout << "Deleting " << fileName << ".\n";
                    fileSystem.deleteFile(fileName);
                    std::cout << fileName << " deleted successfully.\n";
                    printHeader();
                }

                break;
            }
                
            // 8) Exit
            case 8:
                break;

            // default invalid input
            default: {
                std::cerr << "Error: Invalid menu option.\n";
                printHeader();
            }
        }
    }

    return 0;
}

// validate type and change that type into a number
int checkValid(std::string argv) {
    for (int i = 0; i < static_cast<int>(argv.length()); i++) {
        argv[i] = std::toupper(argv[i]);
    }

    if (argv == "CONTIGUOUS") { return 1; }
    else if (argv == "CHAINED") { return 2; }
    else if (argv == "INDEXED") { return 3; }
    else { return -1; }
}

// change type from number into text for checking
std::string outputType(int type) {
    if (type == 1) { return "CONTIGUOUS"; }
    else if (type == 2) { return "CHAINED"; }
    else if (type == 3) { return "INDEXED"; }
    else { return "Error: outputType had an invalid argument\n."; }
}

// print a header to separate text
void printHeader() {
    for (int i = 0; i < 64; i++) {
        std::cout << "=";
    }
    std::cout << std::endl;
}

// print out menu from spec oin given document
void printMenu() {
    std::cout << "1) Display a file\n"
        << "2) Display the file table\n"
        << "3) Display the free space bitmap\n"
        << "4) Display a disk block\n"
        << "5) Copy a file from the simulation to a file on the real system\n"
        << "6) Copy a file from the real system to a file in the simulation\n"
        << "7) Delete a file\n"
        << "8) Exit\n";
}

// prompt user from menu options and mark errors
int getPrompt() {
    // get prompt from user
    std::cout << "Option: ";
    std::string str = getString();

    if (str.length() != 1) { return -1; } // all options are length == 1, error if incorrect
    else if (std::isdigit(str[0]) == false) { return -2; } // return error if not a digit
    else { return std::stoi(str); } // errors checked, return digit
}

// helper function to receive input
std::string getString() {
    std::string str;
    std::getline(std::cin, str);
    return str;
}