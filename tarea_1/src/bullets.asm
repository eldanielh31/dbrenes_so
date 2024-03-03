; ******************************************************
;  * render all bullets
;  *****************************************************

; render all bullets
render_bullets:
  push di
  mov di, _render_bullet
  call _iterate_bullets
  pop di
  ret

; render a single bullet
; SI bullet pointer
_render_bullet:
  push ax
  push dx
  mov al, [si]      ; load status
  mov dx, [si + BULLET_POSITION_OFFSET]  ; load position
  cmp al, ICON_EXPLOSION_BULLET
  jne .set_bullet
  mov bl, FG_GREEN
  add bl, BG_BLACK
  jmp .print
.set_bullet:
  mov al, ICON_BULLET  ; set bullet
  mov bl, FG_RED
  add bl, BG_BLACK
.print:
  call print_object
  pop dx
  pop ax
  ret


; ******************************************************
;  * check for collisions
;  *****************************************************

; check for collisions
; DX position of the object
check_bullet_collisions:
  mov di, _check_bullet_collision
  call _iterate_bullets
.done:
  ret

; check for a collision between a bullet and an object
; DX object position
; SI bullet pointer
_check_bullet_collision:
  push ax
  mov ax, [si + BULLET_POSITION_OFFSET]  ; load position
  cmp ax, dx
  jne .done
  mov dx, INVALID_STATE      ; set position to invalid state
  mov byte [si], ICON_EXPLOSION_BULLET  ; set bullet status to explosion
.done:
  pop ax
  ret


; ******************************************************
;  * create new bullets
;  *****************************************************

; let the player shoot a bullet
create_player_bullet:
  push ax
  mov al, BULLET_STATUS_PLAYER
  call _create_bullet
  pop ax
  ret

; create a new bullet
; DX position of creator
; AL status
_create_bullet:
  push dx
  push di
  cmp al, BULLET_STATUS_PLAYER
  je .player
.player:
  dec dh  ; adjust the creator position
.create:
  mov di, [bullet_list_end]
  mov [di], al  ; save the status
  mov [di + BULLET_POSITION_OFFSET], dx  ; save the position
  add di, BULLET_SIZE
  mov byte [di], 0x00 ; set the end of the list
  mov [bullet_list_end], di ; save the list end
.done:
  pop di
  pop dx
  ret


; ******************************************************
;  * remove a bullet
;  *****************************************************

; delete the bullet at SI
_remove_bullet:
  push ax
  push si
.loop:
  cmp si, [bullet_list_end]
  je .done
  mov al, [si + BULLET_SIZE]  ; copy the status
  mov [si], al
  mov ax, [si + BULLET_SIZE + BULLET_POSITION_OFFSET]  ; copy the position
  mov [si + BULLET_POSITION_OFFSET], ax
  add si, BULLET_SIZE             ; set SI to the next bullet
  jmp .loop
.done:
  sub word [bullet_list_end], BULLET_SIZE ; adjust end of list
  pop si
  pop ax
  ret


; ******************************************************
;  * iterate bullets
;  *****************************************************

; cycle through bullets
; DI address of the loop functions
; calls the function in DX with:
; SI bullet pointer
_iterate_bullets:
  push si
  mov si, bullet_list
.loop:
  cmp si, [bullet_list_end]
  je .done
  call di
  add si, 3
  jmp .loop
.done:
  pop si
  ret
