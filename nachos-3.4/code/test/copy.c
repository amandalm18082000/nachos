#include "syscall.h"

int main()
{
    char fileName_1[50], fileName_2[50], dataFileCopy[255];
    int fileSize,  read, write, setZero = 0, i;
	OpenFileId openIDFile_1, openIDFile_2;
    int choice;

    // Input file name 1
    PrintString("Enter Source file's name: ");
    ReadString(fileName_1,50);

    // Open file 1, file to get data to copy
    openIDFile_1 = Open(fileName_1);
    if (openIDFile_1 == -1) // open file 1 fail because file name invalid
    {
        PrintString("File name 1 is invalid. End Processing!!\n");
    }
    else
    {
        // Input file name 2
        PrintString("Enter Destination file's name: ");
        ReadString(fileName_2,50);

        // Open file 2, file receives data from file 1
        openIDFile_2 = Open(fileName_2);
        if (openIDFile_2 == -1) // open file 2 fail because file name invalid
        {
            PrintString("File name 2 is invalid. End processing!!\n");
        }
        else // open 2 file successfully
        {
            fileSize = Seek(-1, openIDFile_1); // Get file size
            // PrintNum(fileSize);
            setZero = Seek(0, openIDFile_1); // Seek the pointer to file head to read
                // Read each character in file and print
	   for (i = 0; i < fileSize; i++)
	    {
	      read = Read(dataFileCopy, 1, openIDFile_1);
	      // PrintStr(character);
	       Write(dataFileCopy, 1, openIDFile_2);
	    }
        }
    }

    CloseFile(openIDFile_1);
    CloseFile(openIDFile_2);

    Halt();
    return 0;
}