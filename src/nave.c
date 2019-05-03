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

tipo_nave crear_nave(int num_jefe,int num_nave){
	tipo_nave new_nave;
	new_nave-> vida = VIDA_MAX; 
	new_nave-> posx = 0; 
	new_nave-> posy = 0; 
	new_nave-> equipo = num_jefe; 
	new_nave-> numNave = num_nave;
	new_nave-> viva = true; 
	return new_nave;

}
// Intenta a atacar a una posicion con la nave elegido
int nave_atacar(tipo_mapa *mapa, tipo_nave *nave) {
	int origenx = nave->posx;
	int origeny = nave->posy;
	int equipoNave = nave->equipo;
	int i = -20;
	while(i < 20){// coordenada x
		if (origenx + i < MAPA_MAXX && origenx + i < MAPA_MAXX > 0){
			for(int j = -20; j < 20 ; j++){// coordenada y
				if (origeny + j < MAPA_MAXY && origeny + j < MAPA_MAXY > 0){
					tipo_casilla casilla = mapa_get_casilla(mapa, origeny + j, origenx + i);
					if (casilla->equipo != equipoNave){
						mapa_send_misil(mapa, origeny, origenx , origeny + j, origenx + i);
						tipo_nave nave_atacada = mapa_get_nave(mapa, casilla->equipo, casilla->num_nave);
						if (nave_atacada->vida - ATAQUE_DANO < 0){
							nave_destruir(mapa, nave_atacada);
						} else {
							nave_atacada->vida - ATAQUE_DANO;
						}
						return 0;
					}
				}
			}
		}
		i++;
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
	int nummov = rand() % MOVER_ALCANCE;
	bool selected = false;
	while(!selected){}
		switch(direccion) {
			case 0:
				if (mapa_is_casilla_vacia(mapa, nave->posy + nummov, nave->posx) && nave->posy + nummov < MAPA_MAXY){
					nave_mover(nave, nave->posy + nummov, nave->posx);
					nave_cambiarposicion(nave,nave->posy + nummov, nave->posx);
					selected = true;
				}
				break;
			case 1:
				if (mapa_is_casilla_vacia(mapa, nave->posy, nave->posx + nummov) && nave->posx + nummov < MAPA_MAXX){
					nave_mover(nave, nave->posy, nave->posx + nummov);
					nave_cambiarposicion(nave, nave->posy, nave->posx + nummov);
					selected = true;
				}
				break;
			case 2:
				if (mapa_is_casilla_vacia(mapa, nave->posy - nummov, nave->posx) && nave->posy - nummov > 0){
					nave_mover(nave, nave->posy - nummov, nave->posx);
					nave_cambiarposicion(nave, nave->posy - nummov, nave->posx);
					selected = true;
				}
				break;
			case 3:
				if (mapa_is_casilla_vacia(mapa, nave->posy, nave->posx - nummov) && nave->posx - nummov > 0){
					nave_mover(nave, nave->posy, nave->posx - nummov);
					nave_cambiarposicion(nave, nave->posy, nave->posx - nummov);
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


int nave_cambiarposicion(tipo_nave *nave, int posy, int posx){
		nave->posx = posx;
		nave->posy = posy;

}

// Destruye una nave librando sus recursos
int nave_destruir(tipo_mapa *mapa, tipo_nave *nave) {
	nave->vida = 0;
	nave->viva = false;
	mapa_clean_casilla(mapa, nave->posy, nave->posx);
	return -1; // no implementado
}