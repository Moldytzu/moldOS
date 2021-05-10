[bits 64]
ALIGN	4096

;syscall list:
;1 - console write
;255 - exit

%macro	SYSCALL_SAVE	0
	push	r15
	push	r14
	push	r13
	push	r12
	push	r10
	push	r9
	push	r8
	push	rbp
	push	rdi
	push	rsi
	push	rdx
	push	rbx
%endmacro
%macro	SYSCALL_RESTORE	0
	pop	rbx
	pop	rdx
	pop	rsi
	pop	rdi
	pop	rbp
	pop	r8
	pop	r9
	pop	r10
	pop	r12
	pop	r13
	pop	r14
	pop	r15
%endmacro

syscall_entry:
    SYSCALL_SAVE
    push r11
    push rcx
	push rdi
    call SyscallHandler
	pop	rdi
    pop rcx
    pop r11
    lea rdx, [rel SysexitByte]
    cmp rdi, rdx
    je .kernel_exit
    .sysret_exit:
        SYSCALL_RESTORE
        o64    sysret
    .kernel_exit:
        mov        rdi, 0
        call    TSSGetStack
        SYSCALL_RESTORE
        mov        rsp, rax
        ret

EnableSCE:
	mov		rax, syscall_entry
	mov		rdx, rax
	shr		rdx, 0x20
	mov		rcx, 0xc0000082
	wrmsr
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
	push	rsi
	push	rdi ;save our registers

	mov		rdi, 0 
	mov		rsi, rsp
	add		rsi, 16
	call	TSSSetStack ;set the current stack
	
	pop		rcx	;restore our registers
	pop		rsp
	mov		r11, 0x0202 ;set the x86 flags
	o64 sysret ; to userspace and beyond

SysexitByte:
	db 255

EXTERN TSSSetStack
EXTERN TSSGetStack
EXTERN SyscallHandler
GLOBAL EnableSCE
GLOBAL RunInUserspace