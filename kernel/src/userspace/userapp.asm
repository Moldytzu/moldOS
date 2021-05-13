[bits 64]

SECTION .text
ALIGN 4096

UserAPP:
    mov rdi, 1    ; syscall console write
    mov rsi, Text ; string pointer
    syscall

    mov rdi, 0x21 ; syscall yeld
    mov rsi, UserAPP ; set the rip
    syscall

    jmp UserAPP

UserAPP2:
    mov rdi, 1    ; syscall console write
    mov rsi, TextSerial ; string pointer
    syscall

    mov rdi, 0x21 ; syscall yeld
    mov rsi, UserAPP2 ; set the rip
    syscall

    jmp UserAPP2

GLOBAL UserAPP
GLOBAL UserAPP2

Text:
    db "A",0

TextSerial:
    db "B",0