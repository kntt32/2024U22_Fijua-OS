#include <efi.h>
#include <efi_simple_file_system_protocol.h>
#include <efi_file_info.h>
#include <efi_simple_pointer_protocol.h>

#include "elfloader.h"

#define NULL ((void*)0)

#define ELFLOADERMEMLOADAREA_BUFFSIZE (20)


typedef struct _AllocatedPageTable{
    EFI_PHYSICAL_ADDRESS pageStart;
    uintn pages;
} ExpandKernel_AllocatedPageTable;


void __chkstk() {}


void err(IN EFI_SYSTEM_TABLE* SystemTable) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Err");
    while(1);
}


EFI_STATUS efi_main(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE* SystemTable) {
    EFI_STATUS status = 0;
    //console test
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello, World!!\n\r");

    //get EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting EFI_SIMPLE_FILE_SYSTEM_PROTOCOL\n\r");
    static EFI_GUID efiSimpleFileSystemProtocol_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* efiSimpleFileSystemProtocol_volume = NULL;
    status = SystemTable->BootServices->LocateProtocol(&efiSimpleFileSystemProtocol_guid, NULL, (void*)&efiSimpleFileSystemProtocol_volume);
    if(status) err(SystemTable);
    
    //get EFI_FILE_PROTOCOL:root handle
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting EFI_FILE_PROTOCOL:root handle\n\r");
    EFI_FILE_PROTOCOL* efiFileProtocol_root = NULL;
    status =efiSimpleFileSystemProtocol_volume->OpenVolume(efiSimpleFileSystemProtocol_volume, &efiFileProtocol_root);
    if(status) err(SystemTable);
    
    //get EFI_FILE_PROTOCOL:kernelfile handle
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting EFI_FILE_PROTOCOL:kernelfile handle\n\r");
    EFI_FILE_PROTOCOL* efiFileProtocol_kernelfile = NULL;
    status = efiFileProtocol_root->Open(efiFileProtocol_root, &efiFileProtocol_kernelfile, L"kernel.elf", EFI_FILE_MODE_READ, 0);
    if(status) err(SystemTable);

    //get kernelfile size
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting kernelfile size\n\r");
    UINTN fileInfo_kernelfile_size = 0;
    static EFI_GUID efiFileInfoID_guid = EFI_FILE_INFO_ID;
    efiFileProtocol_kernelfile->GetInfo(efiFileProtocol_kernelfile, &efiFileInfoID_guid, &fileInfo_kernelfile_size, NULL);
    char fileInfo_kernelfile_buff[fileInfo_kernelfile_size];
    EFI_FILE_INFO* fileInfo_kernelfile = (void*)fileInfo_kernelfile_buff;
    status = efiFileProtocol_kernelfile->GetInfo(efiFileProtocol_kernelfile, &efiFileInfoID_guid, &fileInfo_kernelfile_size, fileInfo_kernelfile);
    if(status) err(SystemTable);
    unsigned int kernelSize = fileInfo_kernelfile->FileSize;

    //open kernelfile
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Opening kernelfile\n\r");
    char* buff_kernelfile = NULL;
    status = SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, ((kernelSize+0xffe)>>12), (EFI_PHYSICAL_ADDRESS*)&buff_kernelfile);
    if(status) err(SystemTable);

    //read kernelfile to buffer
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Reading kernelfile to buffer\n\r");
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
    static ExpandKernel_AllocatedPageTable allocatePageTable[ELFLOADERMEMLOADAREA_BUFFSIZE];

    EFI_PHYSICAL_ADDRESS allocatePageTable_prevPageStart = 0;
    EFI_PHYSICAL_ADDRESS allocatePageTable_prevPageEnd   = 0;
    EFI_PHYSICAL_ADDRESS allocatePageTable_thisPageStart = 0;
    EFI_PHYSICAL_ADDRESS allocatePageTable_thisPageEnd   = 0;
    for(uintn i=0; i<elfloaderMemloadarea_buffCount; i++) {
        allocatePageTable_thisPageStart = allocatePageTable[i].pageStart;
        allocatePageTable_thisPageEnd   = allocatePageTable[i].pageStart + (allocatePageTable[i].pages<<12);

        allocatePageTable[i].pageStart = (EFI_PHYSICAL_ADDRESS)(elfloaderMemloadarea_buff[i].startAddr);
        allocatePageTable[i].pages = ((elfloaderMemloadarea_buff[i].memSize+0xffe)>>12);

        for(uintn k=0; k<i; k++) {
            if(allocatePageTable[i-1].pages == 0) continue;
            allocatePageTable_prevPageStart = allocatePageTable[k].pageStart;
            allocatePageTable_prevPageEnd   = allocatePageTable[k].pageStart + (allocatePageTable[k].pages<<12);

            
        }
        if(allocatePageTable[i].pages != 0) {
            SystemTable->ConOut->OutputString(SystemTable->ConOut, L";\n\r");
            status = SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, allocatePageTable[i].pages, &(allocatePageTable[i].pageStart));;
            if(status) err(SystemTable);
        }
    }

    //expand kernelfile

    
    //free unnecessary resource
    efiFileProtocol_root->Close(efiFileProtocol_kernelfile);
    efiFileProtocol_kernelfile->Close(efiFileProtocol_kernelfile);



    while(1);

    return 0;
}