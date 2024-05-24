run:
	qemu-system-x86_64 -bios OVMF.fd -hda fat:rw:disk

installboot:
ifeq ($(OS), Windows_NT)
	copy boot\a.efi disk\EFI\BOOT\BOOTX64.EFI
else
	cp boot/a.efi disk/EFI/BOOT/BOOTX64.EFI
endif
