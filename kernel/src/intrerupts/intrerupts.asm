[bits 64]

%macro	PUSH_REG	0
	cld
	push	r15
	push	r14
	push	r13
	push	r12
	push	r11
	push	r10
	push	r9
	push	r8
	push	rbp
	push	rdi
	push	rsi
	push	rdx
	push	rcx
	push	rbx
	push	rax
%endmacro
%macro	POP_REG		0
	cld
	pop	rax
	pop	rbx
	pop	rcx
	pop	rdx
	pop	rsi
	pop	rdi
	pop	rbp
	pop	r8
	pop	r9
	pop	r10
	pop	r11
	pop	r12
	pop	r13
	pop	r14
	pop	r15
%endmacro

%macro PUSHAQ 0
    mov [rsp-0x8], r15
    mov [rsp-0x10], r14
    mov [rsp-0x18], r13
    mov [rsp-0x20], r12
    mov [rsp-0x28], r11
    mov [rsp-0x30], r10
    mov [rsp-0x38], r9
    mov [rsp-0x40], r8
    mov [rsp-0x48], rdi
    mov [rsp-0x50], rsi
    mov [rsp-0x58], rbp
    mov [rsp-0x60], rsp
    mov [rsp-0x68], rbx
    mov [rsp-0x70], rdx
    mov [rsp-0x78], rcx
    mov [rsp-0x80], rax
    sub rsp, 0x80
%endmacro

%macro POPAQ 0
    add rsp, 0x80
    mov r15, [rsp-0x8]
    mov r14, [rsp-0x10]
    mov r13, [rsp-0x18]
    mov r12, [rsp-0x20]
    mov r11, [rsp-0x28]
    mov r10, [rsp-0x30]
    mov r9, [rsp-0x38]
    mov r8, [rsp-0x40]
    mov rdi, [rsp-0x48]
    mov rsi, [rsp-0x50]
    mov rbp, [rsp-0x58]
    mov rbx, [rsp-0x68]
    mov rdx, [rsp-0x70]
    mov rcx, [rsp-0x78]
    mov rax, [rsp-0x80]
%endmacro


InvalideOpcodeHandlerEntry:
	;cli
    PUSHAQ
    call InvalideOpcodeHandler
    POPAQ
	;sti
	ret

GeneralProtectionFaultHandlerEntry:
	;cli
    PUSHAQ
    call GeneralProtectionFaultHandler
    POPAQ
	;sti
	ret
PageFaultHandlerEntry:
    ;cli
	PUSHAQ
    call PageFaultHandler
    POPAQ
	;sti
	ret
DoubleFaultHandlerEntry:
    ;cli
	PUSHAQ
    call DoubleFaultHandler
    POPAQ
	;sti
	ret
KBHandlerEntry:
    ;cli
	PUSHAQ
    call KBHandler
    POPAQ
	;sti
	iretq
MSHandlerEntry:
    ;cli
	PUSHAQ
    call MSHandler
    POPAQ
	;sti
	iretq
PITHandlerEntry:
    ;cli
	PUSHAQ
    call PITHandler
	POPAQ
	;sti
	;add rsp,16
	ret

EXTERN InvalideOpcodeHandler
EXTERN GeneralProtectionFaultHandler
EXTERN PageFaultHandler
EXTERN DoubleFaultHandler
EXTERN KBHandler
EXTERN MSHandler
EXTERN PITHandler

GLOBAL InvalideOpcodeHandlerEntry
GLOBAL GeneralProtectionFaultHandlerEntry
GLOBAL PageFaultHandlerEntry
GLOBAL DoubleFaultHandlerEntry
GLOBAL KBHandlerEntry
GLOBAL MSHandlerEntry
GLOBAL PITHandlerEntry