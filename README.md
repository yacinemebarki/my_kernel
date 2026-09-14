# mini_kernel

mini_kernel is a small 32-bit x86 educational operating system written in NASM and C. It is designed to teach low-level concepts such as booting, protected mode, interrupts, memory management, ELF loading, and basic user-space execution.

## Overview

The project builds a raw disk image containing a boot sector and a minimal protected-mode kernel. It is intentionally small and approachable, making it a good base for learning how an OS starts, loads code, handles processes, and interacts with hardware.

## Current features

mini_kernel already includes a solid base for a minimalist x86 operating system. The current implementation focuses on early boot, processor setup, memory management, and basic user-mode execution.

### Boot and CPU startup

- Protected-mode boot process with GDT setup
- BIOS memory map collection via E820
- Basic kernel loading from disk using BIOS LBA reads
- Transition from 16-bit real mode into 32-bit protected mode
- TSS and task-switch related setup for user mode

### Memory and process management

- Basic page frame / physical memory management
- Simple allocator and memory inspection helpers
- Scheduler and context switching support for cooperative task execution
- Process lifecycle helpers including creation, exit, wait, sleep, and yield semantics
- User-space entry and return-to-user flow

### Hardware and device support

- VGA text-mode console driver
- Keyboard interrupt handling and input support
- PIT timer integration for scheduling and timing primitives
- Exception and interrupt dispatch infrastructure

### Userland and ELF loading

- ELF user program loading and execution
- User-mode syscall interface
- Process creation from ELF images
- Example user programs and shell-like environment

### Kernel tooling and workflow

- Build system for assembling bootloader and kernel objects
- QEMU-based execution for testing and debugging
- In-kernel test helpers and debugging-oriented utilities

These features are intentionally small and educational, which makes the codebase easier to follow while still demonstrating how a real kernel is assembled.

## Project structure

- boot/ — bootloader source and startup code
- kernel/ — kernel logic, drivers, scheduler, memory, and syscall code
- user/ — sample user programs and user-space utilities
- link.ld — linker script
- makefile — build instructions and target definitions

## Prerequisites

Install the dependencies needed to build and run the OS:

```bash
sudo apt install nasm gcc-multilib binutils qemu-system-x86
```

Required tools:

- nasm
- gcc with 32-bit support
- ld / binutils
- qemu-system-i386

## Build

From the repository root:

```bash
make
```

This creates the boot sector and kernel artifacts and produces a raw disk image called `disk.img`.

## Run

```bash
make run
```

Or directly:

```bash
qemu-system-i386 -drive format=raw,file=disk.img
```

## Debugging

Start QEMU with a GDB stub:

```bash
qemu-system-i386 -S -gdb tcp::1234 -drive format=raw,file=disk.img
```

Then connect in another terminal:

```bash
gdb -q -ex "target remote :1234" kernel/kernel.elf
```

## Makefile targets

- `make` / `make all` — build everything
- `make boot` — assemble the boot sector
- `make kernel` — build kernel components and link ELF binaries
- `make run` — launch the OS in QEMU
- `make clean` — remove generated files

## OS TODO roadmap

This project is intentionally small, but there are several areas where contributors can help extend it. The following features are great candidates for new pull requests and experiments:

- [ ] Shell arguments
- [ ] ps command
- [ ] File descriptors
- [ ] Pipe support
- [ ] stdin/stdout redirection
- [ ] Filesystem
- [ ] Better scheduler
- [ ] Signals
- [ ] Dynamic memory improvements
- [ ] SMP
- [ ] Networking

## Good contribution areas

If you want to contribute, a few particularly useful areas are:

- userland shell and command parsing
- process management and scheduler improvements
- ELF loading and memory safety
- keyboard/console UX improvements
- documentation and debugging notes
- kernel testing and cleanup

## Contributing

Contributions are welcome. A good workflow is:

1. Fork the repository.
2. Create a focused feature branch.
3. Keep changes small and understandable.
4. Build the project with `make`.
5. Open a pull request with a clear explanation of the change.

Please keep patches targeted and document any new behavior or limitations in the README or relevant source comments.

## License

This project is provided for educational and experimental purposes. There is no explicit license file yet, so if you plan to distribute or accept external contributions widely, it is recommended to add one.

---
