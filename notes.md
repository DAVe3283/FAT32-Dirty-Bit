Notes
=====

Microsoft Sample Code
---------------------

There is a Windows 95 bug report ([KB174569](http://support.microsoft.com/kb/174569)) that includes source code for how to read and write disk sectors directly.

The sample code is in reference\KB174569.cpp.

Codeguru Sample
---------------

There is a sample program [on codeguru](http://www.codeguru.com/cpp/w-p/system/misc/article.php/c5765/ReadingWriting-Disk-Sectors-Absolute-Disk-ReadWrite.htm) that will read sectors off the disk.

It appears to use the same technique as the Microsoft sample code, but includes support for Windows NT-based operating systems (Windows XP & newer).

What to use
-----------
Since newer OS support doesn't matter for our use case (and they have fsutil.exe), I suggest we use the Microsoft sample code as the basis for the program.

Atomicity
---------
Since directly touching the FAT32 file system header sector is dangerous, and the Windows 98 API calls for doing this only allow per-sector operations, it is important that this application not be interrupted.

This operation _can_ be made atomic! qwindelzorf found a function to call to lock the drive, so there is no longer any danger of interruption and/or disk corruption. This is referenced in the MSDN documentation for Visual Studio 6, but I am not sure where he found this info, since we did not yet have the MSDN CDs at that point.

Incidentally, this was implemented in 9e69021e3c16928214b40d31bf8214bff9ec8246.

FAT32 Notes
-----------

* [FAT32 File System Specification (Microsoft)](http://staff.washington.edu/dittrich/misc/fatgen103.pdf)
* [How FAT Works (Microsoft)](http://technet.microsoft.com/en-us/library/cc776720%28v=ws.10%29.aspx)
