#include "syscall.h"

int main()
{
	char fileName[256];
	char ch[256];
	int fileSize;
	int tmp;
	OpenFileId id;
	PrintString("Content in File");
	PrintString("Input filename: ");

	ReadString(fileName, 256);

	 id= Open(fileName);
	
	 if ( id!=-1 )
	 {
		
	 	fileSize = Seek(-1, id);
	 	tmp=Seek(0, id);
	 	tmp=Read(ch, fileSize, id);
	 	PrintString(ch)	;
	 	CloseFile(id);
	 }
	else
	 {
	 	PrintString("Open file unsuccessfully");
	}
	Halt();
	return 0;
}