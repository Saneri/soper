/*
 * @file simulador.c
 * @author Santeri Suitiala & Roberto Pirck Valdés, grupo 5, practicas 2212
 * @date
 * @brief 
 */

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

#include <mapa.h>

#define SHM_MAPA "/shm_mapa_SOPER"

int main() {
	int ret=0;
	exit(ret);
}

// Manojador de la señal SIGTERM (ctrl + C)
void manejador_SIGINT(int sig) {

}

/*
 * @brief Inicializar la simulación
 * @return 0 si todo ha sido bien, -1 en el caso de error
 */
int init() {

	struct sigaction act;
	int fd_shm;
	tipo_mapa* mapa;

	// Inicializar el manejador para SIGINT
	act.sa_handler = manejador_SIGINT;
	int error = sigaction(SIGINT, &act, NULL);
	if (error < 0) {
		perror("No se pudo agregar el manejador a SIGINT");
		return -1;
	}

	// Inicializar memoria compartida para la mapa
	fd_shm = shm_open(SHM_MAPA, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd_shm < 0) {
		perror("(shm_open) No se pudo inicializar la memoria compartida");
		return -1;
	}
	
	// Map la memoria compartida para el uso de este proceso  
	mapa = mmap(NULL, sizeof(tipo_mapa), PROT_WRITE | PROT_READ, MAP_SHARED, fd_shm, 0);
	if (mapa == MAP_FAILED) {
		perror("(mmap) No se pudo mapear la memoria compartid de mapa");
		return -1;
	}
	
	// Crear procesos jefes
	pid_t pid;
	for (int i=0; i<N_EQUIPOS; i++) {
		pid = fork();
		if (pid < 0) {
			perror("(fork) No se pudo crear nuevo proceso");
			return -1;
		} else if (pid == 0) {
			// Jefe (proceso hijo)
		} else {
			// Simulador (proceso padre)
		}
	}

	return 0;
}

// Intenta a atacar a una posicion con la nave elegido
int nave_atacar(tipo_nave *nave, int targety, int targetx) {
	return -1; // no implementado
}

// Mover nave aleatoriamente a las casilla adyacentes
int nave_mover(tipo_nave *nave) {
	return -1; // no implementado
}

// Destruye una nave librando sus recursos
int nave_destruir(tipo_nave *nave) {
	return -1; // no implementado
}
