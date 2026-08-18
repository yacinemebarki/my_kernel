# mini_kernel

A minimal x86 (32-bit) hobby bootloader + kernel written in NASM and C.

## Overview

This repository builds a raw disk image (`disk.img`) containing a 512-byte boot sector and a simple protected-mode kernel. It is intended as a learning project for low-level x86 systems programming (GDT/IDT, paging, basic drivers, simple scheduler, and user-mode entry).

## Features

- **Protected-mode boot:** Boot sector → protected mode with GDT/TSS setup (`boot/boot.asm`, `kernel/gdt_flush.asm`, `kernel/tss.c`).
- **Paging & virtual memory:** Basic page directory/table setup and mapping helpers (`kernel/pagging.asm`, `kernel/pmm.c`).
- **Interrupts & exceptions:** IDT setup and handlers for keyboard, PIT, and CPU faults (`kernel/idt.c`, `kernel/keyboard_isr.asm`, `kernel/irqo.asm`, `kernel/exception.asm`).
- **Drivers (VGA & keyboard):** Text console (cursor, wrapping, clear, backspace) and keyboard input parsing (`kernel/vga.c`, `kernel/keyboard.c`).
- **Scheduler / context switching:** Timer-driven, simple process switching support and stack restore trampoline (`kernel/process.c`, `kernel/restore_esp.asm`).
- **User-space & syscalls:** Enter user mode and syscall handling for basic user/kernel transitions (`kernel/enter_user_mode.asm`, `kernel/sycall_handeler.asm`, `kernel/syscall.c`, `user/user_space.c`).
- **Memory allocators & tests:** Page allocator, `kmalloc`/`kfree`, inspection helpers and an in-kernel test suite (`kernel/pmm.c`, `kernel/tests.c`).
- **Build/run tooling:** `make` builds `disk.img`, `make run` launches QEMU; build steps and sources are in the `makefile`.

## Prerequisites

- `nasm`
- `gcc` with multilib support (`-m32`) and `binutils`
- `qemu-system-i386` (for running the image)

On Debian/Ubuntu you can install the essentials with:

```bash
sudo apt install nasm gcc-multilib binutils qemu-system-x86
```

## Quick build

From the project root:

```bash
make
```

This builds the boot sector and kernel, then creates `disk.img`.

## Run (QEMU)

- Run with the provided Makefile target:

```bash
make run
```

- Or run directly:

```bash
qemu-system-i386 -drive format=raw,file=disk.img
```

## Debugging with GDB

To debug the kernel with GDB, start QEMU waiting for a GDB connection:

```bash
qemu-system-i386 -S -gdb tcp::1234 -drive format=raw,file=disk.img
```

Then connect from another terminal:

```bash
gdb -q -ex "target remote :1234" kernel/kernel.elf
```

This pauses the guest until the debugger connects (`-S`) and listens on TCP port 1234 for GDB.

## Makefile targets

- `all` / `image`: build the boot sector and kernel and produce `disk.img` (default `make` runs this).
- `boot`: assemble `boot/boot.asm` → `boot/boot.bin`.
- `kernel`: compile and link kernel C/ASM sources → `kernel.elf` and `kernel.bin`.
- `run`: launch `disk.img` in QEMU.
- `clean`: remove build artifacts.

See the `makefile` for the full list of source files used during the build.

## Tests

There is a small in-kernel test suite under `kernel/tests.c` / `kernel/tests.h`. Test helpers and primitives live in `kernel/pmm.c` and related files. Tests can be enabled from the kernel entry point if desired (see `kernel/kernel.c`).

## Project structure (high-level)

- `boot/` — boot sector sources (`boot.asm`)
- `kernel/` — kernel sources (C and assembly)
- `user/` — user-space helpers and sample user programs

Key kernel subsystems included:
- Protected-mode entry, GDT/TSS setup
- IDT and basic interrupt handlers (keyboard, PIT)
- Simple physical memory manager and paging helpers
- VGA text console driver
- Minimal process switching / scheduler
- User-mode entry and syscall handler

## Contributing

Contributions, bug reports, and suggestions are welcome — open an issue or submit a pull request. Keep changes small and focused.

## License

This project is provided for educational purposes. No explicit license is included; add one if you intend to share or accept contributions.

---

If you'd like, I can also:
- add a short build-and-run script,
- add a CONTRIBUTING.md, or
- enable an automated test target in the Makefile.
Tell me which you'd prefer.
