DOSSEG
.MODEL SMALL
.8086

.STACK 64

.DATA
    PortIn  EQU 80h
    PortOut EQU 88h

.CODE
    ; Main loop
    main proc far
        call state0
    MAINLOOP:
        call state1
        call state2
        call state3
        call state4
        jmp  MAINLOOP
    main endp

    ; Delay for DX ticks
    delay proc near
        mov  CX, DX
    DELAYLOOP:
        loop DELAYLOOP
        ret
    delay endp

    ; Blink 3 times between current AL and BL
    blink proc near
        mov  SI, 3h
        mov  AH, AL
        mov  DX, 2FFFh
    BLINKLOOP:
        mov  AL, BL
        out  PortOut, AL
        call delay
        mov  AL, AH
        out  PortOut, AL
        call delay
        dec  SI
        jnz  BLINKLOOP
        ret
    blink endp

    ; State 0: Both directions are red
    state0 proc near
        mov  AL, 36h
        out  PortOut, AL
        mov  DX, 0FFFFh
        call delay
        ret
    state0 endp

    ; State 1: South-north green, eastwest red
    state1 proc near
        mov  AL, 33h
        out  PortOut, AL
        mov  DX, 0FFFFh
        call delay
        call delay
        call delay
        ret
    state1 endp

    ; State 2: South-north green blinks then yellow, east-west red
    state2 proc near
        mov  BL, 37h
        call blink
        mov  AL, 35h
        out  PortOut, AL
        mov  DX, 9FFFh
        call delay
        ret
    state2 endp

    ; State 3: East-west green, southnorth red
    state3 proc near
        mov  AL, 1Eh
        out  PortOut, AL
        mov  DX, 0FFFFh
        call delay
        call delay
        call delay
        ret
    state3 endp

    ; State 4: East-west green blinks then yellow, south-north red
    state4 proc near
        mov  BL, 3Eh
        call blink
        mov  AL, 2Eh
        out  PortOut, AL
        mov  DX, 9FFFh
        call delay
        ret
    state4 endp

END main