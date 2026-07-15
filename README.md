# mini_kernel

A minimal x86 32-bit hobby kernel and bootloader (assembly + C).

## Summary

This repository contains a simple boot sector and a tiny kernel written in C and x86 assembly. It builds a raw disk image `disk.img` containing a 512-byte boot sector and a kernel binary.

## Prerequisites

- nasm
- gcc with multilib support (for `-m32`) and binutils
- qemu-system-i386 (for running the image)

On Debian/Ubuntu you can install:

```bash
sudo apt install nasm gcc-multilib binutils qemu-system-x86
```

## Build

From the project root run:

```bash
make
```

This runs the `boot` and `kernel` targets and creates `disk.img`.

## Run

Start QEMU with:

```bash
make run
```

Or run directly:

```bash
qemu-system-i386 -drive format=raw,file=disk.img
```

## Clean

```bash
make clean
```

## Makefile targets

- `boot` — assemble the boot sector (`boot/boot.asm`) producing `boot/boot.bin`.
- `kernel` — compile kernel C/ASM sources and link into `kernel.elf`, then `objcopy` to `kernel.bin`.
- `image` — creates `disk.img` and writes boot + kernel into it.
- `run` — runs `disk.img` in QEMU.

## Kernel tests

The kernel now includes a structured test module under `kernel/tests.c` and `kernel/tests.h`.

- `run_tests()` executes the current test suite.
- test functions are organized as:
  - `test_paging()`
  - `test_allocation()`
  - `test_kmalloc()`
  - `test_kfree()`

The kernel also implements more runtime helpers in `kernel/pmm.c`:

- `allocate_page()` / `free_page()` for page allocation and mapping
- `kmalloc()` / `kfree()` for heap allocations
- `inspect()` to print heap block metadata
- `build_first_page()` and `map_page()` for paging setup

These tests and helper functions are centralized in `kernel/tests.c` and can be activated from `kernel/kernel.c` when needed.

## Project structure

- boot/
  - boot.asm — 512-byte boot sector source
- kernel/
  - kernel.c — main kernel code
  - kernel_entry.asm — 32-bit entry/setup
  - idt.c / idt.h — IDT setup
  - keyboard.c / keyboard_isr.asm — keyboard driver and ISR
  - pit.c / irqo.asm — PIT timer and ISR
  - pmm.c / pmm.h — physical memory manager
  - vga.c / vga.h — VGA text output
  - types.h, asm_operation.h — helper headers

See the `makefile` for the exact files used during the build.

## Notes

- The build uses `-m32` and freestanding flags; ensure your toolchain supports 32-bit compilation on your host.
- This is a learning/hobby project — feel free to modify and experiment.
