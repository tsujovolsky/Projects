
#include <string.h>
#include <stdio.h>
#include <stdint.h> //MOD
#include <math.h> //MOD
#include <stdlib.h>
#include <libgen.h>
#include <time.h>  //mod

#include "tp2.h"
#include "helper/tiempo.h"
#include "helper/libbmp.h"
#include "helper/utils.h"
#include "helper/imagenes.h"

// ~~~ seteo de los filtros ~~~

extern filtro_t ColorBordes;
extern filtro_t ImagenFantasma;
extern filtro_t PixeladoDiferencial;
extern filtro_t ReforzarBrillo;

filtro_t filtros[4];

// ~~~ fin de seteo de filtros ~~~

// MOD implementacion de listas y funciones de estadistica
typedef struct s_listElem {
  double data;
  struct s_listElem* next;
  struct s_listElem* prev;
} listElem_t;

typedef struct s_list {
  uint32_t size;
  listElem_t* first;
  listElem_t* last;
} list_t;

list_t* listNew() {
  list_t* l = malloc(sizeof(list_t));
  l->first = 0;
  l->last = 0;
  l->size = 0;
  return l;
}

void listAdd(list_t* l, double data) {
  listElem_t* n = malloc(sizeof(listElem_t));
  l->size = l->size + 1;
  n->data = data;
  listElem_t* prev = 0;
  listElem_t* current = l->first;
  listElem_t** pcurrent = &(l->first);
  while (current != 0 && current->data < data) {
    pcurrent = &((*pcurrent)->next);
    prev = current;
    current = *pcurrent;
  }
  n->next = current;
  n->prev = prev;
  *pcurrent = n;
  if (current == 0)
    l->last = n;
  if (current != 0)
    current->prev = n;
}

void listDelete(list_t* l) {
  listElem_t* current = l->first;
  while (current != 0) {
    listElem_t* tmp = current;
    current = current->next;
    free(tmp);
  }
  free(l);
}

void listPrint(list_t* l, FILE* pFile) {
  fprintf(pFile, "[");
  listElem_t* current = l->first;
  if (current == 0) {
    fprintf(pFile, "]");
    return;
  }
  while (current != 0) {
    fprintf(pFile,"%f", current->data);
    current = current->next;
    if (current == 0)
      fprintf(pFile, "]");
    else
      fprintf(pFile, ",");
  }
}
//funciones de filtros
void imprimir_tiempos_ejecucion2(clock_t start, clock_t end, int cant_iteraciones, list_t* l) {
    double segs = (double)(end-start) / CLOCKS_PER_SEC;
    listAdd(l, segs);
}

void correr_filtro_imagen2(configuracion_t *config, aplicador_fn_t aplicador, list_t* l) {
    imagenes_abrir(config);

    clock_t start, end;

    imagenes_flipVertical(&config->src, src_img);
    imagenes_flipVertical(&config->dst, dst_img);
    if(config->archivo_entrada_2 != 0) {
        imagenes_flipVertical(&config->src_2, src_img2);
    }
    start = clock();
    aplicador(config);
    end = clock();
    imagenes_flipVertical(&config->dst, dst_img);

    imagenes_guardar(config);
    imagenes_liberar(config);
    imprimir_tiempos_ejecucion2(start, end, config->cant_iteraciones, l);
}
// funciones de estadistica

double promedio(list_t* l){
    double sum = 0;
    double total = (double)(l->size) * 0.95;
    listElem_t* current = l->first;
    for (int i = 0; i < (int)(total); i++) {
        sum += current->data;
        current = current->next;
    }
    return sum/total;
}

double desvio_estandar (list_t* l){
    double sum = 0;
    double total = (double)(l->size) * 0.95;
    double prom = promedio(l);
    listElem_t* current = l->first;
    for (int i = 0; i < (int)(total); i++) {
        sum += pow((current->data - prom), 2);
        current = current->next;
    }
    return sqrt(sum/total);
}
//

int main( int argc, char** argv ) {

    filtros[0] = ColorBordes; 
    filtros[1] = ImagenFantasma;
    filtros[2] = PixeladoDiferencial;
    filtros[3] = ReforzarBrillo;

    configuracion_t config;
    config.dst.width = 0;
    config.bits_src = 32;
    config.bits_dst = 32;

    procesar_opciones(argc, argv, &config);
    
    // Imprimo info
    if (!config.nombre) {
        printf ( "Procesando...\n");
        printf ( "  Filtro             : %s\n", config.nombre_filtro);
        printf ( "  Implementación     : %s\n", C_ASM( (&config) ) );
        printf ( "  Archivo de entrada : %s\n", config.archivo_entrada);
    }

    snprintf(config.archivo_salida, sizeof  (config.archivo_salida), "%s/%s.%s.%s%s.bmp",
            config.carpeta_salida, basename(config.archivo_entrada),
            config.nombre_filtro,  C_ASM( (&config) ), config.extra_archivo_salida );

    if (config.nombre) {
        printf("%s\n", basename(config.archivo_salida));
        return 0;
    }

    filtro_t *filtro = detectar_filtro(&config);

    //lista de datos para experimentos
    list_t* datos = listNew();
    //

    filtro->leer_params(&config, argc, argv);
    for (int i = 0; i < 10000; ++i){ //tomo el 0.9 de los datos para eliminar outliers
    correr_filtro_imagen2(&config, filtro->aplicador, datos);
    if (i%100 == 0)
    {
        printf("%d\n", i);
    }
    }
    filtro->liberar(&config);

    FILE *pfile = fopen("datos.txt","w");
    listPrint(datos, pfile);
    fprintf(pfile, "\n");
    fprintf(pfile, "Promedio es:%f\n", promedio(datos));
    fprintf(pfile, "Desvio estandar es:%f\n", desvio_estandar(datos));
    fclose(pfile);
    listDelete(datos);

    return 0;
}

filtro_t* detectar_filtro(configuracion_t *config) {
    for (int i = 0; filtros[i].nombre != 0; i++) {
        if (strcmp(config->nombre_filtro, filtros[i].nombre) == 0)
            return &filtros[i];
    }
    fprintf(stderr, "Filtro '%s' desconocido\n", config->nombre_filtro);
    exit(EXIT_FAILURE);
    return NULL;
}

void imprimir_tiempos_ejecucion(unsigned long long int start, unsigned long long int end, int cant_iteraciones) {
    unsigned long long int cant_ciclos = end-start;

    printf("Tiempo de ejecución:\n");
    printf("  Comienzo                          : %llu\n", start);
    printf("  Fin                               : %llu\n", end);
    printf("  # iteraciones                     : %d\n", cant_iteraciones);
    printf("  # de ciclos insumidos totales     : %llu\n", cant_ciclos);
    printf("  # de ciclos insumidos por llamada : %.3f\n", (float)cant_ciclos/(float)cant_iteraciones);
}

void correr_filtro_imagen(configuracion_t *config, aplicador_fn_t aplicador) {
    imagenes_abrir(config);

    clock_t start, end;

    imagenes_flipVertical(&config->src, src_img);
    imagenes_flipVertical(&config->dst, dst_img);
    if(config->archivo_entrada_2 != 0) {
        imagenes_flipVertical(&config->src_2, src_img2);
    }
    start = clock();
    aplicador(config);
    end = clock();
    imagenes_flipVertical(&config->dst, dst_img);

    imagenes_guardar(config);
    imagenes_liberar(config);
    imprimir_tiempos_ejecucion(start, end, config->cant_iteraciones);
}
