#include "stdint.h"

#pragma pack(1)
struct Fat32BPB
{
    uint16_t bytesPerSector; // should be  512, 1024, 2048, 4096
    uint8_t sectorsPerClustor; // should be 1, 2, 4, 8, 16, 32, 64, 128
    uint16_t reservedSectors;
    uint8_t numberOfFats;  // should alwasy be 2
    uint16_t rootEntries;  // should be 0 for FAT32
    uint16_t smallSectors;  // should be 0 for FAT32
    uint8_t mediaDescriptor; // 0xf8 for HDD, 0xf0 for FDD
    uint16_t sectorsPerFat;  // should be 0 for FAT32
    uint16_t sectorsPerTrack;
    uint16_t numberOfHeads;
    uint32_t hiddenSectors; // Must be 0 for unpartitioned media
    uint32_t largeSectors; // FAT32 only: Total sectors in the volume
    uint32_t sectorsPerFat32; // FAT32 only: Sectors occupied by each FAT on the volume
    uint16_t unused;
    uint8_t fileSystemVersion[2]; // FAT32 only: Major and minor version numbers
    uint32_t rootClusterNumber; // FAT32 only: Root cluster number; typically 2
    uint16_t fileSystemInformationSectorNumber; // FAT32 only: Sector number for FSINFO structure
    uint16_t backupBootSector;  // FAT32 only: Sector number of backup boot sector
    uint8_t reserved[12];  // FAT32 only: Reserved for future use
};

struct Fat32BPBExt
{
    uint8_t physicalDriveNumber; // 0x00 for FDD, 0x80 for HDD
    uint8_t dirtyBit; // Should always be 0 for FAT32, but in reality is the volume dirty bit
    uint8_t extendedBootSignature; // Should be 0x28 or 0x29
    uint8_t serialNumber[4];
    char label[11];
    char systemId[8]; // should always contain "FAT32"
};

struct Fat32Header
{
    uint8_t jump[3];
    char oemId[8];
    Fat32BPB bpb;
    Fat32BPBExt bpbExt;
    uint8_t data[420];
    uint8_t endMarker[2];  // Should _always_ be 0x55aa
};

union FatSector
{
    char rawData[SECTOR_SIZE];
    Fat32Header header;
};
#pragma pack()


// Does the sector buffer contain a valid FAT32 Volume ID?
bool ValidFat32(char sector[]);

// Is the dirty bit set? (must be a valid FAT32 Volume ID!)
bool GetVolumeDirty(void* sector);

// Set the dirty bit (must be a valid FAT32 Volume ID!)
bool SetVolumeDirty(void* sector);
