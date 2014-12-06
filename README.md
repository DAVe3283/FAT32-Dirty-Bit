FAT32 Dirty Bit
===============

A program to set/clear the FAT32 dirty bit under Windows 98, as a workaround to make the [isostick][] work as intended.

It also lets you drop ISO files directly on the executable to mount them automatically.

Download
--------

You can download the compiled executable [here][downloadLink].

Install
-------

Copy the executable to your isostick somewhere. Or in every subfolder, for the lazy. It's only 48 [KiB][kibibyte].

Usage
-----

If you are using this with an isostick (our main use case), you can simply drag an ISO onto the executable, and it should mount automatically!

Incidentally, this program works on Windows 7 as well. Windows 7 doesn't allow raw block access (which is a good thing), so the program can't toggle the dirty bit, but the config file _does_ get updated correctly before the program fails. But, happily, Windows 7 toggles the dirty bit automatically, so everything works out!

It might work on other versions of Windows too. Or it might format your hard drive, or [make demons fly out of your nose][nasal demons].

If you are doing more nefarious things, there are command line arguments for you:
```
D:\>DirtyBit.exe
Sets or clears the FAT32 dirty bit on a volume.
Optionally updates the ISO selection on an isostick.

DirtyBit.exe set|clear drive:
DirtyBit.exe disk_image.iso

  set|clear         set or clear the FAT32 dirty bit
  drive:            drive letter (for example d:)
  disk_image.iso    Full path to an ISO file
```

Compiling
---------

It builds great in Visual Studio 6 on Windows 98!

The most pain-free way to do this is to install [Microsoft Virtual PC 2007][VirtualPC].

I recommend Virtual PC because [its origins trace back to Connectix Virtual PC][vpcHistory], which was designed for, and ran on, Windows 98. This support was carried on [through Virtual PC 2007][vpcOsSupport] (but not Windows Virtual PC). This means full guest integration, which is invaluable to maintaining sanity.

I recommend Visual Studio 6 because that is what we had on hand, and wrote this program in. It might compile fine on other versions, but it might not. Since this code is dealing with low-level disk IO, I strongly recommend sticking with an age-appropriate compiler and OS.

Once the (admittedly ancient) environment is setup, simply checkout the repo, and open the workspace (`FAT32 Dirty Bit.dsw`) in Visual C++ 6. Go to the Build menu, and choose Batch Build. Ensure the Release config is selected, and hit build.

Copy your shiny new executable from the Release directory to your isostick, and you're off! (If things don't work, try renaming the executable so there are no spaces in its name.)

Whaa? Why toggle the dirty bit?
-------------------------------

As of this time (2014-12-05) the isostick firmware will not reload the config file until the FAT32 dirty bit is cleared (which requires it to also have been set first). The details are on this [trello page][trelloBug] and this [forum post][].

The Windows 98 3rd party USB stack (and/or removable drive cache policy) is setup in such a way that the FAT32 dirty bit never gets set. This is similar to (technically opposite of) the Linux / Mac OS X setup which also causes the isostick bug.

After some testing, we found that manually toggling byte 0x41 of the first sector of the partition (to 0x01 then back to 0x00) would cause the isostick to reload the config file and mount a new image! We were able to use the [HxD Hex Editor][HxD] to do this.

Isn't there a better way?
-------------------------

Using a hex editor on a live disk is a complex and unnecessarily dangerous process, yes. So we began looking for existing programs to do this.

### fsutil.exe

It seems that fsutil.exe would do exactly what we want, but it came out with Windows XP. After some (half-assed) attempts to run it on Windows 98, we found that it tries some API calls that are not present in Windows 98.

While [KernelEx][] might let this work, we wanted to keep our Windows 98 install stock at the moment.

### dd

dd would seem to be the next most obvious choice. We did manage to find a version of [dd that runs under Windows 98][ddWin98], but "at the moment block devices under Win9x are not supported but that will be added soon".

Since soon is not good enough, we decided we would have to write our own utility. And this is it.

[isostick]:     http://isostick.com/
[downloadLink]: https://drive.google.com/file/d/0B3hK6MOhVmfAZ0R0R1NiS3loVTg/view?usp=sharing
[kibibyte]:     http://en.wikipedia.org/wiki/Kibibyte "kibibyte"
[nasal demons]: http://catb.org/jargon/html/N/nasal-demons.html
[trelloBug]:    https://trello.com/c/HEkjD5c7/18-faster-reloading-of-config-file-no-need-to-eject-flash-on-osx-nix
[forum post]:   http://reboot.pro/topic/18030-lets-talk-dirty-bit/ "Let's talk dirty (bit)"
[HxD]:          http://mh-nexus.de/en/hxd/
[KernelEx]:     http://kernelex.sourceforge.net/
[ddWin98]:      http://www.chrysocome.net/dd
[VirtualPC]:    http://www.microsoft.com/en-us/download/details.aspx?id=4580
[vpcHistory]:   http://en.wikipedia.org/wiki/Windows_Virtual_PC#History
[vpcOsSupport]: http://en.wikipedia.org/wiki/Windows_Virtual_PC#Table_of_supported_operating_systems
