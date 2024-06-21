CC := clang
TARGET := x86_64-elf

ifeq ($(OS), Windows_NT)
INCLUDE := ..\include
else
INCLUDE := ../include
endif

build: a.elf

clean:
ifeq ($(OS), Windows_NT)
	del *.o *.elf
else
	rm *.o *.elf
endif

a.elf: main.o font.o bootwrapper.o x64.o console.o timer.o functions.o memory.o kmalloc.o
	ld.lld --entry entrypoint -z norelro --static -pie -o a.elf $^
ifeq ($(OS), Windows_NT)
	copy a.elf ..\disk\kernel.elf
else
	cp a.elf ../disk/kernel.elf
endif

main.o: main.c console.h x64.h functions.h timer.h memory.h
functions.o: functions.c functions.h x64.h console.h
font.o: font.c font.h fontdata_monospace
console.o: console.c console.h font.h
bootwrapper.o: bootwrapper.s
x64.o: x64.s
keyboard.o: keyboard.c keyboard.h x64.h
timer.o: timer.c timer.h x64.h console.h
memory.o: memory.c memory.h console.h functions.h
kmalloc.o: kmalloc.c functions.h memory.h console.h kmalloc.h


%.o: %.c
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -fshort-wchar -ffreestanding -mno-red-zone -c -fpie $<

%.o: %.s
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -c -fpie $<
