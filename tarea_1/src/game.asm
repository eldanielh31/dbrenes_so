; ******************************************************
;  * initialize the game state
;  *****************************************************
init_game:
  ; initialize the player
  mov word [player_pos], 0x0915

  ; initialize game counter
  mov byte [counter], 15  ;0x3C = 60

  ; initialize the bullets
  mov byte [bullet_list], 0
  mov word [bullet_list_end], bullet_list

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
  je .invaders_win

  ; check whether the player wins
  ; cmp byte [num_invaders_alive], 0
  ; je .player_win

  ; still playing
  mov byte [game_state], GAME_STATE_PLAYING
  jmp .done
.invaders_win:
  mov byte [game_state], GAME_STATE_INVADERS_WIN
  jmp .done
.player_win:
  mov byte [game_state], GAME_STATE_PLAYER_WIN
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
.done:
  ret
