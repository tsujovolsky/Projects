/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "prng.h"
#include "types.h"
#include "sched.h"
#include "i386.h"
#include "tss.h"
#include "screen.h"
#include "mmu.h"
extern void end_game(void);

#define rick  0x0088 + 3
#define morty 0x0090 + 3

int puntos_rick = 0;
int puntos_morty = 0;

//definir los 20 meeseeks

meeseeks_t rick_meeseeks_0 = {
	.cr3 = 0x00100000,
	.task_register = 0x0098 + 3,
	.task_id = 4,
	.indicador_jugador = 0,
	.indicador_meeseeks = 0,
	.codigo_virtual_p = (uint32_t *)0x08000000,
    .pila_virtual_p = (uint32_t *)(0x08002000),
};
meeseeks_t rick_meeseeks_1 = {
	.cr3 = 0x00100000,
	.task_register = 0x00a0 + 3,
	.task_id = 5,
	.indicador_jugador = 0,
	.indicador_meeseeks = 1,
	.codigo_virtual_p = (uint32_t *)0x08002000,
    .pila_virtual_p = (uint32_t *)(0x08004000),
};
meeseeks_t rick_meeseeks_2 = {
	.cr3 = 0x00100000,
	.task_register = 0x00a8 + 3,
	.task_id = 6,
	.indicador_jugador = 0,
	.indicador_meeseeks = 2,
	.codigo_virtual_p = (uint32_t *)0x08004000,
    .pila_virtual_p = (uint32_t *)(0x08006000),
};
meeseeks_t rick_meeseeks_3 = {
	.cr3 = 0x00100000,
	.task_register = 0x00b0 + 3,
	.task_id = 7,
	.indicador_jugador = 0,
	.indicador_meeseeks = 3,
	.codigo_virtual_p = (uint32_t *)0x08006000,
    .pila_virtual_p = (uint32_t *)(0x08008000),
};
meeseeks_t rick_meeseeks_4 = {
	.cr3 = 0x00100000,
	.task_register = 0x00b8 + 3,
	.task_id = 8,
	.indicador_jugador = 0,
	.indicador_meeseeks = 4,
	.codigo_virtual_p = (uint32_t *)0x08008000,
    .pila_virtual_p = (uint32_t *)(0x0800A000),
};
meeseeks_t rick_meeseeks_5 = {
	.cr3 = 0x00100000,
	.task_register = 0x00c0 + 3,
	.task_id = 9,
	.indicador_jugador = 0,
	.indicador_meeseeks = 5,
	.codigo_virtual_p = (uint32_t *)0x0800A000,
    .pila_virtual_p = (uint32_t *)(0x0800C000),
};
meeseeks_t rick_meeseeks_6 = {
	.cr3 = 0x00100000,
	.task_register = 0x00c8 + 3,
	.task_id = 10,
	.indicador_jugador = 0,
	.indicador_meeseeks = 6,
	.codigo_virtual_p = (uint32_t *)0x0800C000,
    .pila_virtual_p = (uint32_t *)(0x0800E000),
};
meeseeks_t rick_meeseeks_7 = {
	.cr3 = 0x00100000,
	.task_register = 0x00d0 + 3,
	.task_id = 11,
	.indicador_jugador = 0,
	.indicador_meeseeks = 7,
	.codigo_virtual_p = (uint32_t *)0x0800E000,
    .pila_virtual_p = (uint32_t *)(0x08010000),
};
meeseeks_t rick_meeseeks_8 = {
	.cr3 = 0x00100000,
	.task_register = 0x00d8 + 3,
	.task_id = 12,
	.indicador_jugador = 0,
	.indicador_meeseeks = 8,
	.codigo_virtual_p = (uint32_t *)0x08010000,
    .pila_virtual_p = (uint32_t *)(0x08012000),
};
meeseeks_t rick_meeseeks_9 = {
	.cr3 = 0x00100000,
	.task_register = 0x00e0 + 3,
	.task_id = 13,
	.indicador_jugador = 0,
	.indicador_meeseeks = 9,
	.codigo_virtual_p = (uint32_t *)0x08012000,
    .pila_virtual_p = (uint32_t *)(0x08014000),
};

meeseeks_t morty_meeseeks_0 = {
	.cr3 = 0x00105000,
	.task_register = 0x00e8 + 3,
	.task_id = 14,
	.indicador_jugador = 1,
	.indicador_meeseeks = 0,
	.codigo_virtual_p = (uint32_t *)0x08000000,
    .pila_virtual_p = (uint32_t *)(0x08002000),
};
meeseeks_t morty_meeseeks_1 = {
	.cr3 = 0x00105000,
	.task_register = 0x00f0 + 3,
	.task_id = 15,
	.indicador_jugador = 1,
	.indicador_meeseeks = 1,
	.codigo_virtual_p = (uint32_t *)0x08002000,
    .pila_virtual_p = (uint32_t *)(0x08004000),
};
meeseeks_t morty_meeseeks_2 = {
	.cr3 = 0x00105000,
	.task_register = 0x00f8 + 3,
	.task_id = 16,
	.indicador_jugador = 1,
	.indicador_meeseeks = 2,
	.codigo_virtual_p = (uint32_t *)0x08004000,
    .pila_virtual_p = (uint32_t *)(0x08006000),
};
meeseeks_t morty_meeseeks_3 = {
	.cr3 = 0x00105000,
	.task_register = 0x0100 + 3,
	.task_id = 17,
	.indicador_jugador = 1,
	.indicador_meeseeks = 3,
	.codigo_virtual_p = (uint32_t *)0x08006000,
    .pila_virtual_p = (uint32_t *)(0x08008000),
};
meeseeks_t morty_meeseeks_4 = {
	.cr3 = 0x00105000,
	.task_register = 0x0108 + 3,
	.task_id = 18,
	.indicador_jugador = 1,
	.indicador_meeseeks = 4,
	.codigo_virtual_p = (uint32_t *)0x08008000,
    .pila_virtual_p = (uint32_t *)(0x0800A000),
};
meeseeks_t morty_meeseeks_5 = {
	.cr3 = 0x00105000,
	.task_register = 0x0110 + 3,
	.task_id = 19,
	.indicador_jugador = 1,
	.indicador_meeseeks = 5,
	.codigo_virtual_p = (uint32_t *)0x0800A000,
    .pila_virtual_p = (uint32_t *)(0x0800C000),
};
meeseeks_t morty_meeseeks_6 = {
	.cr3 = 0x00105000,
	.task_register = 0x0118 + 3,
	.task_id = 20,
	.indicador_jugador = 1,
	.indicador_meeseeks = 6,
	.codigo_virtual_p = (uint32_t *)0x0800C000,
    .pila_virtual_p = (uint32_t *)(0x0800E000),
};
meeseeks_t morty_meeseeks_7 = {
	.cr3 = 0x00105000,
	.task_register = 0x0120 + 3,
	.task_id = 21,
	.indicador_jugador = 1,
	.indicador_meeseeks = 7,
	.codigo_virtual_p = (uint32_t *)0x0800E000,
    .pila_virtual_p = (uint32_t *)(0x08010000),
};
meeseeks_t morty_meeseeks_8 = {
	.cr3 = 0x00105000,
	.task_register = 0x0128 + 3,
	.task_id = 22,
	.indicador_jugador = 1,
	.indicador_meeseeks = 8,
	.codigo_virtual_p = (uint32_t *)0x08010000,
    .pila_virtual_p = (uint32_t *)(0x08012000),
};
meeseeks_t morty_meeseeks_9 = {
	.cr3 = 0x00105000,
	.task_register = 0x0130 + 3,
	.task_id = 23,
	.indicador_jugador = 1,
	.indicador_meeseeks = 9,
	.codigo_virtual_p = (uint32_t *)0x08012000,
    .pila_virtual_p = (uint32_t *)(0x08014000),
};

meeseeks_t* meeseeks_list[20] = {
	[0] = &rick_meeseeks_0,
	[1] = &rick_meeseeks_1,
	[2] = &rick_meeseeks_2,
	[3] = &rick_meeseeks_3,
	[4] = &rick_meeseeks_4,
	[5] = &rick_meeseeks_5,
	[6] = &rick_meeseeks_6,
	[7] = &rick_meeseeks_7,
	[8] = &rick_meeseeks_8,
	[9] = &rick_meeseeks_9,
	[10] = &morty_meeseeks_0,
	[11] = &morty_meeseeks_1,
	[12] = &morty_meeseeks_2,
	[13] = &morty_meeseeks_3,
	[14] = &morty_meeseeks_4,
	[15] = &morty_meeseeks_5,
	[16] = &morty_meeseeks_6,
	[17] = &morty_meeseeks_7,
	[18] = &morty_meeseeks_8,
	[19] = &morty_meeseeks_9,
};

void apagar_semilla(uint32_t x, uint32_t y){
	for (int i = 0; i < 40; ++i){
		if (semillas[i].x == x && semillas[i].y == y){
			semillas[i].estado = 0;
			break;
		}
	}
}

void inicializar_esp0(){
	for (int i = 0; i < 10; ++i){
		meeseeks_list[i]->esp0 = esp_rick_meeseeks[i];
	}
	for (int i = 0; i < 10; ++i){
		meeseeks_list[i+10]->esp0 = esp_morty_meeseeks[i];
	}
}

void activar_meeseeks(int indicador, uint32_t* codigo, int x, int y){
    // Me guardo la info renovada al struct meeseek
	meeseeks_list[indicador]->columna = x;
	meeseeks_list[indicador]->fila = y;
	meeseeks_list[indicador]->movimiento = 7;
	meeseeks_list[indicador]->turnos_vivo = 0;
	meeseeks_list[indicador]->portal = 1;
	meeseeks_list[indicador]->puntero_al_codigo = codigo;
    // Mapeo y guardo el codigo. Si
    mapeo_y_guardo_codigo(indicador);
    // Reseteo la tss
	tss_list[meeseeks_list[indicador]->task_id]->esp0 = meeseeks_list[indicador]->esp0;
	tss_list[meeseeks_list[indicador]->task_id]->cr3 = meeseeks_list[indicador]->cr3;
	tss_list[meeseeks_list[indicador]->task_id]->eax = 0;
	tss_list[meeseeks_list[indicador]->task_id]->ecx = 0;
	tss_list[meeseeks_list[indicador]->task_id]->ebx = 0;
	tss_list[meeseeks_list[indicador]->task_id]->edx = 0;
	tss_list[meeseeks_list[indicador]->task_id]->esi = 0;
	tss_list[meeseeks_list[indicador]->task_id]->edi = 0;
    tss_list[meeseeks_list[indicador]->task_id]->eflags = 0x202;
	tss_list[meeseeks_list[indicador]->task_id]->esp = (uint32_t) meeseeks_list[indicador]->pila_virtual_p;
    tss_list[meeseeks_list[indicador]->task_id]->ebp = (uint32_t) meeseeks_list[indicador]->pila_virtual_p;
	tss_list[meeseeks_list[indicador]->task_id]->eip = (uint32_t) meeseeks_list[indicador]->codigo_virtual_p;
	tss_list[meeseeks_list[indicador]->task_id]->es = ((uint16_t)(13 << 3)) + 0x3;
    tss_list[meeseeks_list[indicador]->task_id]->cs = ((uint16_t)(11 << 3)) + 0x3; //Para el codigo
    tss_list[meeseeks_list[indicador]->task_id]->ss = ((uint16_t)(13 << 3)) + 0x3; //Para el stack
    tss_list[meeseeks_list[indicador]->task_id]->ds = ((uint16_t)(13 << 3)) + 0x3;
    tss_list[meeseeks_list[indicador]->task_id]->fs = ((uint16_t)(13 << 3)) + 0x3;
    tss_list[meeseeks_list[indicador]->task_id]->gs = ((uint16_t)(13 << 3)) + 0x3;
}

// x columna 0 79=0x4F
// y fila    0 39=0x27


void mapeo_y_guardo_codigo(int indicador){
    // CALCULAR DCC FISICA
    uint32_t fisica_x = meeseeks_list[indicador]->columna;
    uint32_t fisica_y = meeseeks_list[indicador]->fila;
    uint32_t dest_fisico_u = 0x400000 + 0x2000*fisica_x + 0x2000*fisica_y*0x50;
    uint32_t cod_virt = (uint32_t)meeseeks_list[indicador]->codigo_virtual_p;
    // MAPEO LAS FISICAS A LA VIRTUAL
    mmu_map_page(meeseeks_list[indicador]->cr3, cod_virt, dest_fisico_u, 0x7);
    mmu_map_page(meeseeks_list[indicador]->cr3, cod_virt+0x1000, dest_fisico_u+0x1000, 0x7);
    // COPIAR LAS PAGINAS DE LA TAREA
    uint32_t *codigo_a_copiar = meeseeks_list[indicador]->puntero_al_codigo;
    uint32_t *cod_virt_p = (uint32_t *)cod_virt;
    for (int i = 0; i < 1024; i++) {
        cod_virt_p[i] = codigo_a_copiar[i];
    }
}

void movimiento (int indicador, int x, int y){
    // Me mapeo temporalmente con Id.Map. las paginas fisicas de pila y codigo
    uint32_t fisica_x = meeseeks_list[indicador]->columna;
    uint32_t fisica_y = meeseeks_list[indicador]->fila;
    uint32_t fisica_u = 0x400000 + 0x2000*fisica_x + 0x2000*fisica_y*0x50;
    mmu_map_page(meeseeks_list[indicador]->cr3, fisica_u, fisica_u, 0x7);
    mmu_map_page(meeseeks_list[indicador]->cr3, fisica_u + 0x1000, fisica_u + 0x1000, 0x7);

    // Mapeo la nueva posicion fisica del meeseek a la direccion virtual fija
    uint32_t fisica_n_x = x;
    uint32_t fisica_n_y = y;
    uint32_t dest_fisico_u = 0x400000 + 0x2000*fisica_n_x + 0x2000*fisica_n_y*0x50;
    uint32_t cod_virt = (uint32_t)meeseeks_list[indicador]->codigo_virtual_p;
    mmu_map_page(meeseeks_list[indicador]->cr3, cod_virt, dest_fisico_u, 0x7);
    mmu_map_page(meeseeks_list[indicador]->cr3, cod_virt + 0x1000, dest_fisico_u + 0x1000, 0x7);

    // Copiamos el codigo y la pila que teniamos antes de movernos
    uint32_t *fisica_u_p = (uint32_t *)fisica_u;
    uint32_t *cod_virt_p = (uint32_t *)cod_virt;
    for (int i = 0; i < 2*1024; i++) {
        cod_virt_p[i] = fisica_u_p[i];
    }

    // Desmapeamos las paginas fisicas anteriores
    mmu_unmap_page(meeseeks_list[indicador]->cr3, fisica_u);
    mmu_unmap_page(meeseeks_list[indicador]->cr3, fisica_u + 0x1000);
}


uint32_t* meeseeks(uint32_t* codigo, int x, int y){
	uint32_t ralph_el_demoledor = 0;
	uint16_t current_tr = rtr();
	if (current_tr == rick){
		if (x < 0 || x > 79 || y < 0 || y > 39 || codigo < (uint32_t*)0x1D00000 || codigo > (uint32_t*)0x1D00FFF){ //posicion invalida o codigo fuera de lugar
		uint32_t ilegal = 1/ralph_el_demoledor;
		return (uint32_t*)ilegal;
		}
		if (map_access(y, x) == 1){
			puntos_rick += 450;
			apagar_semilla(x, y);
			map_set(y, x, 0);
			return 0;
		}
		for (int i = 0; i < 10; ++i){
			if (rick_meeseeks[i] == 0){
				rick_meeseeks[i] = 1;
				activar_meeseeks(i, codigo, x, y);
				map_set(y, x, 2);
				return meeseeks_list[i]->codigo_virtual_p;
				break;
			}
		}
	} else if (current_tr == morty) {
		if (x < 0 || x > 79 || y < 0 || y > 39 || codigo < (uint32_t*)0x1D00000 || codigo > (uint32_t*)0x1D00FFF){
		uint32_t ilegal = 1/ralph_el_demoledor;
		return (uint32_t*)ilegal;
		}
		if (map_access(y, x) == 1){
			puntos_morty += 450;
			apagar_semilla(x, y);
			map_set(y, x, 0);
			return 0;
		}
		for (int i = 0; i < 10; ++i){
			if (morty_meeseeks[i] == 0){
				morty_meeseeks[i] = 1;
				activar_meeseeks(i+10, codigo, x, y);
				map_set(y, x, 3);
				return meeseeks_list[i + 10]->codigo_virtual_p;
				break;
			}
		}
	}
	return 0;
}

uint32_t abs(int a){
	if (a < 0) a = -a;
	return a;
}

uint32_t move(int x, int y){
	uint32_t worked = 0; //devuelve cero si no hubo cambios
	uint16_t current_tr = rtr(); //guardo la tarea que se esta ejecutando en este momento
	if (current_tr == rick) return -1/worked; //para que falle y se desaloje
	if (current_tr == morty) return -1/worked; //para que falle y se desaloje
	for (int i = 0; i < 20; ++i){
		//busco en la lista de meeseeks la info del meeseeks actual
		//como se esta ejecutando, siempre va a encontrarlo y va a ser valido
		if (current_tr == meeseeks_list[i]->task_register){
			int current_x = meeseeks_list[i]->columna;
			int current_y = meeseeks_list[i]->fila;
			if (abs(x) + abs(y) <= meeseeks_list[i]->movimiento) {
				map_set(current_y, current_x, 0); //como se va a mover, ponemos su posicion acutal en vacio
				//calculo las nuevas posiciones
				current_x += x;
				current_y += y;
				//funciones del mapa redondo
				if (current_x >= 80) current_x -= 80;
				if (current_x < 0) current_x += 80;
				if (current_y >= 40) current_y -= 40;
				if (current_y < 0) current_y += 40;
				//ver si cayo en una semilla
				if (map_access(current_y, current_x) == 1){
					//sumo puntos acordemente
					if (meeseeks_list[i]->indicador_jugador == 0) puntos_rick += 450;
					else puntos_morty += 450;
					//apagar el la lista de semillas la semilla que se saco
					apagar_semilla(current_x, current_y);
					//pongo el espacio en vacio
					map_set(current_y, current_x, 0);
					meeseeks_list[i]->columna = current_x;
					meeseeks_list[i]->fila = current_y;
					//desalojamos el meeseeks ya que cumplio su tarea
					desalojar(meeseeks_list[i]->task_register);
					worked = 1; //hubo desplazamiento
				} else{
					//actualizar posicion
					movimiento(i, current_x, current_y);
					meeseeks_list[i]->columna = current_x;
					meeseeks_list[i]->fila = current_y;
					map_set(current_y, current_x, meeseeks_list[i]->indicador_jugador+2);
					//hacer lo del mapeo de la tarea y el copiado del codigo
					worked = 1;
				}
				break;
			}
		}
	}
	return worked;
}

uint32_t distancia_absoluta (int x1, int y1, int x2, int y2){
	return abs(x1 - x2) + abs(y1 - y2);
}

int return_look[2] = {0, 0};

void look(void){
	//agregamos variables incoherentes con tal de que el programa explote si no nos llama un meeseeks
	uint32_t distancia = 150;
	int return_x = -1;
	int return_y = -1;
	uint32_t current_x = 150;
	uint32_t current_y = 150;
	uint16_t current_tr = rtr(); //pedimos la tarea actual
	for (int j = 0; j < 20; ++j){
		if (current_tr == meeseeks_list[j]->task_register){
			current_x = meeseeks_list[j]->columna;
			current_y = meeseeks_list[j]->fila;
			break;
		}
	}
	//comparamos la posicion del meeseeks con las de todas las semillas activas
	for (uint32_t i = 0; i < 40; ++i){
		if (semillas[i].estado == 1){
			int semilla_x = semillas[i].x;
			int semilla_y = semillas[i].y;
			uint32_t distancia_temp = distancia_absoluta(current_x, current_y, semilla_x, semilla_y);
			if (distancia_temp < distancia){
				distancia = distancia_temp;
				return_x = semilla_x - current_x;
				return_y = semilla_y - current_y;
			}
		}
	}
	//devolver en eax y ebx desde C
	return_look[0] = return_x;
	return_look[1] = return_y;
}

void teletransportar (int i){ 
	//un move de una tarea conocida y sin restricciones de movimiento
	// el x y el y los conseguimos aleatoriamente
	int x = Rand_In_Range(0, 79);
	int y = Rand_In_Range(0, 39);
	int current_x = meeseeks_list[i]->columna;
	int current_y = meeseeks_list[i]->fila;
	map_set(current_y, current_x, 0);
	current_x += x;
	current_y += y;
	//funciones del mapa redondo
	if (current_x >= 80) current_x -= 80;
	if (current_x < 0) current_x += 80;
	if (current_y >= 40) current_y -= 40;
	if (current_y < 0) current_y += 40;
	//ver si cayo en una semilla
	if (map_access(current_y, current_x) == 1){
		if (meeseeks_list[i]->indicador_jugador == 0) puntos_rick += 450;
		else puntos_morty += 450;
		//apagar el la lista de semillas la semilla que se saco
		apagar_semilla(current_x, current_y);
		map_set(current_y, current_x, 0);
		meeseeks_list[i]->columna = current_x;
		meeseeks_list[i]->fila = current_y;
		//desalojamos el meeseeks ya que cumplio su tarea
		desalojar(meeseeks_list[i]->task_register);
	} else{
		//actualizar posicion
		uint32_t cr3_viejo = rcr3();
		lcr3(meeseeks_list[i]->cr3);
		//hacer lo del mapeo de la tarea y el copiado del codigo
		movimiento(i, current_x, current_y);
		lcr3(cr3_viejo);
		meeseeks_list[i]->columna = current_x;
		meeseeks_list[i]->fila = current_y;
		map_set(current_y, current_x, meeseeks_list[i]->indicador_jugador+2);
		//falta desmapear?
	}
}

//contamos cuantos meeseeks activos tiene el contrario
int cant_meeseeks_contrarios(int jugador){
	int cant_meeseeks = 0;
	if (jugador == 0){
		for (int i = 0; i < 10; ++i){
			cant_meeseeks += morty_meeseeks[i];
		}
	} else{
		for (int i = 0; i < 10; ++i){
			cant_meeseeks += rick_meeseeks[i];
		}
	}
	return cant_meeseeks;
}

//esta funcion devuelte la posicion en la lista de meeseeks de un meeseeks aleatorio del jugador contrario
int random_meeseeks(int jugador, int cant_meeseeks){
	cant_meeseeks++;
	cant_meeseeks--;
	if (jugador == 0){
		while (1){
			int random = Rand_In_Range(0, 9);
			if (morty_meeseeks[random] == 1){
				return random + 10;
			}
		}
	} else {
		while (1){
			int random = Rand_In_Range(0, 9);
			if (rick_meeseeks[random] == 1){
				return random;
			}
		}
	}
	return -1; // nunca llega
}

void portal(){
	uint16_t current_tr = rtr();
	int i = 0;
	for (int j = 0; j < 20; ++j){
		if (current_tr == meeseeks_list[j]->task_register){
			i = j;
			break;
		}
	}
	if (meeseeks_list[i]->portal == 1){
		meeseeks_list[i]->portal = 0;
		int cant_contrarios = cant_meeseeks_contrarios(meeseeks_list[i]->indicador_jugador);
		if (cant_contrarios > 0){
			int indicador_meeseeks = random_meeseeks(meeseeks_list[i]->indicador_jugador, cant_contrarios); //conseguir uno random de esos
			teletransportar(indicador_meeseeks);
		}
	}
}

void analizar_ganador(void){
	uint32_t cant_semillas_total = 40;
	uint32_t cant_semillas_actual = 0;
	for (uint32_t i = 0; i < cant_semillas_total; ++i){
		if (semillas[i].estado == 1){
			cant_semillas_actual++;
		}
	}
	if (cant_semillas_actual == 0){
		if (puntos_rick > puntos_morty){
			print("Finalizo el juego, gano Rick               ", 0, 0, C_FG_WHITE | C_BG_BLACK);
		}else if(puntos_rick < puntos_morty){
			print("Finalizo el juego, gano Morty               ", 0, 0, C_FG_WHITE | C_BG_BLACK);
		}else{
			print("Finalizo el juego, empataron               ", 0, 0, C_FG_WHITE | C_BG_BLACK);
		}
		end_game();
	}
}

uint16_t aumentar_turnos_vivos(uint16_t tr){
	for (int j = 0; j < 20; ++j){
		if (tr == meeseeks_list[j]->task_register){
			meeseeks_list[j]->turnos_vivo++;
			if (meeseeks_list[j]->turnos_vivo > 1 && (meeseeks_list[j]->turnos_vivo % 2) == 1 && meeseeks_list[j]->movimiento > 1){
				meeseeks_list[j]->movimiento--;
			}
			break;
		}
	}
	return tr;
}

void avanzar_clock(uint32_t jugador, uint32_t meeseek){
	if (jugador == 0){
		if (meeseek == 0){
			relojrick0();
		}else if (meeseek == 1){
			relojrick1();
		}else if (meeseek == 2){
			relojrick2();
		}else if (meeseek == 3){
			relojrick3();
		}else if (meeseek == 4){
			relojrick4();
		}else if (meeseek == 5){
			relojrick5();
		}else if (meeseek == 6){
			relojrick6();
		}else if (meeseek == 7){
			relojrick7();
		}else if (meeseek == 8){
			relojrick8();
		}else if (meeseek == 9){
			relojrick9();
		}
	} else{
		if (meeseek == 0){
			relojmorty0();
		}else if (meeseek == 1){
			relojmorty1();
		}else if (meeseek == 2){
			relojmorty2();
		}else if (meeseek == 3){
			relojmorty3();
		}else if (meeseek == 4){
			relojmorty4();
		}else if (meeseek == 5){
			relojmorty5();
		}else if (meeseek == 6){
			relojmorty6();
		}else if (meeseek == 7){
			relojmorty7();
		}else if (meeseek == 8){
			relojmorty8();
		}else if (meeseek == 9){
			relojmorty9();
		}
	}
}

uint16_t tasks_clocks(uint16_t tr){
	for (int j = 0; j < 20; ++j){
		if (tr == meeseeks_list[j]->task_register){
			avanzar_clock(meeseeks_list[j]->indicador_jugador, meeseeks_list[j]->indicador_meeseeks);
		}
	}
	if (tr == 0x008b){
		relojrick();
	}
	if (tr == 0x0093){
		relojmorty();
	}
	return tr;
}

void limpiar_en_mapa(uint16_t tr){
	for (int j = 0; j < 20; ++j){
		if (tr == meeseeks_list[j]->task_register){
			map_set(meeseeks_list[j]->fila, meeseeks_list[j]->columna, 0);
			break;
		}
	}
}

void game_init(void) {
	inicializar_meeseeks();
	inicializar_esp0();
}