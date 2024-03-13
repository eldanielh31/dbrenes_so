
all:
		rm -rf build
		mkdir build
		nasm -f bin game.asm -o build/game.bin
		nasm -f bin bootloader.asm -o build/bootloader.bin
		cat build/bootloader.bin build/game.bin > build/boot.bin

		qemu-system-x86_64 -fda build/boot.bin

# sudo dd if=/dev/sda1 bs=512 count=1 2>/dev/null | hexdump -C
