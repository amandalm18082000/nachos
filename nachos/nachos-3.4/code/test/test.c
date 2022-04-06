#include "syscall.h"
#include "copyright.h"

int main()
{
	int number;
	PrintString("Nhap mot so: ");
	number = ReadNum();
	PrintString("\n");
	PrintString("So ban da nhap: ");
	PrintNum(number);
	PrintString("\n");
	PrintString("Du doan con so may man cua ban hom nay la: ");
	number = RandomNum();
	PrintNum(number);
	Halt();
	return 0;
}
