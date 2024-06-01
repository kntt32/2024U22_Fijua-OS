#include <efi.h>
#include <efi_simple_file_system_protocol.h>
#include <efi_file_info.h>
#include <efi_graphics_output_protocol.h>

#include "elfloader.h"
#include "subroutine.h"

#define NULL ((void*)0)

#define ELFLOADERMEMLOADAREA_BUFFSIZE (20)


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

KernelInputStruct kernelInput;


void __chkstk() {}


void err(IN EFI_SYSTEM_TABLE* SystemTable) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Err");
    while(1) SystemTable->BootServices->Stall(1000);
}


EFI_STATUS efi_main(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE* SystemTable) {
    EFI_STATUS status = 0;
    uintn tempUintn = 0;
    uintn* tempUintnptr = NULL;
    uint8* tempUint8ptr = NULL;

    //console test
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    SystemTable->ConOut->EnableCursor(SystemTable->ConOut, 1);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello, World!!\n\r");

    //get EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting EFI_SIMPLE_FILE_SYSTEM_PROTOCOL\n\r");
    EFI_GUID efiSimpleFileSystemProtocol_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* efiSimpleFileSystemProtocol_volume = NULL;
    status = SystemTable->BootServices->LocateProtocol(&efiSimpleFileSystemProtocol_guid, NULL, (void*)&efiSimpleFileSystemProtocol_volume);
    if(status) err(SystemTable);
    
    //get EFI_FILE_PROTOCOL:root handle
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting EFI_FILE_PROTOCOL:root handle\n\r");
    EFI_FILE_PROTOCOL* efiFileProtocol_root = NULL;
    status = efiSimpleFileSystemProtocol_volume->OpenVolume(efiSimpleFileSystemProtocol_volume, &efiFileProtocol_root);
    if(status) err(SystemTable);
    
    //get EFI_FILE_PROTOCOL:kernelfile handle
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting EFI_FILE_PROTOCOL:kernelfile handle\n\r");
    EFI_FILE_PROTOCOL* efiFileProtocol_kernelfile = NULL;
    status = efiFileProtocol_root->Open(efiFileProtocol_root, &efiFileProtocol_kernelfile, L"kernel.elf", EFI_FILE_MODE_READ, 0);
    if(status) err(SystemTable);

    //get kernelfile size
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting kernelfile size\n\r");
    UINTN fileInfo_kernelfile_size = 0;
    EFI_GUID efiFileInfoID_guid = EFI_FILE_INFO_ID;
    efiFileProtocol_kernelfile->GetInfo(efiFileProtocol_kernelfile, &efiFileInfoID_guid, &fileInfo_kernelfile_size, NULL);
    char fileInfo_kernelfile_buff[fileInfo_kernelfile_size];
    EFI_FILE_INFO* fileInfo_kernelfile = (void*)fileInfo_kernelfile_buff;
    status = efiFileProtocol_kernelfile->GetInfo(efiFileProtocol_kernelfile, &efiFileInfoID_guid, &fileInfo_kernelfile_size, fileInfo_kernelfile);
    if(status) err(SystemTable);
    unsigned int kernelSize = fileInfo_kernelfile->FileSize;

    //allocate memory to load kernelfile
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Allocating memory to load kernelfile\n\r");
    char* buff_kernelfile = NULL;
    status = SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, ((kernelSize+0xfff)>>12), (EFI_PHYSICAL_ADDRESS*)&buff_kernelfile);
    if(status) err(SystemTable);

    //load kernelfile to buffer
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Loading kernelfile to buffer\n\r");
    UINTN copyof_kernelSize = kernelSize;
    status = efiFileProtocol_kernelfile->Read(efiFileProtocol_kernelfile, &copyof_kernelSize, buff_kernelfile);
    if(status) err(SystemTable);

    //get memarea to expand kernelfile
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting memarea to expand kernelfile\n\r");
    static ElfLoader_MemLoadArea elfloaderMemloadarea_buff[ELFLOADERMEMLOADAREA_BUFFSIZE];
    uintn elfloaderMemloadarea_buffCount = ELFLOADERMEMLOADAREA_BUFFSIZE;
    status = ElfLoader_GetLoadArea(buff_kernelfile, NULL, &elfloaderMemloadarea_buffCount, elfloaderMemloadarea_buff);
    if(status) err(SystemTable);

    //allocate pages to expand kernelfile
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Allocating pages to expand kernelfile\n\r");
    EFI_PHYSICAL_ADDRESS memstart;
    EFI_PHYSICAL_ADDRESS memend;
    for(unsigned int i=0; i<elfloaderMemloadarea_buffCount; i++) {
        if(i == 0 || (EFI_PHYSICAL_ADDRESS)(elfloaderMemloadarea_buff[i].startAddr) < memstart) {
            memstart = (EFI_PHYSICAL_ADDRESS)(elfloaderMemloadarea_buff[i].startAddr);
        }
        if(i == 0 || memend < (EFI_PHYSICAL_ADDRESS)(elfloaderMemloadarea_buff[i].startAddr)+(EFI_PHYSICAL_ADDRESS)(elfloaderMemloadarea_buff[i].memSize)) {
            memend = (EFI_PHYSICAL_ADDRESS)(elfloaderMemloadarea_buff[i].startAddr)+(EFI_PHYSICAL_ADDRESS)(elfloaderMemloadarea_buff[i].memSize);
        }
    }
    memstart = (memstart >> 12) << 12;
    memend = ((memend+0xfff) >> 12) << 12;
    status = SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, (memend-memstart) >> 12, &memstart);
    if(status) err(SystemTable);

    //expand kernelfile
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Expanding kernelfile\n\r");

    //release resource
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Releasing resource\n\r");
    efiFileProtocol_kernelfile->Close(efiFileProtocol_kernelfile);
    efiFileProtocol_root->Close(efiFileProtocol_root);
    SystemTable->BootServices->FreePages((EFI_PHYSICAL_ADDRESS)buff_kernelfile, ((kernelSize+0xfff)>>12));

    //disable dogtimer
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Disable dogtimer\n\r");
    status =  SystemTable->BootServices->SetWatchdogTimer(0, 0x10000, 0, NULL);
    if(status) err(SystemTable);

    //get framebuffer for kernel
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting framebuffer for kernel\n\r");
    EFI_GUID efiGraphicsOutputProtocol_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* efiGraphicsOutputProtocol_interface = NULL;
    status = SystemTable->BootServices->LocateProtocol(&efiGraphicsOutputProtocol_guid, NULL, (VOID**)&efiGraphicsOutputProtocol_interface);
    if(status) err(SystemTable);
    switch(efiGraphicsOutputProtocol_interface->Mode->Info->PixelFormat) {
        case PixelRedGreenBlueReserved8BitPerColor:
            kernelInput.Graphic.mode = 1;
            break;
        case PixelBlueGreenRedReserved8BitPerColor :
            kernelInput.Graphic.mode = 2;
            break;
        default:
            SystemTable->ConOut->OutputString(SystemTable->ConOut, L"This Graphic Mode is Unsupported.\n\r");
            err(SystemTable);
    }
    kernelInput.Graphic.startAddr = efiGraphicsOutputProtocol_interface->Mode->FrameBufferBase;
    kernelInput.Graphic.scanlineWidth = efiGraphicsOutputProtocol_interface->Mode->Info->PixelsPerScanLine;
    kernelInput.Graphic.width = efiGraphicsOutputProtocol_interface->Mode->Info->HorizontalResolution;
    kernelInput.Graphic.height = efiGraphicsOutputProtocol_interface->Mode->Info->VerticalResolution;

    //get memory for kernel
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting memory for Kernel\n\r");
        //get ram size
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"  Getting ramsize\n\r");
        EFI_MEMORY_DESCRIPTOR* memoryMap;
        uintn memoryMapSize = 0;
        uintn mapKey;
        uintn descriptorSize;
        uint32 descriptorVersion;
        SystemTable->BootServices->GetMemoryMap(&memoryMapSize, NULL, &mapKey, &descriptorSize, &descriptorVersion);
        status = SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, (memoryMapSize+descriptorSize+0xfff)>>12, (EFI_PHYSICAL_ADDRESS*)&memoryMap);
        if(status) err(SystemTable);
        memoryMapSize = ((memoryMapSize+descriptorSize+0xfff)>>12)<<12;
        status = SystemTable->BootServices->GetMemoryMap(&memoryMapSize, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
        if(status) err(SystemTable);
        uintn ramSize = 0;
        EFI_MEMORY_DESCRIPTOR* targetMemDescriptor = memoryMap;
        for(uintn i=0; i<memoryMapSize/descriptorSize; i++) {
            tempUintn = (uintn)(targetMemDescriptor->PhysicalStart) + (targetMemDescriptor->NumberOfPages << 12);
            if(targetMemDescriptor->Type == EfiConventionalMemory && ramSize < tempUintn) {
                ramSize = tempUintn;
            }

            targetMemDescriptor = (EFI_MEMORY_DESCRIPTOR*)((EFI_PHYSICAL_ADDRESS)targetMemDescriptor + descriptorSize);
        }

        //set availableRamMap
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"  Setting available ram map\n\r");
        kernelInput.Ram.ramSize = ramSize;
        status = SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, (ramSize+0xfff)>>12, (EFI_PHYSICAL_ADDRESS*)&(kernelInput.Ram.availableRamMap));
        if(status) err(SystemTable);
        tempUintnptr = kernelInput.Ram.availableRamMap;
        for(uintn i=0; i<((ramSize+0xfff)>>12)<<(12-TYPES_UINTN_LN2_SIZE); i++) {
            *tempUintnptr = 0;
            tempUintnptr++;
        }
        status = SystemTable->BootServices->GetMemoryMap(&memoryMapSize, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
        if(status) err(SystemTable);
        targetMemDescriptor = memoryMap;
        tempUint8ptr = (uint8*)(kernelInput.Ram.availableRamMap);
        for(uintn i=0; i<memoryMapSize/descriptorSize; i++) {
            for(uintn k=(uintn)(targetMemDescriptor->PhysicalStart >> 12); k<(uintn)(targetMemDescriptor->PhysicalStart >> 12) + targetMemDescriptor->NumberOfPages; k++) {
                tempUint8ptr[k] = 1;
            }
            targetMemDescriptor = (EFI_MEMORY_DESCRIPTOR*)((EFI_PHYSICAL_ADDRESS)targetMemDescriptor + descriptorSize);
        }



    //run kernel
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Starting Kernel...\n\r");


    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Kernel returned\n\r");
    
    while(1) SystemTable->BootServices->Stall(1000);

    return 0;
}
