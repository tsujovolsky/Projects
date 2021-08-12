; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
;
; Definicion de rutinas de atencion de interrupciones

%include "print.mac"



BITS 32
sched_task_offset:     dd 0x00000000
sched_task_selector:   dw 0x0000

gdtr_o: dd 0x00000000
gdtr_s: dw 0x0000


;; PIC
extern pic_finish1

;; Sched
extern sched_next_task

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;
;; Uso funcion para mostrar en pantalla
extern mostrar_excepcion_en_pantalla
extern desalojar
extern imprimoModoDebug
extern imprimoModoDebug_ec
%macro ISR 1
global _isr%1

_isr%1:
    pushad
    mov eax, %1
    push eax
    push cs
    push ds
    push es
    push ss
    push fs
    push gs
    call imprimoModoDebug
    add esp, 7*4
    xor ecx, ecx
    str cx
    push ecx
    call desalojar
    jmp 0x0080:0
    add esp, 4
    popad
    iret


%endmacro

%macro ISR_EC 1
global _isr%1

_isr%1:
    pushad
    mov eax, %1
    push eax
    push cs
    push ds
    push es
    push ss
    push fs
    push gs
    call imprimoModoDebug_ec
    add esp, 7*4
    xor ecx, ecx
    str cx
    push ecx
    call desalojar
    jmp 0x0080:0
    add esp, 4
    popad
    iret

%endmacro


;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR_EC 8
ISR 9
ISR_EC 10
ISR_EC 11
ISR_EC 12
ISR_EC 13
ISR_EC 14
ISR 15
ISR 16
ISR_EC 17
ISR 18
ISR 19

;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
extern pic_finish1
extern Meeseek_status_update
extern map_update
extern points_update
extern analizar_ganador
extern status_debugger_on_screen
extern aumentar_turnos_vivos
extern tasks_clocks
global _isr32
_isr32:
    pushad
    call pic_finish1
    call next_clock
    call status_debugger_on_screen
    cmp eax, 0 ; if indicador_debugger_on_screen es = 0
    je .cont 
    str cx
    mov ax, 0x0080
    cmp ax, cx
    je .etiq
    jmp 0x0080:0 ; jmp idle
.etiq:
    popad
    iret
.cont:
    call Meeseek_status_update
    call map_update
    call points_update
    call analizar_ganador
    call sched_next_task
    push eax
    call tasks_clocks
    add esp, 4
    push eax
    call aumentar_turnos_vivos
    add esp, 4
    str cx
    cmp ax, cx
    je .fin
    ;xchg bx, bx
    mov word [sched_task_selector], ax
    jmp far [sched_task_offset]

.fin:
    popad
    iret

;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
extern printScanCode ;Declarado en screen.c
global _isr33
_isr33:
    pushad
    in al, 0x60
    
    push eax
    call printScanCode
    add esp, 4
    
    call pic_finish1
    popad
    iret

;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
global _isr88
extern meeseeks
_isr88:
    push ebp
    mov ebp, esp
    pushad
    push ecx
    push ebx
    push eax
    call meeseeks
    add esp, 3*4
    mov [ebp - 4], eax
    ;xchg bx, bx
    xor eax, eax
    mov eax, 0x58
    jmp 0x0080:0
    popad
    pop ebp
    iret

global _isr89
extern portal
_isr89:
    pushad
    call portal
    xor eax, eax
    mov eax, 0x59
    jmp 0x0080:0
    popad
    iret

global _isr100
extern look
extern return_look
_isr100:
    push ebp
    mov ebp, esp
    pushad
    call look
    mov eax, [return_look]
    mov ebx, [return_look + 4]
    mov [ebp - 4], eax
    mov [ebp - 16], ebx
    ;xchg bx, bx
    xor eax, eax
    mov eax, 0x64
    jmp 0x0080:0
    popad
    pop ebp
    iret

global _isr123
extern move
_isr123:
    push ebp
    mov ebp, esp
    pushad
    push ebx
    push eax
    call move
    add esp, 2*4
    mov [ebp - 4], eax
    xor eax, eax
    mov eax, 0x7B
    jmp 0x0080:0
    popad
    pop ebp
    iret

global end_game
end_game:
    cli
    jmp end_game


;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
isrNumber:           dd 0x00000000
isrClock:            db '|/-\'
next_clock:
        pushad
        inc DWORD [isrNumber]
        mov ebx, [isrNumber]
        cmp ebx, 0x4
        jl .ok
                mov DWORD [isrNumber], 0x0
                mov ebx, 0
        .ok:
                add ebx, isrClock
                print_text_pm ebx, 1, 0x0f, 49, 79
                popad
        ret

%macro RELOJ 3
global reloj%1
isrNumber%1:           dd 0x00000000
isrClock%1:            db '|/-\'
reloj%1:
        pushad
        inc DWORD [isrNumber%1]
        mov ebx, [isrNumber%1]
        cmp ebx, 0x4
        jl .ok
                mov DWORD [isrNumber%1], 0x0
                mov ebx, 0
        .ok:
                add ebx, isrClock%1
                print_text_pm ebx, 1, 0x0f, %2, %3
                popad
        ret    

%endmacro

RELOJ rick0, 43, 21
RELOJ rick1, 43, 25
RELOJ rick2, 43, 29
RELOJ rick3, 43, 33
RELOJ rick4, 43, 37
RELOJ rick5, 43, 41
RELOJ rick6, 43, 45
RELOJ rick7, 43, 49
RELOJ rick8, 43, 53
RELOJ rick9, 43, 57

RELOJ morty0, 46, 21
RELOJ morty1, 46, 25
RELOJ morty2, 46, 29
RELOJ morty3, 46, 33
RELOJ morty4, 46, 37
RELOJ morty5, 46, 41
RELOJ morty6, 46, 45
RELOJ morty7, 46, 49
RELOJ morty8, 46, 53
RELOJ morty9, 46, 57

RELOJ morty, 43, 62
RELOJ rick, 43, 17


global imprimir_registros_en_pantalla
extern print_hex
extern C_FG_LIGHT_CYAN
extern C_BG_BLACK

global rellenar_struct
extern imprimir_debug
rellenar_struct:
    push ebp
    mov ebp, esp
    
    mov [edi], eax
    mov [edi+4], ebx
    mov [edi+8], ecx
    mov [edi+12], edx
    mov [edi+16], esi
    mov [edi+20], edi
    mov [edi+24], ebp
    mov [edi+28], esp
    ;mov [edi+32], eip
    mov [edi+36], cs
    mov [edi+38], ds
    mov [edi+40], es
    mov [edi+42], ss
    mov [edi+44], fs
    mov [edi+46], gs

    pop ebp    
    ret
