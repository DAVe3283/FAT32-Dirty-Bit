// FAT32 Dirty Bit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main(int argc, char* argv[])
{
	// Get handle to VWIN32
	HANDLE hDevice = CreateFile(
		"\\\\.\\vwin32",
		0,
		0,
		NULL,
		0,
		FILE_FLAG_DELETE_ON_CLOSE,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Couldn't get handle to VWIN32!\nProgram only supports Windows 98 at the moment.\n");
		return 1;
	}

	// Read a sector
	char buffer[512] = { 0 };
	BOOL result = NewReadSectors(hDevice, 3, 0, 1, (LPBYTE)buffer);

	// Error?
	if (result == FALSE)
	{
		DWORD errNo = GetLastError();
		printf("Unable to read raw sectors! Error code: 0x%08x\n", errNo);
		CloseHandle(hDevice);
		return 1;
	}

	// Display the sector (for debug)
	printf("Got sector:\n\n");
	printf("      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    printf("    /------------------------------------------------\n");
	for (int i(0); i < (sizeof(buffer) / sizeof(*buffer)); ++i)
	{
		// New line header
		if (!(i % 0x10))
		{
			printf("%03x | ", i);
		}

		// Hex value of buffer
		printf("%02x", buffer[i] & 0xff);

		// End of line?
		if ((i % 0x10) == 0xF)
		{
			printf("\n");
		}
		else
		{
			printf(" ");
		}
	}
	printf("\nDone!\n");

	// Done with raw disk I/O
	CloseHandle(hDevice);

	return 0;
}

