#include "stddef.h"
#include "syscall.h"

//0x1D00608 es la direccion donde guardamos el x para los scouts
//0x1D0060C es la direccion donde guardamos el y para los scouts

void scout(void);//buscan semillas, nada mas

void task(void) {
	//seteamos la posicion inicial del primer scout
	int x = 23;
	int y = 33;
	//indicamos la memoria fisica que usaremos para pasar los looks
	int* x_seed = (int*)0x1D00608;
	int* y_seed = (int*)0x1D0060C;
	//por las dudas limpiamos la memoria
	x_seed[0] = 0;
	y_seed[0] = 0;
	//indicamos que la primera vez no queremos alterar los x e y
	uint32_t ret = 1;
	while (1){
		if (ret != 0){//si se puso una semilla...
			x += x_seed[0];//actualizamos los x e y
			if (x < 0 || x > 79) x=0;//y si son invalidos, los seteamos en cero y arrancamos de nuevo
			y += y_seed[0];
			if (y < 0 || y > 39) y=0;
		}
		ret = syscall_meeseeks((uint32_t)&scout, (uint32_t)x, (uint32_t)y);
		//paso un turno
		int8_t x, y;
		syscall_look(&x, &y);
	}
}

void scout(void) {//guarda la posicion
	int8_t* x_seed = (int8_t*)0x1D00608;
	int8_t* y_seed = (int8_t*)0x1D0060C;

	//si no cai en una semilla
	//busco la semilla mas cercana y guardo su posicion
	syscall_look(x_seed, y_seed);

	//uso la portal gun para ser disruptivo
	//syscall_use_portal_gun(); //apagado porq hace mas lento y pierde 

	//me desolojo
	__builtin_trap();

}