#ifndef INCLUDED_PARTITION_H
#define INCLUDED_PARTITION_H

typedef enum {
    Partition_Type_UnKnown,
    Partition_Type_Mbr
} Partition_Type;

typedef struct {
    uint8 isActive;
    uintn startSector;
    uintn sectorCount;
} Partition_Descriptor;

void Partition_Init(void);

uintn Partition_Count(void);

#endif
