// FAT32 Dirty Bit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Print the usage instructions for the program
void PrintUsage(char* exeName);

int main(int argc, char* argv[])
{
    // Validate parameters
    if (argc != 3)
    {
        PrintUsage(argv[0]);
        return (argc != 1);
    }

    // Get set/clear
    const char setClear(toupper(argv[1][0]));
    printf("Got char %c\n", setClear);
    bool setBit(false);
    switch (setClear)
    {
    case 'S':
    case '1':
        setBit = true;
        break;

    case 'C':
    case '0':
        setBit = false;
        break;

    default:
        PrintUsage(argv[0]);
        return 1;
    }

    // Get drive letter
    const char driveLetter(toupper(argv[2][0]));
    if ((driveLetter < 'A') || (driveLetter > 'Z') || (argv[2][1] != ':'))
    {
        PrintUsage(argv[0]);
        return 1;
    }

    // The MS-DOS logical drive number. 0 = default, 1 = A, 2 = B, 3 = C, etc.
    const int driveId(driveLetter - 'A' + 1);

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
    char buffer[SECTOR_SIZE] = { 0 };
    BOOL success = NewReadSectors(
        hDevice,
        driveId,
        0, // Start sector
        1, // Sectors to read
        (LPBYTE)buffer);

    // Returns TRUE on success
    if (success == FALSE)
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
    for (int i(0); i < SECTOR_SIZE; ++i)
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
    printf("\n");

    // Validate FAT 32 Volume ID (sanity check)
    if (ValidFat32(buffer))
    {
        // DEBUG
        printf("Valid FAT32 Volume ID!\n");
    }
    else
    {
        // DEBUG
        printf("Not valid!\n");
    }

    // Done with raw disk I/O
    CloseHandle(hDevice);

    return 0;
}

// Print the usage instructions for the program
void PrintUsage(char* exeName)
{
    printf("Sets or clears the FAT32 dirty bit on a volume.\n");
    printf("\n");
    printf("%s set|clear drive:\n", exeName);
    printf("\n");
    printf("  set|clear     set or clear the FAT32 dirty bit\n");
    printf("  drive:        drive letter (for example d:)\n");
    printf("\n");
}
