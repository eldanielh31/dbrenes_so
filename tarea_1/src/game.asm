; ******************************************************
;  * initialize the game state
;  *****************************************************
init_game:
  ; initialize the player
  mov word [player_pos], 0x0915

  ; initialize game counter
  mov dword [counter], 0x64 ; 100

  ; initialize the bullets
  mov byte [bullet_list], 0
  mov word [bullet_list_end], bullet_list

  ; initializae game state
  mov byte [game_state], GAME_STATE_PLAYING

  ret


; ***************************************************************
;  * update to game_state to check whether the game is finished
;  **************************************************************
; AL 0: still playing
;    1: invaders win
;    2: player wins
update_game_state:
  ; check whether the player is destroyed
  cmp word [player_pos], INVALID_STATE
  je .player_loose

  ; check whether the player wins
  ; cmp byte [num_invaders_alive], 0
  ; je .player_win

  ; check if player finished the game
  mov al, [key_pressed]
  cmp al, GAME_FINISHED_KEY
  je .game_finished_state

  ; check if time is greater than 0
  cmp byte [counter], 0
  jg .decrease_counter

  ; check if time is equal to 0
  cmp byte [counter], 0
  je .player_loose

  ; still playing
  mov byte [game_state], GAME_STATE_PLAYING
  jmp .done

.player_win:
  mov byte [game_state], GAME_STATE_PLAYER_WIN
  jmp .done
.player_loose:
  mov byte [game_state], GAME_STATE_PLAYER_LOOSE
  jmp .done
.game_finished_state:
  mov byte [game_state], GAME_STATE_GAME_FINISHED
  jmp .done
.decrease_counter:
  sub dword [counter], 1
.done:
  ret


; sleep
; DX Duration in micro-seconds
sleep:
  push ax

  mov	ah, 0x86
  int	0x15		; Sleep

  pop ax
  ret


; ******************************************************
;  * move an object
;  *****************************************************
; DX position to move
; AL direction
move:
  cmp al, MOVE_RIGHT
  je .right
  cmp al, MOVE_DOWN
  je .down
  cmp al, MOVE_LEFT
  je .left
  cmp al, MOVE_UP
  je .up

  cmp al, MOVE_LEFT_UP
  je .left_up
  cmp al, MOVE_LEFT_DOWN
  je .left_down
  cmp al, MOVE_RIGHT_UP
  je .right_up
  cmp al, MOVE_RIGHT_DOWN
  je .right_down

.up:
  cmp dh, 1
  jle .done
  sub	word dx, 0x0100
	jmp .done
.down:
  cmp dh, GAME_HEIGHT - 1
  jge .done
  add	word dx, 0x0100
	jmp .done
.left:
  cmp dl, 1
  jle .done
  sub	word dx, 0x0001
	jmp .done
.right:
  cmp dl, GAME_WIDTH - 1
  jge .done
  add	word dx, 0x0001
  jmp .done

.left_up:
  cmp dl, 1
  jle .done
  sub	word dx, 0x0001
  cmp dh, 1
  jle .done
  sub	word dx, 0x0100
  jmp .done

.left_down:
  cmp dl, 1
  jle .done
  sub	word dx, 0x0001
  cmp dh, GAME_HEIGHT - 1
  jge .done
  add	word dx, 0x0100
	jmp .done
  
.right_up:
  cmp dl, GAME_WIDTH - 1
  jge .done
  add	word dx, 0x0001
  cmp dh, 1
  jle .done
  sub	word dx, 0x0100
  jmp .done

.right_down:
  cmp dl, GAME_WIDTH - 1
  jge .done
  add	word dx, 0x0001
  cmp dh, GAME_HEIGHT - 1
  jge .done
  add	word dx, 0x0100
	jmp .done

.done:
  ret
