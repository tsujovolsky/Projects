/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del scheduler
*/

#include "sched.h"
#include "colors.h"
#include "screen.h"
extern void end_game(void);

const uint32_t task_size = 11; //cantidad de tasks en las listas de abajo (al final deberia ser 11)
uint16_t rick_tasks[11] = {0x0088, 0x0098, 0x00a0, 0x00a8, 0x00b0, 0x00b8, 0x00c0, 0x00c8, 0x00d0, 0x00d8, 0x00e0}; //completar con la tss de los 10 meeseeks
uint16_t morty_tasks[11] = {0x0090, 0x00e8, 0x00f0, 0x00f8, 0x0100, 0x0108, 0x0110, 0x0118, 0x0120, 0x0128, 0x0130}; //completar con la tss de los 10 meeseeks
uint32_t indicador_jugador; //cero a rick, 1 a morty
uint32_t indicador_rick_task;
uint32_t indicador_morty_task;


uint32_t rick_meeseeks[10]; // si esta en cero esta muerto, 1 vivo
uint32_t morty_meeseeks[10];

uint32_t map[40*80]; //0 nada, 1 semilla, 2 Messeks de rick, 3 meeseeks de morty


void sched_init(void) {
	indicador_jugador = 0; //primer tarea a leer es de rick
	indicador_rick_task = 0; // la primera tarea que se lee de rick es rick
	indicador_morty_task = 0; // idem morty

	// Se arranca sin meeseks vivos
	
	for (uint32_t i = 0; i < 10; ++i){
		rick_meeseeks[i] = 0;
		morty_meeseeks[i] = 0;
	}

	for (int i = 0; i < 40*80; ++i){
		map[i] = 0;
	}
}

uint16_t sched_next_task(void){//agregar a que funcione con meeseeks
	uint16_t next_task; // Valor a devolver
	if (indicador_jugador == 0){// la tarea que agarro es de rick
		indicador_jugador = 1;
		while (indicador_rick_task != 0 && rick_meeseeks[indicador_rick_task - 1] == 0){
			indicador_rick_task++;
			if (indicador_rick_task == task_size){
				indicador_rick_task = 0;
			}
		}
		next_task = rick_tasks[indicador_rick_task];
		indicador_rick_task++;
		if (indicador_rick_task == task_size){
			indicador_rick_task = 0;
		}
	} else{
		indicador_jugador = 0;
		while (indicador_morty_task != 0 && morty_meeseeks[indicador_morty_task - 1] == 0){
			indicador_morty_task++;
			if (indicador_morty_task == task_size){
				indicador_morty_task = 0;
			}
		}
		next_task = morty_tasks[indicador_morty_task];
		indicador_morty_task++;
		if (indicador_morty_task == task_size){
			indicador_morty_task = 0;
		}
	}
	return (next_task + 3);
 }


uint32_t indice_task(uint16_t task_list[], uint16_t task){
	for (uint32_t i = 0; i < task_size; ++i){
		if (task_list[i] == task) return i;
	}
	return -1;
}

void disable_meeseeks(uint32_t meeseeks[], uint32_t indice){
	meeseeks[indice] = 0;
}

extern void limpiar_en_mapa(uint16_t meeseek_tr);

void desalojar(uint16_t task_register){
	if (indicador_jugador == 1){
		uint32_t indice = indice_task(rick_tasks, task_register - 3);
		if (indice == 0){
			print("Finalizo el juego, gano Morty               ", 0, 0, C_FG_WHITE | C_BG_BLACK);
			end_game();
		}else{
			disable_meeseeks(rick_meeseeks, indice - 1);
			limpiar_en_mapa(task_register);
		}
	} else{
		uint32_t indice = indice_task(morty_tasks, task_register - 3);
		if (indice == 0){
			print("Finalizo el juego, gano Rick                ", 0, 0, C_FG_WHITE | C_BG_BLACK);
			end_game();
		}else{
			disable_meeseeks(morty_meeseeks, indice - 1);
			limpiar_en_mapa(task_register);
		}
	}
}

void map_set(uint32_t fila, uint32_t columna, uint32_t objeto){
	map[columna + 80*fila] = objeto;
}

uint32_t map_access(uint32_t fila, uint32_t columna){
	return map[columna + 80*fila];
}
