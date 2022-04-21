#include "syscall.h"

int main()
{
    int check;
    char filename[256];
    PrintString("Nhap ten file: ");
    ReadString(filename, 256);
    check = CreateFile(filename);
    if (check == 0)
       PrintString("Tao file thanh cong.\n");
    else
       PrintString("Khong tao duoc file.\n");
    Halt();
    return 0;
}

