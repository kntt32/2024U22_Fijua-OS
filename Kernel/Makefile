CC := clang
TARGET := x86_64-elf
FILES := main.c
ifeq ($(OS), Windows_NT)
INCLUDE := ..\include
else
INCLUDE := ../include
endif

build: a.elf

install: build
ifeq ($(OS), Windows_NT)
	copy a.elf ..\disk\kernel.elf
else
	cp a.elf ../disk/kernel.elf
endif

clean:
ifeq ($(OS), Windows_NT)
	del *.o *.elf
else
	rm *.o *.elf
endif

a.elf: main.o font.o bootwrapper.o x64.o console.o timer.o functions.o
	ld.lld --entry entrypoint -z norelro --image-base 0x100000 --static -o a.elf $^

main.o: main.c console.h x64.h functions.h timer.h
functions.o: functions.c functions.h x64.h
font.o: font.c font.h fontdata_monospace
console.o: console.c console.h font.h
bootwrapper.o: bootwrapper.asm
x64.o: x64.asm
keyboard.o: keyboard.c keyboard.h x64.h
timer.o: timer.c timer.h x64.h console.h


%.o: %.c
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -fshort-wchar -ffreestanding -mno-red-zone -c $<

%.o: %.asm
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -c $<
