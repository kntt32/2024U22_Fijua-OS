CC := clang
TARGET := x86_64-elf
FILES := main.c
ifeq ($(OS), Windows_NT)
INCLUDE := ..\include
else
INCLUDE := ../include
endif

build: main.o
	ld.lld --entry kernel_main -z norelro --image-base 0x100000 --static -o a.elf $^

clean:
ifeq ($(OS), Windows_NT)
	del *.o *.elf
else
	rm *.o *.elf
endif



main.o: main.c

%.o: %.c
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -ffreestanding -mno-red-zone -c $^

%.o: %.s
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -ffreestanding -mno-red-zone -c $^
