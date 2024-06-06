ifeq ($(OS), Windows_NT)
OVMFFDPATH := tool\OVMF.fd
else
OVMFFDPATH := tool/OVMF.fd
endif

buildkernel:
	make -C kernel install

buildboot:
	make -C boot install

all: buildboot buildkernel
	

run:
	qemu-system-x86_64 -bios $(OVMFFDPATH) -hda fat:rw:disk
