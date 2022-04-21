#include "syscall.h"
#include "copyright.h"

int main()
{
	char str[100];
	PrintString("Nhap 1 string:  ");
	ReadString(str, 100);
	PrintString("\n");
	PrintString("Ban da nhap: ");
	PrintString(str);
	PrintString("\n");
	Halt();
	return 0;
}
