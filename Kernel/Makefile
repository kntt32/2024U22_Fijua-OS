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

a.elf: main.o font.o bootwrapper.o x64.o console.o timer.o functions.o queue.o memory.o graphic.o task.o  test.o
	ld.lld --entry entrypoint -z norelro --static -pie -o a.elf $^
ifeq ($(OS), Windows_NT)
	copy a.elf ..\disk\kernel.elf
else
	cp a.elf ../disk/kernel.elf
endif

main.o: main.c functions.h graphic.h console.h timer.h memory.h queue.h task.h test.h
functions.o: functions.c functions.h x64.h task.h console.h
queue.o: queue.c queue.h memory.h console.h functions.h
font.o: font.c graphic.h font.h fontdata_monospace
console.o: console.c graphic.h console.h font.h
bootwrapper.o: bootwrapper.s
x64.o: x64.s
keyboard.o: keyboard.c x64.h keyboard.h
timer.o: timer.c timer.h x64.h console.h
memory.o: memory.c memory.h functions.h x64.h
graphic.o: graphic.c graphic.h
task.o: task.c queue.h task.h memory.h x64.h

test.o: test.c test.h queue.h task.h graphic.h


%.o: %.c
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -fshort-wchar -ffreestanding -mno-red-zone -c -O0 -fpie $<

%.o: %.s
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -c -O0 -fpie $<
