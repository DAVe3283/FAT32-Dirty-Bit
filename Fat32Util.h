// Does the sector buffer contain a valid FAT32 Volume ID?
bool ValidFat32(char sector[]);

// Is the dirty bit set? (must be a valid FAT32 Volume ID!)
bool GetVolumeDirty(void* sector);

// Set the dirty bit (must be a valid FAT32 Volume ID!)
bool SetVolumeDirty(void* sectro);
