#ifndef INCLUDED_FILE_H
#define INCLUDED_FILE_H

typedef struct {
    enum {
        File_Directory_Directory,
        File_Directory_File
    } type;
    ascii name[32];
} File_Directory;

void File_Init(void);

uintn File_GetDirectory(const ascii path[], uintn* buffSize, File_Directory buff[]);

#endif
