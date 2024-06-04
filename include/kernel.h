#ifndef INCLUDED_KERNELINPUT_H
#define INCLUDED_KERNELINPUT_H

typedef struct {
    EFI_SYSTEM_TABLE* SystemTablePtr;
    struct {
        uintn mode;//1:rgb 2:rbg
        uintn startAddr;
        uintn scanlineWidth;
        uintn width;
        uintn height;
    } Graphic;
    struct {
        uintn ramSize;
        uintn* availableRamMap;//per 1 byte represent 1 pages'owner id (4KiB) (0:unavailable 1:available 2<=:ownerid)
    } Ram;
} KernelInputStruct;

typedef int (KernelEntryPoint)(KernelInputStruct*);

#endif
