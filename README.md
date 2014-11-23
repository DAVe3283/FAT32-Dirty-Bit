FAT32-Dirty-Bit
===============

A program to set/clear the FAT32 dirty bit under Windows 98, as a workaround to make the [isostick](http://isostick.com/) work as intended.

Whaa? Why toggle the dirty bit?
-------------------------------

As of this time (2014-11-23) the isostick firmware will not reload the config file until the FAT32 dirty bit is cleared (which requires it to also have been set first). The details are on this [trello page](https://trello.com/c/HEkjD5c7/18-faster-reloading-of-config-file-no-need-to-eject-flash-on-osx-nix) and this [forum post](http://reboot.pro/topic/18030-lets-talk-dirty-bit/).

The Windows 98 3rd party USB stack (and/or removable drive cache policy) is setup in such a way that the FAT32 dirty bit never gets set. This is similar to (technically opposite of) the Linux / Mac OS X setup which also causes the isostick bug.

After some testing, we found that manually toggling byte 0x41 of the first sector of the partition (to 0x01 then back to 0x00) would cause the isostick to reload the config file and mount a new image! We were able to use the [HxD Hex Editor](http://mh-nexus.de/en/hxd/) to do this.

Isn't there a better way?
-------------------------

Using a hex editor on a live disk is a complex and unnecessarily dangerous process, yes. So we began looking for existing programs to do this.

### fsutil.exe

It seems that fsutil.exe would do exactly what we want, but it came out with Windows XP. After some (half-assed) attempts to run it on Windows 98, we found that it tries some API calls that are not present in Windows 98.

While [KernelEx](http://kernelex.sourceforge.net/) might let this work, we wanted to keep our Windows 98 install stock at the moment.

### dd

dd would seem to be the next most obvious choice. We did manage to find a version of [dd that runs under Windows 98](http://www.chrysocome.net/dd), but "at the moment block devices under Win9x are not supported but that will be added soon".

Since soon is not good enough, we decided we would have to write our own utility. And this is it.