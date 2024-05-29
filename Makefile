ifeq ($(OS), Windows_NT)
OVMFFDPATH := tool\OVMF.fd
else
OVMFFDPATH := tool/OVMF.fd
endif

build:
	make -C boot install


run:
	qemu-system-x86_64 -bios $(OVMFFDPATH) -hda fat:rw:disk
