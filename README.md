# GR-8 CPU

```
; Basic Instructions
0x00 00000 NOP          ; No Operation
0x01 00001 LD           ; Load from Memory into Register
0x02 00010 ST           ; Store from Register into Memory
0x03 00011 ADD          ; Add Register A and Register B
0x04 00100 SUB          ; Subtract Register B from Register A
0x05 00101 MUL          ; Multiply Register A and Register B
0x06 00110 DIV          ; Divide Register A by Register B
0x07 00111 JMP          ; Jump to Address in Register A
0x08 01000 CDJMP        ; Conditional Jump (if Zero Flag is set)
0x09 01001 CMP          ; Compare Register A and Register B
0x0A 01010 INC          ; Increment Register A
0x0B 01011 DEC          ; Decrement Register A
0x0C 01100 AND          ; Bitwise AND of Register A and Register B
0x0D 01101 OR           ; Bitwise OR of Register A and Register B
0x0E 01110 XOR          ; Bitwise XOR of Register A and Register B
0x0F 01111 SHL          ; Shift Register A Left
0x10 10000 SHR          ; Shift Register A Right
0x11 10001 LDI          ; Load Immediate (Load a value directly into Register A)
0x12 10010 MOV          ; Move from Register A to Register B

; I/O DISPLAY
0x13 10011 DBLBUFSET    ; Set Double Buffering
0x14 10100 DBLBUFCLR    ; Clear Double Buffering
0x15 10101 CHARR        ; Character Rendering
0x16 10110 REFRESH      ; Refresh Screen
0x17 10111 DBLBUFREFRESH ; Double Buffer Refresh

; Memory layout
0x0000 - 0x00FF -> Variables
0x0100 - 0x6FFF -> Code
0x7000 - 0x7FFF -> Empty Space
0x8000 - 0x8FFF -> Sprites
0x9000 - 0x9FFF -> Memory addresses

; RAM layout
0x0000 - 0x00FF -> Register shadows
0x0100 - 0x01FF -> Variables
0x0200 - 0x7FFF -> Unpurposed RAM
0x8000 - 0x8FFF -> Buffer
0x9000 - 0x9FFF -> Unpurposed RAM
```