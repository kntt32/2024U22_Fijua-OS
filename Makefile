ifeq ($(OS), Windows_NT)
OVMFFDPATH := tool\OVMF.fd
else
OVMFFDPATH := tool/OVMF.fd
endif

buildkernel:
	make -C Kernel
	make -C Kernel install

buildboot:
	make -C Boot
	make -C Boot install

all: buildboot buildkernel
	

run:
	qemu-system-x86_64 -bios $(OVMFFDPATH) -hda fat:rw:disk
