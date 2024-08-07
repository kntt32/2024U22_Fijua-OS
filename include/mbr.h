#ifndef INCLUDED_MBR_H
#define INCLUDED_MBR_H

/*
https://wiki.osdev.org/MBR_(x86)#MBR_Format
*/

typedef struct {
    uint8 bootstrap[440];//MBR Bootstrap
    uint32 uniqueDiskId : 4*8;
    uint16 _reserved : 2*8;
    uint8 partitionEntry1[16];
    uint8 partitionEntry2[16];
    uint8 partitionEntry3[16];
    uint8 partitionEntry4[16];
    uint16 signature : 2*8;
} Mbr;

typedef struct {
    uint8 driveAttributes : 1*8;
    uint32 chsAddrPartitionStart : 3*8;
    uint8 partitionType : 1*8;
    uint32 chsAddrPartitionEnd : 3*8;
    uint32 lbaPartitionStart : 4*8;
    uint32 numberOfSectors : 4*8;
} Mbr_PartitionEntry;

#endif
