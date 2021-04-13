[bits 64]

UserAPP:
    mov rax, 1 ; syscall puts
    mov rbx, Text ; string pointer
    mov rcx, 0
    int 0xFF
    jmp $

GLOBAL UserAPP

Text:
    db "Hello!",10,0