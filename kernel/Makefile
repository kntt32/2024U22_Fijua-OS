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

a.elf: main.o font.o bootwrapper.o x64.o
	ld.lld --entry entrypoint -z norelro --image-base 0x100000 --static -o a.elf $^

main.o: main.c
font.o: font.c font.h fontdata_monospace
bootwrapper.o: bootwrapper.asm
x64.o: x64.asm

%.o: %.c
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -ffreestanding -mno-red-zone -c $<

%.o: %.asm
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -c $<
