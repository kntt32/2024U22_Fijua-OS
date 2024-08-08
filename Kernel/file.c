#include <types.h>
#include <kernel.h>
#include <efi_simple_file_system_protocol.h>
#include <efi_file_protocol.h>
#include <efi_file_info.h>
#include "file.h"
#include "console.h"
#include "x64.h"

extern KernelInputStruct* KernelInput;

static EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* Efi_fileProtocol;


//初期化
void File_Init(void) {
    uintn status;
    EFI_GUID Efi_simpleFileSystemProtocol_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* Efi_simpleFileSystemProtocol = NULL;

    Console_Print("File_Init: Initializing...\n");

    status = Efi_Wrapper(
        KernelInput->LoadedImage->SystemTable->BootServices->HandleProtocol,
        KernelInput->LoadedImage->DeviceHandle,
        &Efi_simpleFileSystemProtocol_guid,
        &Efi_simpleFileSystemProtocol);
    if(status) {
        Console_Print("File_Init: Err: Couldn't get EFI_SIMPLE_FILE_SYSTEM_PROTOCOL\n");
        while(1) Hlt();
    }

    //Open Volume
    status = Efi_Wrapper(
        Efi_simpleFileSystemProtocol->OpenVolume,
        Efi_simpleFileSystemProtocol,
        &Efi_fileProtocol);
    if(status) {
        Console_Print("File_Init: Err: Couldn't get EFI_FILE_PROTOCOL\n");
        while(1) Hlt();
    }

    return;
}


//linux形式のPathをWindows形式に変換
uintn File_ConvertPath(ascii path[]) {
    if(path == NULL) return 1;

    for(uintn i=0; 1; i++) {
        if(path[i] == '/') path[i] = '\\';
        if(path[i] == '\0') break;
    }

    return 0;
}


//asciiのPathをutf16leに変換
uintn File_ConvertPath_2UTF16LE(const ascii path[], uint16 output, uintn buffSize) {

}


//ディレクトリの内容物取得 絶対パス
uintn File_GetDirectory(const ascii path[], uintn* buffSize, ascii buff[]) {
    /*
    buffにファイル名がヌル文字で分けられて書き込まれる
    *buffSizeに書き込まれたサイズが返される
    asciiの範囲のファイル名のみサポート
    */
    if(path == NULL) return 1;





    return 0;
}

