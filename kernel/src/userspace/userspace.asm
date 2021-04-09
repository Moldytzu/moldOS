[bits 64]

EnableSCE:
	mov		rcx, 0xc0000080
	rdmsr
	or		eax, 1
	wrmsr
	mov		rcx, 0xc0000081
	rdmsr
	mov		edx, 0x00180008
	wrmsr
    ret

RunInUserspace:
	mov		rcx,rsi	
	mov		rsp,rdi
	mov		r11, 0x0202 
    ;o64 sysret
	ret       

EXTERN GDTSetKernelStack
GLOBAL EnableSCE
GLOBAL RunInUserspace