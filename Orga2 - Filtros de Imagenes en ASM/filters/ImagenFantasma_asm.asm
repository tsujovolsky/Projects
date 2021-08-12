section .rodata 
    maskTransparenciaF: DQ 0xffff000000000000, 0xffff000000000000
    maskTranspB:        DQ 0xff000000ff000000, 0xff000000ff000000
    paraMultiplicar:   DD 0.9
    ceros: DQ 0x00, 0x00
    ocho: DD 8.0
    extern ImagenFantasma_c
    global ImagenFantasma_asm

section .text
;EXPLICACION PIXELES
;Consideramos a una imagen como una matriz de pıxeles. Cada pıxel esta determinado por
;cuatro componentes: los colores azul (b), verde (g) y rojo (r), y la transparencia (a). En nuestro
;caso particular cada una de estas componentes tendra 8 bits (1 byte) de profundidad, es decir,
;estaran representadas por numeros enteros en el rango [0, 255].
;Nombraremos a las matrices de pıxeles src como la imagen de entrada y como dst a la
;imagen destino. Ambas seran almacenadas por filas, tomando el pıxel [0,0] de la matriz como
;el pıxel de la esquina superior izquierda de la imagen. 
;En todos los filtros, el valor de la componente de transparencia debe ser 255.
;
;EXPLICACIÓN FILTRO FANTASMA
;Este filtro genera un efecto de imagen fantasma sobre la imagen destino, utilizando para
;esto la misma imagen original en escala de grises y al doble de tamaño. Se puede pasar como
;parametro a este filtro el offset en x e y donde debe ubicarse la imagen fantasma. No se
;considera como validos, parametros de offset que dejen la imagen a utilizar como fantasma indefinida.
;
;Parametro Descripcion
;offsetx -> Desplazamiento sobre el eje horizontal entre la imagen fantasma y la imagen original.
;offsety -> Desplazamiento sobre el eje vertical entre la imagen fantasma y la imagenoriginal.
;
;A continuacion se presenta el pseudocodigo de la operatoria del filtro presentado:
;
;Para i de 0 a height - 1:
;   Para j de 0 a width - 1:
;       ii = i/2 + offsetx;
;       jj = j/2 + offsety;
;       b = ( src_matrix[ii][jj].r + 2 * src_matrix[ii][jj].g + src_matrix[ii][jj].b ) / 4
;       dst_matrix[i][j].r = SATURAR( src_matrix[i][j].r * 0.9 + b/2 )
;       dst_matrix[i][j].g = SATURAR( src_matrix[i][j].g * 0.9 + b/2 )
;       dst_matrix[i][j].b = SATURAR( src_matrix[i][j].b * 0.9 + b/2 )
;
;Este filtro opera sobre todos los pıxeles de la imagen.
;
;1* El ancho de las imagenes es siempre mayor a 16 pıxeles y multiplo de 8 pıxeles
;
;2* Para el caso de las funciones implementadas en lenguaje ensamblador, se debe trabajar
;con la maxima cantidad de pıxeles en simultaneo. De no ser posible esto para algun filtro,
;debera justificarse debidamente en el informe.
;
;3* El procesamiento de los pıxeles se debera hacer exclusivamente con instrucciones SSE.
;No esta permitido procesarlos con registros de proposito general, salvo para tratamiento
;de casos borde. En tal caso se debera justificar debidamente y hacer un analisis del costo
;computacional.
;
;4* La cantidad mınima a procesar de pıxeles en simultaneo: 2 pıxeles en simultaneo
;
;5* La implementacion de los filtros que solo realicen calculos con numeros enteros o movimientos 
;de memoria no podran perder precision. Es posible que se deseen realizar optimizaciones a
;costo de perder algo de precision. Esto sera aceptable siempre y cuando se analice tambien la
;calidad de la imagen resultante en la version optimizada contra la no optimizada.
;
;void ImagenFantasma_asm (uint8_t *src, uint8_t *dst, int width, int height,
;                         int src_row_size, int dst_row_size, int offsetx, int offsety);

; 2* =>  REGISTRO DE 128 BITS = 16 BYTES = 4 PIXELES 
;    =>  |PIX 3|PIX 2|PIX 1|PIX 0| -> |PIX 7|PIX 6|PIX 5|PIX 4|
;                              ¿está bien?

; 1* =>  Si el ancho 'a', entonces 'a' congruente a 0 (mod 8) --> 'a' congruente a 0 (mod 4) 
;    =>  NO HAY CASOS BORDES, RECORRO DE A 4 PIXELES A LA IMAGEN ORIGINAL
;    =>  PUEDEN HABER CASOS BORDES AL TRASLADAR LA IMAGEN FANTASMA                                        Tomas ayuda plz
;    =>  Hay que chequear si el proximo conjunto va a caer 'entero' dentro de los limites de la matriz
;    =>  Sino, chequear cuántos pixeles están (usar saltos condic para recorrer en simultaneo o de a uno)
;    =>  regComp <- DCC ACTUAL DEL VECTOR (arranca en 0) - ancho
;    =>  regComp == 0 => está afuera
;    =>  regComp == 1 => salgo del ciclo, evaluo 1 pixel solo
;    =>  regComp == 2 => salgo del ciclo, evaluo 2 pixeles simultaneos
;    =>  regComp == 3 => salgo del ciclo, evaluo 3 pixeles simultaneos
;    =>  regComp >= 4 => sigo en el ciclo

ImagenFantasma_asm:
;rdi        <- uint8_t *src 
;rsi        <- uint8_t *dst
;edx        <- int width
;ecx        <- int height
;r8d        <- int src_row_size
;r9d        <- int dst_row_size
;[rbp + 16] <- int offsetx
;[rbp + 24] <- int offsety

    push rbp
    mov rbp, rsp 
    ;(Preservar los registros q no debería modificar)
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8
    ;RBX, R10, R11, R12, R13, R14, R15

    xor rbx, rbx
    mov ebx, r8d 
    mov r8, rbx ; ALARGUE src_row_size

    xor rbx, rbx
    mov ebx, r9d 
    mov r9, rbx ; ALARGUE dst_row_size

    ;OFFSET AL PASAR DE VECTOR RAX
    mov r10, r8
    shr r10, 1 ;divido ancho por la mitad y lo guardo en r10
        
    ;MOVDQU XMM15, [maskTransparencia] ;XMM15 <- MASCARA PARA QUITAR VALOR DE LA TRANSPARENCIA 
    MOVDQU XMM15, [maskTranspB]
    MOVUPS XMM14, [ceros] 
    ;MOVDQU XMM13, [maskTransparenciaF]

    ;me guardo edx en r11
    mov r11d, edx

;ME GUARDO LA DIRECCION DE LA IMAGEN DESPLAZADA
    mov r14, [rbp + 16]
    shl r14, 2 ; R14 <- OFFSETX * 4 = OFF SET EN BYTES          
 
    mov rax, [rbp + 24]
    mul r8
    mov r15, rax ; R5 <- OFFSETY * src_row_size (extendido)

    lea rax, [rdi + R14] ;Lo muevo en eje X
    lea rax, [rax + R15] ;Lo muevo en eje Y
                         ; RAX <- subimagen desplazada

    ;devuelvo
    mov edx, r11d

    ;Armo vector para multiplicar por 0.9
    INSERTPS XMM8, [paraMultiplicar], 00001110b
    MOVUPS XMM9, XMM8
    SHUFPS XMM9, XMM8, 00000000b
    MOVUPS XMM8, XMM9

    ;INSERTPS XMM8, [nueve], 00001110b
    ;MOVUPS XMM9, XMM8
    ;SHUFPS XMM9, XMM8, 00000000b
    ;MOVUPS XMM8, XMM9

    ;INSERTPS XMM9, [diez], 00001110b
    ;MOVUPS XMM10, XMM9
    ;SHUFPS XMM10, XMM9, 00000000b
    ;MOVUPS XMM9, XMM10

    ;DIVPS XMM8, XMM9

    ;Armo vector para dividir por 8
    INSERTPS XMM9, [ocho], 00001110b
    MOVUPS XMM10, XMM9
    SHUFPS XMM10, XMM9, 00000000b
    MOVUPS XMM9, XMM10

    ;declaro contadores de pixeles de ancho y alto
    xor R14D, R14D ;ANCHO
    xor R15D, R15D ;ALTO
    
.ciclo:

    MOVDQU XMM0, [rdi]          ;XMM0 : |j3 i0|j2 i0|j1 i0|j0 i0|   
    MOVDQU XMM1, [rdi + 16]     ;XMM1 : |j7 i0|j6 i0|j5 i0|j4 i0|
    MOVDQU XMM2, [rdi + r8]     ;XMM2 : |j3 i1|j2 i1|j1 i1|j0 i1|
    MOVDQU XMM3, [rdi + r8 + 16];XMM3 : |j7 i1|j6 i1|j5 i1|j4 i1|
    MOVDQU XMM6, [rax] ;XMM6 : |desp j6 i0|desp j4 i0|desp j2 i0|desp j0 i0|
                       ;     : |A R G B .3|A R G B .2|A R G B .1|A R G B .0|

;BUSCO LOS B CORRESPONDIENTES => TRABAJO AL VECTOR XMM6
    ;SETEO A EN 0x00 (uso maskTansp, ESTA EN WORDS)
    PXOR XMM6, XMM15 

    ;EXTIENDO A DOS VECTORES PARA PODER HACER SUMA HORIZONTAL Y NO PERDER PRECISION
    PMOVZXBW XMM4, XMM6 ; LOW EN XMM4  -> | A.1 | R.1 | G.1 | B.1 || A.0 | R.0 | G.0 | B.0 |
    PSRLDQ XMM6, 8
    PMOVZXBW XMM5, XMM6 ; HIGH EN XMM5 -> | A.3 | R.3 | G.3 | B.3 || A.2 | R.2 | G.2 | B.2 |
    ;(Interleave low-order bytes from xmm1 and xmm2/m128 into xmm1)

    ;GUARDO MASK CON VALOR DE G EN XMM6                                                   CAMBIAR A SHUFFLE
    MOVDQU XMM6, XMM15
    PUNPCKLBW XMM6, XMM6
    PSRLDQ XMM6, 4
    MOVDQU XMM7, XMM6
    PAND XMM7, XMM5 ; ME GUARDO EL G (HIGH)
    PAND XMM6, XMM4 ; ME GUARDO EL G (LOW)
    PADDUSW XMM5, XMM7 ; MULTIPLICO G*2 (HIGH)
    PADDUSW XMM4, XMM6 ; MULTIPLICO G*2 (LOW)
    ;(Add packed unsigned word integers from xmm2/m128 to xmm1 and saturate the results)

    ;SUMO HORIZONTAL (saturo o no?)
    PHADDSW XMM4, XMM5 ; XMM4 -> |A.3+R.3|G.3+B.3|A.2+R.2|G.2+B.2||A.1+R.1|G.1+B.1|A.0+R.0|G.0+B.0| LIBERO XMM5
    PHADDSW XMM4, XMM4 ; XMM4 -> |A+R+G+B.3|A+R+G+B.2||A+R+G+B.1|A+R+G+B.0||A+R+G+B.3|A+R+G+B.2||A+R+G+B.1|A+R+G+B.0| (C/U DE 16)
    
    ;PARA DIVIDIR CONVIERTO EN FLOATS
    PMOVZXWD XMM5, XMM4 ; XMM5 -> |A+R+G+B.3|A+R+G+B.2||A+R+G+B.1|A+R+G+B.0|
    CVTDQ2PS XMM4, XMM5
    DIVPS XMM4, XMM9

;CASO XMM0:
    ;SETEO A EN 00 
    PXOR XMM0, XMM15 

    ;LO EXTIENDO EN XMM5 (BAJA) Y XMM0 (ALTA) => ME SEPARO LOS PIXELES DE CADA GRUPO (CORRESPONDEN A DISTINTO B)
    PMOVZXBW XMM5, XMM0 ; GRUPO0 -> XMM5 -> | A.1 | R.1 | G.1 | B.1 || A.0 | R.0 | G.0 | B.0 |
    MOVDQU XMM6, XMM0
    PSRLDQ XMM6, 8 ; |basura|basura|P3|P2|
    PMOVZXBW XMM0, XMM6 ; GRUPO1 -> XMM0 -> | A.3 | R.3 | G.3 | B.3 || A.2 | R.2 | G.2 | B.2 |

    ;MULTIPLICO CADA COMPONENTE POR 0.9
    ;Paso de W a D cada componente
    PMOVZXWD XMM6, XMM5 ; XMM6 -> | A.0 | R.0 | G.0 | B.0 |
    PSRLDQ XMM5, 8
    PMOVZXWD XMM10, XMM5 ; XMM5 -> | A.1 | R.1 | G.1 | B.1 |
    MOVDQU XMM5, XMM10
    PMOVZXWD XMM7, XMM0 ; XMM7 -> | A.2 | R.2 | G.2 | B.2 |
    PSRLDQ XMM0, 8
    PMOVZXWD XMM10, XMM0 ; XMM0 -> | A.3 | R.3 | G.3 | B.3 |
    MOVDQU XMM0, XMM10

    ;Paso de D a PS
    CVTDQ2PS XMM6, XMM6
    CVTDQ2PS XMM5, XMM5
    CVTDQ2PS XMM7, XMM7
    CVTDQ2PS XMM0, XMM0

    ;Multiplico
    MULPS XMM6, XMM8
    MULPS XMM5, XMM8
    MULPS XMM7, XMM8
    MULPS XMM0, XMM8

    ;LE SUMO EL B CORRESPONDIENTE 
    ;=> GRUPO0 LE SUMO EL B0
    MOVUPS XMM11, XMM14
    SHUFPS XMM11, XMM4, 00000000B    ; XMM11 : | B.0 | B.0 |  0  |  0  |
    MOVUPS XMM13, XMM11              ; XMM13 : | B.0 | B.0 |  0  |  0  |
    MOVHLPS XMM11, XMM11             ; XMM11 : | B.0 | B.0 | B.0 | B.0 |
    SHUFPS XMM11, XMM13, 00110000B   ; XMM11 : |  0  | B.0 | B.0 | B.0 |
    ADDPS XMM6, XMM11
    ADDPS XMM5, XMM11

    ;=> GRUPO1 LE SUMO EL B1
    MOVUPS XMM11, XMM14
    SHUFPS XMM11, XMM4, 01010000B    ; XMM11 : | B.1 | B.1 |  0  |  0  |
    MOVUPS XMM13, XMM11              ; XMM13 : | B.1 | B.1 |  0  |  0  |
    MOVHLPS XMM11, XMM11             ; XMM11 : | B.1 | B.1 | B.1 | B.1 |
    SHUFPS XMM11, XMM13, 00110000B   ; XMM11 : |  0  | B.1 | B.1 | B.1 |
    ADDPS XMM7, XMM11
    ADDPS XMM0, XMM11

    ;Paso de PS a D
    CVTTPS2DQ XMM6, XMM6
    CVTTPS2DQ XMM5, XMM5
    CVTTPS2DQ XMM7, XMM7
    CVTTPS2DQ XMM0, XMM0

    ;Paso de D a W
    PACKUSDW XMM6, XMM5 ; XMM6 : | A.1 | R.1 | G.1 | B.1 || A.0 | R.0 | G.0 | B.0 |
    PACKUSDW XMM7, XMM0 ; XMM7 : | A.3 | R.3 | G.3 | B.3 || A.2 | R.2 | G.2 | B.2 |

    ;ACHICO SATURANDO DE WORD A BYTE Y JUNTO LOS 4 PIXELES EN UN XMMN
    PACKUSWB XMM6, XMM7

    ;SETEO A EN FF
    Por XMM6, XMM15 ;(LOW)

    ;LO PEGO EN MEMORIA EN RSI
    MOVDQU [RSI], XMM6


;CASO XMM2 : |j3 i1|j2 i1|j1 i1|j0 i1|
    ;SETEO A EN 00                       
    PXOR XMM2, XMM15 ; (HIGH)

    ;LO EXTIENDO EN XMM5 (BAJA) Y XMM2 (ALTA)
    PMOVZXBW XMM5, XMM2 ; GRUPO0 -> XMM5 -> | A.1 | R.1 | G.1 | B.1 || A.0 | R.0 | G.0 | B.0 |
    MOVDQU XMM6, XMM2
    PSRLDQ XMM6, 8
    PMOVZXBW XMM2, XMM6 ; GRUPO1 -> XMM2 -> | A.3 | R.3 | G.3 | B.3 || A.2 | R.2 | G.2 | B.2 |

    ;MULTIPLICO CADA COMPONENTE POR 0.9
    ;Paso de W a D cada componente
    PMOVZXWD XMM6, XMM5 ; XMM6 -> | A.0 | R.0 | G.0 | B.0 |
    PSRLDQ XMM5, 8
    PMOVZXWD XMM10, XMM5 ; XMM5 -> | A.1 | R.1 | G.1 | B.1 |
    MOVDQU XMM5, XMM10
    PMOVZXWD XMM7, XMM2 ; XMM7 -> | A.2 | R.2 | G.2 | B.2 |
    PSRLDQ XMM2, 8
    PMOVZXWD XMM10, XMM2 ; XMM2 -> | A.3 | R.3 | G.3 | B.3 |
    MOVDQU XMM2, XMM10

    ;Paso de D a PS
    CVTDQ2PS XMM6, XMM6
    CVTDQ2PS XMM5, XMM5
    CVTDQ2PS XMM7, XMM7
    CVTDQ2PS XMM2, XMM2

    ;Multiplico
    MULPS XMM6, XMM8
    MULPS XMM5, XMM8
    MULPS XMM7, XMM8
    MULPS XMM2, XMM8

;LE SUMO EL B CORRESPONDIENTE 
    ;=> GRUPO0 LE SUMO EL B0
    MOVUPS XMM11, XMM14
    SHUFPS XMM11, XMM4, 00000000B    ; XMM11 : | B.0 | B.0 |  0  |  0  |
    MOVUPS XMM13, XMM11              ; XMM13 : | B.0 | B.0 |  0  |  0  |
    MOVHLPS XMM11, XMM11             ; XMM11 : | B.0 | B.0 | B.0 | B.0 |
    SHUFPS XMM11, XMM13, 00110000B   ; XMM11 : |  0  | B.0 | B.0 | B.0 |
    ADDPS XMM6, XMM11
    ADDPS XMM5, XMM11

    ;=> GRUPO1 LE SUMO EL B1 ;USAR SUFFLE
    MOVUPS XMM11, XMM14
    SHUFPS XMM11, XMM4, 01010000B    ; XMM11 : | B.1 | B.1 |  0  |  0  |
    MOVUPS XMM13, XMM11              ; XMM13 : | B.1 | B.1 |  0  |  0  |
    MOVHLPS XMM11, XMM11             ; XMM11 : | B.1 | B.1 | B.1 | B.1 |
    SHUFPS XMM11, XMM13, 00110000B   ; XMM11 : |  0  | B.1 | B.1 | B.1 |
    ADDPS XMM7, XMM11
    ADDPS XMM2, XMM11

    ;Paso de PS a D
    CVTTPS2DQ XMM6, XMM6
    CVTTPS2DQ XMM5, XMM5
    CVTTPS2DQ XMM7, XMM7
    CVTTPS2DQ XMM2, XMM2

    ;Paso de D a W
    PACKUSDW XMM6, XMM5 ; XMM6 : | A.1 | R.1 | G.1 | B.1 || A.0 | R.0 | G.0 | B.0 |
    PACKUSDW XMM7, XMM2 ; XMM7 : | A.3 | R.3 | G.3 | B.3 || A.2 | R.2 | G.2 | B.2 |

    ;ACHICO SATURANDO DE WORD A BYTE Y JUNTO LOS 4 PIXELES EN UN XMMN
    PACKUSWB XMM6, XMM7

    ;SETEO A EN FF
    Por XMM6, XMM15 ;(LOW)

    ;LO PEGO EN MEMORIA EN R11
    LEA R11, [RSI + R9]      ; R11 puntero a j0 i1
    MOVDQU [R11], XMM6


;CASO XMM1 : |j7 i0|j6 i0|j5 i0|j4 i0| 
    ;SETEO A EN 00                       
    PXOR XMM1, XMM15 ; (HIGH)

    ;LO EXTIENDO EN XMM5 (BAJA) Y XMM1 (ALTA)
    PMOVZXBW XMM5, XMM1 ; GRUPO2 -> XMM5 -> | A.5 | R.5 | G.5 | B.5 || A.4 | R.4 | G.4 | B.4 |
    MOVDQU XMM6, XMM1
    PSRLDQ XMM6, 8
    PMOVZXBW XMM1, XMM6 ; GRUPO3 -> XMM1 -> | A.7 | R.7 | G.7 | B.7 || A.6 | R.6 | G.6 | B.6 |

    ;MULTIPLICO CADA COMPONENTE POR 0.9
    ;Paso de W a D cada componente
    PMOVZXWD XMM6, XMM5 ; XMM6 -> | A.4 | R.4 | G.4 | B.4 |
    PSRLDQ XMM5, 8
    PMOVZXWD XMM10, XMM5 ; XMM5 -> | A.5 | R.5 | G.5 | B.5 |
    MOVDQU XMM5, XMM10
    PMOVZXWD XMM7, XMM1 ; XMM7 -> | A.6 | R.6 | G.6 | B.6 |
    PSRLDQ XMM1, 8
    PMOVZXWD XMM10, XMM1 ; XMM2 -> | A.7 | R.7 | G.7 | B.7 |
    MOVDQU XMM1, XMM10

    ;Paso de D a PS
    CVTDQ2PS XMM6, XMM6
    CVTDQ2PS XMM5, XMM5
    CVTDQ2PS XMM7, XMM7
    CVTDQ2PS XMM1, XMM1

    ;Multiplico
    MULPS XMM6, XMM8
    MULPS XMM5, XMM8
    MULPS XMM7, XMM8
    MULPS XMM1, XMM8

    ;LE SUMO EL B CORRESPONDIENTE 
    ;=> GRUPO2 LE SUMO EL B2
    MOVUPS XMM11, XMM14
    SHUFPS XMM11, XMM4, 10100000B    ; XMM11 : | B.2 | B.2 |  0  |  0  |
    MOVUPS XMM13, XMM11              ; XMM13 : | B.2 | B.2 |  0  |  0  |
    MOVHLPS XMM11, XMM11             ; XMM11 : | B.2 | B.2 | B.2 | B.2 |
    SHUFPS XMM11, XMM13, 00110000B   ; XMM11 : |  0  | B.2 | B.2 | B.2 |
    ADDPS XMM6, XMM11
    ADDPS XMM5, XMM11

    ;=> GRUPO3 LE SUMO EL B3
    MOVUPS XMM11, XMM14
    SHUFPS XMM11, XMM4, 11110000B    ; XMM11 : | B.3 | B.3 |  0  |  0  |
    MOVUPS XMM13, XMM11              ; XMM13 : | B.3 | B.3 |  0  |  0  |
    MOVHLPS XMM11, XMM11             ; XMM11 : | B.3 | B.3 | B.3 | B.3 |
    SHUFPS XMM11, XMM13, 00110000B   ; XMM11 : |  0  | B.3 | B.3 | B.3 |
    ADDPS XMM7, XMM11
    ADDPS XMM1, XMM11

    ;Paso de PS a D
    CVTTPS2DQ XMM6, XMM6
    CVTTPS2DQ XMM5, XMM5
    CVTTPS2DQ XMM7, XMM7
    CVTTPS2DQ XMM1, XMM1

    ;Paso de D a W
    PACKUSDW XMM6, XMM5 ; XMM6 : | A.5 | R.5 | G.5 | B.5 || A.4 | R.4 | G.4 | B.4 |
    PACKUSDW XMM7, XMM1 ; XMM7 : | A.7 | R.7 | G.7 | B.7 || A.6 | R.6 | G.6 | B.6 |

    ;ACHICO SATURANDO DE WORD A BYTE Y JUNTO LOS 4 PIXELES EN UN XMMN
    PACKUSWB XMM6, XMM7

    ;SETEO A EN FF
    Por XMM6, XMM15 ;(LOW)

    ;LO PEGO EN MEMORIA EN R11
    LEA R11, [RSI + 16]      ; R11 puntero a j4 i0
    MOVDQU [R11], XMM6


;CASO XMM3 : |j7 i1|j6 i1|j5 i1|j4 i1|
    ;SETEO A EN 00                       
    PXOR XMM3, XMM15 ; (HIGH)

    ;LO EXTIENDO EN XMM5 (BAJA) Y XMM3 (ALTA)
    PMOVZXBW XMM5, XMM3 ; GRUPO2 -> XMM5 -> | A.5 | R.5 | G.5 | B.5 || A.4 | R.4 | G.4 | B.4 |
    MOVDQU XMM6, XMM3
    PSRLDQ XMM6, 8
    PMOVZXBW XMM3, XMM6 ; GRUPO3 -> XMM3 -> | A.7 | R.7 | G.7 | B.7 || A.6 | R.6 | G.6 | B.6 |

    ;MULTIPLICO CADA COMPONENTE POR 0.9
    ;Paso de W a D cada componente
    PMOVZXWD XMM6, XMM5 ; XMM6 -> | A.4 | R.4 | G.4 | B.4 |
    PSRLDQ XMM5, 8
    PMOVZXWD XMM10, XMM5 ; XMM5 -> | A.5 | R.5 | G.5 | B.5 |
    MOVDQU XMM5, XMM10
    PMOVZXWD XMM7, XMM3 ; XMM7 -> | A.6 | R.6 | G.6 | B.6 |
    PSRLDQ XMM3, 8
    PMOVZXWD XMM10, XMM3 ; XMM2 -> | A.7 | R.7 | G.7 | B.7 |
    MOVDQU XMM3, XMM10

    ;Paso de D a PS
    CVTDQ2PS XMM6, XMM6
    CVTDQ2PS XMM5, XMM5
    CVTDQ2PS XMM7, XMM7
    CVTDQ2PS XMM3, XMM3

    ;Multiplico
    MULPS XMM6, XMM8
    MULPS XMM5, XMM8
    MULPS XMM7, XMM8
    MULPS XMM3, XMM8

;LE SUMO EL B CORRESPONDIENTE 
    ;=> GRUPO2 LE SUMO EL B2
    MOVUPS XMM11, XMM14
    SHUFPS XMM11, XMM4, 10100000B    ; XMM11 : | B.2 | B.2 |  0  |  0  |
    MOVUPS XMM13, XMM11              ; XMM13 : | B.2 | B.2 |  0  |  0  |
    MOVHLPS XMM11, XMM11             ; XMM11 : | B.2 | B.2 | B.2 | B.2 |
    SHUFPS XMM11, XMM13, 00110000B   ; XMM11 : |  0  | B.2 | B.2 | B.2 |
    ADDPS XMM6, XMM11
    ADDPS XMM5, XMM11

    ;=> GRUPO3 LE SUMO EL B3
    MOVUPS XMM11, XMM14
    SHUFPS XMM11, XMM4, 11110000B    ; XMM11 : | B.3 | B.3 |  0  |  0  |
    MOVUPS XMM13, XMM11              ; XMM13 : | B.3 | B.3 |  0  |  0  |
    MOVHLPS XMM11, XMM11             ; XMM11 : | B.3 | B.3 | B.3 | B.3 |
    SHUFPS XMM11, XMM13, 00110000B   ; XMM11 : |  0  | B.3 | B.3 | B.3 |
    ADDPS XMM7, XMM11
    ADDPS XMM3, XMM11

    ;Paso de PS a D
    CVTTPS2DQ XMM6, XMM6
    CVTTPS2DQ XMM5, XMM5
    CVTTPS2DQ XMM7, XMM7
    CVTTPS2DQ XMM3, XMM3

    ;Paso de D a W
    PACKUSDW XMM6, XMM5 ; XMM6 : | A.5 | R.5 | G.5 | B.5 || A.4 | R.4 | G.4 | B.4 |
    PACKUSDW XMM7, XMM3 ; XMM7 : | A.7 | R.7 | G.7 | B.7 || A.6 | R.6 | G.6 | B.6 |

    ;ACHICO SATURANDO DE WORD A BYTE Y JUNTO LOS 4 PIXELES EN UN XMMN
    PACKUSWB XMM6, XMM7

    ;SETEO A EN FF
    por XMM6, XMM15 ;(LOW)

    ;LO PEGO EN MEMORIA EN R11
    LEA R11, [RSI + R9 + 16] ; R13 puntero a j4 i1
    MOVDQU [R11], XMM6


;LO ULTIMO DEL CICLO
    ;avanzo en imagen source
    ADD RDI, 32 ;V2
    ;avanzo en imagen destino
    ADD RSI, 32 ;V2
    ;avanzo en imagen desplazada
    ADD RAX, 16 ;V1
    ;avanzo contador de pixeles
    ADD R14D, 8
    ;chequeo si seguir en el ciclo
    CMP EDX, R14D ;ANCHO - PROXIMO J
    JNE .ciclo
;SI LLEGUE AL ANCHO, SALTO A LA OTRA FILA
.termineFila:
    xor R14D, R14D ;Seteo el J en 0
    ADD R15D, 2
    CMP ECX, R15D ;ALTO - PROXIMO I
    JE .finCiclo
    ADD RDI, R8
    ADD RSI, R9
    add rax, r10
    JMP .ciclo
;rdi        <- uint8_t *src 
;rsi        <- uint8_t *dst
;edx        <- int width
;ecx        <- int height
;r8d        <- int src_row_size
;r9d        <- int dst_row_size
;[rbp + 16] <- int offsetx
;[rbp + 24] <- int offsety

;SI LLEGUE A LA ULTI FILA => SALGO DEL CICLO
.finCiclo:
    add RSP, 8
    pop R15
    pop R14
    pop R13
    pop R12
    pop RBX
    pop RBP
    ret
