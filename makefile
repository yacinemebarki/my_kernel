AS = nasm
CC = gcc
LD = ld

ASFLAGS = -f elf32
CFLAGS = -ffreestanding -m32 -nostdlib -c -fno-pic -fno-pie
LDFLAGS = -m elf_i386 -T link.ld

KERNEL = kernel.elf
IMG = disk.img

BOOT = boot/boot.asm
KERNEL_C = kernel/kernel.c
KERNEL_ENTRY = kernel/kernel_entry.asm
KEYBOARD_C = kernel/keyboard.c
KEYBOARD_O = kernel/keyboard.o
KEYBOARD_ISR = kernel/keyboard_isr.asm
KEYBOARD_ISR_O = kernel/keyboard_isr.o
VGA_C = kernel/vga.c 
VGA_O = kernel/vga.o
IDT_C = kernel/idt.c 
IDT_O = kernel/idt.o 
PIT_C = kernel/pit.c 
PIT_O = kernel/pit.o
PIT_ISR = kernel/irqo.asm
PIT_ISR_O = kernel/irqo.o
PMM_C = kernel/pmm.c
PMM_O = kernel/pmm.o
PMM_ISR = kernel/pagging.asm
PMM_ISR_O = kernel/pagging.o

BOOT_BIN = boot/boot.bin
KERNEL_BIN = kernel.bin
KERNEL_O = kernel/kernel.o
KERNEL_ENTRY_O = kernel/kernel_entry.o

.PHONY: all boot kernel image run clean

all: image

boot:
	nasm -f bin $(BOOT) -o $(BOOT_BIN)


kernel:
	$(CC) $(CFLAGS) $(KERNEL_C) -o $(KERNEL_O)
	$(CC) $(CFLAGS) $(KEYBOARD_C) -o $(KEYBOARD_O)
	$(CC) $(CFLAGS) $(VGA_C) -o $(VGA_O)
	$(CC) $(CFLAGS) $(IDT_C) -o $(IDT_O)
	$(CC) $(CFLAGS) $(PIT_C) -o $(PIT_O) 
	$(CC) $(CFLAGS) $(PMM_C) -o $(PMM_O)
	nasm -f elf32 $(KERNEL_ENTRY) -o $(KERNEL_ENTRY_O)
	nasm -f elf32 $(KEYBOARD_ISR) -o $(KEYBOARD_ISR_O)
	nasm -f elf32 $(PIT_ISR) -o $(PIT_ISR_O)
	nasm -f elf32 $(PMM_ISR) -o $(PMM_ISR_O)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(KERNEL_ENTRY_O) $(KEYBOARD_ISR_O) $(PMM_ISR_O) $(PIT_ISR_O) $(KERNEL_O) $(KEYBOARD_O) $(PIT_O) $(VGA_O) $(IDT_O) $(PMM_O)
	objcopy -O binary $(KERNEL) $(KERNEL_BIN)

image: boot kernel
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=$(BOOT_BIN) of=$(IMG) conv=notrunc
	dd if=$(KERNEL_BIN) of=$(IMG) seek=1 conv=notrunc

run:
	qemu-system-i386 -drive format=raw,file=$(IMG)

clean:
	rm -f boot/*.bin kernel/*.o *.elf $(IMG)