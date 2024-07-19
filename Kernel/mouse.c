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

extern KernelInputStruct* KernelInput;

static EFI_SIMPLE_POINTER_PROTOCOL* Efi_SimplePointerProtocol;
static EFI_SIMPLE_POINTER_RESET Efi_Mouse_Reset;
static EFI_SIMPLE_POINTER_GET_STATE Efi_Mouse_GetState;
static EFI_SIMPLE_POINTER_STATE Efi_Mouse_State;

static sintn Mouse_y = 0;
static sintn Mouse_x = 0;

void Mouse_Init(void) {
    uintn status;

    //Get Efi Protocol Interface
    EFI_GUID guid = EFI_SIMPLE_POINTER_PROTOCOL_GUID;
    status = Efi_Wrapper(KernelInput->LoadedImage->SystemTable->BootServices->LocateProtocol, &guid, NULL, &Efi_SimplePointerProtocol);
    if(status) {
        Console_Print("Err in Mouse_Init\n");
        Halt();
    }

    Efi_Mouse_Reset = Efi_SimplePointerProtocol->Reset;
    Efi_Mouse_GetState = Efi_SimplePointerProtocol->GetState;

    status = Efi_Wrapper(Efi_Mouse_Reset, Efi_SimplePointerProtocol, 0);
    if(status) {
        Console_Print("Mouse_Init: Cannot reset mouse\n");
        Halt();
    }

    Layer_Mouse_NotifyMove(Mouse_x, Mouse_y);

    Timer_Set(Mouse_Move, 10000);

    return;
}

void Mouse_Move(void) {
    uintn status;
    status = Efi_Wrapper(Efi_Mouse_GetState, Efi_SimplePointerProtocol, &Efi_Mouse_State);
    if(!status) {
        Mouse_x += Efi_Mouse_State.RelativeMovementX/0x1000;
        Mouse_y += Efi_Mouse_State.RelativeMovementY/0x1000;
        if(Mouse_x < 0) Mouse_x = 0;
        if(Mouse_y < 0) Mouse_y = 0;
        Layer_Mouse_NotifyMove(Mouse_x, Mouse_y);
    }
    return;
}

