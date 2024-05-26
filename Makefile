build:
	make -C boot install


run:
	qemu-system-x86_64 -bios OVMF.fd -hda fat:rw:disk
