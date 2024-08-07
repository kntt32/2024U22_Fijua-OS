#include <types.h>
#include <kernel.h>
#include <efi.h>
#include <efi_loaded_image_protocol.h>
#include <efi_block_io_protocol.h>
#include <mbr.h>
#include "partition.h"
#include "x64.h"
#include "console.h"
#include "functions.h"


static void Partition_SetEfiBlockIoProtocol(void);

static void Partition_Mount(void);

extern KernelInputStruct* KernelInput;

static EFI_BLOCK_IO_PROTOCOL* Efi_blockIoProtocol;//ブートメディアのインターフェイス

static Partition_Descriptor partitionDesciptor[4];


//初期化
void Partition_Init(void) {
    Console_Print("Partition_Init: Initializing...\n");

    //Efi_BlockIoProtocolをセット
    Partition_SetEfiBlockIoProtocol();    

    //パーティションをマウント
    Partition_Mount();
        
    return;
}


//Efi_blockIoProtocolにセット
static void Partition_SetEfiBlockIoProtocol(void) {
    EFI_GUID Efi_blockIoProtocol_guid = EFI_BLOCK_IO_PROTOCOL_GUID;
    uintn status = 0;

    //BlockIoProtocolを取得
    status = Efi_Wrapper(
        KernelInput->LoadedImage->SystemTable->BootServices->HandleProtocol,
        KernelInput->LoadedImage->DeviceHandle,
        &Efi_blockIoProtocol_guid,
        &Efi_blockIoProtocol
        );
    if(status) {
        Console_Print("Partition_Init: Failed to Initialize Boot Media's Block IO Protocol : 1\n");
        while(1) Hlt();
    }

    status = Efi_Wrapper(
        Efi_blockIoProtocol->Reset,
        Efi_blockIoProtocol,
        0);
    if(status) {
        Console_Print("Partition_Init: Failed to Initialize Boot Media's Block IO Protocol : 2\n");
        while(1) Hlt();
    }

    return;
}


//MBRパーティションに対応しているか判定
static Partition_Type Partition_Mount_GetPartitionType(void) {
    /*
    MBRパーティションに対応しているならPartition_Type_Mbrを返す
    そうでないならPartition_Type_UnKnownを返す
    
    判定は第一セクタ[0x1fe],[0x1ff]が0x55,0xaaであり、[0x026],[0x042]が0x28か0x29でないならMBRと判定する
    */
    uintn status;

    uint8 firstSectorBuff[Efi_blockIoProtocol->Media->BlockSize];
    status = Efi_Wrapper(
        Efi_blockIoProtocol->ReadBlocks,
        Efi_blockIoProtocol,
        Efi_blockIoProtocol->Media->MediaId,
        0,
        Efi_blockIoProtocol->Media->BlockSize,
        firstSectorBuff
    );
    if(status) {
        Console_Print("Partition_Mount_IsMbr: Failed to Mount Storage\n");
        ascii strbuff[17];
        SPrintIntX(status, 17, strbuff);
        Console_Print(strbuff);
        while(1) Hlt();
    }

    Console_Print("Partition_Mount_GetPartitionType: ");
    if((firstSectorBuff[0x1fe] == 0x55 && firstSectorBuff[0x1ff] == 0xaa)) {
        Console_Print("A:1\n")
    }else {
        Console_Print("A:2\n");
    }
    Console_Print("Partition_Mount_GetPartitionType: ");
    if(firstSectorBuff[0x026] != 0x28 && firstSectorBuff[0x026] != 0x29) {
        Console_Print("B:1\n");
    }else {
        Console_Print("B:2\n");
    }
    Console_Print("Partition_Mount_GetPartitionType: ");
    if(firstSectorBuff[0x042] != 0x28 && firstSectorBuff[0x042] != 0x29) {
        Console_Print("C:1\n");
    }else {
        Console_Print("C:2\n");
    }

    if(
        (firstSectorBuff[0x1fe] == 0x55 && firstSectorBuff[0x1ff] == 0xaa)
        && (firstSectorBuff[0x026] != 0x28 && firstSectorBuff[0x026] != 0x29)
        && (firstSectorBuff[0x042] != 0x28 && firstSectorBuff[0x042] != 0x29)
    ) {
        return Partition_Type_Mbr;
    }

    return Partition_Type_UnKnown;
}


//パーティションをマウント
static void Partition_Mount(void) {
    /*
    1.MBRか判定
        2.もしMBRなら四つのパーティションを設定
        2.もしMBRでなければSSDすべてをひとつのパーティションとして判断する
    ただしGPTには非対応（起動するSSDはUSBメモリを想定しており、大容量/大量のパーティションをつくれるという利点は活用できず、無駄な実装でしかないため）
    */

    //1.MBRか判定
    Partition_Type partitionType = Partition_Mount_GetPartitionType();

    //2
    switch(partitionType) {
        case Partition_Type_UnKnown:
            //2.MBRなら四つのパーティションをマウント
            Console_Print("IsFatDisk\n");
            break;
        case Partition_Type_Mbr:
            //2.MBRでないならSSDすべてを一つのパーティションとしてマウント
            Console_Print("IsMbr\n");
            partitionDesciptor[0].isActive = 1;
            partitionDesciptor[0].startSector = 0;
            partitionDesciptor[0].sectorCount = Efi_blockIoProtocol->Media->LastBlock;
            for(uintn i=1; i<4; i++) {
                partitionDesciptor[i].isActive = 0;
            }
            break;
        default:
            Console_Print("Partition_Mount: UnKnown Err\n");
            while(1) Hlt();
    }


    while(1) Hlt();

    return;
}
