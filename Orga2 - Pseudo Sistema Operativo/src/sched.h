/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del scheduler.
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "types.h"

void sched_init();
uint16_t sched_next_task();
void desalojar(uint16_t task_register);
void map_set(uint32_t fila, uint32_t columna, uint32_t objeto);
uint32_t map_access(uint32_t fila, uint32_t columna);

//Agregado por el colo
uint32_t rick_meeseeks[10]; // si esta en cero esta muerto, 1 vivo
uint32_t morty_meeseeks[10];
uint32_t map[80*40];

#endif //  __SCHED_H__
