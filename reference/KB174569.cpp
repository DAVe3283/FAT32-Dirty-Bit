#include <windows.h>

#define VWIN32_DIOC_DOS_INT25     2
#define VWIN32_DIOC_DOS_INT26     3
#define VWIN32_DIOC_DOS_DRIVEINFO 6

typedef struct _DIOC_REGISTERS {
    DWORD reg_EBX;
    DWORD reg_EDX;
    DWORD reg_ECX;
    DWORD reg_EAX;
    DWORD reg_EDI;
    DWORD reg_ESI;
    DWORD reg_Flags;
} DIOC_REGISTERS, *PDIOC_REGISTERS;

#define CARRY_FLAG 1

#pragma pack(1)
typedef struct _DISKIO {
    DWORD  dwStartSector;   // starting logical sector number
    WORD   wSectors;        // number of sectors
    DWORD  dwBuffer;        // address of read/write buffer
} DISKIO, * PDISKIO;
#pragma pack()

/*------------------------------------------------------------------
ReadLogicalSectors (hDev, bDrive, dwStartSector, wSectors, lpSectBuff)

Purpose:
    Reads sectors from a logical drive.  Uses Int 25h.

Parameters:
    hDev
        Handle of VWIN32

    bDrive
        The MS-DOS logical drive number. 1 = A, 2 = B, 3 = C, etc.

    dwStartSector
        The first logical sector to read

    wSectors
        The number of sectors to read

    lpSectBuff
        The caller-supplied buffer that will contain the sector data

Return Value:
    Returns TRUE if successful, or FALSE if failure.

Comments:
    This function does not validate its parameters.
------------------------------------------------------------------*/
BOOL ReadLogicalSectors (HANDLE hDev,
                         BYTE   bDrive,
                         DWORD  dwStartSector,
                         WORD   wSectors,
                         LPBYTE lpSectBuff)
{
    BOOL           fResult;
    DWORD          cb;
    DIOC_REGISTERS reg = {0};
    DISKIO         dio = {0};

    dio.dwStartSector = dwStartSector;
    dio.wSectors      = wSectors;
    dio.dwBuffer      = (DWORD)lpSectBuff;

    reg.reg_EAX = bDrive - 1;   // Int 25h drive numbers are 0-based.
    reg.reg_EBX = (DWORD)&dio;
    reg.reg_ECX = 0xFFFF;       // use DISKIO struct

    fResult = DeviceIoControl(hDev, VWIN32_DIOC_DOS_INT25,
                              &reg, sizeof(reg),
                              &reg, sizeof(reg), &cb, 0);

    // Determine if the DeviceIoControl call and the read succeeded.
    fResult = fResult && !(reg.reg_Flags & CARRY_FLAG);

    return fResult;
}


/*------------------------------------------------------------------
WriteLogicalSectors (hDev, bDrive, dwStartSector, wSectors, lpSectBuff)

Purpose:
    Writes sectors to a logical drive. Uses Int 26h

Parameters:
    hDev
        Handle of VWIN32

    bDrive
        The MS-DOS logical drive number. 1 = A, 2 = B, 3 = C, etc.

    dwStartSector
        The first logical sector to write

    wSectors
        The number of sectors to write

    lpSectBuff
        The caller-supplied buffer that contains the sector data

Return Value:
    Returns TRUE if successful, or FALSE if failure.

Comments:
    This function does not validate its parameters.
------------------------------------------------------------------*/
BOOL WriteLogicalSectors (HANDLE hDev,
                          BYTE   bDrive,
                          DWORD  dwStartSector,
                          WORD   wSectors,
                          LPBYTE lpSectBuff)
{
    BOOL           fResult;
    DWORD          cb;
    DIOC_REGISTERS reg = {0};
    DISKIO         dio = {0};

    dio.dwStartSector = dwStartSector;
    dio.wSectors      = wSectors;
    dio.dwBuffer      = (DWORD)lpSectBuff;

    reg.reg_EAX = bDrive - 1;   // Int 26h drive numbers are 0-based.
    reg.reg_EBX = (DWORD)&dio;
    reg.reg_ECX = 0xFFFF;       // use DISKIO struct

    fResult = DeviceIoControl(hDev, VWIN32_DIOC_DOS_INT26,
                              &reg, sizeof(reg),
                              &reg, sizeof(reg), &cb, 0);

    // Determine if the DeviceIoControl call and the write succeeded.
    fResult = fResult && !(reg.reg_Flags & CARRY_FLAG);

    return fResult;
}


/*------------------------------------------------------------------
NewReadSectors(hDev, bDrive, dwStartSector, wSectors, lpSectBuff)

Purpose:
    Reads the specified number of sectors into a caller-supplied
    buffer. Uses Int 21h function 7305h

Parameters:
    hDev
        Handle of VWIN32

    bDrive
        The MS-DOS logical drive number. 0 = default, 1 = A, 2 = B,
        3 = C, etc.

    dwStartSector
        The first sector to read.

    wSectors
        The number of sectors to read.

    lpSectBuff
        The caller-supplied buffer to read into.

Return Value:
    Returns TRUE if successful, or FALSE if failure.

Comments:
    This function does not validate its parameters.  It assumes that
    lpSectBuff is allocated by the caller and is large enough to
    hold all of the data from all of the sectors being read.
------------------------------------------------------------------*/
BOOL NewReadSectors (HANDLE hDev,
                     BYTE   bDrive,
                     DWORD  dwStartSector,
                     WORD   wSectors,
                     LPBYTE lpSectBuff)
{
    BOOL           fResult;
    DWORD          cb;
    DIOC_REGISTERS reg = {0};
    DISKIO         dio;

    dio.dwStartSector = dwStartSector;
    dio.wSectors      = wSectors;
    dio.lpBuffer      = (DWORD)lpSectBuff;

    reg.reg_EAX = 0x7305;   // Ext_ABSDiskReadWrite
    reg.reg_EBX = (DWORD)&dio;
    reg.reg_ECX = -1;
    reg.reg_EDX = bDrive;   // Int 21h, fn 7305h drive numbers are 1-based

    fResult = DeviceIoControl(hDev, VWIN32_DIOC_DOS_DRIVEINFO,
                              &reg, sizeof(reg),
                              &reg, sizeof(reg), &cb, 0);

    // Determine if the DeviceIoControl call and the read succeeded.
    fResult = fResult && !(reg.reg_Flags & CARRY_FLAG);

    return fResult;
}


/*------------------------------------------------------------------
NewWriteSectors(hDev, bDrive, dwStartSector, wSectors, lpSectBuff)

Purpose:
    Writes the specified number of sectors from a caller-supplied
    buffer. Uses Int 21h function 7305h

Parameters:
    hDev
        Handle of VWIN32

    bDrive
        The MS-DOS logical drive number. 0 = default, 1 = A, 2 = B,
        3 = C, etc.

    dwStartSector
        The first sector to write.

    wSectors
        The number of sectors to write.

    lpSectBuff
        The caller-supplied buffer from which to write.

Return Value:
    Returns TRUE if successful, or FALSE if failure.

Comments:
    This function does not validate its parameters.  It assumes that
    lpSectBuff is allocated by the caller and is large enough to
    hold all of the data to be written.
------------------------------------------------------------------*/
BOOL NewWriteSectors (HANDLE hDev,
                     BYTE   bDrive,
                     DWORD  dwStartSector,
                     WORD   wSectors,
                     LPBYTE lpSectBuff)
{
    BOOL           fResult;
    DWORD          cb;
    DIOC_REGISTERS reg = {0};
    DISKIO         dio;

    dio.dwStartSector = dwStartSector;
    dio.wSectors      = wSectors;
    dio.lpBuffer      = (DWORD)lpSectBuff;

    reg.reg_EAX = 0x7305;   // Ext_ABSDiskReadWrite
    reg.reg_EBX = (DWORD)&dio;
    reg.reg_ECX = -1;
    reg.reg_EDX = bDrive;   // Int 21h, fn 7305h drive numbers are 1-based

    reg.reg_ESI = 0x6001;   // Normal file data (See function
                            // documentation for other values)


    fResult = DeviceIoControl(hDev, VWIN32_DIOC_DOS_DRIVEINFO,
                              &reg, sizeof(reg),
                              &reg, sizeof(reg), &cb, 0);

    // Determine if the DeviceIoControl call and the write succeeded.
    fResult = fResult && !(reg.reg_Flags & CARRY_FLAG);

    return fResult;
}
