/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de la tabla de descriptores globales
*/

#include "gdt.h"

// Definimos los segmentos, con los indices desde el 10
#define GDT_IDX_CS_N0 10
#define GDT_IDX_CS_N3 11
#define GDT_IDX_DS_N0 12
#define GDT_IDX_DS_N3 13
#define GDT_IDX_VIDEO 14
#define TSS_TAREA_INICIAL 15
#define TSS_TAREA_IDLE 16
#define TSS_TAREA_RICK 17
#define TSS_TAREA_MORTY 18
#define TSS_R_M0 19
#define TSS_R_M1 20
#define TSS_R_M2 21
#define TSS_R_M3 22
#define TSS_R_M4 23
#define TSS_R_M5 24
#define TSS_R_M6 25
#define TSS_R_M7 26
#define TSS_R_M8 27
#define TSS_R_M9 28
#define TSS_M_M0 29
#define TSS_M_M1 30
#define TSS_M_M2 31
#define TSS_M_M3 32
#define TSS_M_M4 33
#define TSS_M_M5 34
#define TSS_M_M6 35
#define TSS_M_M7 36
#define TSS_M_M8 37
#define TSS_M_M9 38

  /* Completar la Tabla de Descriptores Globales (GDT) con 4 segmentos, 
     dos para codigo de nivel 0 y 3; y otros dos para datos de nivel 0 y 3. 
     Estos segmentos deben direccionar los primeros 201MB de memoria. En la gdt,
     por restriccion del trabajo practico, las primeras 10 posiciones se 
     consideran utilizadas y por ende no deben utilizarlas. El primer indice 
     que deben usar para declarar los segmentos, es el 10 (contando desde cero). */

gdt_entry_t gdt[GDT_COUNT] = {
    /* Descriptor nulo*/
    /* Offset = 0x00 */
    [GDT_IDX_NULL_DESC] =
        {
            .limit_15_0 = 0x0000,
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .type = 0x0,
            .s = 0x00,
            .dpl = 0x00,
            .p = 0x00,
            .limit_19_16 = 0x00,
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0,
            .g = 0x00,
            .base_31_24 = 0x00,
        },
    [GDT_IDX_CS_N0] =
        {
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .base_31_24 = 0x00,
            .limit_15_0 = 0xC8FF, // son 51456 - 1 paginas de 4KiB
            .limit_19_16 = 0x00,
            .type = 0xA, // Type: Code, Execute/Read, No Conforming 
            .s = 0x01,
            .dpl = 0x00, // NIVEL
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x1, //Voy a estar en modo protegido
            .g = 0x01,
        },
    [GDT_IDX_CS_N3] =
        {
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .base_31_24 = 0x00,
            .limit_15_0 = 0xC8FF, // LIMITE ES TAM - 1
            .limit_19_16 = 0x00,
            .type = 0xA, // Type: Code, Execute/Read,No Conforming 
            .s = 0x01,
            .dpl = 0x03, // NIVEL
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x1, //Voy a estar en modo protegido
            .g = 0x01,
        },
    [GDT_IDX_DS_N0] =
        {
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .base_31_24 = 0x00,
            .limit_15_0 = 0xC8FF, // LIMITE ES TAM - 1
            .limit_19_16 = 0x00,
            .type = 0x2, // Type: Data, Read/Write
            .s = 0x01,
            .dpl = 0x00, // NIVEL
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x1, //Voy a estar en modo protegido
            .g = 0x01,
        },
    [GDT_IDX_DS_N3] =
        {
            .base_15_0 = 0x0000,
            .base_23_16 = 0x00,
            .base_31_24 = 0x00,
            .limit_15_0 = 0xC8FF, // LIMITE ES TAM - 1
            .limit_19_16 = 0x00,
            .type = 0x2, // Type: Data, Read/Write
            .s = 0x01,
            .dpl = 0x03, // NIVEL
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x1, //Voy a estar en modo protegido
            .g = 0x01,
        },
    [GDT_IDX_VIDEO] =
        {
            .base_15_0 = 0x8000, //SETEAR BASE
            .base_23_16 = 0x0B,
            .base_31_24 = 0x00,
            .limit_15_0 = 0x1F3F, // 0x1F40 = 8000 decimal, como el g=0 esto son 80x50x2 Bytes
            .limit_19_16 = 0x00,
            .type = 0x2, // Type: Data, Read/Write
            .s = 0x01,
            .dpl = 0x00, // NIVEL
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x1, //Voy a estar en modo protegido
            .g = 0x00,
        },

        
    [TSS_TAREA_INICIAL] =
        {
            .base_15_0 = 0,
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x00, // NIVEL 0 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

        
    [TSS_TAREA_IDLE] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x00, // NIVEL 0 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

     [TSS_TAREA_RICK] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },
        
    [TSS_TAREA_MORTY] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_R_M0] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_R_M1] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_R_M2] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_R_M3] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_R_M4] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_R_M5] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_R_M6] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_R_M7] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_R_M8] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_R_M9] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_M_M0] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_M_M1] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_M_M2] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_M_M3] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_M_M4] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_M_M5] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_M_M6] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_M_M7] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_M_M8] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },

    [TSS_M_M9] =
        {
            .base_15_0 = 0, 
            .base_23_16 = 0,
            .base_31_24 = 0,
            .limit_15_0 = 0x0067, // LIMITE MINIMO
            .limit_19_16 = 0x00,
            .type = 0x9, // Busy = 0 => De base no está en ejecución
            .s = 0x00, // ES DE SISTEMA
            .dpl = 0x3, // NIVEL 3 :no modificable por el usuario
            .p = 0x01,  // Supongo que esta en la RAM
            .avl = 0x0,
            .l = 0x0,
            .db = 0x0, //Debe ser 0 segun el tipo de descriptor
            .g = 0x00,
        },


};

gdt_descriptor_t GDT_DESC = {sizeof(gdt) - 1, (uint32_t)&gdt};
