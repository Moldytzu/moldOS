[bits 64]

asmemset:
    mov rcx,rdx
    mov rax,rsi
    rep stosb
    ret

GLOBAL asmemset