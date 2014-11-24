// FAT32 Dirty Bit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Print the usage instructions for the program
void PrintUsage(char *exeName);

int main(int argc, char *argv[])
{
    // ISO mounting mode (full isostick integration)
    if (argc == 2)
    {
        // Expand path, because Windows 98 gives us a short path by default
        char fullPath[MAX_PATH];
        DWORD res = GetLongPathName(
            argv[1],
            fullPath,
            sizeof(fullPath)/sizeof(*fullPath));
        if (res == 0)
        {
            DWORD errNo = GetLastError();
            fprintf(stderr, "Error expanding file path: Error Number 0x%0x\n", errNo);
            return 1;
        }

        // Get drive letter and verify initial path
        const char driveLetter(toupper(fullPath[0]));
        if ((driveLetter < 'A') || (driveLetter > 'Z') ||
            (fullPath[1] != ':') || (fullPath[2] != '\\'))
        {
            PrintUsage(argv[0]);
            return 1;
        }

        // Get relative ISO path
        char *const relativeIsoPath = fullPath + 3;

        // Verify it is an ISO file
        const char isoExtension[] = ".iso";
        size_t extLen(strlen(isoExtension));
        size_t pathLen(strlen(relativeIsoPath));
        if (_strnicmp(relativeIsoPath + pathLen - extLen, isoExtension, extLen))
        {
            // Not a .iso file!
            fprintf(stderr, "File does not appear to be an ISO image.\n");
            return 1;
        }

        // Build config file path
        char configFilePath[] = "?:\\Config\\iso_filename.txt";
        configFilePath[0] = driveLetter;

        // Open configuration file
        FILE *configFile = fopen(configFilePath, "w");
        if (!configFile)
        {
            fprintf(stderr, "Could not open configuration file for write.\n");
            fprintf(stderr, "Error 0x%08x: %s\n", errno, strerror(errno));
            return 1;
        }

        // Write configuration file
        fprintf(configFile, "%s\n", relativeIsoPath);

        // Close configuration file
        fclose(configFile);

        // Init Fat32Util for the selected drive
        Fat32Util util;
        if (!util.init(driveLetter))
        {
            fprintf(stderr, "Could not get volume handle! Aborting...\n");
            return 1;
        }

        // Lock drive to prevent shenanigans
        util.lockDrive();

        // Read first sector
        if(!util.readSector(0))
        {
            fprintf(stderr, "Could not read sector! Aborting...\n");
            return 1;
        }

        // Get a copy of the data
        Fat32Util::Fat32Header sector = util.getData();

        // Validate FAT 32 Volume ID (sanity check)
        if(!util.validate())
        {
            fprintf(stderr, "Invalid sector read! Aborting...\n");
            return 1;
        }

        // Set the dirty bit
        sector.bpbExt.dirtyBit = 0x01;

        // Store updated sector
        if (!util.writeSector(0, sector))
        {
            fprintf(stderr, "Failed writing sector! Aborting...\n");
            return 1;
        }

        // Clear the dirty bit
        sector.bpbExt.dirtyBit = 0x00;

        // Store updated sector
        if (!util.writeSector(0, sector))
        {
            fprintf(stderr, "Failed writing sector! Aborting...\n");
            return 1;
        }

        // Unlock drive
        util.unlockDrive();

        // Done!
        return 0;
    } // argc == 2

    // Set/clear dirty bit mode
    else if (argc == 3)
    {
        // Get set or clear
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
        if ((driveLetter < 'A') || (driveLetter > 'Z') ||
            (argv[2][1] != ':'))
        {
            PrintUsage(argv[0]);
            return 1;
        }

        // Init Fat32Util for the selected drive
        Fat32Util util;
        if (!util.init(driveLetter))
        {
            fprintf(stderr, "Could not get volume handle! Aborting...\n");
            return 1;
        }

        // Read first sector
        if(!util.readSector(0))
        {
            fprintf(stderr, "Could not read sector! Aborting...\n");
            return 1;
        }

        // Get a copy of the data
        Fat32Util::Fat32Header sector = util.getData();

        // Validate FAT 32 Volume ID (sanity check)
        if(!util.validate())
        {
            fprintf(stderr, "Invalid sector read! Aborting...\n");
            return 1;
        }

        // Change the bit in our copy of the sector
        sector.bpbExt.dirtyBit = setDirty ? 0x01 : 0x00;

        // Store updated sector
        if (!util.writeSector(0, sector))
        {
            fprintf(stderr, "Failed writing sector! Aborting...\n");
            return 1;
        }
    } // argc == 3

    // Invalid/unknown arguments
    else
    {
        PrintUsage(argv[0]);
        return 1;
    }

    // Done!
    return 0;
}

// Print the usage instructions for the program
void PrintUsage(char *exeName)
{
    printf("Sets or clears the FAT32 dirty bit on a volume.\n");
    printf("Optionally updates the ISO selection on an isostick.\n");
    printf("\n");
    printf("%s set|clear drive:\n", exeName);
    printf("%s disk_image.iso\n", exeName);
    printf("\n");
    printf("  set|clear         set or clear the FAT32 dirty bit\n");
    printf("  drive:            drive letter (for example d:)\n");
    printf("  disk_image.iso    Full path to an ISO file\n");
    printf("\n");
}
