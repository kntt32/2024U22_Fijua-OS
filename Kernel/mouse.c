#include <kernel.h>
#include <types.h>
#include <efi.h>
#include <efi_simple_pointer_protocol.h>
#include "mouse.h"
#include "graphic.h"
#include "layer.h"
#include "timer.h"
#include "functions.h"
#include "console.h"
#include "x64.h"

#define Mouse_SupportHardwareCount (10)

extern KernelInputStruct* KernelInput;

static uintn Efi_SimplePointerProtocol_Count = 0;
static EFI_SIMPLE_POINTER_PROTOCOL* Efi_SimplePointerProtocol[Mouse_SupportHardwareCount];

static sintn Mouse_y = 0;
static sintn Mouse_x = 0;
static const sintn Mouse_speed = 20;

void Mouse_Init(void) {
    uintn status;

    //Get Efi Protocol Interface
    EFI_GUID guid = EFI_SIMPLE_POINTER_PROTOCOL_GUID;
#if 0
    status = Efi_Wrapper(KernelInput->LoadedImage->SystemTable->BootServices->LocateProtocol, &guid, NULL, &Efi_SimplePointerProtocol);
    if(status) {
        Console_Print("Err in Mouse_Init\n");
        Halt();
    }
#else
    {
        uintn handleBuffSize = 0;
        status = Efi_Wrapper(KernelInput->LoadedImage->SystemTable->BootServices->LocateHandle, ByProtocol, &guid, NULL, &handleBuffSize, NULL);
        EFI_HANDLE handleBuff[handleBuffSize/sizeof(EFI_HANDLE)];
        handleBuffSize = sizeof(handleBuff);
        status = Efi_Wrapper(KernelInput->LoadedImage->SystemTable->BootServices->LocateHandle, ByProtocol, &guid, NULL, &handleBuffSize, handleBuff);
        if(status || handleBuffSize == 0) {
            Console_Print("Mouse_Init: Mouse handle not found\n");
            Halt();
        }

        Efi_SimplePointerProtocol_Count = 0;
        for(uintn i=0; i<(handleBuffSize/sizeof(EFI_HANDLE)); i++) {
            if(Mouse_SupportHardwareCount <= Efi_SimplePointerProtocol_Count) break;
            status = Efi_Wrapper(KernelInput->LoadedImage->SystemTable->BootServices->HandleProtocol, handleBuff[i], &guid, &(Efi_SimplePointerProtocol[Efi_SimplePointerProtocol_Count]));
            if(status == 0) Efi_SimplePointerProtocol_Count++;
        }
    }
#endif
    
#if 0
    Efi_Mouse_Reset = Efi_SimplePointerProtocol->Reset;
    Efi_Mouse_GetState = Efi_SimplePointerProtocol->GetState;

    status = Efi_Wrapper(Efi_Mouse_Reset, Efi_SimplePointerProtocol, 0);
    if(status) {
        Console_Print("Mouse_Init: Cannot reset mouse\n");
        Halt();
    }
#else

    

    {
        for(sintn i=0; i<(sintn)Efi_SimplePointerProtocol_Count; i++) {
            status = Efi_Wrapper(Efi_SimplePointerProtocol[i]->Reset, Efi_SimplePointerProtocol[i], 0);
            if(status) {
                for(uintn k=i+1; k<Efi_SimplePointerProtocol_Count; k++) {
                    Efi_SimplePointerProtocol[i-1] = Efi_SimplePointerProtocol[i];
                }
                Efi_SimplePointerProtocol_Count--;
                i--;
            }
        }
        if(Efi_SimplePointerProtocol_Count == 0) {
            Console_Print("Mouse_Init: Mouse interface not found\n");
            Halt();
        }
    }
#endif

    Timer_Set(Mouse_Move, 10000);

    return;
}

void Mouse_Move(void) {
#if 0
    uintn status;
    status = Efi_Wrapper(Efi_Mouse_GetState, Efi_SimplePointerProtocol, &Efi_Mouse_State);
    if(!status) {
        Mouse_x += Efi_Mouse_State.RelativeMovementX/0x1000;
        Mouse_y += Efi_Mouse_State.RelativeMovementY/0x1000;
        if(Mouse_x < 0) Mouse_x = 0;
        if(Mouse_y < 0) Mouse_y = 0;
        Layer_Mouse_NotifyUpdate(Mouse_x, Mouse_y);
    }
    return;
#else
    static EFI_SIMPLE_POINTER_STATE Efi_Mouse_State;
    uintn status;
    uintn updateFlag = 0;
    sintn divNumX;
    sintn divNumY;
    for(uintn i=0; i<Efi_SimplePointerProtocol_Count; i++) {
        status = Efi_Wrapper(Efi_SimplePointerProtocol[i]->GetState, Efi_SimplePointerProtocol[i], &Efi_Mouse_State);
        if(status == 0) {
            updateFlag = 1;

            divNumX = Efi_SimplePointerProtocol[i]->Mode->ResolutionX;
            divNumY = Efi_SimplePointerProtocol[i]->Mode->ResolutionY;
            if(divNumX != 0) Mouse_x += Mouse_speed*Efi_Mouse_State.RelativeMovementX/divNumX;
            if(divNumY != 0) Mouse_y += Mouse_speed*Efi_Mouse_State.RelativeMovementY/divNumY;
        }
    }
    if(updateFlag) {
        if(Mouse_x < 0) Mouse_x = 0;
        if(Mouse_y < 0) Mouse_y = 0;
        Layer_Mouse_NotifyUpdate(Mouse_x, Mouse_y);
    }

#endif
}

