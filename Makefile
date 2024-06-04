ifeq ($(OS), Windows_NT)
OVMFFDPATH := tool\OVMF.fd
else
OVMFFDPATH := tool/OVMF.fd
endif

buildboot:
	make -C boot install

buildkernel:
	make -C kernel install

all: buildboot buildkernel
	

run:
	qemu-system-x86_64 -bios $(OVMFFDPATH) -hda fat:rw:disk
