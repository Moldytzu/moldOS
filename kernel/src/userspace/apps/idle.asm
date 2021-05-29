[bits 64]

SECTION .text
ALIGN 4096

IdleTask:
    mov rdi, 0x21 ; syscall yeld
    mov rsi, IdleTask ; set the rip
    syscall  

GLOBAL IdleTask