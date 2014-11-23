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
