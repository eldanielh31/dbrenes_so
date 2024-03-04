clear_screen:
  mov ax, 0x0700
  mov bh, 0x0f
  xor cx, cx
  mov dx, 0x1950
  int 0x10
  ret

; print the window
; AX: title
; BX: options
print_window:
  push si
  push dx
  push bx
  push ax
  mov bl, FG_LIGHT_GRAY
  add bl, BG_BLACK
  mov dl, [display_offset]
  add dl, 3
  ; top
  mov dh, 0x05
  mov si, window_bar
  call print_string
  ; space 1
  inc dh
  mov si, window_space
  call print_string
  ; title
  inc dh
  pop si
  call print_string
  ; space 2
  inc dh
  mov si, window_space
  call print_string
  ; options
  inc dh
  pop si
  call print_string
  ; space 3
  inc dh
  mov si, window_space
  call print_string
  ; bottom
  inc dh
  mov si, window_bar
  call print_string
  ; return
  pop dx
  pop si
  ret

; DX position of char
; AL character
; BL attribute
print_object:
  add byte dl, [display_offset]
  call move_cursor
  sub byte dl, [display_offset]
  call print_char
  ret

; DX cursor position
move_cursor:
  mov ah, 0x02
  xor bh, bh
  int 0x10
  ret

get_cursor_position:
  push cx
  mov ah, 0x03 ; read cursor position
  xor bh, bh
  int 0x10
  pop cx
  ret

; DX cursor position
move_cursor_next:
  push dx
  call get_cursor_position
  inc dl
  cmp dl, 0x50 ; right edge of screen
  jle .done
  inc dh       ; move cursor to new line
  mov dl, 0x00
  cmp dh, 0x19 ; end of screen
  jle .done
  call clear_screen
.done:
  call move_cursor
  pop dx
  ret

; AL character
; BL attribute
print_char:
  push cx
  mov ah, 0x09 ; indicating the write Char/Attr pair function
  mov bh, 0x00 ; page number
  mov cx, 0x01 ; repeat count
  int 0x10
  call move_cursor_next
  pop cx
  ret

; BL attribute
; DX position
; SI string pointer
print_string:
  push ax
  call move_cursor
.loop:
  mov al, [si]
  cmp al, 0x00
  je .done
  call print_char
  inc si
  jmp .loop
.done:
  pop ax
  ret

; print keys to screen
render_controlls:
  push si
  push dx
  mov bl, FG_LIGHT_GRAY
  add bl, BG_BLACK
  mov dx, 0x0000

  ; print movemente
  mov si, space_string
  call print_string
  inc dh
  mov si, movement_string
  call print_string
  inc dh

  mov si, left_string
  call print_string
  inc dh
  mov si, right_string
  call print_string
  inc dh
  mov si, up_string
  call print_string
  inc dh
  mov si, down_string
  call print_string
  inc dh

  mov si, space_string
  call print_string
  inc dh

  mov si, left_up_string
  call print_string
  inc dh
  mov si, left_down_string
  call print_string
  inc dh
  mov si, right_up_string
  call print_string
  inc dh
  mov si, right_down_string
  call print_string
  inc dh
  
  mov si, space_string
  call print_string
  inc dh

  ; print shoot key
  mov si, shoot_string
  call print_string
  inc dh

  mov si, space_string
  call print_string
  inc dh

  ; print finish game key
  mov si, finish_game_string
  call print_string
  inc dh

  mov si, space_string
  call print_string
  inc dh
  
  ; print time
  mov si, time_string
  call print_string
  inc dh

  mov si, space_string
  call print_string
  inc dh
  mov si, space_string
  call print_string
  inc dh
  mov si, space_string
  call print_string
  inc dh

  ; print painting
  cmp byte [player_can_shoot], 0
  je not_painting

  cmp byte [player_can_shoot], 1
  je painting

not_painting:
  mov si, not_painting_string
  call print_string
  inc dh
  mov si, space_string
  call print_string
  jmp done

painting:
  mov si, painting_string
  call print_string
  inc dh
  mov si, space_string
  call print_string
  jmp done

done:
  pop dx
  pop si
  ret


int_to_str:
  ;**********************************************
  ; print time value
  ; Convert the value of the counter to a string
  lea si, [counter]
  lea di, buffer
  ; call itoa

  ; Print the string
  lea si, counter_string
  call print_string
  lea si, buffer
  call print_string

  ; Print a newline
  ;lea si, newline
  ;call print_string

  pop dx
  pop si
  ret


itoa:
    mov eax, [si]      ; Get the value of the counter
    mov ecx, 10        ; Divisor for extracting digits
    mov edi, buffer    ; Destination buffer
    add edi, 3         ; Point to the end of the buffer

reverseLoop:
    xor edx, edx       ; Clear any previous remainder
    div ecx            ; Divide eax by 10, quotient in eax, remainder in edx
    add dl, '0'        ; Convert the digit to ASCII
    dec edi            ; Move the destination pointer backward
    mov [edi], dl      ; Store the digit in the buffer
    test eax, eax      ; Check if quotient is zero
    jnz reverseLoop    ; If not, repeat the loop

    ; Copy the result to the beginning of the buffer
    lea si, [edi]
    lea di, buffer
    mov ecx, 4         ; Length of the string, including null terminator
    rep movsb

    ret
