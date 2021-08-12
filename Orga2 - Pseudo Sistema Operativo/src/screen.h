/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del scheduler
*/

#ifndef __SCREEN_H__
#define __SCREEN_H__

/* Definicion de la pantalla */
#define VIDEO_FILS 50
#define VIDEO_COLS 80

#include "colors.h"
#include "defines.h"
#include "stdint.h"
//#include <stdio.h> // Agregado
//#include <stdlib.h> //Agregado

/* Estructura de para acceder a memoria de video */
typedef struct ca_s {
  uint8_t c;
  uint8_t a;
} ca;

void print(const char* text, uint32_t x, uint32_t y, uint16_t attr);
void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y,
               uint16_t attr);
void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y,
               uint16_t attr);

void screen_draw_box(uint32_t fInit, uint32_t cInit, uint32_t fSize,
                     uint32_t cSize, uint8_t character, uint8_t attr);

void screen_draw_layout(void);

void printScanCode(uint8_t scancode);//RUTINA DE TECLADO

void print_Mega_Seeds(void);

void Meeseek_status_update(void);

void points_update(void);

extern uint8_t indicador_debugger;

typedef struct str_semilla_t {
  uint32_t x;
  uint32_t y;
  uint32_t estado;
} __attribute__((__packed__, aligned(8))) semilla_t;

semilla_t semillas[40];

uint32_t Rand_In_Range ( int min, int max );
int myRand ();

uint32_t status_debugger(void);
uint32_t status_debugger_on_screen(void);
void debug_print(uint32_t isr_id);

typedef struct str_param_pusheados_ec {
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;

  uint32_t errorcode;
  uint32_t eip3;
  uint32_t cs3;
  uint32_t eflags;
  uint32_t esp3;
  uint32_t ss3;
}__attribute__((__packed__, aligned(4))) param_pusheados_ec;

typedef struct str_param_pusheados {
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  
  uint32_t eip3;
  uint32_t cs3;
  uint32_t eflags;
  uint32_t esp3;
  uint32_t ss3;
}__attribute__((__packed__, aligned(4))) param_pusheados;

#endif //  __SCREEN_H__
