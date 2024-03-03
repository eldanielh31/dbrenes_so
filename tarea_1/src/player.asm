; ******************************************************
;  * move
;  *****************************************************
move_player:
  push ax
  push dx

  ; load data
  mov dx, [player_pos]
  mov al, [key_pressed]

  cmp al, MOVE_LEFT_KEY
  je .left
  cmp al, MOVE_RIGHT_KEY
  je .right
  cmp al, MOVE_UP_KEY
  je .up
  cmp al, MOVE_DOWN_KEY
  je .down
  cmp al, SHOOT_KEY
  je .shoot
  jmp .check
.shoot:
  cmp byte [player_can_shoot], 0
  je .switch_shoot
  mov byte [player_can_shoot], 0
  jmp .check
.left:
  mov al, MOVE_LEFT
  call move
  jmp .make_shoot
.right:
  mov al, MOVE_RIGHT
  call move
  jmp .make_shoot
.up:
  mov al, MOVE_UP
  call move
  jmp .make_shoot
.down:
  mov al, MOVE_DOWN
  call move
  jmp .make_shoot
.switch_shoot:
  mov byte [player_can_shoot], 1
.make_shoot:
  cmp byte [player_can_shoot], 0
  je .check
.true_shoot:
  call create_player_bullet
.check:
  call check_bullet_collisions
  mov [player_pos], dx
.done:
  pop dx
  pop ax
  ret

; ******************************************************
;  * render
;  *****************************************************
render_player:
  push ax
  push dx
  mov dx, [player_pos]
  cmp dx, INVALID_STATE
  je .done
  mov al, ICON_PLAYER
  mov bl, FG_BRIGHT_GREEN
  add bl, BG_BLACK
  call print_object
.done:
  pop dx
  pop ax
  ret
