#include "syscall.h"
#include "copyright.h"

int main()
{
	
	OpenFileId id;
	char buffer[255];
	int read,write;
	id = Open("hi.txt");
	
	
	//PrintString("\n1\n");
	Write("zzzzzzzzsadasdasdasdzzzzz",255,id);
	
	read= Read(buffer,255,id);
	PrintString(buffer);
	//PrintString("\n2\n");
	Halt();
	return 0;
}
	

