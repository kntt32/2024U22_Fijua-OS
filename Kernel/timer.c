#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "x64.h"
#include "console.h"

#define TIMER_MAX_NUMBER (10)

typedef struct {
    EFI_EVENT eventId;
    uintn isEnabled;
} Timer_TimerTable;

extern KernelInputStruct* KernelInput;

static EFI_CREATE_EVENT Efi_CreateEvent = NULL;
static EFI_SET_TIMER Efi_SetTimer       = NULL;
static EFI_RAISE_TPL Efi_RaiseTPL       = NULL;
static EFI_RESTORE_TPL Efi_RestoreTPL   = NULL;
static EFI_CLOSE_EVENT Efi_CloseEvent   = NULL;

static Timer_TimerTable TimerTable[TIMER_MAX_NUMBER];

void Timer_Init(void) {
    EFI_BOOT_SERVICES* bootServices = KernelInput->LoadedImage->SystemTable->BootServices;

    Efi_CreateEvent = bootServices->CreateEvent;
    Efi_SetTimer    = bootServices->SetTimer;
    Efi_RaiseTPL    = bootServices->RaiseTPL;
    Efi_RestoreTPL  = bootServices->RestoreTPL;
    Efi_CloseEvent  = bootServices->CloseEvent;

    for(uintn i=0; i<TIMER_MAX_NUMBER; i++) {
        TimerTable[i].isEnabled = 0;
    }

    return;
}

static void Timer_NotifyHandler(void (*notifyFunction)(void)) {
    wrapper(Efi_RaiseTPL, TPL_APPLICATION, 0, 0, 0, 0);
    //notifyFunction();
    Console_Print("A\n");
    wrapper(Efi_RestoreTPL, TPL_CALLBACK, 0, 0, 0, 0);
    return;
}

//time:100ns
sintn Timer_Set(void* notifyFunction, uintn time) {
    EFI_EVENT event;
    uintn status;
    
    status = wrapper(Efi_CreateEvent, EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (uintn)Timer_NotifyHandler, (uintn)notifyFunction, (uintn)&event);
    if(status) return -1;

    status = wrapper(Efi_SetTimer, (uintn)event, TimerPeriodic, time, 0, 0);
    if(status) {
        wrapper(Efi_CloseEvent, (uintn)event, 0, 0, 0, 0);
        return -1;
    }

    for(int i=0; i<TIMER_MAX_NUMBER; i++) {
        if(!TimerTable[i].isEnabled) {
            TimerTable[i].eventId = event;
            TimerTable[i].isEnabled = 1;
            return i;
        }
    }


    return -2;
}

void Timer_Delete() {

}







