#include "syscall.h"
#include "copyright.h"

int main()
{
    char fileName[100];
    int length, id, temp;
    int len=50;
   
    PrintString("Enter file's name: ");
    ReadString(fileName, 100);
	
    id = Open(fileName);
    if (id != -1)
    {
       CloseFile(id);
       temp=Remove(fileName);
       PrintString("File delete successful\n");
    }
    else
        PrintString("Failed to delete file\n");
    Halt();
}
	
