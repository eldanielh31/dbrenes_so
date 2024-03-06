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
  
  cmp al, MOVE_LEFT_UP_KEY
  je .left_up
  cmp al, MOVE_LEFT_DOWN_KEY
  je .left_down
  cmp al, MOVE_RIGHT_UP_KEY
  je .right_up
  cmp al, MOVE_RIGHT_DOWN_KEY
  je .right_down
  cmp al, SHOOT_KEY
  je .shoot
  cmp al, DELETE_KEY
  je .shoot_delete

  ;No key press
  jmp .check

.shoot_delete:
  cmp byte [player_can_delete], 0
  je .switch_delete
  mov byte [player_can_delete], 0
  jmp .check

.shoot:
  cmp byte [player_can_shoot], 0
  je .switch_shoot
  mov byte [player_can_shoot], 0
  jmp .check

.left:
  mov al, MOVE_LEFT
  call move
  mov byte [current_color], FG_BLUE
  jmp .make_shoot
.right:
  mov al, MOVE_RIGHT
  call move
  mov byte [current_color], FG_CYAN
  jmp .make_shoot
.up:
  mov al, MOVE_UP
  call move
  mov byte [current_color], FG_RED
  jmp .make_shoot
.down:
  mov al, MOVE_DOWN
  call move
  mov byte [current_color], FG_MAGENTA
  jmp .make_shoot
.left_up:
  mov al, MOVE_LEFT_UP
  call move
  mov byte [current_color], FG_YELLOW
  jmp .make_shoot
.left_down:
  mov al, MOVE_LEFT_DOWN
  call move
  mov byte [current_color], FG_BRIGHT_YELLOW
  jmp .make_shoot
.right_up:
  mov al, MOVE_RIGHT_UP
  call move
  mov byte [current_color], FG_WHITE
  jmp .make_shoot
.right_down:
  mov al, MOVE_RIGHT_DOWN
  call move
  mov byte [current_color], FG_DARK_GRAY
  jmp .make_shoot

.switch_delete:
  mov byte [player_can_delete], 1
  jmp .check
.switch_shoot:
  mov byte [player_can_shoot], 1
  jmp .check
.make_shoot:
  call check_square
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

check_square:
  cmp byte[player_prev_mov], al
  jne .player_next_pos
  jmp .done

  .player_next_pos:
    cmp byte[player_count_mov], 4
    je .player_check_win_square

    inc byte[player_count_mov]
    mov byte[player_prev_mov], al
    jmp .done
  
  .player_check_win_square:
    mov byte [player_win_square], 1
    jmp .done

    cmp [player_init_pos], dx
    jne .player_clear_status_win
    mov byte [player_win_square], 1
    jmp .done

  .player_clear_status_win:
    mov byte[player_count_mov], 0
    mov [player_init_pos], dx

  .done:
    ret
