#include <types.h>
#include <kernel.h>
#include <efi_simple_file_system_protocol.h>
#include <efi_file_protocol.h>
#include <efi_file_info.h>
#include "file.h"
#include "console.h"
#include "x64.h"
#include "functions.h"

extern KernelInputStruct* KernelInput;

static EFI_FILE_PROTOCOL* Efi_fileProtocol;


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


//ディレクトリの内容物取得 絶対パス
uintn File_GetDirectory(const ascii path[], uintn* buffCount, File_Directory buff[]) {
    /*
    buffにファイルリストが書き込まれる
    *buffSizeに書き込まれたサイズが返される
    asciiの範囲のファイル名のみサポート
    */
    if(path == NULL || buffCount == NULL || (*buffCount != 0 && buff == NULL)) return 1;
    uintn status;

    EFI_FILE_PROTOCOL* temp_Efi_fileProtocol = NULL;
    
    if(path[0] != '\0') {
        //パスを変換
        uintn pathStrLength = Functions_CountStr(path); 
        uint16 pathTemp[pathStrLength+1];
        for(uintn i=0; i<pathStrLength+1; i++) {
            Functions_ASCII2UTF16LE((path[i] == '/')?('\\'):(path[i]), pathTemp+i);
        }

        //ハンドルの取得
        status = Efi_Wrapper(
            Efi_fileProtocol->Open,
            Efi_fileProtocol,
            &temp_Efi_fileProtocol,
            pathTemp,
            EFI_FILE_MODE_READ,
            0
            );
        if(status) return 2;
    }else {
        temp_Efi_fileProtocol = Efi_fileProtocol;

    }

    //ディレクトリか判定
    EFI_GUID Efi_fileInfoId = EFI_FILE_INFO_ID;

    uintn fileInfo_buffSize = 0;
    Efi_Wrapper(
        temp_Efi_fileProtocol->GetInfo,
        temp_Efi_fileProtocol,
        &Efi_fileInfoId,
        &fileInfo_buffSize,
        NULL
        );
    uint8 fileInfo_buff[fileInfo_buffSize];
    status = Efi_Wrapper(
        temp_Efi_fileProtocol->GetInfo,
        temp_Efi_fileProtocol,
        &Efi_fileInfoId,
        &fileInfo_buffSize,
        fileInfo_buff);
    if(status) {
        if(path[0] != '\0') {
            Efi_Wrapper(
                temp_Efi_fileProtocol->Close,
                temp_Efi_fileProtocol);
        }
        return 3;
    }

    if(!(((EFI_FILE_INFO*)fileInfo_buff)->Attribute & EFI_FILE_DIRECTORY)) {
        if(path[0] != '\0') {
            Efi_Wrapper(
                temp_Efi_fileProtocol->Close,
                temp_Efi_fileProtocol);
        }
        return 4;
    }

    uintn buff_addedIndex = 0;
    //ディレクトリの中身を取得
    for(uintn i=0; ; i++) {
        uintn directory_buffSize = 0;
        Efi_Wrapper(
            temp_Efi_fileProtocol->Read,
            temp_Efi_fileProtocol,
            &directory_buffSize,
            NULL);
        uint8 directory_buff[directory_buffSize];
        status = Efi_Wrapper(
            temp_Efi_fileProtocol->Read,
            temp_Efi_fileProtocol,
            &directory_buffSize,
            directory_buff);

        if(directory_buffSize == 0) {
            if(*buffCount <= buff_addedIndex) {
                *buffCount = i;
                if(path[0] != '\0') {
                    Efi_Wrapper(
                        temp_Efi_fileProtocol->Close,
                        temp_Efi_fileProtocol);
                }
                return 5;
            }
            break;
        }
        if(status) {
            if(path[0] != '\0') {
                Efi_Wrapper(
                    temp_Efi_fileProtocol->Close,
                    temp_Efi_fileProtocol);
            }
            return 6;
        }

        //buffに追加
        if(*buffCount <= buff_addedIndex) continue;
        buff[buff_addedIndex].type = (((EFI_FILE_INFO*)directory_buff)->Attribute & EFI_FILE_DIRECTORY)?(File_Directory_Directory):(File_Directory_File);
        Functions_UTF16LE2ASCII_Str(31, ((EFI_FILE_INFO*)directory_buff)->FileName, buff[buff_addedIndex].name);
        buff[buff_addedIndex].name[31] = '\0';
        if(!(Functions_IsEqualStr(".", buff[buff_addedIndex].name) || Functions_IsEqualStr("..", buff[buff_addedIndex].name))) {
            buff_addedIndex ++;
        }
    }

    *buffCount = buff_addedIndex;

    if(path[0] != '\0') {
        Efi_Wrapper(
            temp_Efi_fileProtocol->Close,
            temp_Efi_fileProtocol);
    }

    return 0;
}

