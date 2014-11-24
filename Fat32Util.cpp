#include "stdafx.h"

// Does the sector buffer contain a valid FAT32 Volume ID?
bool ValidFat32(FatSector& sector)
{
    // Verify according to Microsoft FAT specification document

    // Verify jump is valid (2 allowed forms)
    if (!
        (((sector.header.jump[0] == 0xEB) && (sector.header.jump[2] = 0x90)) ||
          (sector.header.jump[0] == 0xE9)))
    {
        return false;
    }

    // OEM Name doesn't matter

    // Bytes per sector (we only implement 512)
    if (sector.header.bpb.bytesPerSector != SECTOR_SIZE)
    {
        return false;
    }

    // Sectors per cluster
    switch (sector.header.bpb.sectorsPerClustor)
    {
    case 1:
    case 2:
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
        break;

    default:
        return false;
    }

    // Reserved sectors
    if (sector.header.bpb.reservedSectors == 0)
    {
        return false;
    }

    // We only support 2 FATs
    if (sector.header.bpb.numberOfFats != 2)
    {
        return false;
    }

    // Root entry count
    if (sector.header.bpb.rootEntries != 0)
    {
        return false;
    }

    // Small sector count
    if (sector.header.bpb.smallSectors != 0)
    {
        return false;
    }

    // Media descriptor
    switch (sector.header.bpb.mediaDescriptor)
    {
    case 0xF0:
    case 0xF8:
    case 0xF9:
    case 0xFA:
    case 0xFB:
    case 0xFC:
    case 0xFD:
    case 0xFE:
    case 0xFF:
        break;

    default:
        return false;
    }

    // Sectors per FAT
    if (sector.header.bpb.sectorsPerFat != 0)
    {
        return false;
    }

    // Sectors per track doesn't matter
    // Number of heads doesn't matter
    // Hidden sectors doesn't matter

    // Total sectors
    if (sector.header.bpb.largeSectors == 0)
    {
        return false;
    }

    // Sectors per FAT32 doesn't matter (not sure how to validate?)
    // Extra flags ("unused") doesn't matter

    // Version (Win98 only supports 0:0)
    if ((sector.header.bpb.fileSystemVersion[0] != 0) ||
        (sector.header.bpb.fileSystemVersion[1] != 0))
    {
        return false;
    }

    #ifdef PEDANTIC_VERIFY
    // Root cluster number
    if (sector.header.bpb.rootClusterNumber != 2)
    {
        return false;
    }

    // File system information sector number doesn't matter (usually 1)
    if (sector.header.bpb.fileSystemInformationSectorNumber != 1)
    {
        return false;
    }

    // Backup boot sector
    if (sector.header.bpb.backupBootSector != 6)
    {
        return false;
    }
    #endif // PEDANTIC_VERIFY

    // Reserved should all be 0
    for (int i(0); i < (sizeof(sector.header.bpb.reserved) / sizeof(*sector.header.bpb.reserved)); ++i)
    {
        if (sector.header.bpb.reserved[i] != 0)
        {
            return false;
        }
    }

    // Drive number
    if ((sector.header.bpbExt.physicalDriveNumber != 0x80) ||
        (sector.header.bpbExt.physicalDriveNumber != 0x00))
    {
        return false;
    }

    // Dirty bit
    if ((sector.header.bpbExt.dirtyBit != 0x00) ||
        (sector.header.bpbExt.dirtyBit != 0x01))
    {
        return false;
    }

    // Extended boot signature
    if (sector.header.bpbExt.extendedBootSignature != 0x29)
    {
        return false;
    }

    // Serial number doesn't matter
    // Volume label doesn't matter

    // File system ID should always be "FAT32   "
    if (!strncmp(sector.header.bpbExt.systemId, "FAT32   ", 8))
    {
        return false;
    }

    // Data doesn't matter

    // End marker
    if ((sector.header.endMarker[0] != 0x55) ||
        (sector.header.endMarker[0] != 0xAA))
    {
        return false;
    }

    // Probably valid!
    return true;
}
