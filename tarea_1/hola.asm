ORG 0x8000    ; Dirección de inicio del código

section .text
    global _start

_start:
    ; Configuración del modo de video
    mov ah, 0x00    ; Función 0x00 de int 0x10 - Set Video Mode
    mov al, 0x03    ; Modo de video 80x25, 16 colores
    int 0x10        ; Llamar a la interrupción del BIOS para configurar el modo de video

    ; Imprimir mensaje en pantalla
    mov ah, 0x09    ; Función 0x09 de int 0x10 - Write String
    mov si, mensaje ; Puntero al mensaje
    mov bh, 0x00    ; Página de visualización
    mov bl, 0x07    ; Atributo de color (blanco sobre negro)
    mov cx, longitud_mensaje    ; Longitud del mensaje
    int 0x10        ; Llamar a la interrupción del BIOS para escribir el mensaje

    ; Bucle infinito
    jmp $           ; Saltar a la dirección actual (bucle infinito)

mensaje db 'Hola, mundo!', 0x0D, 0x0A, 0x00    ; Mensaje a imprimir
longitud_mensaje equ $ - mensaje    ; Calcular la longitud del mensaje

times 510 - ($ - $$) db 0    ; Rellenar con ceros hasta la posición 510
dw 0xAA55                     ; Firma de arranque del sector de arranque
