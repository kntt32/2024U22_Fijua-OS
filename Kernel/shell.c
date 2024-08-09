#include <types.h>
#include "graphic.h"
#include "queue.h"
#include "task.h"
#include "file.h"

#include "shell.h"
#include "app_x64.h"


#define Shell_Default_StrBuffSize (128)


typedef enum {
    Shell_CmdType_UnKnown,
    Shell_CmdType_Cls,
    Shell_CmdType_Echo,
    Shell_CmdType_Ls
} Shell_CmdType;


Shell_CmdType Shell_GetCmd(const ascii shellInput[]);
const ascii* Shell_GetCmdInputStart(const ascii shellInput[]);

void Shell_Cmd_Echo(const ascii cmdInput[]);
void Shell_Cmd_Cls(void);
void Shell_Cmd_Ls(const ascii cmdInput[Shell_Default_StrBuffSize], ascii workingPath[Shell_Default_StrBuffSize]);





sintn Shell_Main(void) {
    sintn status;

    ascii workingPath[Shell_Default_StrBuffSize] = "";

    ascii strBuff[Shell_Default_StrBuffSize];
    while(1) {
        App_Syscall_StdOut("shell> ", sizeof("shell> "));
        
        status = App_Syscall_StdIn(strBuff, sizeof(strBuff));
        if(status != 0) {
            App_Syscall_StdOut("Shell: too large input\n", sizeof("Shell: too large input\n"));
            continue;
        }
        
        Shell_CmdType cmdType = Shell_GetCmd(strBuff);
        const ascii* cmdInput = Shell_GetCmdInputStart(strBuff);
        switch(cmdType) {
            case Shell_CmdType_Cls:
                Shell_Cmd_Cls();
                break;
            case Shell_CmdType_Echo:
                Shell_Cmd_Echo(cmdInput);
                break;
            case Shell_CmdType_Ls:
                Shell_Cmd_Ls(cmdInput, workingPath);
                break;
            case Shell_CmdType_UnKnown:
                App_Syscall_StdOut("Shell: UnSupported Input: ", sizeof("Shell: UnSupported Input: "));
                App_Syscall_StdOut(strBuff, Shell_Default_StrBuffSize);
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
    //Ls
    if(Shell_Cmd_IsEqualStr("ls", shellInput)) {
        return Shell_CmdType_Ls;
    }

    return Shell_CmdType_UnKnown;
}


const ascii* Shell_GetCmdInputStart(const ascii shellInput[]) {
    if(shellInput == NULL) return NULL;

    for(uintn i=0; 1; i++) {
        if(shellInput[i] == ' ' && shellInput[i+1] != ' ') return shellInput+i+1;
        if(shellInput[i] == '\0') return NULL;
    }
}


//作業パスを相対パスから本当のパスを取得
static void Shell_GetRealPath(const ascii workingPath[Shell_Default_StrBuffSize], const ascii relativePath[Shell_Default_StrBuffSize], ascii realPath[Shell_Default_StrBuffSize*2]) {
    uintn pathLength = 0;
    for(uintn i=0; i<Shell_Default_StrBuffSize; i++) {
        realPath[pathLength] = workingPath[i];
        pathLength ++;
        if(realPath[pathLength-1] == '\0') break;
    }
    if(relativePath != NULL) {
        pathLength --;
        realPath[pathLength] = '/';
        pathLength ++;
        for(uintn i=0; i<Shell_Default_StrBuffSize; i++) {
            realPath[pathLength] = relativePath[i];
            pathLength ++;
            if(realPath[pathLength-1] == '\0') break;
        }
    }
    return;
}


//Clsコマンド
void Shell_Cmd_Cls(void) {
    App_Syscall_StdOut_Cls();
    return;
}


//Echoコマンド
void Shell_Cmd_Echo(const ascii cmdInput[]) {
    App_Syscall_StdOut(cmdInput, Shell_Cmd_CountStr(cmdInput));

    return;
}


//lsコマンド
void Shell_Cmd_Ls(const ascii cmdInput[Shell_Default_StrBuffSize], ascii workingPath[Shell_Default_StrBuffSize]) {
    uintn status;

    ascii realPath[Shell_Default_StrBuffSize*2];

    Shell_GetRealPath(workingPath, cmdInput, realPath);

    App_Syscall_StdOut(realPath, Shell_Default_StrBuffSize*2);
    App_Syscall_StdOut(":\n", 3);

    uintn buffCount = 0;
    App_Syscall_GetFileList(realPath, Shell_Default_StrBuffSize*2, &buffCount, NULL);
    File_Directory buff[buffCount];
    status = App_Syscall_GetFileList(realPath, Shell_Default_StrBuffSize*2, &buffCount, buff);
    if(status) {
        App_Syscall_StdOut("ls: Err", sizeof("ls: Err"));
        return;
    }

    for(uintn i=0; i<buffCount; i++) {
        App_Syscall_StdOut(buff[i].name, 32);
        App_Syscall_StdOut(" ", sizeof(" "));
    }

    return;
}
