[bits 64]

SECTION .text
ALIGN 4096

LLInit:
    mov rdi, 1     ; syscall console write
    mov rsi, WelcomeText ; string pointer
    mov rdx, 1     ; console out
    syscall

    mov rdi, 0xFF ; exit
    mov rsi, 0    ; code 0 (SUCCES)
    syscall

GLOBAL LLInit

WelcomeText:
    db "Welcome to LowLevelOS!",10,"Copyright Moldu' (Nov. 2020 - May 2021)",0