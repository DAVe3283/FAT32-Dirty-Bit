#include "stdafx.h"

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
    dio.dwBuffer      = (DWORD)lpSectBuff;

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
    dio.dwBuffer      = (DWORD)lpSectBuff;

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



int LockLogicalVolume(HANDLE hVWin32Device, int volume, int lock_level,
                      int permissions, BOOL isFat32 )
{
    DIOC_REGISTERS reg;
    BOOL bResult;
    DWORD cb;

    reg.reg_EAX = 0x440d; // generic IOCTL
    reg.reg_ECX = isFat32 ? 0x484a : 0x084a; // lock logical volume
    reg.reg_EBX = volume | (lock_level << 8);
    reg.reg_EDX = permissions;
    reg.reg_Flags = 1; // preset the carry flag

    bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
                               &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

    if ( !bResult || (reg.reg_Flags & 1) )
        return (reg.reg_EAX & 0xffff);

    return 0;
}


int UnlockLogicalVolume(HANDLE hVWin32Device, int volume, BOOL isFat32 )
{
    DIOC_REGISTERS reg;
    BOOL bResult;
    DWORD cb;

    reg.reg_EAX = 0x440d;
    reg.reg_ECX = isFat32 ? 0x486a : 0x086a; // lock logical volume
    reg.reg_EBX = volume;
    reg.reg_Flags = 1; // preset the carry flag

    bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
                               &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

    if ( !bResult || (reg.reg_Flags & 1) ) return -1;
    return 0;
}
