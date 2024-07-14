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

main.o: main.c console.h x64.h functions.h timer.h memory.h graphic.h task.h queue.h
functions.o: functions.c functions.h x64.h console.h
queue.o: queue.c queue.h memory.h
font.o: font.c font.h fontdata_monospace graphic.h
font_legacy.o: font_legacy.c font_legacy.h fontdata_monospace
console.o: console.c console.h font.h x64.h
bootwrapper.o: bootwrapper.s
x64.o: x64.s
keyboard.o: keyboard.c keyboard.h x64.h
timer.o: timer.c timer.h x64.h console.h
memory.o: memory.c memory.h console.h functions.h x64.h
graphic.o: graphic.c graphic.h functions.h
task.o: task.c task.h memory.h console.h functions.h x64.h

test.o: test.c test.h graphic.h queue.h task.h


%.o: %.c
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -fshort-wchar -ffreestanding -mno-red-zone -c -fpie $<

%.o: %.s
	$(CC) -I $(INCLUDE) -Wall -Wextra --target=$(TARGET) -c -fpie $<
