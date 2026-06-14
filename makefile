AS = nasm
CC = gcc
LD = ld

ASFLAGS = -f elf32
CFLAGS = -ffreestanding -m32 -nostdlib -c
LDFLAGS = -T link.ld -m elf_i386

KERNEL = kernel.elf
IMG = disk.img

BOOT = boot/boot.asm
KERNEL_C = kernel/kernel.c
KERNEL_ENTRY = kernel/kernel_entry.asm

BOOT_BIN = boot/boot.bin
KERNEL_O = kernel/kernel.o
KERNEL_ENTRY_O = kernel/kernel_entry.o

.PHONY: all boot kernel image run clean

all: image

boot:
	nasm -f bin $(BOOT) -o $(BOOT_BIN)

kernel:
	$(CC) $(CFLAGS) $(KERNEL_C) -o $(KERNEL_O)
	nasm -f elf32 $(KERNEL_ENTRY) -o $(KERNEL_ENTRY_O)
	
	$(LD) $(LDFLAGS) -o $(KERNEL) $(KERNEL_ENTRY_O) $(KERNEL_O)

image: boot kernel
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=$(BOOT_BIN) of=$(IMG) conv=notrunc
	dd if=$(KERNEL) of=$(IMG) seek=1 conv=notrunc

run:
	qemu-system-i386 -drive format=raw,file=$(IMG)

clean:
	rm -f boot/*.bin kernel/*.o *.elf $(IMG)