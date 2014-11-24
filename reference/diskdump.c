//
// DISKDUMP.C -- Win32 console app for displaying Win95 disk structures
// Stan Mitchell, 1996
//
#include <stdio.h>
#include <windows.h>
#include "tag.h"

#define VWIN32_DIOC_DOS_IOCTL   1   // DOS ioctl calls 4400h-4411h
#define VWIN32_DIOC_DOS_INT25   2   // absolute disk read, DOS int 25h
#define VWIN32_DIOC_DOS_DRIVEINFO 6 // OEM Service Release 2
//   Int 21h, 73xxh extensions
typedef struct _DIOC_REGISTERS
{
    DWORD reg_EBX;
    DWORD reg_EDX;
    DWORD reg_ECX;
    DWORD reg_EAX;
    DWORD reg_EDI;
    DWORD reg_ESI;
    DWORD reg_Flags;
} DIOC_REGISTERS, *PDIOC_REGISTERS;

#define   LEVEL1_LOCK   1
#define   LEVEL1_LOCK_MAX_PERMISSION      0x0001

#define   DRIVE_IS_REMOTE                 0x1000
#define   DRIVE_IS_SUBST                  0x8000

/*********************************************************
 **** Note: all MS-DOS data structures must be packed ****
 ****       on a one-byte boundary.                   ****
 *********************************************************/
#pragma pack(1)

typedef struct _DISKIO
{
    DWORD diStartSector;    // sector number to start at
    WORD  diSectors;        // number of sectors
    DWORD diBuffer;         // address of buffer
} DISKIO, *PDISKIO;


typedef struct _BOOTSECT
{
    BYTE    bsJump[3];      // jmp instruction
    char    bsOemName[8];   // OEM name and version

    // This portion is the BPB (BIOS Parameter Block)
    WORD    bsBytesPerSec;            // bytes per sector
    BYTE    bsSecPerClust;            // sectors per cluster
    WORD    bsResSectors;              // number of reserved sectors
    BYTE    bsFATs;                            // number of file allocation tables
    WORD    bsRootDirEnts;            // number of root-directory entries
    WORD    bsSectors;                    // total number of sectors
    BYTE    bsMedia;                      // media descriptor
    WORD    bsFATsecs;                    // number of sectors per FAT
    WORD    bsSecPerTrack;            // number of sectors per track
    WORD    bsHeads;                      // number of read/write heads
    DWORD   bsHiddenSectors;      // number of hidden sectors
    DWORD   bsHugeSectors;            // number of sectors if bsSectors==0
    // End of BPB

    BYTE    bsDriveNumber;              // 80h if first hard drive
    BYTE    bsReserved;
    BYTE    bsBootSignature;    // 29h if extended boot-signature record
    DWORD   bsVolumeID;                      // volume ID number
    char    bsVolumeLabel[11];  // volume label
    char    bsFileSysType[8];   // file-system type (FAT12 or FAT16)
} BOOTSECTOR, *PBOOTSECTOR;

typedef struct _DIRENTRY
{
    char    deName[8];                      // base name
    char    deExtension[3];              // extension
    BYTE    deAttributes;                // file or directory attributes
    BYTE    deReserved[6];
    WORD    deLastAccessDate;        // *New Win95* - last access date
    WORD    deEAhandle;                      // *New FAT32* - high word of starting cluster
    WORD    deCreateTime;                // creation or last modification time
    WORD    deCreateDate;                // creation or last modification date
    WORD    deStartCluster;              // starting cluster of the file or directory
    DWORD   deFileSize;                      // size of the file in bytes
} DIRENTRY, *PDIRENTRY;

typedef struct _LONGDIRENTRY
{
    char    leSequence;                  // sequence byte:1,2,3,..., last entry is or'ed with 40h
    wchar_t leName[5];                  // Unicode characters of name
    BYTE    leAttributes;            // Attributes: 0fh
    BYTE    leType;                          // Long Entry Type: 0
    BYTE    leChksum;                    // Checksum for matching short name alias
    wchar_t leName2[6];                  // More Unicode characters of name
    WORD    leZero;                          // reserved
    wchar_t leName3[2];                  // More Unicode characters of name
} LONGDIRENTRY, *PLONGDIRENTRY;

////////////////// FAT32 Structures ///////////////////

// BPB for a Fat32 partition
typedef struct _A_BF_BPB
{
    WORD    A_BF_BPB_BytesPerSector;
    BYTE    A_BF_BPB_SectorsPerCluster;
    WORD    A_BF_BPB_ReservedSectors;
    BYTE    A_BF_BPB_NumberOfFATs;
    WORD    A_BF_BPB_RootEntries;
    WORD    A_BF_BPB_TotalSectors;
    BYTE    A_BF_BPB_MediaDescriptor;
    WORD    A_BF_BPB_SectorsPerFAT;
    WORD    A_BF_BPB_SectorsPerTrack;
    WORD    A_BF_BPB_Heads;
    WORD    A_BF_BPB_HiddenSectors;
    WORD    A_BF_BPB_HiddenSectorsHigh;
    WORD    A_BF_BPB_BigTotalSectors;
    WORD    A_BF_BPB_BigTotalSectorsHigh;
    WORD    A_BF_BPB_BigSectorsPerFat;
    WORD    A_BF_BPB_BigSectorsPerFatHi;
    WORD    A_BF_BPB_ExtFlags;
    WORD    A_BF_BPB_FS_Version;
    WORD    A_BF_BPB_RootDirStrtClus;
    WORD    A_BF_BPB_RootDirStrtClusHi;
    WORD    A_BF_BPB_FSInfoSec;
    WORD    A_BF_BPB_BkUpBootSec;
    WORD    A_BF_BPB_Reserved[6];
} A_BF_BPB, PA_BF_BPB;

typedef struct _BOOTSECT32
{
    BYTE    bsJump[3];      // jmp instruction
    char    bsOemName[8];   // OEM name and version

    // This portion is the FAT32 BPB
    A_BF_BPB  bpb;

    BYTE    bsDriveNumber;              // 80h if first hard drive
    BYTE    bsReserved;
    BYTE    bsBootSignature;    // 29h if extended boot-signature record
    DWORD   bsVolumeID;                      // volume ID number
    char    bsVolumeLabel[11];  // volume label
    char    bsFileSysType[8];   // file-system type (FAT32)
} BOOTSECTOR32, *PBOOTSECTOR32;

#define            FSINFOSIG      0x61417272L
typedef struct       _BIGFATBOOTFSINFO
{
    DWORD      bfFSInf_Sig;                        // signature given by FSINFOSIG
    DWORD      bfFSInf_free_clus_cnt;      //
    DWORD      bfFSInf_next_free_clus;      //
    DWORD      bfFSInf_resvd[3];                //
} BIGFATBOOTFSINFO, *PBIGFATBOOTFSINFO;

typedef struct DPB        // FAT12, FAT16 standard DPB
{
    BYTE            dpb_drive;
    BYTE            dpb_unit;
    WORD            dpb_sector_size;
    BYTE            dpb_cluster_mask;
    BYTE            dpb_cluster_shift;
    WORD            dpb_first_fat;
    BYTE            dpb_fat_count;
    WORD            dpb_root_entries;
    WORD            dpb_first_sector;
    WORD            dpb_max_cluster;
    WORD            dpb_fat_size;
    WORD            dpb_dir_sector;
    DWORD            dpb_reserved2;
    BYTE            dpb_media;
    union
    {
        BYTE    dpb_first_access;        // non-FAT32
        BYTE    dpb_reserved;                // FAT32
    };
    DWORD            dpb_reserved3;
    WORD            dpb_next_free;
    WORD            dpb_free_cnt;
} DPB, *PDPB;

typedef struct EXTDPB        // FAT32 extended DPB
{
    DPB            std;
    WORD      extdpb_free_cnt_hi;
    WORD      extdpb_flags;
    WORD      extdpb_FSInfoSec;
    WORD      extdpb_BkUpBootSec;
    DWORD      extdpb_first_sector;
    DWORD      extdpb_max_cluster;
    DWORD      extdpb_fat_size;
    DWORD      extdpb_root_clus;
    DWORD      extdpb_next_free;
} EXTDPB, *PEXTDPB;

typedef struct MID
{
    WORD  midInfoLevel;       // information level, must be 0
    DWORD midSerialNum;       // serial number for the medium
    char  midVolLabel[11];    // volume label for the medium
    char  midFileSysType[8];  // type of file system as 8-byte ASCII
} MID, *PMID;

#pragma pack()

HANDLE hVWin32Device;
BOOTSECTOR            bs;            // from first logical sector of drive
BOOTSECTOR32      bs32;      // from first logical sector of drive
EXTDPB dpb;                          // extended DPB for FAT32 drives
MID    media;

int ReadAbsoluteSectors( int volume, DWORD start_sector,
                         WORD num_sectors, void *pBuf )
{
    DIOC_REGISTERS reg;
    DISKIO di;
    BOOL bResult;
    DWORD cb;

    reg.reg_EAX      = volume;  // zero-based volume number
    reg.reg_EBX      = (DWORD)&di;
    reg.reg_ECX      = 0xffff;  // use DISKIO structure
    reg.reg_Flags    = 1;       // preset the carry flag
    di.diStartSector = start_sector;
    di.diSectors     = num_sectors;
    di.diBuffer      = (DWORD)pBuf;
    bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_INT25,
                               &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

    if ( !bResult || (reg.reg_Flags & 1) ) return -1;
    return 0;
}

int ReadAbsoluteSectors32( int volume, DWORD start_sector,
                           WORD num_sectors, void *pBuf )
{
    DIOC_REGISTERS reg;
    DISKIO di;
    BOOL bResult;
    DWORD cb;

    reg.reg_EAX      = 0x7305;  // Ext_ABSDiskReadWrite
    reg.reg_EBX      = (DWORD)&di;
    reg.reg_ECX      = 0xffff;  // use DISKIO structure
    reg.reg_EDX      = volume;  // one-based volume number
    reg.reg_ESI      = 0;       // READ operation
    reg.reg_Flags    = 0;       // preset the carry flag
    di.diStartSector = start_sector;
    di.diSectors     = num_sectors;
    di.diBuffer      = (DWORD)pBuf;

    bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_DRIVEINFO,
                               &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

    if ( /*!bResult ||*/ (reg.reg_Flags & 1) ) return -1;
    return 0;
}


int GetExtendedDPB( int volume )
{
    DIOC_REGISTERS reg;
    BOOL bResult;
    DWORD cb;
    struct
    {
        WORD parmsize;
        EXTDPB edpb;
    } edp;

    edp.parmsize = sizeof( EXTDPB );
    reg.reg_EAX = 0x7302; // Get_ExtDPB
    reg.reg_ECX = sizeof( edp );
    reg.reg_EDX = volume;
    reg.reg_EDI = (DWORD)&edp;
    reg.reg_Flags = 1; // preset the carry flag

    bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_DRIVEINFO,
                               &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

    // This function will fail for versions of Win95 earlier than OSR2
    if ( !bResult || (reg.reg_Flags & 1) ) return -1;

    // FAT32 drives have a dpb_fat_size of 0,
    //   the value in extdpb_fat_size is used instead.
    //   dpb_fat_size is a WORD, extdpb_fat_size is DWORD

    memcpy( &dpb, &edp.edpb, sizeof( EXTDPB ) );

    if ( edp.edpb.std.dpb_fat_size == 0 ) return 0; // FAT32 extended DPB

    if ( edp.parmsize == sizeof( EXTDPB ) ) return 1; // FAT12 or FAT16 extended DPB
}

#if 0
int GetVolumeLockState( int volume, BOOL bFlatFAT,
                        WORD *lock_level, WORD *perm )
{
    DIOC_REGISTERS reg;
    BOOL bResult;
    DWORD cb;

    *perm = *lock_level = 0;

    reg.reg_EAX = 0x440d; // generic IOCTL
    reg.reg_ECX = bFlatFAT ? 0x4870 : 0x0870; // get lock state
    reg.reg_EBX = volume;
    reg.reg_Flags = 1; // preset the carry flag

    bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
                               &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

    if ( !bResult || (reg.reg_Flags & 1) )
        return (reg.reg_EAX & 0xffff);

    *lock_level = (WORD)reg.reg_EAX & 0xffff; // current lock level
    *perm       = (WORD)reg.reg_ECX & 0xffff; // lock permissions
    return 0;
}
#endif

int VolumeCheck( int volume, WORD *flags )
{
    DIOC_REGISTERS reg;
    BOOL bResult;
    DWORD cb;

    reg.reg_EAX = 0x4409; // Is Drive Remote
    reg.reg_EBX = volume; // one-based drive number
    reg.reg_Flags = 1; // preset the carry flag

    bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
                               &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

    if ( !bResult || (reg.reg_Flags & 1) )
        return (reg.reg_EAX & 0xffff);

    *flags = (WORD)(reg.reg_EDX & 0xffff);
    return 0;
}

int GetMediaID( int volume, MID *pMid )
{
    DIOC_REGISTERS reg;
    BOOL bResult;
    DWORD cb;

    reg.reg_EAX = 0x440d; // IOCTL for block device
    reg.reg_EBX = volume; // one-based drive number
    reg.reg_ECX = 0x0866; // Get Media ID
    reg.reg_EDX = (DWORD)pMid;
    reg.reg_Flags = 1; // preset the carry flag

    bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
                               &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

    if ( !bResult || (reg.reg_Flags & 1) )
        return (reg.reg_EAX & 0xffff);

    return 0;
}


int LockLogicalVolume( int volume, int lock_level,
                       int permissions, BOOL bFlatFAT )
{
    DIOC_REGISTERS reg;
    BOOL bResult;
    DWORD cb;

    reg.reg_EAX = 0x440d; // generic IOCTL
    reg.reg_ECX = bFlatFAT ? 0x484a : 0x084a; // lock logical volume
    reg.reg_EBX = volume | (lock_level << 8);
    reg.reg_EDX = permissions;
    reg.reg_Flags = 1; // preset the carry flag

    bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
                               &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

    if ( !bResult || (reg.reg_Flags & 1) )
        return (reg.reg_EAX & 0xffff);

    return 0;
}

int UnlockLogicalVolume( int volume, BOOL bFlatFAT )
{
    DIOC_REGISTERS reg;
    BOOL bResult;
    DWORD cb;

    reg.reg_EAX = 0x440d;
    reg.reg_ECX = bFlatFAT ? 0x486a : 0x086a; // lock logical volume
    reg.reg_EBX = volume;
    reg.reg_Flags = 1; // preset the carry flag

    bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
                               &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

    if ( !bResult || (reg.reg_Flags & 1) ) return -1;
    return 0;
}

void PrintFat12Pair( BYTE **ppFat )
{
    WORD wFatEntry, nib;
    BYTE *pFat = *ppFat;
    wFatEntry = *pFat++;
    nib = *pFat & 0x0f;
    wFatEntry |= (nib << 8);
    printf( "%03x ", wFatEntry );

    nib = *pFat++ & 0xf0;
    wFatEntry = *pFat++;
    wFatEntry <<= 4;
    nib >>= 4;
    printf( "%03x ", wFatEntry | nib );
    *ppFat = pFat;
}

void DumpFat( void *pBuf, BOOL bFat12, BOOL bFat16, BOOL bFat32 )
{
    int i, j;

    if ( bFat12 )
    {
        BYTE *pFat = (BYTE *)pBuf;
        // print out the first 16 FAT table entries
        for ( j = 0; j < 2; j++ )
        {
            printf( " %3d ", j * 8 );
            for ( i = 0; i < 4; i++ )
                PrintFat12Pair( &pFat );
            printf( "\n" );
        }
    }
    else if ( bFat16 )
    {
        WORD *pFat = (WORD *)pBuf;
        //  Print out the first 16 FAT table entries
        for ( j = 0; j < 2; j++ )
        {
            printf( " %3d ", j * 8 );
            for ( i = 0; i < 8; i++ )
                printf( "%04x ", *pFat++ );
            printf( "\n" );
        }
    }
    else if ( bFat32 )
    {
        DWORD *pFat32 = (DWORD *)pBuf;
        //  Print out the first 32 FAT32 table entries.
        //      The most significant 4 bits are not part of the cluster number.
        for ( j = 0; j < 8; j++ )
        {
            printf( " %3d ", j * 8 );
            for ( i = 0; i < 8; i++ )
                printf( "%07lx ", (*pFat32++) & 0xfffffff );
            printf( "\n" );
        }
    }
    else
    {
        printf( "**** Unknown FAT size ****\n" );
        return;
    }
}


char *DOSTimeStr( WORD time, char *pbuf )
{
    unsigned short two_secs, minutes, hours;
    two_secs = time & 0x001f; // two second intervals
    minutes = (time & 0x07e0) >> 5;
    hours   = (time & 0xf800) >> 11;
    sprintf( pbuf, "%02d:%02d:%02d", hours, minutes, 2 * two_secs );
    return pbuf;
}

char *DOSDateStr( WORD date, char *pbuf )
{
    unsigned short year, month, day;
    if ( date != 0 )
    {
        day     = date & 0x001f;
        month   = (date & 0x01e0) >> 5;
        year    = (date & 0xfe00) >> 9;
        sprintf( pbuf, "%02d-%02d-%02d", month, day, year + 80 );
    }
    else
        sprintf( pbuf, "xx-xx-xx" );
    return pbuf;
}

void DumpDirEntries( void *pBuf, int bufsize, BOOL isFat32 )
{
    BYTE *pDir = pBuf,
          *pEnd = pDir + bufsize;
    int i;

    printf( "\n  Name          Attrib       Creation      Last      Start      Size  Chksum\n" );
    printf(   "                            Date/Time     Access    Cluster           \n" );
    while ( (pDir < pEnd) && *pDir )
    {

        // Skip over deleted directory entries
        if (*pDir == 0xe5)
        {
            pDir += sizeof( DIRENTRY );
        }

        // Type 0 "long" directory entry
        else if (*(pDir + 0x0b) == 0xf && *(pDir + 0x0c) == 0 )
        {
            int cnt;
            wchar_t szName[14];
            PLONGDIRENTRY pl = (PLONGDIRENTRY)pDir;

            if ( pl->leSequence & 0x40 )
                printf( "%c ", pl->leSequence );
            else
                printf( "%d ", pl->leSequence );

            memset( szName, 0, sizeof( szName ) );

            // print out the Unicode characters in this entry
            swprintf( szName, L"%.5s", pl->leName );
            if ( pl->leName2[0] != 0 && pl->leName2[0] != 0xffff )
                swprintf( szName, L"%s%.6s", szName, pl->leName2 );
            if ( pl->leName3[0] != 0 && pl->leName3[0] != 0xffff )
                swprintf( szName, L"%s%.2s", szName, pl->leName3 );

            cnt = wcslen( szName );
            while ( cnt < 13 )
            {
                szName[cnt] = L' ';
                cnt++;
            }
            wprintf( L"%s ", szName );

            // Display the attributes - should be 'vshr'
            printf( "%c", pl->leAttributes & 0x20 ? 'a' : '-' );
            printf( "%c", pl->leAttributes & 0x10 ? 'd' : '-' );
            printf( "%c", pl->leAttributes & 0x08 ? 'v' : '-' );
            printf( "%c", pl->leAttributes & 0x04 ? 's' : '-' );
            printf( "%c", pl->leAttributes & 0x02 ? 'h' : '-' );
            printf( "%c", pl->leAttributes & 0x01 ? 'r' : '-' );

            printf( "%48s %02x\n", "", pl->leChksum );
            pDir += sizeof( DIRENTRY );
        }

        // Other Type "long" directory entry
        else if (*(pDir + 0x0b) == 0xf && *(pDir + 0x0c) != 0 )
        {
            printf( "! " );
            for ( i = 0; i < 16; i++ ) printf( "%02x ", *pDir++ );
            printf( "\n" );
            printf( "  " );
            for ( i = 0; i < 16; i++ ) printf( "%02x ", *pDir++ );
            printf( "\n" );
            pDir += sizeof( DIRENTRY );
        }

        // "Short" directory entry
        else
        {
            PDIRENTRY pd = (PDIRENTRY)pDir;
            char dos_time[20];
            char dos_date[20];
            printf( "  " );
            printf( "%.8s %.3s  ", pd->deName, pd->deExtension );
            printf( "%c", pd->deAttributes & 0x20 ? 'a' : '-' );
            printf( "%c", pd->deAttributes & 0x10 ? 'd' : '-' );
            printf( "%c", pd->deAttributes & 0x08 ? 'v' : '-' );
            printf( "%c", pd->deAttributes & 0x04 ? 's' : '-' );
            printf( "%c", pd->deAttributes & 0x02 ? 'h' : '-' );
            printf( "%c", pd->deAttributes & 0x01 ? 'r' : '-' );
            printf( " %s %s", DOSDateStr( pd->deCreateDate, dos_date ),
                    DOSTimeStr( pd->deCreateTime, dos_time ) );
            printf( " %s", DOSDateStr( pd->deLastAccessDate, dos_date ));
            if ( !isFat32 )
                printf( "    %04x    %8lu\n", pd->deStartCluster, pd->deFileSize );
            else
                printf( "  %04x%04x %8lu\n", pd->deEAhandle,
                        pd->deStartCluster, pd->deFileSize );
            pDir += sizeof( DIRENTRY );
        }
    }
}

#if 0
void DumpExtendedDpb( void )
{

    printf( "\n Dump Extended DPB ...\n" );

    printf( "  Drive:               %02x\n", dpb.std.dpb_drive );
    printf( "  Unit:                %02x\n", dpb.std.dpb_unit );
    printf( "  Sector Size:         %04x\n", dpb.std.dpb_sector_size );
    printf( "  Cluster Mask:        %02x\n", dpb.std.dpb_cluster_mask );
    printf( "  Cluster Shift:       %02x\n", dpb.std.dpb_cluster_shift );
    printf( "  First Fat:                  %04x\n", dpb.std.dpb_first_fat );
    printf( "  Fat Count:           %02x\n", dpb.std.dpb_fat_count );
    printf( "  Root Entries:            %04x\n", dpb.std.dpb_root_entries );
    printf( "  First Sector:            %04x\n", dpb.std.dpb_first_sector );
    printf( "  Max Cluster:                  %04x\n", dpb.std.dpb_max_cluster );
    printf( "  Fat Size:                  %04x\n", dpb.std.dpb_fat_size );
    printf( "  Dir Sector:                  %04x\n", dpb.std.dpb_dir_sector );
    printf( "  Reserved2:                  %08lx\n", dpb.std.dpb_reserved2 );
    printf( "  Media:                        %02x\n", dpb.std.dpb_media );
    printf( "  Reserved:                  %02x\n", dpb.std.dpb_reserved );
    printf( "  Reserved3:                  %08lx\n", dpb.std.dpb_reserved3 );
    printf( "  Next Free:                  %04x\n", dpb.std.dpb_next_free );
    printf( "  Free Count:                  %04x\n", dpb.std.dpb_free_cnt );
    printf( "  *Free Count Hi:            %04x\n", dpb.extdpb_free_cnt_hi );
    printf( "  *Flags:                        %04x\n", dpb.extdpb_flags );
    printf( "  *FSInfoSec:                  %04x\n", dpb.extdpb_FSInfoSec );
    printf( "  *BkUpBootSec:            %04x\n", dpb.extdpb_BkUpBootSec );
    printf( "  *First Sector:            %08lx\n", dpb.extdpb_first_sector );
    printf( "  *Max Cluster:            %08lx\n", dpb.extdpb_max_cluster );
    printf( "  *Fat Size:                  %08lx\n", dpb.extdpb_fat_size );
    printf( "  *Root Cluster:            %08lx\n", dpb.extdpb_root_clus );
    printf( "  *Next Free:                  %08lx\n", dpb.extdpb_next_free );
}
#endif

// FAT12 and FAT16 Boot Sectors
void DumpBootSector( void )
{

    printf( "\n Sector 0 - BOOTSECTOR structure ...\n" );
    printf( "  OEM Name:                      %.8s\n", bs.bsOemName );
    printf( "  Bytes/Sector:                  %04x\n", bs.bsBytesPerSec );
    printf( "  Sectors/Cluster:               %02x\n", bs.bsSecPerClust );
    printf( "  Reserved Sectors:              %04x\n", bs.bsResSectors );
    printf( "  Number FATs:                   %02x\n", bs.bsFATs );
    printf( "  Number Root Directory Entries: %04x\n", bs.bsRootDirEnts );

    if ( bs.bsSectors != 0 )
        printf( "  Total number Sectors:          %04x\n", bs.bsSectors );
    else if ( bs.bsHugeSectors )
        printf( "  Total number Sectors:          %08lx\n", bs.bsHugeSectors );

    printf( "  Media Descriptor:              %02X\n", bs.bsMedia );
    printf( "  Number of Sectors/FAT:         %04x\n", bs.bsFATsecs );
    printf( "  Sectors/Track:                 %04x\n", bs.bsSecPerTrack );
    printf( "  Heads:                         %04x\n", bs.bsHeads );
    printf( "  Hidden Sectors:                %08lx\n", bs.bsHiddenSectors );
    printf( "  BIOS Drive Number:             %02x\n", bs.bsDriveNumber );
    printf( "  Boot Signature:                %02x\n", bs.bsBootSignature );
    printf( "  Volume ID:                     %08lx\n", bs.bsVolumeID );
    printf( "  Volume Label:                  %.11s\n", bs.bsVolumeLabel );
    printf( "  File System Type:              %.8s\n\n", bs.bsFileSysType );

    if ( bs.bsResSectors > 1 )
        printf( "\n Sectors 1 - %d Reserved Sectors\n", bs.bsResSectors - 1 );
}

void DumpFSInfo( int sector, BYTE *pbuf )
{
    PBIGFATBOOTFSINFO pfsinfo = NULL;
    BYTE *pend = pbuf + 512;

    printf( "\n Sector %d  - FAT32 FS Info Sector...\n", sector );
    while ( TRUE )
    {
        PDWORD pdw = (DWORD *)pbuf;
        if ( *pdw == FSINFOSIG )
        {
            pfsinfo = (PBIGFATBOOTFSINFO)pdw;
            break;
        }
        pbuf += 4;
    }

    printf( "  Signature:                     %08lx\n", pfsinfo->bfFSInf_Sig );
    printf( "  Free Clusters:                 %08lx\n", pfsinfo->bfFSInf_free_clus_cnt );
    printf( "  Next Free Cluster:             %08lx\n", pfsinfo->bfFSInf_next_free_clus );
}

// FAT32 Boot Sector
void DumpBootSector32( void )
{
    DWORD totalhidden, totalsectors, sectorsperfat, rootdirstartclus;
    int i;

    printf( "\n Sector 0 - FAT32 BOOTSECTOR structure ...\n" );
    printf( "  OEM Name:                      %.8s\n", bs32.bsOemName );
    printf( "  Bytes/Sector:                  %04x\n", bs32.bpb.A_BF_BPB_BytesPerSector );
    printf( "  Sectors/Cluster:               %02x\n", bs32.bpb.A_BF_BPB_SectorsPerCluster );
    printf( "  Reserved Sectors:              %04x\n", bs32.bpb.A_BF_BPB_ReservedSectors );
    printf( "  Number FATs:                   %02x\n", bs32.bpb.A_BF_BPB_NumberOfFATs );
    printf( "  Number Root Directory Entries: %04x\n", bs32.bpb.A_BF_BPB_RootEntries );

    if ( bs32.bpb.A_BF_BPB_TotalSectors != 0 )
        printf( "  Total number Sectors:          %04x\n", bs32.bpb.A_BF_BPB_TotalSectors );

    printf( "  Media Descriptor:              %02X\n", bs32.bpb.A_BF_BPB_MediaDescriptor );
    printf( "  Number of Sectors/FAT:         %04x\n", bs32.bpb.A_BF_BPB_SectorsPerFAT );
    printf( "  Sectors/Track:                 %04x\n", bs32.bpb.A_BF_BPB_SectorsPerTrack );
    printf( "  Heads:                         %04x\n", bs32.bpb.A_BF_BPB_Heads );
    totalhidden = (bs32.bpb.A_BF_BPB_HiddenSectorsHigh << 16) + bs32.bpb.A_BF_BPB_HiddenSectors;
    printf( "  Hidden Sectors:                %08lx\n", totalhidden );
    totalsectors = (bs32.bpb.A_BF_BPB_BigTotalSectorsHigh << 16) + bs32.bpb.A_BF_BPB_BigTotalSectors;
    printf( "  Big Total number Sectors:      %08lx\n", totalsectors );
    sectorsperfat = (bs32.bpb.A_BF_BPB_BigSectorsPerFatHi << 16) + bs32.bpb.A_BF_BPB_BigSectorsPerFat;
    printf( "  *Big Sectors per FAT:          %08lx\n", sectorsperfat );
    printf( "  *Extended flags:               %04x\n", bs32.bpb.A_BF_BPB_ExtFlags );
    printf( "  *File System Version:          %04x\n", bs32.bpb.A_BF_BPB_FS_Version );
    rootdirstartclus = (bs32.bpb.A_BF_BPB_RootDirStrtClusHi << 16) + bs32.bpb.A_BF_BPB_RootDirStrtClus;
    printf( "  *Root Dir Start Cluster:       %08lx\n", rootdirstartclus );
    printf( "  *File System Info Sector:      %04x\n", bs32.bpb.A_BF_BPB_FSInfoSec );
    printf( "  *Backup Boot Sector:           %04x\n", bs32.bpb.A_BF_BPB_BkUpBootSec );

    for (i = 0; i < 6; i++)
        printf( "  *Reserved[%d]:                  %04x\n", i, bs32.bpb.A_BF_BPB_Reserved[i] );

    printf( "  BIOS Drive Number:             %02x\n", bs32.bsDriveNumber );
    printf( "  Boot Signature:                %02x\n", bs32.bsBootSignature );
    printf( "  Volume ID:                     %08lx\n", bs32.bsVolumeID );
    printf( "  Volume Label:                  %.11s\n", bs32.bsVolumeLabel );
    printf( "  File System Type:              %.8s\n", bs32.bsFileSysType );
}

void ErrorExit0( char cDrive, char *msg )
{
    printf( "\n Volume %c %s\n", cDrive, msg );
    CloseHandle( hVWin32Device );
    CloseTagmon();
    exit( 1 );
}

void ErrorExit1( int error, char cDrive )
{
    printf( "\n ERROR %d: Failed to lock Volume %c.\n", error, cDrive );
    CloseHandle( hVWin32Device );
    CloseTagmon();
    exit( 1 );
}

void ErrorExit2( int vol1, char cDrive, DWORD sector, BOOL bFat32, BYTE *pBuf )
{
    printf( "\n ERROR: Failed to read sector %d, volume %c.\n", sector, cDrive );
    free( pBuf );
    UnlockLogicalVolume( vol1, bFat32 );
    CloseHandle( hVWin32Device );
    CloseTagmon();
    exit( 1 );
}


void main( int argc, char *argv[] )
{
    BOOL bFat12 = FALSE, bFat16 = FALSE, bFat32 = FALSE;
    unsigned short sector_size, sectors_per_fat, sectors_per_clus, drive_flags;
    DWORD dwReservedSectors, first_fat_sector1, second_fat_sector1;
    BYTE *pReadBuf;
    char szOut[80], szFSType[9];
    int vol1, volz, retc, bufsize;

    char c = 'C'; // default drive letter

    printf( "\n DISKDUMP 1.1 - FAT Disk Structures for Win9x\n"
            " Stan Mitchell, 1996-97. Usage: diskdump [volume letter].\n" );

    if ( argc == 2 )   /* Check for a command line argument */
    {
        if ( isalpha( *argv[1] ) )
        {
            c = toupper( *argv[1] );
        }
    }

    // One-based volume number
    vol1 = c - 'A' + 1; // requested volume, 'A'=1, 'B'=2, 'C'=3, etc.
    // Zero-based volume number
    volz = vol1 - 1;

    OpenTagmon();
    sprintf( szOut, "==== diskdump (%c) ====", c );
    EmitString( szOut );

    hVWin32Device = CreateFile( "\\\\.\\vwin32", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL );

    retc = VolumeCheck( vol1, &drive_flags );
    if ( retc ) ErrorExit0( c, "is invalid drive" );

    retc = GetMediaID( vol1, &media );
    if ( retc )
    {
        // Network drive
        if ( retc == ERROR_INVALID_FUNCTION && drive_flags & DRIVE_IS_REMOTE )
            ErrorExit0( c, "is Network drive" );

        // Media not accessible
        if ( retc == ERROR_ACCESS_DENIED )
            ErrorExit0( c, "- media is inaccessible" );

        ErrorExit0( c, "- other error" );
    }

    if ( !memcmp( media.midFileSysType, "CDROM", 5 ) ||
            !memcmp( media.midFileSysType, "CD001", 5 ) ||
            !memcmp( media.midFileSysType, "CDAUDIO", 5 ) )
        ErrorExit0( c, "is CD-ROM drive" );

    printf( " Volume %c", c );
    if ( drive_flags & DRIVE_IS_SUBST ) printf( " (Subst drive)" );

    // Does the system support the FAT32 extensions? (OSR2 or newer)
    retc = GetExtendedDPB( vol1 );
    if ( retc < 0 )
        printf( " is FAT12/FAT16, standard DPB.\n" );
    if ( retc > 0 )
        printf( " is FAT12/FAT16, extended DPB.\n" );
    if ( retc == 0 )
    {
        printf( " is FAT32, extended DPB.\n" );
        bFat32 = TRUE;
    }

    // Get the Level1 Exclusive Volume Lock for the Volume
    EmitString( "== Lock Logical Vol" );
    retc = LockLogicalVolume( vol1, LEVEL1_LOCK, LEVEL1_LOCK_MAX_PERMISSION, bFat32 );
    if ( retc && bFat32 )   // try again using FAT12/FAT16
    {
        retc = LockLogicalVolume( vol1, LEVEL1_LOCK, LEVEL1_LOCK_MAX_PERMISSION, FALSE );
        if ( retc != 0 ) ErrorExit1( retc, c );
    }

    pReadBuf = (BYTE *)malloc( 2048 );
    EmitString( "== Read Boot Sector (0)" );
    if ( bFat32 )
    {
        if ( ReadAbsoluteSectors32( vol1, 0, 1, pReadBuf ) != 0 )
            ErrorExit2( vol1, c, 0, bFat32, pReadBuf );
    }
    else if ( ReadAbsoluteSectors( volz, 0, 1, pReadBuf ) < 0 )
        ErrorExit2( vol1, c, 0, bFat32, pReadBuf );

    memset( szFSType, 0, sizeof( szFSType ) );

    // Store some disk parameters for later calculations
    if ( bFat32 )
    {
        memcpy( &bs32, pReadBuf, sizeof( BOOTSECTOR32 ) );
        memcpy( szFSType, bs32.bsFileSysType, 8 );
        dwReservedSectors = bs32.bpb.A_BF_BPB_ReservedSectors;
        sector_size = bs32.bpb.A_BF_BPB_BytesPerSector;
        sectors_per_fat = (bs32.bpb.A_BF_BPB_BigSectorsPerFatHi << 16) +
                          bs32.bpb.A_BF_BPB_BigSectorsPerFat;
        sectors_per_clus = bs32.bpb.A_BF_BPB_SectorsPerCluster;
        DumpBootSector32();
    }
    else
    {
        memcpy( &bs, pReadBuf, sizeof( BOOTSECTOR ) );
        memcpy( szFSType, bs.bsFileSysType, 8 );
        dwReservedSectors = bs.bsResSectors;
        sector_size = bs.bsBytesPerSec;
        sectors_per_fat = bs.bsFATsecs;
        sectors_per_clus = bs.bsSecPerClust;
        DumpBootSector();
    }

    // If this is a FAT32 volume, display its BIGFATBOOTFSINFO structure
    if ( bFat32 && bs32.bpb.A_BF_BPB_FSInfoSec != 0xffff )
    {
        if ( ReadAbsoluteSectors32( vol1, bs32.bpb.A_BF_BPB_FSInfoSec, 1, pReadBuf ) != 0 )
            ErrorExit2( vol1, c, bs32.bpb.A_BF_BPB_FSInfoSec, bFat32, pReadBuf );
        DumpFSInfo( bs32.bpb.A_BF_BPB_FSInfoSec, pReadBuf );
    }

    free( pReadBuf );

    if ( !bFat32 )
    {
        if ( !strncmp( szFSType, "FAT12", 5 ) ) bFat12 = TRUE;
        else if ( !strncmp( szFSType, "FAT16", 5 ) ) bFat16 = TRUE;
    }

    first_fat_sector1 = dwReservedSectors;
    pReadBuf = (BYTE *)malloc( sector_size );
    sprintf( szOut, "== Start Read First FAT (%lxH)", first_fat_sector1 );
    EmitString( szOut );

    // Read the first sector of the first FAT
    if ( bFat32 )
    {
        if ( ReadAbsoluteSectors32( vol1, first_fat_sector1, 1, pReadBuf ) != 0 )
            ErrorExit2( vol1, c, first_fat_sector1, bFat32, pReadBuf );
    }
    else if ( ReadAbsoluteSectors( volz, first_fat_sector1, 1, pReadBuf ) < 0 )
        ErrorExit2( vol1, c, first_fat_sector1, bFat32, pReadBuf );

    printf( "\n Sector %d - First File Allocation Table ...\n", dwReservedSectors );
    DumpFat( pReadBuf, bFat12, bFat16, bFat32 );
    printf( "  ...\n" );

    memset( pReadBuf, 0, sector_size );
    second_fat_sector1 = dwReservedSectors + sectors_per_fat;
    sprintf( szOut, "== Start Read Second FAT (%lxH)", second_fat_sector1 );
    EmitString( szOut );

    // Read the first sector of the second FAT
    if ( bFat32 )
    {
        if ( ReadAbsoluteSectors32( vol1, second_fat_sector1, 1, pReadBuf ) != 0 )
            ErrorExit2( vol1, c, second_fat_sector1, bFat32, pReadBuf );
    }
    else if ( ReadAbsoluteSectors( volz, second_fat_sector1, 1, pReadBuf ) < 0 )
        ErrorExit2( vol1, c, second_fat_sector1, bFat32, pReadBuf );

    printf( "\n Sector %d - Second File Allocation Table ...\n", second_fat_sector1 );
    DumpFat( pReadBuf, bFat12, bFat16, bFat32 );
    printf( "  ...\n" );
    free( pReadBuf );

    // Dump out the Root Directory Entries
    if ( !bFat32 )
    {
        unsigned short sect_cnt = ((32 * bs.bsRootDirEnts) + sector_size - 1) / sector_size;
        DWORD root_dir_ent = dwReservedSectors + sectors_per_fat * 2;
        bufsize = sect_cnt * sector_size;
        pReadBuf = malloc( bufsize );
        memset( pReadBuf, 0, bufsize );

        sprintf( szOut, "== Start Read Root DIR (%lxH)", root_dir_ent );
        EmitString( szOut );
        if ( ReadAbsoluteSectors( volz, root_dir_ent, sect_cnt, pReadBuf ) < 0 )
            ErrorExit2( vol1, c, root_dir_ent, bFat32, pReadBuf );
        printf( "\n Sector %d - Root Directory Entries ...\n", root_dir_ent );
        DumpDirEntries( pReadBuf, bufsize, bFat32 );
        printf( "  ...\n" );
        free( pReadBuf );

        printf( "\n Sector %d - First available cluster.\n", root_dir_ent + sect_cnt );
    }
    else
    {
        DWORD dwRootDirStartClus, dwStartSector, dwRootDirStartSector;
        bufsize = sectors_per_clus * sector_size;
        pReadBuf = (BYTE *)malloc( bufsize );
        dwRootDirStartClus = (bs32.bpb.A_BF_BPB_RootDirStrtClusHi << 16) +
                             bs32.bpb.A_BF_BPB_RootDirStrtClus;
        dwRootDirStartClus -= 2; // the first two clusters in the FAT are reserved
        dwStartSector = dwReservedSectors + sectors_per_fat * 2;
        dwRootDirStartSector = dwStartSector +
                               dwRootDirStartClus * bs32.bpb.A_BF_BPB_SectorsPerCluster;

        //      For FAT32 drives, space is not reserved for Root Directory Entries.
        //      The Root Directory Entries are allocated clusters just like any
        //      other file.
        printf( "\n Sector %d - First available cluster.\n", dwStartSector );

        //      Read in one whole cluster of the Root Directory cluster chain
        if ( ReadAbsoluteSectors32( vol1, dwRootDirStartSector, sectors_per_clus, pReadBuf ) != 0 )
            ErrorExit2( vol1, c, dwRootDirStartSector, bFat32, pReadBuf );

        printf( "\n Sector %d - Root Directory Entries ...\n", dwRootDirStartSector );
        DumpDirEntries( pReadBuf, bufsize, bFat32 );
        printf( "  ...\n" );
        free( pReadBuf );
    }

    EmitString( "== Unlock Logical Vol" );
    UnlockLogicalVolume( vol1, bFat32 );

    CloseHandle( hVWin32Device );
    EmitString( "==== diskdump ends ====" );
    CloseTagmon();
    exit( 0 );
}
