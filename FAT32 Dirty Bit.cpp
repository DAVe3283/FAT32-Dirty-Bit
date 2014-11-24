// FAT32 Dirty Bit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Print the usage instructions for the program
void PrintUsage(char *exeName);

int main(int argc, char *argv[])
{
    // Validate parameters
    if (argc != 3)
    {
        PrintUsage(argv[0]);
        return (argc != 1);
    }

    // Get set/clear
    const char setClear(toupper(argv[1][0]));

    bool setDirty(false);
    switch (setClear)
    {
    case 'S':
    case '1':
        setDirty = true;
        break;

    case 'C':
    case '0':
        setDirty = false;
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

    Fat32Util util;
    if (!util.init(driveLetter))
    {
        printf("Could not get volume handle! Aborting...\n");
        return 1;
    }

    // Read a sector
    Fat32Util::Fat32Header sector = util.readSector(0);
    // Validate FAT 32 Volume ID (sanity check)
    if(!util.validate())
    {
        printf("Invalid sector read! Aborting...\n");
        return 1;
    }

    // Change the bit in our copy of the sector
    sector.bpbExt.dirtyBit = setDirty ? 0x01 : 0x00;

    if (!util.writeSector(0, sector))
    {
        printf("Failed writing sector! Aborting...\n");
        return 1;
    }

    return 0;
}

// Print the usage instructions for the program
void PrintUsage(char *exeName)
{
    printf("Sets or clears the FAT32 dirty bit on a volume.\n");
    printf("\n");
    printf("%s set|clear drive:\n", exeName);
    printf("\n");
    printf("  set|clear     set or clear the FAT32 dirty bit\n");
    printf("  drive:        drive letter (for example d:)\n");
    printf("\n");
}
