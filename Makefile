run:
	qemu-system-x86_64 -bios OVMF.fd -hda fat:rw:disk

installboot:
	copy boot\a.efi disk\EFI\BOOT\BOOTX64.EFI

