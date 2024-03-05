; constants
org 0x8000
; GAME WINDOW PARAMETERS
%define GAME_WIDTH 33
%define GAME_HEIGHT 25


; SPECIAL CONSTANT
%define INVALID_STATE 0x0000

; ICONS
%define ICON_PLAYER 232
%define ICON_INVADER 'T'
%define ICON_BULLET '|'
%define ICON_EXPLOSION_BULLET ' '
%define ICON_WALL '#'

; GAME DIFFICULTY LEVEL KEYS
%define GAME_EASY_LEVEL_KEY '1'
%define GAME_MEDIUM_LEVEL_KEY '2'
%define GAME_HARD_LEVEL_KEY '3'

; INVADERS DIFFICULTY LEVEL (invaders shoot cycles)
%define INVADERS_EASY_LEVEL 6

; PLAYER SHOOTING INITIAL STATE (PLAYER_CAN_SHOOT)
%define PLAYER_CAN_SHOOT_GLOBAL 0

; PLAYER DELETE INITIAL STATE (PLAYER_CAN_DELETE)
%define PLAYER_CAN_DELETE_GLOBAL 0

; GAME STATES
%define GAME_STATE_PLAYING 0
%define GAME_STATE_PLAYER_WIN 1
%define GAME_STATE_PLAYER_LOOSE 2
%define GAME_STATE_GAME_FINISHED 3

; PLAY KEYS
%define START_KEY ' '
%define RETRY_KEY 'r'
%define GAME_FINISHED_KEY 'f'

%define MOVE_LEFT_KEY 'j'
%define MOVE_RIGHT_KEY 'l'
%define MOVE_UP_KEY 'i'
%define MOVE_DOWN_KEY 'k'

%define MOVE_LEFT_UP_KEY 'a'
%define MOVE_LEFT_DOWN_KEY 'q'
%define MOVE_RIGHT_UP_KEY 'd'
%define MOVE_RIGHT_DOWN_KEY 'e'

%define SHOOT_KEY ' '
%define DELETE_KEY 'z'

; MOVE DIRECTIONS
%define MOVE_UP 0
%define MOVE_RIGHT 1
%define MOVE_DOWN 2
%define MOVE_LEFT 3

%define MOVE_LEFT_UP 4
%define MOVE_LEFT_DOWN 5
%define MOVE_RIGHT_UP 6
%define MOVE_RIGHT_DOWN 7

%define MOVE_RESET 8

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
%include "./src/player.asm"
%include "./src/arena.asm"


; main loop
main:
  call intro
.game:
  call set_global_variables
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

; set glob var
set_global_variables:
  mov byte [player_can_shoot], PLAYER_CAN_SHOOT_GLOBAL
  mov byte [player_can_delete], PLAYER_CAN_DELETE_GLOBAL
.done:
  ret


; game loop
game:
  call init_game
.loop:

  ; get key if available
  call check_key

  ; move
  ; call move_bullets
  call move_player

  ; render
  call clear_screen
  call render_arena
  call render_bullets
  call render_player

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
  cmp byte [game_state], GAME_STATE_PLAYER_LOOSE
  je .player_loose
  cmp byte [game_state], GAME_STATE_GAME_FINISHED
  je .game_finished

.player_win:
  mov ax, end_string_w
  mov bx, end_string_o
  call print_window
  jmp .wait

.player_loose:
  mov ax, end_string_l
  mov bx, end_string_o
  call print_window
  jmp .wait

.game_finished:
  mov ax, end_string_game_finished
  mov bx, end_string_o
  call print_window
  jmp .wait

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
end_string_l db "#        YOU LOOSE         #", 0
end_string_game_finished db "#  YOU FINISHED THE GAME!  #", 0
end_string_o db "#    Press R to restart    #", 0

; controls
space_string db " ", 0 

movement_string db " Movement:", 0 
left_string db " J = move left", 0
right_string db " L = move right", 0
up_string db " I = move up", 0
down_string db " K = move down", 0

left_up_string db " A = move left up", 0
left_down_string db " Q = move left down", 0
right_up_string db " D = move right up", 0
right_down_string db " E = move right down", 0

shoot_string db " SPACE = print color", 0

painting_string db " Painting = YES", 0
not_painting_string db " Painting = NO", 0

deleting_string db " Deleting = YES", 0
not_deleting_string db " Deleting = NO", 0

time_string db " Time left: ", 0
counter_string db " ", 0

finish_game_string db " F = finish the game", 0

section .data
  newline db 0xA ; newline character

segment .bss
  ; display properties
  display_offset resb 1

  ; current color to print
  current_color resb 1

  ; keyboard
  key_pressed resb 1

  ; game state
  ; 0: still playing
  ; 1: player wins
  ; 2: invaders win
  game_state resb 1

  ; game counter
  counter resd 1
  buffer resb 4    ; Reserve space for the converted string

  ; player
  player_pos resw 1

  player_can_shoot resb 1
  player_can_delete resb 1

  ; bullets:  0x STATUS PY PX
  ; STATUS == 0: end of list
  ; STATUS == #: explosion
  ; STATUS == p: player bullet
  ; STATUS == i: invader bullet

  bullet_list_end resw 1
  bullet_list resb 1
