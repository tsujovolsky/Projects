/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "defines.h"
#include "kassert.h"
#include "mmu.h"

//tss_t tss_list[24]; //24 entradas, initial, idle, rick, morty y meeseecks de cada uno

tss_t tss_initial = {0};


//b) Completar la entrada de la TSS de la tarea Idle con la informacion de la tarea Idle. Esta
//informacion se encuentra en el archivo tss.c. La tarea Idle se encuentra en la direccion
//0x00018000. La pila se alojara en la misma direccion que la pila del kernel y sera mapeada con
//identity mapping. Esta tarea ocupa 1 pagina de 4KB y debe ser mapeada con identity mapping.
//Ademas, la misma debe compartir el mismo CR3 que el kernel.


tss_t tss_idle = {
    .ptl = 0, //Apunta a esta misma tarea
    .esp0 = 0,
    .ss0 = 0,
    .esp1 = 0,
    .ss1 = 0,
    .esp2 = 0,
    .ss2 = 0,
    .cr3 = 0x25000, //Debe ser pasado por parametro (igual que el kernel)
    .eip = 0x00018000, //A donde saltar al ejecutarse
    .eflags = 0x202, //Por default con el Flag de Interrupciones
    .eax = 0,
    .ecx = 0,
    .edx = 0,
    .ebx = 0,
    .esp = 0x25000,//STACK POINTER Igual que la pila del kernel
    .ebp = 0x25000,
    .esi = 0,
    .edi = 0,
    .es = 12 << 3,
    .cs = 10 << 3, //Para el codigo
    .ss = 12 << 3, //Para el stack
    .ds = 12 << 3,
    .fs = 12 << 3,
    .gs = 12 << 3,
    .ldt = 0,
    .dtrap = 0,
    .iomap = 0xFFFF,
};

tss_t tss_rick = {0};
tss_t tss_morty = {0};
tss_t tss_rm0 = {0};
tss_t tss_rm1 = {0};
tss_t tss_rm2 = {0};
tss_t tss_rm3 = {0};
tss_t tss_rm4 = {0};
tss_t tss_rm5 = {0};
tss_t tss_rm6 = {0};
tss_t tss_rm7 = {0};
tss_t tss_rm8 = {0};
tss_t tss_rm9 = {0};
tss_t tss_mm0 = {0};
tss_t tss_mm1 = {0};
tss_t tss_mm2 = {0};
tss_t tss_mm3 = {0};
tss_t tss_mm4 = {0};
tss_t tss_mm5 = {0};
tss_t tss_mm6 = {0};
tss_t tss_mm7 = {0};
tss_t tss_mm8 = {0};
tss_t tss_mm9 = {0};


tss_t* tss_list[24] = {
    [0] = &tss_initial,
    [1] = &tss_idle,
    [2] = &tss_rick,
    [3] = &tss_morty,
    [4] = &tss_rm0,
    [5] = &tss_rm1,
    [6] = &tss_rm2,
    [7] = &tss_rm3,
    [8] = &tss_rm4,
    [9] = &tss_rm5,
    [10] = &tss_rm6,
    [11] = &tss_rm7,
    [12] = &tss_rm8,
    [13] = &tss_rm9,
    [14] = &tss_mm0,
    [15] = &tss_mm1,
    [16] = &tss_mm2,
    [17] = &tss_mm3,
    [18] = &tss_mm4,
    [19] = &tss_mm5,
    [20] = &tss_mm6,
    [21] = &tss_mm7,
    [22] = &tss_mm8,
    [23] = &tss_mm9,

};

void gdt_tss_init(uint32_t task_id){
    gdt[task_id+15].base_15_0 = (uint16_t) ((uint32_t)(tss_list[task_id])); 
    gdt[task_id+15].base_23_16 = (uint8_t) (((uint32_t)(tss_list[task_id])) >> 16);
    gdt[task_id+15].base_31_24 = (uint8_t) (((uint32_t)(tss_list[task_id])) >> 24);
}


void tss_init(uint32_t task_id, uint32_t cr3, uint32_t eip, uint32_t inicio_stack) {

    uint32_t esp0 = mmu_next_free_kernel_page();

    tss_list[task_id]->esp0 = esp0 + 0x1000;
    tss_list[task_id]->ss0 = ((uint16_t)(12<<3));
    tss_list[task_id]->cr3 = cr3; //Debe ser pasado por parametro (igual que el kernel)
    tss_list[task_id]->eip = eip; //A donde saltar al ejecutarse
    tss_list[task_id]->eflags = 0x202; //Por default sin el Flag de Interrupciones
    tss_list[task_id]->esp = inicio_stack; //STACK POINTER Igual que la pila del kernel
    tss_list[task_id]->ebp = inicio_stack;
    tss_list[task_id]->es = ((uint16_t)(13 << 3)) + 0x3;
    tss_list[task_id]->cs = ((uint16_t)(11 << 3)) + 0x3; //Para el codigo
    tss_list[task_id]->ss = ((uint16_t)(13 << 3)) + 0x3; //Para el stack
    tss_list[task_id]->ds = ((uint16_t)(13 << 3)) + 0x3;
    tss_list[task_id]->fs = ((uint16_t)(13 << 3)) + 0x3;
    tss_list[task_id]->gs = ((uint16_t)(13 << 3)) + 0x3;
    tss_list[task_id]->iomap = 0xFFFF;

    gdt_tss_init(task_id);

}

paddr_t esp_rick_meeseeks[10];
paddr_t esp_morty_meeseeks[10];


void inicializar_meeseeks(void){
    for (int i = 0; i < 10; ++i){
        uint32_t task_id = i + 4;
        tss_init(task_id, 0x00100000, 0, 0);
        esp_rick_meeseeks[i] = tss_list[task_id]->esp0;
    }
    for (int i = 0; i < 10; ++i){
        uint32_t task_id = i + 14;
        tss_init(task_id, 0x00105000, 0, 0);
        esp_morty_meeseeks[i] = tss_list[task_id]->esp0;
    }
}

