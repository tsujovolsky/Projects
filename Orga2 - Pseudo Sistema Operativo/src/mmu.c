/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
#include "kassert.h"

//Escribir las rutinas encargadas de inicializar el directorio y tablas de paginas para el kernel
//(mmu_init_kernel_dir). Se debe generar un directorio de paginas que mapee, usando identity
//mapping, las direcciones 0x00000000 a 0x003FFFFF, como ilustra la figura 3. Ademas, esta
//función debe inicializar el directorio de paginas en la dirección 0x25000 y las tablas de paginas
//segun muestra la figura 2.

unsigned int prox_pag_libre;


void mmu_init() {
  prox_pag_libre = 0x100000; // Donde arranca el area libre del kernel
}


paddr_t mmu_next_free_kernel_page() {
  unsigned int pagina_libre = prox_pag_libre;
  prox_pag_libre += 1024*4; // 4 kbytes 
  return pagina_libre;
}

paddr_t mmu_init_kernel_dir(void) {
  page_directory_entry *pd = (page_directory_entry *) 0x25000;
  page_table_entry *pt_0 =  (page_table_entry *) 0x26000; //Segun muestra la fig2
  
  for (int i = 0; i < 1024; i++) {
      pd[i] = (page_directory_entry){0};
      pt_0[i] = (page_table_entry){0};
  }
  
  pd[0].p = 1;
  pd[0].r_w = 1;
  pd[0].u_s = 0;
  pd[0].page_table_base =  ((uint32_t)pt_0)>>12; //Tomo solo la dirección base de la tabla de paginas
  
  for(int i = 0; i < 1024; i++){
      pt_0[i].p = 1;
      pt_0[i].r_w = 1;
      pt_0[i].u_s = 0;
      pt_0[i].physical_address_base = i;
  }
  return (uint32_t) pd;
}

void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint32_t attrs) {
    uint16_t directoryIdx = virt >> 22; // Me quedo con el directorio de la page.
    uint16_t tableIdx = (virt >> 12) & 0x3FF; // Me quedo con los bits de la tabla

    uint32_t pde = (cr3 & ~0xFFF); //En PDE tengo la base de la Directory Page Table
    pde += directoryIdx*4; // Acá nuesto pde tiene la posicion de memoria que queremos

    page_directory_entry *p = (page_directory_entry *)pde;

    if(p->p != 1) {
      
        //newPt es un puntero a una página
        paddr_t newPt = mmu_next_free_kernel_page(); // Pido una nueva pagina

        uint32_t *pt_data = (uint32_t *) newPt;

        for (int i = 0; i < 1024; i++) {
        	pt_data[i]=0x00;
        }

        // Esto queda a discrecion nuestra como interpretarlo. (Es decir, que parametros se pasan. La fucnion es nuestra).
        p->p = 1;
        if(attrs == 2 || attrs == 3 || attrs == 6 || attrs == 7){// 0010 || 0011 || 0110 || 0111
			p->r_w = 1;
        } else { p->r_w = 0;}
        if(attrs == 4 || attrs == 5 || attrs == 6 || attrs == 7){// 0100 || 0101 || 0110 || 0111
			p->u_s = 1;
        } else { p->u_s = 0;}

        p->page_table_base = (newPt >> 12);        
    }
    uint32_t PT = (p->page_table_base << 12);
    PT += tableIdx*4; // Acá PT tiene la pos de memoria que queremos en nuestra tabla de pags.

    page_table_entry *PT_p = (page_table_entry *) PT;
    PT_p->physical_address_base = (phy >> 12);
    
    PT_p->p = 1;
    if(attrs == 2 || attrs == 3 || attrs == 6 || attrs == 7){// 0010 || 0011 || 0110 || 0111
		 PT_p->r_w = 1;
    } else { PT_p->r_w = 0;}
    if(attrs == 4 || attrs == 5 || attrs == 6 || attrs == 7){// 0100 || 0101 || 0110 || 0111
		 PT_p->u_s = 1;
    } else { PT_p->u_s = 0;}

    tlbflush();
}

void mmu_unmap_page(uint32_t cr3, vaddr_t virt) {

    uint16_t directoryIdx = virt >> 22; // Me quedo con el directorio de la page.
    uint16_t tableIdx = (virt >> 12) & 0x3FF; // Me quedo con los bits de la tabla

    uint32_t pde = (cr3 & ~0xFFF); //En PDE tengo la base de la Directory Page Table
    pde += directoryIdx*4; // Acá nuesto pde tiene la posicion de memoria que queremos

    page_directory_entry *p = (page_directory_entry *)pde;

    if(p->p != 0){
        uint32_t PT = (p->page_table_base << 12);
        PT += tableIdx*4; // Acá PT tiene la pos de memoria que queremos en nuestra tabla de pags.
        page_table_entry *PT_p = (page_table_entry *) PT;

        PT_p->physical_address_base = 0;
        PT_p->p = 0;
	}
    tlbflush();
}

paddr_t mmu_init_task_dir(uint32_t cr3, paddr_t src_kernel, vaddr_t dst_virtual, paddr_t dst_phy, size_t pages){
    // 1) SOLICITO PAGINAS LIBRES PARA PD Y PT
    paddr_t pd_new = mmu_next_free_kernel_page();
    uint32_t *pd_new_uint = (uint32_t *) pd_new;
    page_directory_entry *pd_cast = (page_directory_entry *)pd_new_uint;
    
    for (int i = 0; i < 1024; i++) {
    	pd_new_uint[i]=0x00;
    }

    // Estoy en la entrada de indice 0 de la tabla directorio de paginas
    pd_cast[0].p = 0;
    pd_cast[0].r_w = 1;
    pd_cast[0].u_s = 1; //user

    // 2) MAPEAR CON ID MAPPING LOS 4 PRIMEROS MB
    for (uint32_t i = 0; i < 1024 ; i++){
        mmu_map_page(pd_new, i << 12, i << 12, 0); //6 o 0?
        // Tomo el cr3 que depende del mapa de la tarea actual
        // La direccion vitual es aquella con directorio 0, indice de tabla i esimo y offset 0 
        // La direccion fisica es la i esima página (i << 12)
        // Los atributos son 1 porque se permite escritura, pero es nivel user
    }
    
    // 3) IDENTIFICAR CODIGO DE TAREA QUE DEBE SER COPIADO DESDE EL KERNEL (SRC)
    // Para eso esta la entrada src_kernel
    
    // 4) IDENTIFICAR POSICION DE MEMORIA A DONDE COPIAR (DST)
    // Para eso esta la entrada dst_virtual
    
    // 5) MAPEAR DE SER NECESARIO LA POSICION DST O SRC DEL CODIGO
    if (dst_virtual > 0x3FFFFF ) { // 4096*1024 // Si está mas alla de los 4MB debe mapearse
        vaddr_t lineal = dst_virtual;
        paddr_t fisica =  dst_phy;
        size_t contador = pages;
        while (contador != 0) {
            mmu_map_page(cr3, lineal, fisica, 6);
            contador--;
            lineal += 0x1000;
            fisica += 0x1000;
        }
    }
    
    // 6) COPIAR LAS PAGINAS DE LA TAREA
    int total_paginas = pages * 1024;
    uint32_t *dst_virtual_p = (uint32_t *)dst_virtual;
    uint32_t *src_kernel_p = (uint32_t *)src_kernel;
    for (int i = 0; i < total_paginas; i++) {
        dst_virtual_p[i] = src_kernel_p[i];
    }
    
    // 7) MAPEAR LA TAREA COPIADA (DST) AL NUEVO ESQUEMA QUE SE ESTA CONSTRUYENDO
    uint32_t *dst_phy_p = (uint32_t *)dst_phy;
    for (uint32_t i = 0; i < pages; i++){
        // Estoy en la entrada de indice i de la tabla directorio de paginas
        uint32_t phy = i * ((uint32_t) 1024);
        mmu_map_page(pd_new, (uint32_t)(dst_virtual_p + phy), (uint32_t)(dst_phy_p + phy), 6);
    }
    
    // 8) DESMAPEAR, DE SER NECESARIO, LAS PAGS MAPEADAS PARA PODER COPIAR
    if (dst_virtual > 0x3FFFFF) {
        // Si está mas alla de los 4MB debe desmapearse
        vaddr_t lineal = dst_virtual;
        size_t contador = pages;
        while (contador != 0) {
            mmu_unmap_page(cr3, lineal);
            contador--;
            lineal += 0x1000;
        }
    }
    return pd_new;
}