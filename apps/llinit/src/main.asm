[bits 64]
section .text
ALIGN 4096

llosMain:
    mov rdi, 0xFF
    mov rsi, 9     ; exit code
    syscall

Text:
    db "App!",0

GLOBAL llosMain
