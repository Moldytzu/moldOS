[bits 64]

SECTION .text
ALIGN 4096

IdleTask:
    mov rdi, 0x3 ; update screen syscall
    syscall
    jmp IdleTask

GLOBAL IdleTask