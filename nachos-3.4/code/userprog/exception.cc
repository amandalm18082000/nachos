// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2 // Chua ma lenh syscall va ket qua thuc hien o syscall se tra nguoc ve
//		arg1 -- r4 // Tham so thu 1
//		arg2 -- r5 // Tham so thu 2
//		arg3 -- r6 // Tham so thu 3
//		arg4 -- r7 // Tham so thu 4
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void IncreasePC()
{
	int counter = machine->ReadRegister(PCReg);		// read the contents of current PCregister and assign into counter variable
	machine->WriteRegister(PrevPCReg, counter);		// store counter variable into previous PCregister
	counter = machine->ReadRegister(NextPCReg);		// read the contents of next PCregister and assign into counter variable
	machine->WriteRegister(PCReg, counter);			// store counter variable into current PCregister
	machine->WriteRegister(NextPCReg, counter + 4); // store counter + 4 variable into next PCregister
	return;
}

char *User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char *kernelBuf = NULL;

	kernelBuf = new char[limit + 1]; // need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);

	// printf("\n Filename u2s:");
	for (i = 0; i < limit; i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		// printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

void ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
		{
			DEBUG('a', "Shutdown, initiated by user program.\n"); // utility.cc
			printf("\nShutdown, initiated by user program.");
			interrupt->Halt();
			break;
		}
		case SC_CreateFile:
		{
			int virtAddr;
			char *filename;
			DEBUG('a', "\n SC_Create call ...");
			// DEBUG(dbgFile, "\n Reading virtual address of filename");

			// check for exception
			virtAddr = machine->ReadRegister(4);
			// DEBUG(dbgFile, "\n Reading filename.");
			filename = User2System(virtAddr, 32 + 1); // MaxFileLength là = 32
			if (filename == NULL)
			{
				printf("\n Not enough memory in system");
				// DEBUG(dbgFile, "\n Not enough memory in system");
				machine->WriteRegister(2, -1); // trả về lỗi cho chương trình người dùng
				delete filename;
				return;
			}

			// DEBUG(dbgFile, "\n Finish reading filename.");
			// DEBUG(dbgFile,"\n File name : '"<<filename<<"'");
			// Create file with size = 0
			// Dùng đối tượng fileSystem của lớp OpenFile để tạo file, việc tạo file này là sử dụng các
			// thủ tục tạo file của hệ điều hành Linux, chúng ta không quản ly trực tiếp các block trên
			// đĩa cứng cấp phát cho file, việc quản ly các block của file trên ổ đĩa là một đồ án khác
			if (!fileSystem->Create(filename, 0))
			{
				printf("\n Error create file '%s'", filename);
				machine->WriteRegister(2, -1);
				delete filename;
				return;
			}
			machine->WriteRegister(2, 0); // trả về cho chương trình người dùng thành công
			delete filename;
			return;
		}
		case SC_Open:
		{
			int address = machine->ReadRegister(4);
			int pos = 0;
			char name[10];
			while (1)
			{
				int data;
				machine->ReadMem(address + pos, 1, &data);
				name[pos++] = (char)data;
				if (!data)
				{
					break;
				}
			}
			OpenFile *openfile = fileSystem->Open(name);
			machine->WriteRegister(2, int(openfile));
			IncreasePC();
			return;
		}

		case SC_CloseFile:
		{
			int id = machine->ReadRegister(4);
			OpenFile* openFile = (OpenFile*)id;
			openFile = NULL;

			if (openFile == 0)
			{
				machine->WriteRegister(2, 0);
			}
			else
			{
				machine->WriteRegister(2, -1);
			}
			delete openFile;
			IncreasePC();
			return;

		}

		
		case SC_Seek:
		{
			int pos = machine->ReadRegister(4);
			int id = machine->ReadRegister(5);
			DEBUG('S', "Received Seek syscall (r4 = %d, r5 = %d): ", pos, id);

			OpenFile* openFile = (OpenFile*)id;
			if (id == 0 || id == 1)
			{
				printf("\nKhong the seek tren file console.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			pos = (pos == -1) ? openFile->Length() : pos;
			if (pos > openFile->Length() || pos < 0)
			{
				printf("\nError: Out of range position!.");
				machine->WriteRegister(2, -1);
			}
			else
			{
				openFile->Seek(pos);
				machine->WriteRegister(2, pos);
			}
			IncreasePC();
			return;
		}

		case SC_Read:
		{
			int virtAddr = machine->ReadRegister(4);
			int size = machine->ReadRegister(5);
			int fileID = machine->ReadRegister(6);
			int prePos, nextPos,realSize;
			char *buf;
			//if (fileID < 0 || fileID > 9)
			//{
			//	DEBUG('a', "Fail to read file.\nFileID is out of openf table.\n");
			//	printf("Fail to read file.\nFileID is out of openf table.\n");
			//	machine->WriteRegister(2, -1);
			//	IncreasePC();
			//	return;
			//}
			OpenFile *openFile = (OpenFile *)fileID;
			//if (openFile == NULL)
			//{
			//	DEBUG('a', "Fail to read file.\nFile has not been opened yet.\n");
			//	printf("Fail to read file.\nFile has not been opened yet.\n");
			//	machine->WriteRegister(2, -1);
			////	IncreasePC();
			//	return;
			//}
			//if (openFile->type == 3)
			//{
			//	DEBUG('a', "Fail to read file.\nStdout is not readable.\n");
			//	printf("Fail to read file.\nStdout is not readable.\n");
			//	machine->WriteRegister(2, -1);
			//	IncreasePC();
			//	return;
			//}
			prePos = openFile->GetCurrentPos();
			buf = User2System(virtAddr, size);
			memset(buf, 0, sizeof buf);
			if (openFile->type == 2)
			{
				int realSize = gSynchConsole->Read(buf, size);
				System2User(virtAddr, realSize, buf);
				machine->WriteRegister(2, realSize);
				delete buf;
				IncreasePC();
				return;
			}
			else
			{
				realSize = openFile->Read(buf, size);
				System2User(virtAddr, realSize, buf);
				machine->WriteRegister(2, realSize);
				delete buf;
				IncreasePC();
				return;
			}
			
		 
		}
		case SC_Write:
		{
			int bufAddr = machine->ReadRegister(4);
			int numBuffer = machine->ReadRegister(5);
			int mIndex =  machine->ReadRegister(6);
			int prevPos, nextPos;
			char *buf ;
			OpenFile *openFile = (OpenFile *)mIndex;
					// Check mIndex
			//if (mIndex < 0 || mIndex > 14)
			//{
			//	printf("\nKhong the write vi id nam ngoai bang mo ta file.");
			//	machine->WriteRegister(2, -1);
			//	IncreasePC();
			//	return ;
			//}
			
			
			
			if (openFile == NULL)
			{
				printf("stdout mode 1 \n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return ;
				
			}
			if (openFile->type == 1 || openFile->type == 2)
			{
				printf("\nKhong the write file stdin hoac file only read.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return ;
			}
			prevPos = openFile->GetCurrentPos();
			buf = User2System(bufAddr, numBuffer); 
			if (openFile->type == 0)
			{
				if ((openFile->Write(buf, numBuffer)) > 0)
				{
					// So byte thuc su = NewPos - OldPos
					nextPos = openFile->GetCurrentPos();
					machine->WriteRegister(2, nextPos - prevPos);
					delete buf;
					
				}
			}
			printf(buf);
			if (openFile->type == 3) // Xet truong hop con lai ghi file stdout (type quy uoc la 3)
			{
				int i = 0;
				while (buf[i] != 0 && buf[i] != '\n') // Vong lap de write den khi gap ky tu '\n'
				{
					gSynchConsole->Write(buf + i, 1); // Su dung ham Write cua lop SynchConsole 
					i++;
				}
				buf[i] = '\n';
				gSynchConsole->Write(buf + i, 1); // Write ky tu '\n'
				machine->WriteRegister(2, i - 1); // Tra ve so byte thuc su write duoc
				delete buf;
				
			}
			IncreasePC();
			return;
		}
		case SC_Remove:
		{
			int virAddress = machine->ReadRegister(4);
			char * fileName=User2System(virAddress,32+1);
			machine->WriteRegister(2,fileSystem->Remove(fileName));
			delete[] fileName;
			IncreasePC();
			break;
			return ;
				
		}
			// Read Num
		case SC_ReadNum:
		{
			DEBUG('a', "Read integer in console.\n");
			int MAX_BUFFER = 255;
			char *buffer;
			buffer = new char[MAX_BUFFER + 1];

			int total_bytes = gSynchConsole->Read(buffer, MAX_BUFFER); // Length of buffer
			int number = 0;
			int start_index = 0;
			int end_index = 0;

			// Check number is negative or positive
			bool IsNegative = false;
			if (buffer[0] == '-')
			{
				IsNegative = true;
				start_index = 1;
			}

			for (int i = start_index; i < total_bytes; i++)
			{
				if (buffer[i] == '.')
				{
					int j = i + 1;
					for (j; j < total_bytes; j++)
					{
						if (buffer[j] != '0')
						{
							printf("\nThe number is NOT VALID !");
							DEBUG('a', "\nThe integer number is not valid");
							machine->WriteRegister(2, 0);
							IncreasePC();
							delete buffer;
							return;
						}
					}
					end_index = i - 1;
					break;
				}
				else if (buffer[i] < '0' && buffer[i] > '9')
				{
					printf("\nThe number is NOT VALID !");
					DEBUG('a', "\nThe integer number is not valid");
					machine->WriteRegister(2, 0);
					IncreasePC();
					delete buffer;
					return;
				}

				end_index = i;
			}

			// Convert string to number
			for (int i = start_index; i <= end_index; i++)
			{
				number = number * 10 + (int)(buffer[i] - 48);
			}

			if (IsNegative)
			{
				number *= -1;
			}

			machine->WriteRegister(2, number);
			IncreasePC();
			delete buffer;
			return;
		}

			// Print Int
		case SC_PrintNum:
		{
			int number = machine->ReadRegister(4);

			if (number == 0)
			{
				gSynchConsole->Write("0", 1);
				IncreasePC();
				return;
			}

			// Check number is negative or positive
			bool IsNegative = false;
			int n = 0;
			int start_index = 0;

			if (number < 0)
			{
				IsNegative = true;
				number *= -1;
				start_index = 1;
			}

			// Find the length of number
			int temp = number;

			while (temp != 0)
			{
				n++;
				temp /= 10;
			}

			// Make a string to print number
			char *buffer;
			int MAX_BUFFER = 255;
			buffer = new char[MAX_BUFFER];

			for (int i = start_index + n - 1; i >= start_index; i--)
			{
				buffer[i] = (char)((number % 10) + 48);
				number /= 10;
			}

			// Add sign (positive (+) or negative(-))

			if (IsNegative)
			{
				buffer[0] = '-';
				buffer[n + 1] = '\0';
				gSynchConsole->Write(buffer, n + 1);
				delete buffer;
				IncreasePC();
				return;
			}

			buffer[n] = '\0';
			gSynchConsole->Write(buffer, n);
			delete buffer;
			IncreasePC();
			return;
		}

		case SC_RandomNum:
		{
			DEBUG('a', "Random an integer in console.\n");
			RandomInit(0);
			int random = Random() % 100;
			machine->WriteRegister(2, random);
			IncreasePC();
			return;
		}
			// Read Char
		case SC_ReadChar:
		{
			// int maxBytes = 255;
			char *buffer = new char[255];
			int numBytes = gSynchConsole->Read(buffer, 255);

			if (numBytes > 1) // Neu nhap nhieu hon 1 ky tu thi khong hop le
			{
				printf("Chi duoc nhap duy nhat 1 ky tu!");
				DEBUG('a', "\nERROR: Chi duoc nhap duy nhat 1 ky tu!");
				machine->WriteRegister(2, 0);
			}
			else if (numBytes == 0) // Ky tu rong
			{
				printf("Ky tu rong!");
				DEBUG('a', "\nERROR: Ky tu rong!");
				machine->WriteRegister(2, 0);
			}
			else
			{
				// Chuoi vua lay co dung 1 ky tu, lay ky tu o index = 0, return vao thanh ghi R2
				printf("Thanh cong: ");
				char c = buffer[0];
				machine->WriteRegister(2, c);
			}

			delete buffer;
			IncreasePC();
			return;
		}

			// Print Char
		case SC_PrintChar:
		{
			char c = (char)machine->ReadRegister(4); // Doc ki tu tu thanh ghi r4
			gSynchConsole->Write(&c, 1);			 // In ky tu tu bien c, 1 byte
			IncreasePC();
			return;
		}

			// Read String
		case SC_ReadString:
		{
			int virtualAddr, length;
			char *buffer;
			virtualAddr = machine->ReadRegister(4);
			length = machine->ReadRegister(5);
			buffer = User2System(virtualAddr, length);
			gSynchConsole->Read(buffer, length);
			System2User(virtualAddr, length, buffer);
			delete buffer;
			IncreasePC();
			return;
		}

			// Print String
		case SC_PrintString:
		{
			int virtualAddr;
			char *buffer;
			virtualAddr = machine->ReadRegister(4);
			buffer = User2System(virtualAddr, 255);
			int length = 0;
			while (buffer[length] != 0)
				length++;
			gSynchConsole->Write(buffer, length + 1);
			delete buffer;
			IncreasePC();
			return;
		}
		}
		break;

	// Tra quyen dieu khien ve cho he dieu hanh
	case NoException:
		return;

	// Hien thi ra thong bao loi va Halt he thong
	case PageFaultException:
		DEBUG('a', "\nPageFault Error - No valid translation found !");
		printf("PageFault Error - No valid translation found !");
		ASSERT(FALSE);
		interrupt->Halt();
		break;
	case ReadOnlyException:
		DEBUG('a', "\nReadOnly Error - Write attempted to page marked read-only !");
		printf("ReadOnly Error - Write attempted to page marked read-only !");
		ASSERT(FALSE);
		interrupt->Halt();
		break;
	case BusErrorException:
		DEBUG('a', "\nBus Error - Translation resulted in an invalid physical address !");
		printf("Bus Error - Translation resulted in an invalid physical address !");
		ASSERT(FALSE);
		interrupt->Halt();
		break;
	case AddressErrorException:
		DEBUG('a', "\nAddress Error - Unaligned reference or one that was beyond the end of the address space");
		printf("Address Error - Unaligned reference or one that was beyond the end of the address space");
		ASSERT(FALSE);
		interrupt->Halt();
		break;
	case OverflowException:
		DEBUG('a', "\nOverflow Error - Integer overflow in add or sub !");
		printf("Overflow Error - Integer overflow in add or sub !");
		ASSERT(FALSE);
		interrupt->Halt();
		break;
	case IllegalInstrException:
		DEBUG('a', "\nIllegal In Str Error - Unimplemented or reserved instr !");
		printf("Illegal In Str Error - Unimplemented or reserved instr !");
		ASSERT(FALSE);
		interrupt->Halt();
		break;
	case NumExceptionTypes:
		DEBUG('a', "\nNum Types Error ! ");
		printf("Num Types Error ! ");
		ASSERT(FALSE);
		interrupt->Halt();
		break;
	default:
		printf("Unexpected user mode exception %d %d\n", which, type);
		ASSERT(FALSE);
	}
}

