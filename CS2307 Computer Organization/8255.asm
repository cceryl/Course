DOSSEG
.MODEL SMALL
.8086

.STACK 64

.DATA
    ; 8255 Ports
    PortA    EQU 90h
    PortB    EQU 92h
    PortC    EQU 94h
    PortCrtl EQU 96h

    ; Digits and Lights
    Digit1   EQU 00000001b
    Digit2   EQU 00000010b
    Digit3   EQU 00000100b
    Digit4   EQU 00001000b
    LowMask  EQU 00001111b
    HighMask EQU 11110000b

    ; 7-Segment Display Table for 0-F
    NumberSegmentTab DB 3Fh ; 0
                     DB 06h ; 1
                     DB 5Bh ; 2
                     DB 4Fh ; 3
                     DB 66h ; 4
                     DB 6Dh ; 5
                     DB 7Dh ; 6
                     DB 07h ; 7
                     DB 7Fh ; 8
                     DB 6Fh ; 9
                     DB 77h ; A
                     DB 7Ch ; B
                     DB 39h ; C
                     DB 5Eh ; D
                     DB 79h ; E
                     DB 71h ; F

.CODE
    ; Main
    main proc far
        call init
    MainLoop:
        call read
        ; Value: PortC 0-3, Digit 2nd, 4th
        mov  DL, AL
        and  DL, LowMask
        mov  DH, AL
        and  DH, HighMask
        or   DH, Digit2
        or   DH, Digit4
        call display
        ; Value: PortC 4-7, Digit 1st, 3rd
        mov  DL, AL
        and  DL, HighMask
        push CX
        mov  CL, 4
        shr  DL, CL
        pop  CX
        mov  DH, AL
        and  DH, HighMask
        or   DH, Digit1
        or   DH, Digit3
        call display
        jmp  MainLoop
    main endp

    ; Initialization
    init proc near
        ; Proteus Simulation Patch
        in   AL, 0
        ; Initialize Data Segment
        mov  DX, @DATA
        mov  DS, DX
        ; Set 8255 in Mode 0: PortA Output, PortB Output, PortC Input
        mov  AL, 10001001b
        out  PortCrtl, AL
        ret
    init endp

    ; Display the DL value, with display mode DH
    display proc near
        push AX
        push BX
        ; Get the 7-segment value NumberSegmentTab[DL]
        mov  BH, 0h
        mov  BL, DL
        add  BX, OFFSET NumberSegmentTab
        mov  AL, [BX]
        out  PortB, AL
        ; Set display position and lights
        mov  AL, DH
        xor  AL, LowMask
        out  PortA, AL
        ; Reset display position to avoid ghosting
        mov  AL, DH
        or   AL, LowMask
        out  PortA, AL
        pop  BX
        pop  AX
        ret
    display endp

    ; Read data from PortC to AL
    read proc near
        in   AL, PortC
        not  AL
        ret
    read endp

END main