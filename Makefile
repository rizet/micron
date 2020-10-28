.DEFAULT_GOAL := all

# ifeq (, $(shell which grub-mkrescue 2>/dev/null))
# 	ifeq (, $(shell which grub2-mkrescue 2>/dev/null))
# 		$(error "No grub-mkrescue or grub2-mkrescue in $(PATH)")
# 	else
#		MKRESCUE = grub2-mkrescue
# 	endif
# endif


ARCH := UNSPECIFIED
BUILD_DIR := bin
SRC_DIR := src
RES_DIR := res
FONTS_DIR := res/fonts
KERNEL_SRC_DIR := $(SRC_DIR)/kernel
BOOTLOADER_SRC_DIR := $(SRC_DIR)/bootloader
HEADERS_DIR := $(SRC_DIR)/include
OBJ_DIR := $(BUILD_DIR)/obj
OBJ_DIR_RES := $(OBJ_DIR)/res
CC := gcc -I$(HEADERS_DIR) -pipe -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wnon-virtual-dtor -g -fpic -m32 -march=i686 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-pic -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
CXX := g++ -I$(HEADERS_DIR) -pipe -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wnon-virtual-dtor -g -fpic -m32 -march=i686 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-pic -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
MINGW := x86_64-w64-mingw32-gcc -ffreestanding -I$(HEADERS_DIR) -I$(HEADERS_DIR)/efi/x86_64 -I$(HEADERS_DIR)/efi/protocol
MINGW_LINK := x86_64-w64-mingw32-gcc -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -lgcc
ASM := nasm -f elf
STATIC_LINK := llvm-ar
OBJCOPY := objcopy -O elf32-i386 -B i386 -I binary
CXX_LINK := g++ -m32 -Wl,--as-needed -Wl,--no-undefined -m32 -march=i686 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-pic -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive
IMAGE_GEN := $(RES_DIR)/GenerateImage

export MKRESCUE
export GCC
export BUILD_DIR

kernel: clean
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR_RES)
	$(ASM) $(KERNEL_SRC_DIR)/idt.asm -o $(OBJ_DIR)/idt.o
	$(ASM) $(KERNEL_SRC_DIR)/gdt.asm -o $(OBJ_DIR)/gdt.o
	$(ASM) $(KERNEL_SRC_DIR)/interrupts.asm -o $(OBJ_DIR)/ISR.o
	$(ASM) $(KERNEL_SRC_DIR)/memset.asm -o $(OBJ_DIR)/MemSet.o
	$(ASM) $(KERNEL_SRC_DIR)/paging.asm -o $(OBJ_DIR)/paging.o
	$(CC)  -MF$(OBJ_DIR)/kstart.S.o.d -o $(OBJ_DIR)/kstart.S.o -c $(KERNEL_SRC_DIR)/kstart.S
	$(CXX) -MF$(OBJ_DIR)/gdt.cxx.o.d -o $(OBJ_DIR)/gdt.cxx.o -c $(KERNEL_SRC_DIR)/gdt.cxx
	$(CXX) -MF$(OBJ_DIR)/pic.cxx.o.d -o $(OBJ_DIR)/pic.cxx.o -c $(KERNEL_SRC_DIR)/pic.cxx
	$(CXX) -MF$(OBJ_DIR)/paging.cxx.o.d -o $(OBJ_DIR)/paging.cxx.o -c $(KERNEL_SRC_DIR)/paging.cxx
	$(CXX) -MF$(OBJ_DIR)/io.cxx.o.d -o $(OBJ_DIR)/io.cxx.o -c $(KERNEL_SRC_DIR)/io.cxx
	$(CXX) -MF$(OBJ_DIR)/terminal.cxx.o.d -o $(OBJ_DIR)/terminal.cxx.o -c $(KERNEL_SRC_DIR)/terminal.cxx
	$(CXX) -MF$(OBJ_DIR)/debugging.cxx.o.d -o $(OBJ_DIR)/debugging.cxx.o -c $(KERNEL_SRC_DIR)/debugging.cxx
	$(CXX) -MF$(OBJ_DIR)/kutil.cxx.o.d -o $(OBJ_DIR)/kutil.cxx.o -c $(KERNEL_SRC_DIR)/kutil.cxx
	$(CXX) -MF$(OBJ_DIR)/kbd.cxx.o.d -o $(OBJ_DIR)/kbd.cxx.o -c $(KERNEL_SRC_DIR)/kbd.cxx
	$(CXX) -MF$(OBJ_DIR)/gfx.cxx.o.d -o $(OBJ_DIR)/gfx.cxx.o -c $(KERNEL_SRC_DIR)/gfx.cxx
	$(CXX) -MF$(OBJ_DIR)/string.cxx.o.d -o $(OBJ_DIR)/string.cxx.o -c $(KERNEL_SRC_DIR)/string.cxx
	$(CXX) -MF$(OBJ_DIR)/kmain.cxx.o.d -o $(OBJ_DIR)/kmain.cxx.o -c $(KERNEL_SRC_DIR)/kmain.cxx -DARCH=\"$(ARCH)\"
	$(OBJCOPY) $(FONTS_DIR)/font.psf $(OBJ_DIR_RES)/font.o
	$(CXX_LINK) -o $(BUILD_DIR)/microCORE.kernel $(OBJ_DIR)/kmain.cxx.o $(OBJ_DIR)/gdt.o $(OBJ_DIR)/idt.o $(OBJ_DIR)/ISR.o $(OBJ_DIR)/MemSet.o $(OBJ_DIR)/paging.o $(OBJ_DIR)/kstart.S.o $(OBJ_DIR)/gdt.cxx.o $(OBJ_DIR)/io.cxx.o $(OBJ_DIR)/paging.cxx.o $(OBJ_DIR)/terminal.cxx.o $(OBJ_DIR)/pic.cxx.o $(OBJ_DIR)/debugging.cxx.o $(OBJ_DIR)/kutil.cxx.o $(OBJ_DIR)/kbd.cxx.o $(OBJ_DIR_RES)/font.o $(OBJ_DIR)/gfx.cxx.o $(OBJ_DIR)/string.cxx.o -T $(RES_DIR)/Linkerscript

bootloader:
	$(MINGW) -c $(BOOTLOADER_SRC_DIR)/loader.c -o $(OBJ_DIR)/loader.o
	$(MINGW) -c $(BOOTLOADER_SRC_DIR)/data.c -o $(OBJ_DIR)/data.o
	$(MINGW_LINK) -e efi_main -o $(BUILD_DIR)/bootloader.efi $(OBJ_DIR)/loader.o $(OBJ_DIR)/data.o

image: kernel bootloader
	$(IMAGE_GEN) $(BUILD_DIR)/microCORE.kernel $(BUILD_DIR)/bootloader.efi

qemu:
	$(MAKE) image ARCH=i686
	qemu-system-x86_64 -bios res/OVMF.fd -cdrom microNET.iso -m 512M
	
clean:
	rm -rfv $(BUILD_DIR) iso

all: image
