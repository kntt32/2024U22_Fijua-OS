#include <types.h>
#include "graphic.h"
#include "queue.h"
#include "task.h"
#include "shell.h"
#include "app_x64.h"


typedef enum {
    Shell_CmdType_UnKnown,
    Shell_CmdType_Cls,
    Shell_CmdType_Echo
} Shell_CmdType;


Shell_CmdType Shell_GetCmd(const ascii shellInput[]);
void Shell_Cmd_Echo(const ascii shelInput[]);
void Shell_Cmd_Cls(void);


sintn Shell_Main(void) {
    sintn status;

    ascii strBuff[128];
    while(1) {
        App_Syscall_StdOut("shell> ", sizeof("shell> "));
        
        status = App_Syscall_StdIn(strBuff, sizeof(strBuff));
        if(status != 0) {
            App_Syscall_StdOut("Shell: too large input\n", sizeof("Shell: too large input\n"));
            continue;
        }
        
        Shell_CmdType cmdType = Shell_GetCmd(strBuff);
        switch(cmdType) {
            case Shell_CmdType_Cls:
                Shell_Cmd_Cls();
                break;
            case Shell_CmdType_Echo:
                Shell_Cmd_Echo(strBuff);
                break;
            case Shell_CmdType_UnKnown:
                App_Syscall_StdOut("Shell: UnSupported Input: ", sizeof("Shell: UnSupported Input: "));
                App_Syscall_StdOut(strBuff, 128);
                break;
            default:
                App_Syscall_StdOut("Shell: Unknown Err Occured", sizeof("Shell: Unknown Err Occured"));
        }
        if(cmdType != Shell_CmdType_Cls) {
            App_Syscall_StdOut("\n\n", sizeof("\n\n"));
        }
    }

    return 0;
}


//str1が終わるまででstr2がstr1と完全に等しいかどうか
uintn Shell_Cmd_IsEqualStr(const ascii str1[], const ascii str2[]) {
    if(str1 == NULL || str2 == NULL) return 0;

    for(uintn i=0; 1; i++) {
        if(str1[i] == '\0') return 1;
        if(str1[i] != str2[i]) return 0;
    }
    return 1;
}


//NULL文字も含めた文字列の文字数を取得
uintn Shell_Cmd_CountStr(const ascii str[]) {
    if(str == NULL) return 0;

    for(uintn i=0; 1; i++) {
        if(str[i] == '\0') return i+1;
    }
    return 0;
}


//Shellへのユーザー入力からコマンドを取得
Shell_CmdType Shell_GetCmd(const ascii shellInput[]) {
    if(shellInput == NULL) return Shell_CmdType_UnKnown;

    uintn cmdLength = 0;

    while(1) {
        if(shellInput[cmdLength] == ' ' || shellInput[cmdLength] == '\0') break;
        cmdLength ++;
    }

    //Cls
    if(Shell_Cmd_IsEqualStr("cls", shellInput)) {
        return Shell_CmdType_Cls;
    }
    //Echo
    if(Shell_Cmd_IsEqualStr("echo", shellInput)) {
        return Shell_CmdType_Echo;
    }

    return Shell_CmdType_UnKnown;
}
sintn Syscall_StdOut_Cls(void);

//Clsコマンド
void Shell_Cmd_Cls(void) {
    App_Syscall_StdOut_Cls();
    return;
}


//Echoコマンド
void Shell_Cmd_Echo(const ascii shellInput[]) {
    if(shellInput == NULL) return;

    uintn cmdLength = 0;
    while(1) {
        if(shellInput[cmdLength] == ' ' || shellInput[cmdLength] == '\0') break;
        cmdLength ++;
    }

    const ascii* echoTarget = shellInput + cmdLength + 1;
    App_Syscall_StdOut(echoTarget, Shell_Cmd_CountStr(echoTarget));

    return;
}
