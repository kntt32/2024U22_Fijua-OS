ifeq ($(OS), Windows_NT)
OVMFFDPATH := tool\OVMF.fd
else
OVMFFDPATH := tool/OVMF.fd
endif

kernel:
	make -C kernel
	make -C kernel install

boot:
	make -C boot
	make -C boot install

all: boot kernel
	

run:
	qemu-system-x86_64 -bios $(OVMFFDPATH) -hda fat:rw:disk
