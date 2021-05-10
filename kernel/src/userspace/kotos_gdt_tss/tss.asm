[bits 64]

;This file's author is KeepKonect.
;All credits go to him

GLOBAL LoadTSS

LoadTSS:
    push rbp
    mov rbp, rsp
    ltr di    
    leave
    ret