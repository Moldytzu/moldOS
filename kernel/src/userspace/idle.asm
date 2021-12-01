[bits 64]

SECTION .text

IdleTask:
    mov rdi, 0x3 ; update screen syscall
    syscall
    jmp IdleTask

GLOBAL IdleTask