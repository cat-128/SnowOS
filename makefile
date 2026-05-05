AS = nasm
CXX = g++
LD = ld

KERNEL_BIN = snowOS.bin
OBJ_FILES = boot.o kernel.o

# Compilation flags
ASFLAGS = -felf32
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
LDFLAGS = -m32 -T linker.ld

all: $(KERNEL_BIN)

# link the kernel
$(KERNEL_BIN): $(OBJ_FILES)
	$(LD) -m elf_i386 -T linker.ld -o $(KERNEL_BIN) $(OBJ_FILES)

# assemble boot.s
boot.o: boot.s
	$(AS) $(ASFLAGS) boot.s -o boot.o

# compile kernel.c
kernel.o: kernel.cpp
	$(CXX) $(CFLAGS) -c kernel.cpp -o kernel.o

run: $(KERNEL_BIN)
	qemu-system-i386 -kernel $(KERNEL_BIN)

clean:
	rm -f $(OBJ_FILES) $(KERNEL_BIN)