[bits 64]

UserAPP:
    mov rdi, 1    ; syscall console write
    mov rsi, Text ; string pointer
    syscall

    mov rdi, 2    ; syscall serial write
    mov rsi, TextSerial ; string pointer
    syscall

    mov rdi, 0xFF ;syscall exit
    mov rsi, 1 ; exit code
    syscall
    jmp $

GLOBAL UserAPP

Text:
    db "This should be in the normal console",10,0

TextSerial:
    db "and this should be in the serial console",10,0