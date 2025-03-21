DOSSEG
.MODEL SMALL
.8086

.STACK 64

.DATA

.CODE
    ; Main
    main proc far
        mov  AX, 4000h
        mov  DS, AX
        mov  BX, 0h
        ; Write 66BBh to memory
    WriteLoop:
        mov  WORD PTR [BX], 0BBBBh
        inc  BX
        inc  BX
        mov  WORD PTR [BX], 6666h
        inc  BX
        inc  BX
        jnz  WriteLoop
    WaitLoop:
        jmp  WaitLoop
    main endp

END main