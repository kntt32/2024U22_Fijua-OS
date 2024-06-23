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
static EFI_SIGNAL_EVENT Efi_SignalEvent = NULL;
static EFI_EVENT eventId;


void Timer_Init(void) {
    EFI_BOOT_SERVICES* bootServices = KernelInput->LoadedImage->SystemTable->BootServices;

    Efi_CreateEvent = bootServices->CreateEvent;
    Efi_SetTimer    = bootServices->SetTimer;
    Efi_RaiseTPL    = bootServices->RaiseTPL;
    Efi_RestoreTPL  = bootServices->RestoreTPL;
    Efi_CloseEvent  = bootServices->CloseEvent;
    Efi_SignalEvent = bootServices->SignalEvent;

    return;
}


sintn Timer_Set(void (*callback)(void), uintn sec100ns) {
    uintn status;
    status = wrapper(Efi_CreateEvent, EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (uintn)Timer_Wrapper, (uintn)callback, (uintn)&eventId);
    if(status) return -1;
    status = wrapper(Efi_SetTimer, (uintn)eventId, TimerPeriodic, sec100ns, 0, 0);
    if(status) return -2;

    return 0;
}


sintn Timer_Stop() {
    uintn status;
    status = wrapper(Efi_CloseEvent, (uintn)eventId, 0, 0, 0, 0);
    return 0;
}




