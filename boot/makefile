all: disk.img

bootloader:
	nasm -f bin kernel.asm -o kernel.bin

kernel:
	nasm -f bin boot.asm -o boot.bin

disk.img: bootloader kernel
	dd if=/dev/zero of=disk.img bs=512 count=2880
	dd if=kernel.bin of=disk.img bs=512 conv=notrunc seek=0
	dd if=boot.bin of=disk.img bs=512 conv=notrunc seek=1



