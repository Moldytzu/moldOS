[bits 64]
section .text

moldMain:
    mov rdi, 0x1  ; write
    mov rsi, WelcomeText ; the Offset
    mov rdx, 0x1  ; stdout
    mov r8,  1    ; load the string from the executable 
    syscall

    mov rdi, 0xFF ; exit syscall
    mov rsi, 2021 ; exit code
    syscall
    jmp $

WelcomeText:
    db "Welcome to moldOS!",10,"By Moldu' (Nov. 2020 - Oct. 2021)",0

global moldMain