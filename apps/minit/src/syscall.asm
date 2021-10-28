isyscall: ;RDI=syscall number ;RSI=arg1 ;RDX=arg2 ;r8=arg3
    push rbp
    mov rbp, rsp

    mov r8,rcx
    syscall

    pop rbp
    ret

global isyscall