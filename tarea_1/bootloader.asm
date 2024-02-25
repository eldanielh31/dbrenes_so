org 0x7c00 ; indica la posicion de memoria donde estara el codigo
SECTOR_AMOUNT equ 0x4 ; Sectores que abarcara el software, 1 sector equivale a 512bytes
jmp short start


start:
;inicializa registros
cli ;deshabilita las interrupciones globales
xor ax, ax ;se establece los registros a 0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax
mov sp, 0x6ef0 ; se coloca el stack pointer a una posicion, en este ejemplo es el mismo que quemu
sti ;habilita interrupciones

mov ah, 0 ;se resetea el modo del disco
int 0x13 ;interrupcion para utilizar el disco con el bios
        ; read from harddrive and write to ram
mov bx, 0x8000 ; bx = direccion de memoria para leer (puntero)
mov al, SECTOR_AMOUNT ; al = cantidad de sectores para lectura
mov ch, 0 ;cilindro =0
mov dh, 0 ;cabeza = 0
mov cl, 2 ;sector = 2
mov ah, 2 ;ah = 2 lee desde el disco
int 0x13 ;interrupcion para utilizar el disco con el bios
jmp 0x8000

;padding and signature
times 510-($-$$) db 0 ;rellena a 512 con cero
db 0x55
db 0xaa ;numero magico, es para indicar que es un bootloader
