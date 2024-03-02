; constants
org 0x8000
; GAME WINDOW PARAMETERS
%define GAME_WIDTH 33
%define GAME_HEIGHT 25

; GAME PARAMETERES
%define NUM_INVADERS 12
%define INVADERS_MOVE_CYCLES 40
%define BULLETS_MOVE_CYCLE 28

; SPECIAL CONSTANT
%define INVALID_STATE 0x0000

; ICONS
%define ICON_PLAYER 'M'
%define ICON_INVADER 'T'
%define ICON_BULLET '|'
%define ICON_EXPLOSION_BULLET '#'
%define ICON_WALL '#'

; GAME DIFFICULTY LEVEL KEYS
%define GAME_EASY_LEVEL_KEY '1'
%define GAME_MEDIUM_LEVEL_KEY '2'
%define GAME_HARD_LEVEL_KEY '3'

; INVADERS DIFFICULTY LEVEL (invaders shoot cycles)
%define INVADERS_EASY_LEVEL 6

; GAME STATES
%define GAME_STATE_PLAYING 0
%define GAME_STATE_PLAYER_WIN 1
%define GAME_STATE_INVADERS_WIN 2

; PLAY KEYS
%define START_KEY ' '
%define RETRY_KEY 'r'

%define MOVE_LEFT_KEY 'j'
%define MOVE_RIGHT_KEY 'l'
%define MOVE_UP_KEY 'i'
%define MOVE_DOWN_KEY 'k'
%define SHOOT_KEY ' '

; MOVE DIRECTIONS
%define MOVE_UP 0
%define MOVE_RIGHT 1
%define MOVE_DOWN 2
%define MOVE_LEFT 3
%define MOVE_RESET 4

; BULLET
%define BULLET_STATUS_END_OF_LIST 0
%define BULLET_STATUS_EXPLOSION '#'
%define BULLET_STATUS_PLAYER 'p'
%define BULLET_STATUS_INVADER 'i'

%define BULLET_STATUS_OFFSET 0
%define BULLET_STATUS_SIZE 1
%define BULLET_POSITION_OFFSET BULLET_STATUS_OFFSET + BULLET_STATUS_SIZE
%define BULLET_POSITION_SIZE 2
%define BULLET_SIZE BULLET_POSITION_OFFSET + BULLET_POSITION_SIZE

; INVADER
%define INVADER_POSITION_OFFSET 0
%define INVADER_POSITION_SIZE 2
%define INVADER_SIZE INVADER_POSITION_OFFSET + INVADER_POSITION_SIZE

; COLOR
%define FG_BLACK          00000000b
%define BG_BLACK          00000000b
%define FG_BLUE           00000001b
%define BG_BLUE           00010000b
%define FG_GREEN          00000010b
%define BG_GREEN          00100000b
%define FG_CYAN           00000011b
%define BG_CYAN           00110000b
%define FG_RED            00000100b
%define BG_RED            01000000b
%define FG_MAGENTA        00000101b
%define BG_MAGENTA        01010000b
%define FG_YELLOW         00000110b
%define BG_YELLOW         01100000b
%define FG_LIGHT_GRAY     00000111b
%define BG_LIGHT_GRAY     01110000b
%define FG_DARK_GRAY      00001000b
%define BG_DARK_GRAY      10000000b
%define FG_BRIGHT_BLUE    00001001b
%define BG_BRIGHT_BLUE    10010000b
%define FG_BRIGHT_GREEN   00001010b
%define BG_BRIGHT_GRENN   10100000b
%define FG_BRIGHT_CYAN    00001011b
%define BG_BRIGHT_CYAN    10110000b
%define FG_BRIGHT_RED     00001100b
%define BG_BRIGHT_RED     11000000b
%define FG_BRIGHT_MAGENTA 00001101b
%define BG_BRIGHT_MAGENTA 11010000b
%define FG_BRIGHT_YELLOW  00001110b
%define BG_BRIGHT_YELLOW  11100000b
%define FG_WHITE          00001111b
%define BG_WHITE          11110000b

; clear the cursor blinking
mov	ah, 0x01
mov	cx, 0x2000
int 	0x10

; calculate game screen position
mov ah, 0x0F
int 0x10 ; load the number of columns
sub byte ah, 1
sub ah, GAME_WIDTH
sar ah, 1
mov [display_offset], ah


jmp main

; include dependencies
%include "./src/keyboard.asm"
%include "./src/display.asm"
%include "./src/game.asm"

%include "./src/bullets.asm"
; %include "./src/invaders.asm"
%include "./src/player.asm"
%include "./src/arena.asm"


; main loop
main:
  call intro
.game:
  call select_difficulty_level
  call game
  call end
  jmp .game


; intro screen
intro:
  call clear_screen

  mov ax, intro_string_t
  mov bx, intro_string_o
  call print_window
.wait:
  call get_key
  mov al, [key_pressed]
  cmp al, START_KEY
  je .done
  jmp .wait
.done:
  ret

; select difficulty level
select_difficulty_level:
  je .easy_level
.easy_level:
  mov byte [invaders_shoot_cycles], INVADERS_EASY_LEVEL
  jmp .done
.done:
  ret


; game loop
game:
  call init_game
.loop:

  ; get key if available
  call check_key

  ; move
  call move_bullets
  call move_player
  ; call move_invaders

  ; render
  call clear_screen
  call render_arena
  call render_bullets
  call render_player
  ; call render_invaders
  call render_controlls

  ; update to game state
  call update_game_state
  cmp byte [game_state], GAME_STATE_PLAYING
  jne .done

  mov cx, 0x0000  ; 0.05 seconds (cx:dx)
  mov dx, 0x1388  ; 0x00001388 = 5000
  call sleep
  jmp	.loop
.done:
  ret


; end screen
end:
  cmp byte [game_state], GAME_STATE_PLAYER_WIN
  je .player_win
  mov ax, end_string_l
  jmp .print_game_result
.player_win:
  mov ax, end_string_w
.print_game_result:
  mov bx, end_string_o
  call print_window
.wait:
  call get_key
  mov al, [key_pressed]
  cmp al, RETRY_KEY
  je .done
  jmp .wait
.done:
  ret


; window
window_bar db "############################", 0
window_space db "#                          #", 0

; intro
intro_string_t db "#       MICRO MUNDOS       #", 0
intro_string_o db "#   Press SPACE to start   #", 0

; end
end_string_w db "#       PLAYER  wins       #", 0
end_string_l db "#       INVADERS win       #", 0
end_string_o db "#    Press R to restart    #", 0

; controls
left_string db "J = move left", 0
right_string db "L = move right", 0
up_string db "I = move up", 0
down_string db "K = move down", 0
shoot_string db "SPACE = print", 0


segment .bss
  ; display properties
  display_offset resb 1

  ; keyboard
  key_pressed resb 1

  ; game state
  ; 0: still playing
  ; 1: player wins
  ; 2: invaders win
  game_state resb 1

  ; player
  player_pos resw 1
  ; invaders

  ; invaders resw NUM_INVADERS
  ; num_invaders_alive resb 1
  ; invaders_move_direction resb 1
  ; invaders_move_cycle resb 1
  ; invaders_shoot_cycle resb 1
  invaders_shoot_cycles resb 1

  ; bullets:  0x STATUS PY PX
  ; STATUS == 0: end of list
  ; STATUS == #: explosion
  ; STATUS == p: player bullet
  ; STATUS == i: invader bullet

  bullets_move_cycle resb 1

  bullet_list_end resw 1
  bullet_list resb 1
