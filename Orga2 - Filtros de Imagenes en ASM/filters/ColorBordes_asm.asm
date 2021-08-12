extern ColorBordes_c
global ColorBordes_asm

section .data
AlphaReboot: DQ 0xFF000000FF000000, 0xFF000000FF000000
;AlphaReboot: DD 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000

section .text
ColorBordes_asm:
; RDI = *src
; RSI = *dst
; EDX = int width (en pixeles)
; ECX = int height (en pixeles)
; R8D = int src_row_size (en bytes)
; R9D = int dst_row_size (en bytes)
; >.< 

; b ColorBordes_asm.asm:9

%DEFINE ARB xmm15
%DEFINE RegBlanco xmm14

push rbp
mov rbp, rsp 
push rbx
push r12
push r13
push r14

; Busco arreglar el tema del desplazamiento 11/10/2020
xor rax, rax
mov eax, r8d

; Paso mi "reset" de aplha a xmm15(ARB)
pxor ARB, ARB
;movq ARB, [AlphaReboot]; xmm15 = 0x00 | 0x00 | 0x000000FF | 0x000000FF
movdqu ARB, [AlphaReboot]; xmm15 = 0x000000FF | 0x000000FF | 0x000000FF | 0x000000FF

; Me coloco en xmm14 todos unos
PCMPEQB RegBlanco, RegBlanco ; xmm14 = 0xFFFF...FFFF

;***De entrada debo incrementar RSI en una fila. Puedo aprovechar para llenarlo de px blancos (uso momentaneamente r12 y r13)
;Tomo como precondición que el ancho de las imagenes es múltiplo de 8 y mayor que 16.
xor r12, r12 ; limpio r12. R12 = contadorAncho
xor r13, r13 ; limpio r13.
mov r13d, edx ; r13d = ancho

.PrimFilaBlanca:
cmp r12d, r13d
je .finPrimFilaBlanca

movdqu [rsi], RegBlanco ; Muevon cuatro pixeles blancos
add r12d, 4 ; Aumenta el contador en 4 pixeles
add rsi, 16 ; Aumento rsi en 16 bytes = 4 pixeles
jmp .PrimFilaBlanca
;***fin cicloPrimFilaBlanca

.finPrimFilaBlanca: ; Avanzo mi puntero para que apunte a la segunda fila de la matriz
movd [rsi], RegBlanco ; Coloco el pixel del comienzo de la fila Nº2
add rsi, 4 ; Aumento mi puntero (*dst)

; Acá debería tener mi imagen toda la primera fila blanco,
; y el primer elemento de la segunda fila blanco.
; *dst debería estar apuntando a "(1,1)"

; ### A partir de acá comienza el código dado por la catedra: ###

;ContadorAltura
xor r10, r10 ; r10 va a ser mi contador
xor r11, r11
mov r11d, ecx 	; r11 = altura
dec r11d 		; r11 = altura - 1
; De esta manera hago el i = 1 y el i = altura - 2

.cicloAltura: ;For i de 1 a height - 2:
inc r10d 
cmp r10d, r11d
je .fin

; Restauro el valor del ciclo interior
xor r12, r12; limpio r12. R12 = contadorAncho
inc r12d
mov r13d, edx ; r13d = ancho
dec r13d ; r13d = ancho - 1
; De esta manera hago el j = 1 y el j = ancho - 2

.cicloAncho: ;For j de 1 a width - 2:
;r=0 g=0 b=0
pxor xmm0, xmm0

;Matriz (Los datos aquí son pixeles)
; a0 a1 a2 a3 | a4 a5 ...
; b0 b1 b2 b3 | b4 b5 ...
; c0 c1 c2 c3 | c4 c5 ...
; ———————————/
; d0 d1 d2 d3 d4 d5 ...
; e0 e1 e2 e3 e4 e5 ...
; .....................
;	|
;   |
;	V
;Traigo de memoria todo lo que necesito
movdqu xmm1, [rdi]
movdqu xmm2, [rdi + rax]
movdqu xmm3, [rdi + rax*2]
; Registros:
; xmm1 = a3 | a2 | a1 | a0  
; xmm2 = b3 | b2 | b1 | b0 
; xmm3 = c3 | c2 | c1 | c0 

.cicloJJ:
; *** IDEAS ***
; La suma/resta horizontal utiliza words. Por lo tanto va a estar dificil implementarla con esto.
;
; PABSB = Absoluto para 8 bit integers
; PSUBUSB = Sub Int unsigned saturation
; PSUBB
;PSRLDQ hace shift a byte de una DoubleQuadword a derecha
;
; Interpreto que matrix[a][b] ==> a hace ref a las filas y b a las columnas.
;
; 01001100 01100001 00100000 01110010 01100101 00100000 01110000 01110101 
; 01110100 01100001 00100000 01101101 01100001 01100100 01110010 01100101
; *******

;****1***
;r += abs( src_matrix[j-1][i-1].r - src_matrix[j-1][i+1].r )
;g += abs( src_matrix[j-1][i-1].g - src_matrix[j-1][i+1].g )
;b += abs( src_matrix[j-1][i-1].b - src_matrix[j-1][i+1].b )

; a0-a2 && a1-a3

movdqu xmm4, xmm1 ; xmm4 = xmm1
PSRLDQ xmm4, 8 ; xmm4 = -- | -- | a3 | a2.
PMOVZXBW xmm5, xmm4;  xmm5 =  a3 | a2. (Values of 16 bits)
PMOVZXBW xmm4, xmm1;  xmm4 =  a1 | a0. (Values of 16 bits)

PSUBW xmm4, xmm5   ;xmm4 =  a1-a3 | a0-a2
PABSW xmm4, xmm4   ;xmm4 =  abs(a1-a3) | abs(a0-a2)

PADDUSW xmm0, xmm4 ;xmm0 = -- | -- | old + abs(a1-a3) | old + abs(a0-a2)
; ¡xmm2 se preservo! (para el cicloII)

;****2***
;r += abs( src_matrix[j][i-1].r - src_matrix[j][i+1].r )
;g += abs( src_matrix[j][i-1].g - src_matrix[j][i+1].g )
;b += abs( src_matrix[j][i-1].b - src_matrix[j][i+1].b )

; b0 - b2 && b1 - b3

movdqu xmm6, xmm2 ; xmm6 = xmm2
PSRLDQ xmm6, 8 ; xmm6 = -- | -- | b3 | b2.
PMOVZXBW xmm7, xmm6;  xmm7 =  b3 | b2. (Values of 16 bits)
PMOVZXBW xmm6, xmm2;  xmm6 =  b1 | b0. (Values of 16 bits)

PSUBW xmm6, xmm7  ;xmm6 = b1-b3 | b0-b2
PABSW xmm6, xmm6  ;xmm6 = abs(b1-b3) | abs(b0-b2)
PADDUSW xmm0, xmm6 ;xmm0 =  old + abs(b1-b3) | old + abs(b0-b2)
; ¡xmm3 se preservo! (para el cicloII)

;****3***
;r += abs( src_matrix[j+1][i-1].r - src_matrix[j+1][i+1].r )
;g += abs( src_matrix[j+1][i-1].g - src_matrix[j+1][i+1].g )
;b += abs( src_matrix[j+1][i-1].b - src_matrix[j+1][i+1].b )

; c0 - c2 && c1 - c3

movdqu xmm8, xmm3 ; xmm8 = xmm3
PSRLDQ xmm8, 8 ; xmm8 = -- | -- | b3 | b2.
PMOVZXBW xmm9, xmm8;  xmm9 =  b3 | b2. (Values of 16 bits)
PMOVZXBW xmm8, xmm3;  xmm8 =  b1 | b0. (Values of 16 bits)

PSUBW xmm8, xmm9  ;xmm8 = c1-c3 | c0-c2
PABSW xmm8, xmm8  ;xmm8 = abs(b3-b1) | abs(b2-b0)
PADDUSW xmm0, xmm8;xmm0 = old + abs(c1-c3) | old + abs(c0-c2)
; ¡xmm4 se preservo! (para el cicloII)

; -------------------------------------------------
; CREO QUE SE PUEDE REDUCIR EN 1 EL REGISTRO QUE USO CADA VEZ
; -------------------------------------------------

; (Se mantuvieron) Registros:
; xmm1 = a3 | a2 | a1 | a0  
; xmm2 = b3 | b2 | b1 | b0 
; xmm3 = c3 | c2 | c1 | c0 

; Registors liberados
; xmm5 a xmm13

.cicloII:
;****1***
;r += abs( src_matrix[j-1][i-1].r - src_matrix[j+1][i-1].r )
;g += abs( src_matrix[j-1][i-1].g - src_matrix[j+1][i-1].g )
;b += abs( src_matrix[j-1][i-1].b - src_matrix[j+1][i-1].b )

; a0 - c0 && a1 - c1

PMOVZXBW xmm4, xmm1; xmm4 =  a1 | a0.(Values of 16 bits)
PMOVZXBW xmm5, xmm3; xmm5 =  c1 | c0.(Values of 16 bits)

PSUBW xmm4, xmm5  ; xmm5 = a1-c1 | a0-c0
PABSW xmm4, xmm4  ; xmm5 = abs(a1-c1) | abs(a0-c0)
PADDUSW xmm0, xmm4; xmm0 = old + abs(a1-c1) | old + abs(a0-c0)

;****2***
;r += abs( src_matrix[j-1][i].r - src_matrix[j+1][i].r )
;g += abs( src_matrix[j-1][i].g - src_matrix[j+1][i].g )
;b += abs( src_matrix[j-1][i].b - src_matrix[j+1][i].b )

; a1 - c1 && a2 - c2

PSRLDQ xmm1, 4 ;xmm1 = -- | a3 | a2 | a1 
PSRLDQ xmm3, 4 ;xmm3 = -- | c3 | c2 | c1

PMOVZXBW xmm4, xmm1; xmm4 =  a2 | a1.(Values of 16 bits)
PMOVZXBW xmm5, xmm3; xmm5 =  c2 | c1.(Values of 16 bits)

PSUBW xmm4, xmm5  ; xmm5 = a2-c2 | a1-c1
PABSW xmm4, xmm4  ; xmm5 = abs(a2-c2) | abs(a1-c1)
PADDUSW xmm0, xmm4; xmm0 = old + abs(a2-c2) | old + abs(a1-c1)

;****3***
;r += abs( src_matrix[j-1][i+1].r - src_matrix[j+1][i+1].r )
;g += abs( src_matrix[j-1][i+1].g - src_matrix[j+1][i+1].g )
;b += abs( src_matrix[j-1][i+1].b - src_matrix[j+1][i+1].b )

; a2 - c2 && a3 - c3

PSRLDQ xmm1, 4 ;xmm1 = -- | -- | a3 | a2  
PSRLDQ xmm3, 4 ;xmm3 = -- | -- | c3 | c2 

PMOVZXBW xmm4, xmm1; xmm4 =  a3 | a2.(Values of 16 bits)
PMOVZXBW xmm5, xmm3; xmm5 =  c3 | c2.(Values of 16 bits)

PSUBW xmm4, xmm5  ; xmm5 = a3-c3 | a2-c2
PABSW xmm4, xmm4  ; xmm5 = abs(a3-c3) | abs(a2-c2)
PADDUSW xmm0, xmm4; xmm0 = old + abs(a3-c3) | old + abs(a2-c2)

; -------------------------------------------------

; Interpreto que aca no es necesario saturar, ya que lo hacemos en la suma.
;dst_matrix[j][i].r = SATURAR(r)
;dst_matrix[j][i].g = SATURAR(g)
;dst_matrix[j][i].b = SATURAR(b)

; ASEGURARSE DE QUE LAS TRANSPARENCIAS SE MANTIENEN COMO 255 (Alpha)
;PADDUSB xmm0, ARB; Se supone que con esto restauro mis transparencias
packuswb xmm0, xmm0 ; xmm0 = valor2 | valor1 | valor2 | valor1 
por xmm0, ARB;
movq [rsi], xmm0 ; Muevo solamente 2 pixeles a memoria

; -------------------------------------------------
;Incremento los contadores y direcciones correspondientes 
; modifico rdi
add rdi, 8
; modifico rsi
add rsi, 8

;modifico el contador del cicloAncho
inc r12d
inc r12d

;RAMIFICACION
cmp r12d, r13d
jne .cicloAncho ;si todavía no me pase sigo ciclando
; Si llegue aca es que me pase

add rdi, 8 ;(Así voy a la siguiente fila con mi *src)
movq [rsi], RegBlanco; Muevo 2 pixeles blancos.
add rsi, 8 ; Avanzo mi *dst a la sig. fila "(sigFila,1)".
jmp .cicloAltura

.fin: 
; Si llegue aquí, mi *dst debería estar apuntando a la última fila de 
; la matriz en la posición (ultFila,1)
sub rsi, 4 ; ahora apunta a (ultFila,0)

; Esta función actúa igual que la primera, simplemente hago blanca la última fila
xor r12, r12 ; limpio r12. R12 = contadorAncho
xor r13, r13 ; limpio r13
mov r13d, edx ; r13d = ancho

.UltFilaBlanca:
cmp r12d, r13d
je .fout
movdqu [rsi], RegBlanco ; Muevo cuatro pixeles blancos
add r12d, 4
add rsi, 16 ; Aumento rsi en 16 bytes = 4 pixeles
jmp .UltFilaBlanca
;***fin cicloUltFilaBlanca

.fout:
; NOTA: La funcion es void, asi que no retorno nada.
pop r14
pop r13
pop r12
pop rbx
pop rbp
ret



