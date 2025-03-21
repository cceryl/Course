DOSSEG
.MODEL SMALL
.8086

.STACK 64

.DATA
    ; 8255 ports
    PortA    EQU 121h
    PortB    EQU 123h
    PortC    EQU 125h
    PortCrtl EQU 127h

    ; 8253 ports
    Timer0    EQU 100h
    Timer1    EQU 102h
    Timer2    EQU 104h
    TimerCtrl EQU 106h

    ; Interrupt vector: 0830 % 224 + 32 = 190 = 0BEh
    TimerIV EQU 0BEh
    
    ; Time cycles
    Time50ms EQU 50000
    Time10ms EQU 10000

.CODE
    ; Macros for I/O ports larger than 255
    wout macro port
        push DX
        mov  DX, port
        out  DX, AL
        pop  DX
    endm

    win macro port
        push DX
        mov  DX, port
        in   AL, DX
        pop  DX
    endm

    ; Main
    main proc far
        call init
        ; Use PC0 to control Timer1, which will interrupt and reset the watchdog every 10ms
    MainLoop:
        win  PortC
        and  AL, 10111111b
        mov  AH, AL
        and  AH, 00000001b
        mov  CL, 6
        shl  AH, CL
        or   AL, AH
        wout PortC
        jmp  MainLoop
    main endp

    ; Initialization
    init proc near
        ; Patch Proteus bug
        in   AL, 0h
        ; Init data segment & extra segment
        mov  AX, @DATA
        mov  DS, AX
        mov  AX, 0h
        mov  ES, AX
        ; Init 8255: PortA output, PortB output, PortCU output, PortCL input
        mov  AL, 81h
        wout PortCrtl
        ; Init 8253: Timer0 mode0 10ms, Timer1 uninitialized, Timer2 mode0 50ms
        call resetTimer
        ; Init IVT: TimerIV -> TimerISR
        mov  ES: [TimerIV * 4], OFFSET buttonISR
        mov  ES: [TimerIV * 4 + 2], SEG buttonISR
        ; Enable interrupts
        sti
        ret
    init endp

    ; Timer ISR that resets the watchdog
    buttonISR proc far
        call resetTimer
        iret
    buttonISR endp

    ; Reset Timer0 & Timer2
    resetTimer proc near
        push AX
        mov  AL, 30h
        wout TimerCtrl
        mov  AX, Time10ms
        wout Timer0
        mov  AL, AH
        wout Timer0
        mov  AL, 0B0h
        wout TimerCtrl
        mov  AX, Time50ms
        wout Timer2
        mov  AL, AH
        wout Timer2
        pop  AX
        ret
    resetTimer endp

END main