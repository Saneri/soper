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

#include "simulador.h"
#include "nave.h"
#include "mapa.h"

/*
 * @brief Crear nuevo struct tipo_nave
 * @param num_jefe el numero identificador del jefe de la nave y del equipo
 * @param num_nave el numero identificador del numero de la nave dentro del equipo
 * @return Struct tipo_nave inicializado
 */
tipo_nave crear_nave (int num_jefe, int num_nave, int posx, int posy) {
	
	tipo_nave new_nave;
	new_nave.vida = VIDA_MAX; 
	new_nave.posx = posx; 
	new_nave.posy = posy; 
	new_nave.equipo = num_jefe; 
	new_nave.numNave = num_nave;
	new_nave.viva = true;
	return new_nave; 
}

/*
 * @brief Funcion para ejecutar tipo_nave
 * @param num_jefe el numero identificador del jefe de la nave y del equipo
 * @param num_nave el numero identificador del numero de la nave dentro del equipo
 * @param pipe_jefe[2] la tuberia que se utiliza para comuniacion enter jefe y nave
 */
int ejecutar_nave(int num_jefe, int num_nave, int pipe_jefe[2]) {
	
	mqd_t queue = mq_open(MQ_NAME, O_WRONLY, S_IRUSR | S_IWUSR, NULL);
	if (queue == (mqd_t) -1) {
		perror("(mq_open) No se pudo abrir la cola de mensajes de la nave");
		return -1;
	}
	close(pipe_jefe[1]);

	Mensaje msg;
	strcpy(msg.texto, "NAVE_LISTO");
	
	char msg_jefe[80];
	while (1) {
		printf("Sim Nave %d/%d: leyendo siguiente mensaje del PIPE\n", num_jefe, num_nave);
		read(pipe_jefe[0], msg_jefe, sizeof(msg_jefe));
		printf("Nave: %s\n", msg_jefe);
		if (strcmp(msg_jefe, "MOVER_ALEATORIO") == 0) {
			printf("MOVER_ALEATORIO\n");
		} else if (strcmp(msg_jefe, "ATACAR") == 0) {
			printf("ATACAR\n");
		} else if (strcmp(msg_jefe, "FIN") == 0) {
			close(pipe_jefe[0]);
			mq_close(queue);
			return 0;
		} else {
			perror("Nave ha sacado un mensaje invalido");
			close(pipe_jefe[0]);
			mq_close(queue);
			return -1;
		}
		memset(msg_jefe, 0, sizeof(msg_jefe));
		if (mq_send(queue, (char*) &msg, sizeof(msg), 1) < 0) {
			perror("(mq_send) No se pudo enviar mensaje");
			return -1;
		}
	}
	return -1;
}

/*
 * @brief Funcion para intentar atacar a una posicion con la nave elegida
 * @param mapa estructura con la informacion necesaria de el mapa
 * @param nave estructura con la informacion de la nave que va a realizar el ataque
 */
int nave_atacar(tipo_mapa *mapa, tipo_nave *nave) {
	int direccion = rand() % 4;
	int origenx = nave->posx;
	int origeny = nave->posy;
	int equipoNave = nave->equipo;
	int i;
	switch (direccion){
		case 0:
					i = -20;
					while(i < 20){// coordenada x
						if (origenx + i < MAPA_MAXX && origenx + i >= 0){
							for(int j = -20; j < 20 ; j++){// coordenada y
								if (origeny + j < MAPA_MAXY && origeny + j >= 0){
									tipo_casilla casilla = mapa_get_casilla(mapa, origeny + j, origenx + i);
									if (casilla.equipo != equipoNave && casilla.equipo != -1  ){
										mapa_send_misil(mapa, origeny, origenx , origeny + j, origenx + i);
										tipo_nave nave_atacada = mapa_get_nave(mapa, casilla.equipo, casilla.numNave);
										if (nave_atacada.vida - ATAQUE_DANO <= 0){
											nave_destruir(mapa, &nave_atacada);
										} else {
											nave_atacada.vida -= ATAQUE_DANO;
											mapa_set_nave(mapa, nave_atacada);
										}
										return 0;
									}
								}
							}
						}
						i++;
					}
					break;
		case 1:
					i = 20;
					while(i > -20){// coordenada x
						if (origenx + i < MAPA_MAXX && origenx + i >= 0){
							for(int j = 20; j > -20 ; j--){// coordenada y
								if (origeny + j < MAPA_MAXY && origeny + j >= 0){
									tipo_casilla casilla = mapa_get_casilla(mapa, origeny + j, origenx + i);
									if (casilla.equipo != equipoNave && casilla.equipo != -1  ){
										mapa_send_misil(mapa, origeny, origenx , origeny + j, origenx + i);
										tipo_nave nave_atacada = mapa_get_nave(mapa, casilla.equipo, casilla.numNave);
										if (nave_atacada.vida - ATAQUE_DANO <= 0){
											nave_destruir(mapa, &nave_atacada);
										} else {
											nave_atacada.vida -= ATAQUE_DANO;
											mapa_set_nave(mapa, nave_atacada);
										}
										return 0;
									}
								}
							}
						}
						i--;
					}
					break;

		case 2:
					i = -20;
					while(i < 20){// coordenada y
						if (origeny + i < MAPA_MAXY && origeny + i >= 0){
							for(int j = 20; j > -20 ; j--){// coordenada x
								if (origenx + j < MAPA_MAXX && origenx + j >= 0){
									tipo_casilla casilla = mapa_get_casilla(mapa, origeny + i, origenx + j);
									if (casilla.equipo != equipoNave && casilla.equipo != -1  ){
										mapa_send_misil(mapa, origeny, origenx , origeny + i, origenx + j);
										tipo_nave nave_atacada = mapa_get_nave(mapa, casilla.equipo, casilla.numNave);
										if (nave_atacada.vida - ATAQUE_DANO <= 0){
											nave_destruir(mapa, &nave_atacada);
										} else {
											nave_atacada.vida -= ATAQUE_DANO;
											mapa_set_nave(mapa, nave_atacada);
										}
										return 0;
									}
								}
							}
						}
						i++;
					}
					break;

		case 3:
					i = 20;
					while(i > -20){// coordenada y
						if (origeny + i < MAPA_MAXY && origeny + i >= 0){
							for(int j = -20; j < 20 ; j++){// coordenada x
								if (origenx + j < MAPA_MAXX && origenx + j >= 0){
									tipo_casilla casilla = mapa_get_casilla(mapa, origeny + i, origenx + j);
									if (casilla.equipo != equipoNave && casilla.equipo != -1  ){
										mapa_send_misil(mapa, origeny, origenx , origeny + i, origenx + j);
										tipo_nave nave_atacada = mapa_get_nave(mapa, casilla.equipo, casilla.numNave);
										if (nave_atacada.vida - ATAQUE_DANO <= 0){
											nave_destruir(mapa, &nave_atacada);
										} else {
											nave_atacada.vida -= ATAQUE_DANO;
											mapa_set_nave(mapa, nave_atacada);
										}
										return 0;
									}
								}
							}
						}
						i--;
					}
					break;

		default:
					break;
	}

	return -1;
}

/*
 * @brief Funcion para realizar un movimiento de una nave y modifica el mapa
 * @param mapa estructura con la informacion necesaria de el mapa
 * @param nave estructura con la informacion de la nave que va a realizar el movimiento
 * @param targety la coordenada de la posicion en y a moverse
 * @param targetx la coordenada de la posicion en x a moverse
 */
int nave_mover(tipo_mapa *mapa, tipo_nave *nave, int targety, int targetx){
	mapa_clean_casilla(mapa, nave->posy, nave->posx);
	return 0;
}


/*
 * @brief Funcion para realizar un movimiento aleatorio de direccion y cantidad por el mapa
 * @param mapa estructura con la informacion necesaria de el mapa
 * @param nave estructura con la informacion de la nave que va a realizar el movimiento
 */
int nave_mover_aleatorio(tipo_mapa *mapa, tipo_nave *nave) {
	bool selected = false;
	bool direction_tries[4] = {false,false,false,false};
	while(!selected) {
		if ( direction_tries[0] == true &&  direction_tries[1] == true && direction_tries[2] == true && direction_tries[3] == true) {
			break;
		}
		int direccion = rand() % 4;
		int nummov = rand() % MOVER_ALCANCE + 1;
		switch(direccion) {
			case 0:
				if (mapa_is_casilla_vacia(mapa, nave->posy + nummov, nave->posx) && (nave->posy + nummov < MAPA_MAXY)){
					nave_mover(mapa, nave, nave->posy + nummov, nave->posx);
					nave_cambiarposicion(mapa,nave,nave->posy + nummov, nave->posx);
					selected = true;
				}
				direction_tries[direccion] = true;
				break;
			case 1:
				if (mapa_is_casilla_vacia(mapa, nave->posy, nave->posx + nummov) && (nave->posx + nummov < MAPA_MAXX)){
					nave_mover(mapa, nave, nave->posy, nave->posx + nummov);
					nave_cambiarposicion(mapa,nave, nave->posy, nave->posx + nummov);
					selected = true;
				}
				direction_tries[direccion] = true;
				break;
			case 2:
				if (mapa_is_casilla_vacia(mapa, nave->posy - nummov, nave->posx) && (nave->posy - nummov > 0)){
					nave_mover(mapa, nave, nave->posy - nummov, nave->posx);
					nave_cambiarposicion(mapa,nave, nave->posy - nummov, nave->posx);
					selected = true;
				}
				direction_tries[direccion] = true;
				break;
			case 3:
				if (mapa_is_casilla_vacia(mapa, nave->posy, nave->posx - nummov) && (nave->posx - nummov > 0)){
					nave_mover(mapa, nave, nave->posy, nave->posx - nummov);
					nave_cambiarposicion(mapa,nave, nave->posy, nave->posx - nummov);
					selected = true;
				}
				direction_tries[direccion] = true;
				break;
			default:
				selected = false;
				return -1;
		}
	}

	return 0;
}

/*
 * @brief Fumcion auxiliar para cambiar los datos de posicion de la nave
 * @param nave estructura con la informacion de la nave que va a cambiar su posicion
 * @param posy la coordenada de la posicion en y a cambiar
 * @param posx la coordenada de la posicion en x a cambiar
 */
int nave_cambiarposicion(tipo_mapa *mapa, tipo_nave *nave, int posy, int posx){
	nave->posx = posx;
	nave->posy = posy;
	mapa_set_nave(mapa,*nave);
	return 0;
}

/*
 * @brief Funcion para destruir una nave con sus correspondientes recursos
 * @param mapa estructura con la informacion de el mapa
 * @param nave estructura con la informacion de la nave que va a realizar el movimiento
 */
 int nave_destruir(tipo_mapa *mapa, tipo_nave *nave) {
	nave->vida = 0;
	nave->viva = false;
	mapa_clean_casilla(mapa, nave->posy, nave->posx);
	return 0;
}
