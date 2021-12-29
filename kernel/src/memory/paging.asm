[bits 64]
GLOBAL LoadPML4

LoadPML4:
    mov cr3, rdi
    add rsp, rsi
    add rbp, rsi
    ret