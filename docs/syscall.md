To issue a system call in moldOS you need to set some registers and then issue the 'syscall' instruction.

# Table o' registers
    - RDX is the syscall number
    - RDI is the first argument
    - RSI is the second argument
    - R8 is the third argument
    - R9 is the fourth argument
