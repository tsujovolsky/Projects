/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del scheduler
*/

#include "screen.h"
#include "mmu.h" // Agregado
#include "i386.h" // Agregado
#include "gdt.h" // Agregado
#include "tss.h" // Agregado
#include "sched.h" // Agregado
#include "game.h"
#include "prng.h"

//Flags
uint8_t indicador_debugger = 0; // Arrancamos con el debugger apagado
uint8_t indicador_debugger_on_screen = 0;
uint8_t indicador_mem_video_map = 0;
uint8_t map_update_flag = 1;
uint32_t mem_video_map[8];

//Variables globales
char mnemonic[21][4] = {
                         "DE", // 0
                         "DB",
                         "-",
                         "BP",
                         "OF",
                         "BR",
                         "UD",
                         "NM",
                         "DF",
                         "-",
                         "TS",
                         "NP",
                         "SS",
                         "GP", // 13
                         "PF", // 14
                         "-",
                         "MF",
                         "AC",
                         "MC",
                         "XM",
                         "VE" //20
                     };

char exc_name[21][13] = {
                         "Div by 0", // 0
                         "Debug",
                         "Non-mask int",
                         "Breakpoint",
                         "Overflow",
                         "B-Range Exc",
                         "Inv Opcode",
                         "DNA",
                         "Double Fault",
                         "-",
                         "Inv TSS",
                         "Seg not p",
                         "SS Fault",
                         "GP Fault", // 13
                         "Page Fault", // 14
                         "-",
                         "FP Exc",
                         "Alig Check",
                         "Mach Check",
                         "SIMD FPE",
                         "Virt Exc" //20
                     };
                     

void print(const char* text, uint32_t x, uint32_t y, uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  int32_t i;
  for (i = 0; text[i] != 0; i++) {
    p[y][x].c = (uint8_t)text[i];
    p[y][x].a = (uint8_t)attr;
    x++;
    if (x == VIDEO_COLS) {
      x = 0;
      y++;
    }
  }
}

void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  uint32_t i;
  uint8_t letras[16] = "0123456789";

  for (i = 0; i < size; i++) {
    uint32_t resto = numero % 10;
    numero = numero / 10;
    p[y][x + size - i - 1].c = letras[resto];
    p[y][x + size - i - 1].a = attr;
  }
}

void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  int32_t i;
  uint8_t hexa[8];
  uint8_t letras[16] = "0123456789ABCDEF";
  hexa[0] = letras[(numero & 0x0000000F) >> 0];
  hexa[1] = letras[(numero & 0x000000F0) >> 4];
  hexa[2] = letras[(numero & 0x00000F00) >> 8];
  hexa[3] = letras[(numero & 0x0000F000) >> 12];
  hexa[4] = letras[(numero & 0x000F0000) >> 16];
  hexa[5] = letras[(numero & 0x00F00000) >> 20];
  hexa[6] = letras[(numero & 0x0F000000) >> 24];
  hexa[7] = letras[(numero & 0xF0000000) >> 28];
  for (i = 0; i < size; i++) {
    p[y][x + size - i - 1].c = hexa[i];
    p[y][x + size - i - 1].a = attr;
  }
}

void screen_draw_box(uint32_t fInit, uint32_t cInit, uint32_t fSize, uint32_t cSize, uint8_t character, uint8_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
  uint32_t f;
  uint32_t c;
  for (f = fInit; f < fInit + fSize; f++) {
    for (c = cInit; c < cInit + cSize; c++) {
      p[f][c].c = character;
      p[f][c].a = attr;
    }
  }
}


// AUXILIAR PARA LA RUTINA DE ATENCION DE INTERRUPCIONES DEL TECLADO
void printScanCode(uint8_t scancode) { // POR QUÉ DE 8 si la función toma uno de 32 bits
    if (!(scancode & 0x80) && scancode < 0x0C && 0x01 < scancode) {
        int res = 0;
        if (scancode != 0x0B){
            res = scancode - 0x01;
        }
        print_hex(res, 2, 78, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY);
    }else if(scancode == 0x95){ // La Y tiene scancode 95
        if(indicador_debugger == 0){
          // Esta apagado, lo quiero prender
          indicador_debugger = 1;
          print("Modo Debug-On", 78-14, 0, C_FG_WHITE | C_BG_BLACK);
        } else { 
          // Esta prendido, lo quiero apagar
          indicador_debugger = 0;
          // Dejo de mostrar la pantalla
          indicador_debugger_on_screen = 0;

          print_hex(0, 2+13, 78-14, 0, C_FG_BLACK | C_BG_BLACK);  
        }
    }
}
 
uint32_t Rand_In_Range ( int min, int max ){ // max-min must be <10000!! 
  return (rand() % (max+1-min) + min) ;  
}

void set_Mega_Seeds(){
  //srand(12345);
  // 40 Megasemillas hay
  //uint8_t* img = 0xB8000;
  for (int i = 0; i < 40; i++){
  semillas[i].x = 0;
  semillas[i].y = 0;
  semillas[i].estado = 0;
  }
  for (int i = 0; i < 40; ++i){
    while(semillas[i].estado == 0){
      int fila = Rand_In_Range(0,39);
      int colum = Rand_In_Range(0,79);
      if(map[colum + 80*fila] == 0){ // Agregado del colo
        semillas[i].x = colum;
        semillas[i].y = fila;
        semillas[i].estado = 1;
        map_set(fila, colum, 1);
      }
    }
  }
}


void map_update(){
  if (indicador_debugger_on_screen == 0){
    for (uint32_t i = 0; i < 40; ++i){
      for (uint32_t j = 0; j < 80; ++j){
        uint32_t object = map_access(i, j);
        if (object == 1){
          print("S", j, i + 1, C_FG_LIGHT_BROWN | C_BG_GREEN); 
        }else if (object == 2){
          print("M", j, i + 1, C_FG_RED | C_BG_GREEN);
        }else if (object == 3){
          print("M", j, i + 1, C_FG_BLUE| C_BG_GREEN);
        }else {
          print(".", j, i + 1, C_FG_GREEN | C_BG_GREEN);
        }
      }
    }
  }
}

void points_update(){
  print_dec(puntos_rick, 7, 6, 45, C_FG_WHITE | C_BG_RED);
  print_dec(puntos_morty, 7, 67, 45, C_FG_WHITE | C_BG_BLUE);
}

void Meeseek_status_update(){ 
  for (uint32_t i = 0; i < 10; ++i){
    if (rick_meeseeks[i] == 0){
      print("X", (21+i*4), 43, C_FG_WHITE | C_BG_BLACK);
    }

    if (morty_meeseeks[i] == 0){
      print("X", (21+i*4), 46, C_FG_WHITE | C_BG_BLACK);
    }

  }

}

uint32_t status_debugger(){
  return (( uint32_t ) indicador_debugger);
}

uint32_t status_debugger_on_screen(){
  return (( uint32_t ) indicador_debugger_on_screen);
}


void imprimoModoDebug(uint16_t gs, uint16_t fs, uint16_t ss, uint16_t es, uint16_t ds,
                      uint16_t cs, int n_excep, param_pusheados param){
    if(indicador_debugger == 1){
        //La pantalla mostrara el debug
        indicador_debugger_on_screen = 1;

        //Imprimimos por pantalla lo nec.
        //Imprimimos el bloque negro
        screen_draw_box(1, 20, 40, 40, 0x00, 0x00);

        // Imprimimos la tarea actual
        uint16_t tarea_actual = rtr();
        if (tarea_actual == 15 << 3){ // es el initial
            tarea_actual = 0;
        } else if (tarea_actual == 16 << 3){ // es el idle
            tarea_actual = 1;
        } else { // es alguna de nivel 3
            tarea_actual = ((tarea_actual - 3) >> 3) - 15;
        }
        print_hex(tarea_actual, 3, 50, 2, C_FG_LIGHT_CYAN | C_BG_BLACK);

        //Imprimimos los datos en pantalla
        //Imprimo los CR
        print("CR0", 46, 7, C_FG_WHITE | C_BG_BLACK);
        print_hex(rcr0(), 8, 50, 7, C_FG_LIGHT_CYAN | C_BG_BLACK);

        print("CR2", 46, 9, C_FG_WHITE | C_BG_BLACK);
        print_hex(rcr2(), 8, 50, 9, C_FG_LIGHT_CYAN | C_BG_BLACK);

        print("CR3", 46, 11, C_FG_WHITE | C_BG_BLACK);
        print_hex(rcr3(), 8, 50, 11, C_FG_LIGHT_CYAN | C_BG_BLACK);

        print("CR4", 46, 13, C_FG_WHITE | C_BG_BLACK);
        print_hex(rcr4(), 8, 50, 13, C_FG_LIGHT_CYAN | C_BG_BLACK);
        breakpoint();

        uint32_t edi = (uint32_t) param.edi;
        uint32_t esi = (uint32_t) param.esi;
        uint32_t ebp = (uint32_t) param.ebp;
        uint32_t esp = (uint32_t) param.esp;
        uint32_t ebx = (uint32_t) param.ebx;
        uint32_t edx = (uint32_t) param.edx;
        uint32_t ecx = (uint32_t) param.ecx;
        uint32_t eax = (uint32_t) param.eax;
        
        uint32_t eipN3 = (uint32_t) param.eip3;
        //uint32_t csN3 = (uint32_t) param.cs3;
        uint32_t eflags = (uint32_t) param.eflags;
        uint32_t *espN3 = (uint32_t*) param.esp3;
        //uint32_t ssN3 = (uint32_t) param.ss3;

        uint32_t st1 = 0 ;
        uint32_t st2 = 0 ;
        uint32_t st3 = 0 ;

        if((uint32_t)(espN3) != ebp) {
            st1 = (uint32_t) *espN3;
            if((uint32_t)(espN3 + 4) != ebp) {
                st2 = (uint32_t) *(espN3 + 4);
                if((uint32_t)(espN3 + 8) != ebp) {
                    st3 = (uint32_t) *(espN3 + 8);
                }
            }
        }
        breakpoint();
    //Impresion de Errores
      //Impresión del hashtag, parentesis y corchetes                        
      print("[  ]", 21+7+10, 3,C_FG_LIGHT_CYAN | C_BG_BLACK);
      print("(#", 21+10, 3, C_FG_LIGHT_CYAN | C_BG_BLACK);
      print(")", 21+14, 3, C_FG_LIGHT_CYAN | C_BG_BLACK);
      
    // ---
      print(exc_name[n_excep], 21, 3, C_FG_LIGHT_CYAN | C_BG_BLACK); 
      print_dec(n_excep, 2, 21+7+11, 3, C_FG_LIGHT_CYAN | C_BG_BLACK);
      print(mnemonic[n_excep], 21+12, 3, C_FG_LIGHT_CYAN | C_BG_BLACK);
                
    //Impresion del error code
      print("err", 46, 15, C_FG_WHITE | C_BG_BLACK);
      //print_hex(errorcode, 8, 50, 15, C_FG_LIGHT_CYAN | C_BG_BLACK);
         
         //Impresion de los regs de proposito general
         print("EAX", 21, 5, C_FG_WHITE | C_BG_BLACK);
         print_hex(eax, 8, 25, 5, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EBX", 21, 7, C_FG_WHITE | C_BG_BLACK);
         print_hex(ebx, 8, 25, 7, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("ECX", 21, 9, C_FG_WHITE | C_BG_BLACK);
         print_hex(ecx, 8, 25, 9, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EDX", 21, 11, C_FG_WHITE | C_BG_BLACK);
         print_hex(edx, 8, 25, 11, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("ESI", 21, 13, C_FG_WHITE | C_BG_BLACK);
         print_hex(esi, 8, 25, 13, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EDI", 21, 15, C_FG_WHITE | C_BG_BLACK);
         print_hex(edi, 8, 25, 15, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EBP", 21, 17, C_FG_WHITE | C_BG_BLACK);
         print_hex(ebp, 8, 25, 17, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("ESP", 21, 19, C_FG_WHITE | C_BG_BLACK);
         print_hex(esp, 8, 25, 19, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EIP", 21, 21, C_FG_WHITE | C_BG_BLACK);
         print_hex(eipN3, 8, 25, 21, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("CS", 22, 23, C_FG_WHITE | C_BG_BLACK);
         print_hex(cs, 8, 25, 23, C_FG_LIGHT_CYAN | C_BG_BLACK);       

         print("DS", 22, 25, C_FG_WHITE | C_BG_BLACK);
         print_hex(ds, 8, 25, 25, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("ES", 22, 27, C_FG_WHITE | C_BG_BLACK);
         print_hex(es, 8, 25, 27, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("FS", 22, 29, C_FG_WHITE | C_BG_BLACK);
         print_hex(fs, 8, 25, 29, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("GS", 22, 31, C_FG_WHITE | C_BG_BLACK);
         print_hex(gs, 8, 25, 31, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("SS", 22, 33, C_FG_WHITE | C_BG_BLACK);
         print_hex(ss, 8, 25, 33, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EFLAGS", 21, 36, C_FG_WHITE | C_BG_BLACK);
         print_hex(eflags, 8, 28, 36, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("Stack", 40, 18, C_FG_WHITE | C_BG_BLACK);
         print_hex(st1, 8, 40, 20, C_FG_LIGHT_CYAN | C_BG_BLACK);
         print_hex(st2, 8, 40, 22, C_FG_LIGHT_CYAN | C_BG_BLACK);
         print_hex(st3, 8, 40, 24, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("Backtrace", 40, 18+9, C_FG_WHITE | C_BG_BLACK);
         uint32_t ebp_limite;

         if (4 > tarea_actual) {
            ebp_limite = tss_list[tarea_actual]->ebp;
         } else {
            ebp_limite = (uint32_t) (meeseeks_list[tarea_actual-4]->pila_virtual_p);
         }

        if (ebp_limite-4 >= (uint32_t)(ebp + 4)){ 
            uint32_t *ebp_p = (uint32_t*) ebp;
            uint32_t *ebp1_p = (uint32_t*) ebp_p[0];
            //ebp = ebp_p[0]; // El ebp acutal es desreferenciar el ebp
            uint32_t ret1_p = (uint32_t) ((ebp_p)[1]);
            print_hex(ret1_p, 8, 40, 18+11, C_FG_LIGHT_CYAN | C_BG_BLACK);
           if (ebp_limite-4 >= (uint32_t)(ebp1_p + 1)){
                uint32_t *ebp2_p = (uint32_t*) ebp1_p[0];
                uint32_t ret2_p = (uint32_t) ((ebp1_p)[1]);
                print_hex(ret2_p, 8, 40, 18+13, C_FG_LIGHT_CYAN | C_BG_BLACK);
                if (ebp_limite-4 >= (uint32_t)(ebp2_p + 1)){
                    uint32_t *ebp3_p = (uint32_t*) ebp2_p[0];
                    uint32_t ret3_p = (uint32_t)((ebp2_p)[1]);
                    print_hex(ret3_p, 8, 40, 18+15, C_FG_LIGHT_CYAN | C_BG_BLACK);
                    if (ebp_limite-4 >= (uint32_t)(ebp3_p + 1)){
                      uint32_t ret4_p = (uint32_t)((ebp3_p)[1]);
                      print_hex(ret4_p, 8, 40, 18+17, C_FG_LIGHT_CYAN | C_BG_BLACK);
                    } else {
                            print_hex(0x00000000, 8, 40, 18+17, C_FG_LIGHT_CYAN | C_BG_BLACK);
                    }
                } else {
                        print_hex(0x00000000, 8, 40, 18+15, C_FG_LIGHT_CYAN | C_BG_BLACK);
                        print_hex(0x00000000, 8, 40, 18+17, C_FG_LIGHT_CYAN | C_BG_BLACK);
                }
            } else {
                    print_hex(0x00000000, 8, 40, 18+13, C_FG_LIGHT_CYAN | C_BG_BLACK);
                    print_hex(0x00000000, 8, 40, 18+15, C_FG_LIGHT_CYAN | C_BG_BLACK);
                    print_hex(0x00000000, 8, 40, 18+17, C_FG_LIGHT_CYAN | C_BG_BLACK);
            }               
        } else {
              print_hex(0x00000000, 8, 40, 18+11, C_FG_LIGHT_CYAN | C_BG_BLACK);
              print_hex(0x00000000, 8, 40, 18+13, C_FG_LIGHT_CYAN | C_BG_BLACK);
              print_hex(0x00000000, 8, 40, 18+15, C_FG_LIGHT_CYAN | C_BG_BLACK);
              print_hex(0x00000000, 8, 40, 18+17, C_FG_LIGHT_CYAN | C_BG_BLACK);
        }
    }
}


void imprimoModoDebug_ec(uint16_t gs, uint16_t fs, uint16_t ss, uint16_t es, uint16_t ds,
                      uint16_t cs, int n_excep, param_pusheados_ec param){
    if(indicador_debugger == 1){
        //La pantalla mostrara el debug
        indicador_debugger_on_screen = 1;

        //Imprimimos por pantalla lo nec.
        //Imprimimos el bloque negro
        screen_draw_box(1, 20, 40, 40, 0x00, 0x00);

        // Imprimimos la tarea actual
        uint16_t tarea_actual = rtr();
        if (tarea_actual == 15 << 3){ // es el initial
            tarea_actual = 0;
        } else if (tarea_actual == 16 << 3){ // es el idle
            tarea_actual = 1;
        } else { // es alguna de nivel 3
            tarea_actual = ((tarea_actual - 3) >> 3) - 15;
        }
        print_hex(tarea_actual, 3, 50, 2, C_FG_LIGHT_CYAN | C_BG_BLACK);

        //Imprimimos los datos en pantalla
        //Imprimo los CR
        print("CR0", 46, 7, C_FG_WHITE | C_BG_BLACK);
        print_hex(rcr0(), 8, 50, 7, C_FG_LIGHT_CYAN | C_BG_BLACK);

        print("CR2", 46, 9, C_FG_WHITE | C_BG_BLACK);
        print_hex(rcr2(), 8, 50, 9, C_FG_LIGHT_CYAN | C_BG_BLACK);

        print("CR3", 46, 11, C_FG_WHITE | C_BG_BLACK);
        print_hex(rcr3(), 8, 50, 11, C_FG_LIGHT_CYAN | C_BG_BLACK);

        print("CR4", 46, 13, C_FG_WHITE | C_BG_BLACK);
        print_hex(rcr4(), 8, 50, 13, C_FG_LIGHT_CYAN | C_BG_BLACK);

        uint32_t edi = (uint32_t) param.edi;
        uint32_t esi = (uint32_t) param.esi;
        uint32_t ebp = (uint32_t) param.ebp;
        uint32_t esp = (uint32_t) param.esp;
        uint32_t ebx = (uint32_t) param.ebx;
        uint32_t edx = (uint32_t) param.edx;
        uint32_t ecx = (uint32_t) param.ecx;
        uint32_t eax = (uint32_t) param.eax;
        
        uint32_t errorcode = (uint32_t) param.errorcode;
        uint32_t eipN3 = (uint32_t) param.eip3;
        //uint32_t csN3 = (uint32_t) param.cs3;
        uint32_t eflags = (uint32_t) param.eflags;
        uint32_t *espN3 = (uint32_t* ) param.esp3;
        //uint32_t ssN3 = (uint32_t) param.ss3;

        uint32_t st1 = 0 ;
        uint32_t st2 = 0 ;
        uint32_t st3 = 0 ;

        if((uint32_t)espN3 != ebp) {
            st1 = (uint32_t) *espN3;
            if((uint32_t)(espN3 + 4) != ebp) {
                st2 = (uint32_t) *(espN3 + 4);
                if((uint32_t)(espN3 + 8) != ebp) {
                    st3 = (uint32_t) *(espN3 + 8);
                }
            }
        }
        
        //Impresion de Errores
      //Impresión del hashtag, parentesis y corchetes                        
      print("[  ]", 21+7+10, 3,C_FG_LIGHT_CYAN | C_BG_BLACK);
      print("(#", 21+10, 3, C_FG_LIGHT_CYAN | C_BG_BLACK);
      print(")", 21+14, 3, C_FG_LIGHT_CYAN | C_BG_BLACK);
      
    // ---
      print(exc_name[n_excep], 21, 3, C_FG_LIGHT_CYAN | C_BG_BLACK); 
      print_dec(n_excep, 2, 21+7+11, 3, C_FG_LIGHT_CYAN | C_BG_BLACK);
      print(mnemonic[n_excep], 21+12, 3, C_FG_LIGHT_CYAN | C_BG_BLACK);
                
    //Impresion del error code
      print("err", 46, 15, C_FG_WHITE | C_BG_BLACK);
      print_hex(errorcode, 8, 50, 15, C_FG_LIGHT_CYAN | C_BG_BLACK);
         
         //Impresion de los regs de proposito general
         print("EAX", 21, 5, C_FG_WHITE | C_BG_BLACK);
         print_hex(eax, 8, 25, 5, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EBX", 21, 7, C_FG_WHITE | C_BG_BLACK);
         print_hex(ebx, 8, 25, 7, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("ECX", 21, 9, C_FG_WHITE | C_BG_BLACK);
         print_hex(ecx, 8, 25, 9, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EDX", 21, 11, C_FG_WHITE | C_BG_BLACK);
         print_hex(edx, 8, 25, 11, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("ESI", 21, 13, C_FG_WHITE | C_BG_BLACK);
         print_hex(esi, 8, 25, 13, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EDI", 21, 15, C_FG_WHITE | C_BG_BLACK);
         print_hex(edi, 8, 25, 15, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EBP", 21, 17, C_FG_WHITE | C_BG_BLACK);
         print_hex(ebp, 8, 25, 17, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("ESP", 21, 19, C_FG_WHITE | C_BG_BLACK);
         print_hex(esp, 8, 25, 19, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EIP", 21, 21, C_FG_WHITE | C_BG_BLACK);
         print_hex(eipN3, 8, 25, 21, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("CS", 22, 23, C_FG_WHITE | C_BG_BLACK);
         print_hex(cs, 8, 25, 23, C_FG_LIGHT_CYAN | C_BG_BLACK);       

         print("DS", 22, 25, C_FG_WHITE | C_BG_BLACK);
         print_hex(ds, 8, 25, 25, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("ES", 22, 27, C_FG_WHITE | C_BG_BLACK);
         print_hex(es, 8, 25, 27, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("FS", 22, 29, C_FG_WHITE | C_BG_BLACK);
         print_hex(fs, 8, 25, 29, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("GS", 22, 31, C_FG_WHITE | C_BG_BLACK);
         print_hex(gs, 8, 25, 31, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("SS", 22, 33, C_FG_WHITE | C_BG_BLACK);
         print_hex(ss, 8, 25, 33, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("EFLAGS", 21, 36, C_FG_WHITE | C_BG_BLACK);
         print_hex(eflags, 8, 28, 36, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("Stack", 40, 18, C_FG_WHITE | C_BG_BLACK);
         print_hex(st1, 8, 40, 20, C_FG_LIGHT_CYAN | C_BG_BLACK);
         print_hex(st2, 8, 40, 22, C_FG_LIGHT_CYAN | C_BG_BLACK);
         print_hex(st3, 8, 40, 24, C_FG_LIGHT_CYAN | C_BG_BLACK);

         print("Backtrace", 40, 18+9, C_FG_WHITE | C_BG_BLACK);
         uint32_t ebp_limite;

         if (4 > tarea_actual) {
            ebp_limite = tss_list[tarea_actual]->ebp;
         } else {
            ebp_limite = (uint32_t) (meeseeks_list[tarea_actual-4]->pila_virtual_p);
         }

        if (ebp_limite-4 >= (uint32_t)(ebp + 4)){ 
            uint32_t *ebp_p = (uint32_t*) ebp;
            uint32_t *ebp1_p = (uint32_t*) ebp_p[0];
            //ebp = ebp_p[0]; // El ebp acutal es desreferenciar el ebp
            uint32_t ret1_p = (uint32_t) ((ebp_p)[1]);
            print_hex(ret1_p, 8, 40, 18+11, C_FG_LIGHT_CYAN | C_BG_BLACK);
           if (ebp_limite-4 >= (uint32_t)(ebp1_p + 1)){
                uint32_t *ebp2_p = (uint32_t*) ebp1_p[0];
                uint32_t ret2_p = (uint32_t) ((ebp1_p)[1]);
                print_hex(ret2_p, 8, 40, 18+13, C_FG_LIGHT_CYAN | C_BG_BLACK);
                if (ebp_limite-4 >= (uint32_t)(ebp2_p + 1)){
                    uint32_t *ebp3_p = (uint32_t*) ebp2_p[0];
                    uint32_t ret3_p = (uint32_t)((ebp2_p)[1]);
                    print_hex(ret3_p, 8, 40, 18+15, C_FG_LIGHT_CYAN | C_BG_BLACK);
                    if (ebp_limite-4 >= (uint32_t)(ebp3_p + 1)){
                      uint32_t ret4_p = (uint32_t)((ebp3_p)[1]);
                      print_hex(ret4_p, 8, 40, 18+17, C_FG_LIGHT_CYAN | C_BG_BLACK);
                    } else {
                            print_hex(0x00000000, 8, 40, 18+17, C_FG_LIGHT_CYAN | C_BG_BLACK);
                    }
                } else {
                        print_hex(0x00000000, 8, 40, 18+15, C_FG_LIGHT_CYAN | C_BG_BLACK);
                        print_hex(0x00000000, 8, 40, 18+17, C_FG_LIGHT_CYAN | C_BG_BLACK);
                }
            } else {
                    print_hex(0x00000000, 8, 40, 18+13, C_FG_LIGHT_CYAN | C_BG_BLACK);
                    print_hex(0x00000000, 8, 40, 18+15, C_FG_LIGHT_CYAN | C_BG_BLACK);
                    print_hex(0x00000000, 8, 40, 18+17, C_FG_LIGHT_CYAN | C_BG_BLACK);
            }               
        } else {
              print_hex(0x00000000, 8, 40, 18+11, C_FG_LIGHT_CYAN | C_BG_BLACK);
              print_hex(0x00000000, 8, 40, 18+13, C_FG_LIGHT_CYAN | C_BG_BLACK);
              print_hex(0x00000000, 8, 40, 18+15, C_FG_LIGHT_CYAN | C_BG_BLACK);
              print_hex(0x00000000, 8, 40, 18+17, C_FG_LIGHT_CYAN | C_BG_BLACK);
        }
    }
}
