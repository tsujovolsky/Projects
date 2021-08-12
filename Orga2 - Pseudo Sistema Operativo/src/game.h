/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de las rutinas asociadas al juego.
*/

#ifndef __GAME_H__
#define __GAME_H__
#include "types.h"

typedef enum e_task_type {
  Rick = 1,
  Morty = 2,
  Meeseeks = 3,
} task_type_t;

typedef struct str_meeseeks {
	//variables de reseteo
	uint32_t esp0;
	uint32_t* codigo_virtual_p;
    uint32_t* pila_virtual_p;
	//propiedades del meeseeks
	uint32_t* puntero_al_codigo;
	int fila;  //0 <= fila < 40
	int columna; //0 <= columna < 80
	uint32_t movimiento;
	uint32_t turnos_vivo;
	uint32_t portal; //0 o 1
	//identificadores del meeseeks
	uint32_t cr3; //mapa de memoria
	uint16_t task_register; //identificar la tss
	uint32_t task_id; //acceder a la tss por medio de la lista
	uint32_t indicador_jugador; //0 rick, 1 morty
	uint32_t indicador_meeseeks; //0 al 9
  
} __attribute__((__packed__, aligned(8))) meeseeks_t;

void game_init(void);
void analizar_ganador(void);
void mapeo_y_guardo_codigo(int indicador);

int puntos_rick;
int puntos_morty;

meeseeks_t* meeseeks_list[20];

extern void relojrick0();
extern void relojrick1();
extern void relojrick2();
extern void relojrick3();
extern void relojrick4();
extern void relojrick5();
extern void relojrick6();
extern void relojrick7();
extern void relojrick8();
extern void relojrick9();
extern void relojrick();

extern void relojmorty0();
extern void relojmorty1();
extern void relojmorty2();
extern void relojmorty3();
extern void relojmorty4();
extern void relojmorty5();
extern void relojmorty6();
extern void relojmorty7();
extern void relojmorty8();
extern void relojmorty9();
extern void relojmorty();


#endif //  __GAME_H__
