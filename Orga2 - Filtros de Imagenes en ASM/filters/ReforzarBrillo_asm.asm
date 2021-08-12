section .data
transparencia: DQ 0xff000000ff000000, 0xff000000ff000000
mascara_verde: DQ 0x00000000ffff0000, 0x00000000ffff0000

section .bss
buffer:

section .text
extern ReforzarBrillo_c
global ReforzarBrillo_asm


ReforzarBrillo_asm:

; La proxima linea debe ser replazada por el codigo asm
;jmp ReforzarBrillo_c

;armo el stackframe
	push rbp
	mov rbp, rsp
	push rbx
	push r12
	push r13
	push r14

;*src -> rdi
;*dst -> rsi
;int width -> edx
;int height -> ecx
;int src_row_size -> r8d
;int dst_row_size -> r9d
;int umbralSup -> [rbp + 0]
;int umbralInf -> [rbp + 8]
;int brilloSup -> [rbp + 16]
;int brilloInf -> [rbp + 24]

;elimino basura
	xor rbx, rbx
	xor r12, r12
	xor r13, r13
	xor r14, r14

	mov ebx, [rbp + 16]
	mov r12d, [rbp + 24]
	mov r13b, [rbp + 32]
	mov r14b, [rbp + 40]

;armo mascara de transparencia
	movdqu xmm10, [transparencia]

;armo mascara verde
	movdqu xmm11, [mascara_verde]

;armo cero
	pxor xmm12, xmm12

;armo el vector de suma de brillo [bs|bs|bs|0|x4]
	pinsrb xmm0, r13b, 00000000
	pshufb xmm0, xmm12
	psubusb xmm0, xmm10

;armo el vector de resta de brillo [bi|bi|bi|0|x4]
	pinsrb xmm1, r14b, 00000000
	pshufb xmm1, xmm12
	psubusb xmm1, xmm10

;armo el vector del umbral superior [Ums|Ums|Ums|Ums]
	mov [buffer], ebx
	mov [buffer + 4], ebx
	mov [buffer + 8], ebx
	mov [buffer + 12], ebx
	movdqu xmm2, [buffer]

;armo el vector del umbral inferior [Umi|Umi|Umi|Umi]
	mov [buffer], r12d
	mov [buffer + 4], r12d
	mov [buffer + 8], r12d
	mov [buffer + 12], r12d
	movdqu xmm3, [buffer]

;inicializo el ciclo
	xor rax, rax
	mov eax, edx
	xor rdx, rdx
	mov edx, ecx
	mul rdx
	mov rcx, rax 
	shr rcx, 2;rcx tiene cuantas veces se debe ejecutar el ciclo

.ciclo:
;cargo datos
	movdqu xmm4, [rdi]
	movdqu xmm5, xmm4 ;uso xmm5 para realizar operaciones sobre los registros 

;calculo  el brillo:
	psubusb xmm5, xmm10 ;quito la transparencia para que no interfiera con los calculos
	pmovzxbw xmm7, xmm5
	movhlps xmm5, xmm5; operacion con float para la experimentacion
	;PSRLDQ xmm5, 8; xmm5[p1|p2|p3|p4] -> [**|**|p1|p2]
	pmovzxbw xmm6, xmm5
	;xmm6 tiene los primeros dos pixeles extendidos a word, y xmm7 tiene los segundos dos, ambos extendidos a word y sin transparencia

	;les sumo su verde de nuevo
	movdqu xmm13, xmm7
	pand xmm13, xmm11
	paddusw xmm7, xmm13

	movdqu xmm13, xmm6
	pand xmm13, xmm11
	paddusw xmm6, xmm13

	;realizo las sumas horizontales
	phaddsw xmm7, xmm6
	phaddsw xmm7, xmm7
	pmovzxwd xmm7, xmm7
	psrld xmm7, 2 ;divido por cuatro la suma B + 2G + R
	movdqu xmm6, xmm7
;xmm6 y xmm7 tienen como int de 32bit los brillos de los 4 pixeles

;xmm0 tiene el bs
;xmm1 tiene el bi
;xmm2 tiene el Ums
;xmm3 tiene el Umi
;xmm4 tiene los 4 pixeles originales
;xmm10 tiene la mascara de transparencia
;xmm11 tiene la mascara verde
;xmm12 tiene el cero de 128bit

	psubd xmm6, xmm2
	pcmpgtd xmm6, xmm12
;xmm6 mascara del umbral superior

	movdqu xmm5, xmm3
	psubd xmm5, xmm7
	pcmpgtd xmm5, xmm12
;xmm5 tiene la mascara del umbral inferior

;sumo y resto brillos
	movdqu xmm7, xmm0
	pand xmm7, xmm6 ;aplico la mascara al brillo adicional
	paddusb xmm4, xmm7 ;sumo el brillo adicional

	movdqu xmm7, xmm1
	pand xmm7, xmm5 ;aplico la mascara al brillo decremental
	psubusb xmm4, xmm7 ;resto el brillo decremental
	
	movdqu [rsi], xmm4

;avanzo el ciclo
	add rdi, 16
	add rsi, 16
	dec rcx
	cmp rcx, 0x0
	je .fin
	jmp .ciclo

.fin:
;desarmo el stackframe
	pop r14
	pop r13
	pop r12
	pop rbx
	pop rbp

ret
