[bits 64]

SECTION .text
ALIGN 4096

UserAPP:
    mov rdi, 1     ; syscall console write
    mov rsi, Text ; string pointer
    mov rdx, 1     ; console out
    syscall

    mov rdi, 0xFF ; syscall exit
    mov rsi, 0 ; exit code 0
    syscall

GLOBAL UserAPP

Text:
    db "App!",0