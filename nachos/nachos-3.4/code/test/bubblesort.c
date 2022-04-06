#include "syscall.h"
#include "copyright.h"

int main()
{
	int MAX_ARRAY = 101;
	int A[MAX_ARRAY];
	int n = 0;
	int temp = 0;
	int i = 0;
	int j = 0;
	
	PrintString("\n");

	// Length of Array
	while (n <= 0 || n > 100) 
	{
		PrintString("Nhap kich thuoc cua mang: ");
		n = ReadNum();
	}
	
	// Input Array
	PrintString("Nhap mang: \n");
	for (i = 0; i < n; i++) 
	{
		PrintString("Nhap so thu: ");
		PrintNum(i);
		PrintString("\t");
		A[i] = ReadNum();
	}
	
	PrintString("Mang ban da nhap la: \n");
	for (i = 0; i < n; i++)
		{
			PrintNum(A[i]);
			PrintString("\t");
		}
	// Bubble Sort
	
	for (i = 1; i < n; i++) 
	{
		for (j = n - 1; j >= i; j--) 
		{
			if (A[j] < A[j - 1]) 
			{
				temp = A[j];
				A[j] = A[j - 1];
				A[j - 1] = temp;
			}
		}
	}
	
	// Output
	PrintString("\n");
	PrintString("Mang sau khi sap xep lai: \n");

	for (i = 0; i < n; i++) 
	{
		PrintNum(A[i]);
		PrintString("\t");
	}	
	
	PrintString("\n");
	Halt();

	return 0;
}
