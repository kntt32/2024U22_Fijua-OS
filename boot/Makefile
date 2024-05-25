CC := clang
TARGET := x86_64-pc-win32-coff
FILES := main.c
ifeq ($(OS), Windows_NT)
INCLUDE := ..\include
else
INCLUDE := ../include
endif

build: main.o elfloader.o
	lld-link /subsystem:efi_application /entry:efi_main /out:a.efi $^

clean:
ifeq ($(OS), Windows_NT)
	del *.o *.efi
else
	rm *.o *.efi
endif



main.o: main.c

elfloader.o: elfloader.c elfloader.h

%.o: %.c
	$(CC) -I $(INCLUDE) -target $(TARGET) -mno-red-zone -fno-stack-protector -fshort-wchar -Wall -Wextra -c $<

%.o: %.s
	$(CC) -target $(TARGET) -mno-red-zone -fno-stack-protector -fshort-wchar -Wall -Wextra -c $<
