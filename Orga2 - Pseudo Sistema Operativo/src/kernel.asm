; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "print.mac"

global start

;(1)
extern  GDT_DESC ; DECLARO COMO EXTERNO AL DESCRIPTOR DE GDT YA QUE ESTA EN OTRO ARCHIVO
%define segmentoDeCodigo 10<<3 ; DEFINO AL SEGMENTO PARA EL JMPFAR CON EL INDICE 10

;(2)
extern idt_init
extern IDT_DESC
extern pic_reset
extern pic_enable

;(4)
extern mmu_init
extern mmu_init_kernel_dir
extern mmu_next_free_kernel_page

extern tss_init

extern set_Mega_Seeds

BITS 16
;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
start_rm_msg db     'Iniciando kernel en Modo Real'
start_rm_len equ    $ - start_rm_msg

start_pm_msg db     'Iniciando kernel en Modo Protegido'
start_pm_len equ    $ - start_pm_msg

lu_msg db 'Libretas: 37/19  -  78/19  -  113/19'
lu_len equ $ - lu_msg
;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    print_text_rm start_rm_msg, start_rm_len, 0x07, 0, 0


    ; Habilitar A20
    call A20_disable
    call A20_check
    call A20_enable
    call A20_check
    
    ; Cargar la GDT
    lgdt [GDT_DESC] 

    ; Setear el bit PE del registro CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Saltar a modo protegido
    jmp segmentoDeCodigo:modoProtegido

BITS 32
modoProtegido:
    
;Pinto los caracteres
    ; Establecer selector de segmento fs
    xor ax, ax
    mov ax, 14<<3
    mov fs, ax  ;  Selector de Video /// El 14 corresponde al indice
    
    xor eax, eax
.cicloPintadorNegro:
    mov byte [fs:eax], 0x00
    add eax, 2
    cmp eax, 160
    jne .cicloPintadorNegro

.cicloPintadorVerde:
    ;offset se maneja dentro del limite
    ;por ende, va desde 0x0 hasta 0x8000
    mov byte [fs:eax], 0x00
    inc eax
    mov byte [fs:eax], 0xA0 ;Seteo el background en light green  y el foreground en negro
    inc eax
    cmp eax, 6560
    jne .cicloPintadorVerde

    ;seteo la r
    mov eax, 6754 ;posicion del pixel en el rango 0:4000 (x2)
    mov byte [fs:eax], 0x52 ;caracter ascii del R
    inc eax
    mov byte [fs:eax], 0x04 ;backgound black, foreground red

    ;seteo la m
    mov eax, 6844
    mov byte [fs:eax], 0x4D ;caracter ascii del M
    inc eax
    mov byte [fs:eax], 0x01 ;background black, foreground blue

    ;seteo los indicadores de los meeseek rojos
    mov eax, 6762
    mov bl, 0x30 ;bl indica el ascii del 0 y se incrementara para imprimir los numeros del 0 al 10
.meeseeksRojos:
    ;seteo el cero
    mov byte [fs:eax], 0x30
    inc eax
    mov byte [fs:eax], 0x04
    inc eax
    ;seteo el numero
    mov byte [fs:eax], bl
    inc eax
    mov byte [fs:eax], 0x04
    add eax, 5 ;salto 2 pixeles y paso al caracter del proximo
    inc bl
    cmp bl, 0x3A ; si bl se paso del ascii del 9, ya puse todos y salgo
    jne .meeseeksRojos

    ;seteo los indicadores de los meeseek azules 
    ;usando el mismo procedimiento que con los meeseks rojos
    mov eax, 7242
    mov bl, 0x30
.meeseeksAzules:
    ;seteo el cero
    mov byte [fs:eax], 0x30
    inc eax
    mov byte [fs:eax], 0x01
    inc eax
    ;seteo el numero
    mov byte [fs:eax], bl
    inc eax
    mov byte [fs:eax], 0x01
    add eax, 5
    inc bl
    cmp bl, 0x3A
    jne .meeseeksAzules

    ;seteo la barra roja
    mov eax, 7051
    xor bl, bl ;usamos bl como contador de pixeles en una fila
    xor bh, bh ;usamos bh como indicador de fila
.barraRoja:
    mov byte [fs:eax], 0x4F ;background rojo, foreground white
    add eax, 2 ;avanzamos al siguiente pixel
    inc bl
    cmp bl, 0x09 ;si ya agregue 9 bloques rojos, salto de fila
    jne .barraRoja
    sub eax, 18 ;vuelvo al principio de la fila
    add eax, 160 ;sumo un tamanio horizontal de la pantalla (bajo a la fila siguiente)
    xor bl, bl ;seteo el contador de pixeles por fila en cero
    inc bh ;aumente la fila
    cmp bh, 0x03 ;si ya hice las tres filas, salgo
    jne .barraRoja

    ;seteo la barra azul
    ;usando el mismo procedimiento que con la barra roja
    mov eax, 7173
    xor bl, bl
    xor bh, bh
.barraAzul:
    mov byte [fs:eax], 0x1F
    add eax, 2
    inc bl
    cmp bl, 0x09
    jne .barraAzul
    sub eax, 18
    add eax, 160
    xor bl, bl
    inc bh
    cmp bh, 0x03
    jne .barraAzul
;fin del pintado

    ; Establecer selectores de segmentos
    xor ax, ax ; 0000000000000 0 00
    ; Quiero que el indice tenga valor 12, IT = 0 y RPL = 0
    mov ax, 12<<3 ;  Selector de DS N0 /// El 12 corresponde al indice
    mov ds, ax                    
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Establecer la base de la pila
    mov ebp, 0x25000
    mov esp, 0x25000
    
    ; Imprimir mensaje de bienvenida
    print_text_pm start_pm_msg, start_pm_len, 0x07, 0, 0

    ; Inicializar pantalla
   
    ; Inicializar el manejador de memoria
    call mmu_init
 
    ; Inicializar el directorio de paginas
    call mmu_init_kernel_dir
    
    ; Cargar directorio de paginas
    mov cr3, eax

    ; Habilitar paginacion
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ;Ej 4.c Imprimimos libretas
    print_text_pm lu_msg, lu_len, 0x07, 0, 0 


;prueba map unmap
    ;xchg bx, bx   ; BREAKPOINT
    
    ;+++EXTRA
    ;extern mmu_map_page
    ;push 2
    ;push 0x00400000
    ;push 0x0050E000
    ;mov eax, cr3
    ;push eax
	;call mmu_map_page
	;add esp, 16

	;extern mmu_unmap_page
	;push 0x0050E000
	;mov eax, cr3
    ;push eax
	;call mmu_unmap_page
	;add esp, 8
;termina la prueba


;definimos la base de la entrada en gdt de tss initial
    extern gdt_tss_init
    mov eax, 0
    push eax
    call gdt_tss_init

;definimos la base de la entrada en gdt de tss initial
    mov eax, 1
    push eax
    call gdt_tss_init
    
    ; EJ5
	extern mmu_init_task_dir

;Inicializar RICK y sus meeseeks
	push 4;pages
	push 0x1D00000;dst_phy
	push 0x1D00000;dst_virtual
	push 0x10000;src_kernel
	mov eax, cr3
    push eax
    call mmu_init_task_dir


    mov ecx, eax ;Me guardo el CR3 del mapa de la tarea   

    push 0x1D04000 ;inicio_stack
    push 0x1D00000 ;inicio_codigo
    push ecx ;CR3 del mapa
    push 2 
    call tss_init

    ;EJ5
;Inicializar MORTY y sus meeseeks
	push 4;pages
	push 0x1D04000;dst_phy
	push 0x1D00000;dst_virtual
	push 0x14000;src_kernel
	mov eax, cr3
    push eax
    call mmu_init_task_dir

    mov ecx, eax ;Me guardo el CR3 del mapa de la tarea   
    push 0x1D04000 ;inicio_stack
    push 0x1D00000 ;inicio_codigo
    push ecx ;CR3 del mapa
    push 3 ; 3 es el task_id de morty
    call tss_init

    extern game_init
    call game_init


;Ejercicio 5.d
    ;mov cr3, eax; 
    ;xor ax, ax
    ;mov ax, 14<<3
    ;mov fs, ax

    ;mov byte [fs:0], 0x00
    ;mov byte [fs:1], 0xD0

    ;mov byte [fs:0], 0x00
    ;mov byte [fs:1], 0x00

    ;mov ax, 0x0060
    ;mov fs, ax
;Fin Ejercicio 5.d

    ; Inicializar la IDT
    call idt_init
    
    ; Cargar IDT
    lidt [IDT_DESC]
 
    ; Configurar controlador de interrupciones
    call pic_reset ; remapear PIC
    call pic_enable ; habilitar PIC

    extern sched_init
    call sched_init

    ;Incializo las Mega_Seeds
    call set_Mega_Seeds

    ; Cargar tarea inicial
    mov ax, 0x0078
    ltr ax

    ; Saltar a la primera tarea: Idle
    jmp 0x0080:0

    ;xchg bx, bx
    
    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
