#include "syscall.h"
#include "copyright.h"

int main()
{
	char chr;
	PrintString("Nhap vao 1 ky tu: ");
	chr = ReadChar();
	PrintString("Ky tu cua ban: ");
	PrintChar(chr);
	PrintString("\n");
	Halt();
	return 0;
}
