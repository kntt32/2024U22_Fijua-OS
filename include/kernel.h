#ifndef INCLUDED_KERNELINPUT_H
#define INCLUDED_KERNELINPUT_H

#include <efi.h>
#include <efi_loaded_image_protocol.h>

typedef struct {
        uintn mode;//1:rgb 2:bgr
        uintn startAddr;
        uintn scanlineWidth;
        uintn width;
        uintn height;
    } GraphicData;

typedef struct {
    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    GraphicData Graphic;
    struct {
        uintn ramSize;
        uintn* availableRamMap;//per 2 byte represent 1 pages'owner id (4KiB) (0:unavailable 1:available 2<=:ownerid)
    } Ram;
    void* tester;
} KernelInputStruct;

typedef int (KernelEntryPoint)(KernelInputStruct*);

#endif
