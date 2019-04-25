#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

#include <nave.h>
#include <mapa.h>


// Intenta a atacar a una posicion con la nave elegido
int nave_atacar(tipo_mapa *mapa, tipo_nave *nave, int targety, int targetx) {
	if(nave->equipo != mapa->casillas[targety][targetx].equipo){
		mapa_send_misil(mapa, nave->posy, nave->posx, targety, targetx);
		return 0;
	} else{

	}
	return -1; // no implementado
}

// Mover nave aleatoriamente a las casilla adyacentes
// Direcciones 
//	0 ---- N
//	1 ---- E
//	2 ---- S
//	3 ---- W
int nave_mover(tipo_mapa *mapa, tipo_nave *nave, int targety, int targetx){
	mapa_clean_casilla(mapa, nave->posy, nave->posx);
	mapa_set_nave(mapa, tipo_nave nave);
}

/*
*
*/

int nave_mover_aleatorio(tipo_mapa *mapa, tipo_nave *nave) {
	int direccion = rand() % 4;
	bool selected = false;
	while(!selected){}
		switch(direccion) {
			case 0:
				if (mapa_is_casilla_vacia(mapa, nave->posy + 1, nave->posx){
					nave_mover(nave, nave->posy + 1, nave->posx);
					selected = true;
				}
				break;
			case 1:
				if (mapa_is_casilla_vacia(mapa, nave->posy, nave->posx + 1){
					nave_mover(nave, nave->posy, nave->posx + 1);
					selected = true;
				}
				break;
			case 2:
				if (mapa_is_casilla_vacia(mapa, nave->posy - 1, nave->posx){
					nave_mover(nave, nave->posy - 1, nave->posx);
					selected = true;
				}
				break;
			case 3:
				if (mapa_is_casilla_vacia(mapa, nave->posy, nave->posx - 1){
					nave_mover(nave, nave->posy, nave->posx - 1);
					selected = true;
				}
				break;
			default:
				selected = false					
				return -1;
		}
	}
	return 0;
}

// Destruye una nave librando sus recursos
int nave_destruir(tipo_mapa *mapa, tipo_nave *nave) {
	nave->vida = 0;
	nave->viva = false;
	mapa_clean_casilla(mapa, nave->posy, nave->posx);
	return -1; // no implementado
}