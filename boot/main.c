#include <efi.h>
#include <efi_simple_file_system_protocol.h>
#include <efi_file_info.h>
#include <efi_simple_pointer_protocol.h>

#define NULL ((void*)0)

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
    status = SystemTable->BootServices->AllocatePages(AllocateAnyPage, EfiLoaderData, (kernelSize>>12)+1, buff_kernelfile);
    if(status) err(SystemTable);
    

    //expand kernelfile

    


    while(1);

    return 0;
}