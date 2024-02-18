
all:
		rm -rf build
		mkdir build
		nasm -f bin src/hola.asm -o build/hola.bin
		nasm -f bin src/bootloader.asm -o build/bootloader.bin
		cat build/bootloader.bin build/hola.bin > build/game.bin

		qemu-system-x86_64 -fda build/game.bin