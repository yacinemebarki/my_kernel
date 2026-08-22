AS = nasm
CC = gcc
LD = ld

ASFLAGS = -f elf32
CFLAGS = -ffreestanding -m32 -nostdlib -c -fno-pic -fno-pie -Ikernel -Iuser
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
TESTS_C = kernel/tests.c
TESTS_O = kernel/tests.o
PROCESS_C = kernel/process.c
PROCESS_O = kernel/process.o
RESTORE_ESP_ISR = kernel/restore_esp.asm
RESTORE_ESP_O = kernel/restore_esp.o
EXCEPTION_ISR = kernel/exception.asm
EXCEPTION_ISR_O = kernel/exception.o
TSS_C = kernel/tss.c 
TSS_O = kernel/tss.o 
TSS_FLUSH_ASM = kernel/tss_flush.asm 
TSS_FLUSH_O = kernel/tss_flush.o 
GDT_FLUSH_ASM = kernel/gdt_flush.asm 
GDT_FLUSH_O = kernel/gdt_flush.o
USER_SPACE_C = user/user_space.c 
USER_SPACE_O = user/user_space.o 
ENTER_USER_MODE_ISER = kernel/enter_user_mode.asm 
ENTER_USER_MODE_ISER_O = kernel/enter_user_mode.o
SYCALL_HANDELER_ISER = kernel/sycall_handeler.asm 
SYCALL_HANDELER_ISER_O = kernel/sycall_handeler.o 
SYSCALL_C = kernel/syscall.c
SYSCALL_O = kernel/syscall.o
USER_TEST_C = user/user_test.c 
USER_TEST_O = user/user_test.o  
ELF_C = kernel/elf.c
ELF_O = kernel/elf.o
ELF_TEST_C = user/elf_test.c
ELF_TEST_O = user/elf_test.o
ELF_TEST = user/elf_test.elf
ELF_TEST_BIN = user/elf_test.bin
ELF_TEST_OBJ = user/elf_test_blob.o

BOOT_BIN = boot/boot.bin
KERNEL_BIN = kernel.bin
KERNEL_O = kernel/kernel.o
KERNEL_ENTRY_O = kernel/kernel_entry.o

.PHONY: all boot kernel image run clean

all: elf_test image

boot:
	nasm -f bin $(BOOT) -o $(BOOT_BIN)


elf_test:
	$(CC) -m32 -ffreestanding -fno-pie -fno-pic -nostdlib \
		-c $(ELF_TEST_C) -o $(ELF_TEST_O)

	$(LD) -m elf_i386 -Ttext 0x08048000 \
		-e _start \
		-o $(ELF_TEST) $(ELF_TEST_O) 

	objcopy -I binary -O elf32-i386 -B i386 \
		$(ELF_TEST) $(ELF_TEST_OBJ)


kernel:
	$(CC) $(CFLAGS) $(KERNEL_C) -o $(KERNEL_O)
	$(CC) $(CFLAGS) $(KEYBOARD_C) -o $(KEYBOARD_O)
	$(CC) $(CFLAGS) $(VGA_C) -o $(VGA_O)
	$(CC) $(CFLAGS) $(IDT_C) -o $(IDT_O)
	$(CC) $(CFLAGS) $(PIT_C) -o $(PIT_O) 
	$(CC) $(CFLAGS) $(PMM_C) -o $(PMM_O)
	$(CC) $(CFLAGS) $(TESTS_C) -o $(TESTS_O)
	$(CC) $(CFLAGS) $(PROCESS_C) -o $(PROCESS_O)
	$(CC) $(CFLAGS) $(TSS_C) -o $(TSS_O)
	$(CC) $(CFLAGS) $(USER_SPACE_C) -o $(USER_SPACE_O)
	$(CC) $(CFLAGS) $(SYSCALL_C) -o $(SYSCALL_O)
	$(CC) $(CFLAGS) $(USER_TEST_C) -o $(USER_TEST_O)
	$(CC) $(CFLAGS) $(ELF_C) -o $(ELF_O)
	nasm -f elf32 $(KERNEL_ENTRY) -o $(KERNEL_ENTRY_O)
	nasm -f elf32 $(KEYBOARD_ISR) -o $(KEYBOARD_ISR_O)
	nasm -f elf32 $(PIT_ISR) -o $(PIT_ISR_O)
	nasm -f elf32 $(PMM_ISR) -o $(PMM_ISR_O)
	nasm -f elf32 $(EXCEPTION_ISR) -o $(EXCEPTION_ISR_O)
	nasm -f elf32 $(RESTORE_ESP_ISR) -o $(RESTORE_ESP_O)
	nasm -f elf32 $(TSS_FLUSH_ASM) -o $(TSS_FLUSH_O)
	nasm -f elf32 $(GDT_FLUSH_ASM) -o $(GDT_FLUSH_O)
	nasm -f elf32 $(ENTER_USER_MODE_ISER) -o $(ENTER_USER_MODE_ISER_O)
	nasm -f elf32 $(SYCALL_HANDELER_ISER) -o $(SYCALL_HANDELER_ISER_O)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(KERNEL_ENTRY_O) $(KEYBOARD_ISR_O) $(PMM_ISR_O) $(PIT_ISR_O) $(KERNEL_O) $(KEYBOARD_O) $(PIT_O) $(VGA_O) $(IDT_O) $(PMM_O) $(TESTS_O) $(PROCESS_O) $(RESTORE_ESP_O) $(EXCEPTION_ISR_O) $(TSS_FLUSH_O) $(GDT_FLUSH_O) $(TSS_O) $(USER_SPACE_O) $(SYSCALL_O) $(ENTER_USER_MODE_ISER_O) $(SYCALL_HANDELER_ISER_O) $(USER_TEST_O) $(ELF) $(ELF_O) $(ELF_TEST_OBJ)
	objcopy -O binary $(KERNEL) $(KERNEL_BIN)

image: boot kernel
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=$(BOOT_BIN) of=$(IMG) conv=notrunc
	dd if=$(KERNEL_BIN) of=$(IMG) seek=1 conv=notrunc

run:
	qemu-system-i386 -drive format=raw,file=$(IMG)

clean:
	rm -f boot/*.bin kernel/*.o *.elf $(IMG)