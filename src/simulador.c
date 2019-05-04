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

#include "mapa.h"
#include "jefe.h"

// Los recursos: 0 si no esta inicializado y 1 si hay que librar el recurso
int sigue_jugando = 1;
int recurso_shared_memory = 0; 
int recurso_mmap = 0;
int recurso_mqueue = 0;
tipo_mapa* mapa;
mqd_t queue;



/*
 * @brief rutina para librar todas las recursos que ha utilizado proceso simulador
 */
void librar_recursos_proceso_simulador() {	
	printf("Librando recursos\n");
	if (recurso_mmap) {
		munmap(mapa, sizeof(*mapa));
	}
	if (recurso_mqueue) {
		mq_close(queue);
	}
}

/*
 * @brief Manejador de la señal SIGTERM (ctrl + C)
 */
void manejador_SIGINT(int sig) {
	sigue_jugando = 0;
}


/*
 * @brief La rutina del proceso simulador que es el padre de los jefes 
 */
void proceso_simulador() {

	// Inicializar message queue
	printf("Simulador gestionando MQ\n");
       	struct mq_attr attributes = {
		.mq_flags = 0,
		.mq_maxmsg = 10,
		.mq_curmsgs = 0,
		.mq_msgsize = sizeof(char) * 80 
	};	
	queue = mq_open(MQ_NAME, O_CREAT | O_EXCL | O_RDONLY, S_IRUSR | S_IWUSR, &attributes);
	if (queue == (mqd_t) -1) {
		perror("(mq_open) No se pudo abrir la cola de mensajes para el simulador");
		return;
	}
	recurso_mqueue = 1;	
	
	// Inicializar tuberias para comunicar con jefes
	int pipes[N_EQUIPOS][2];
	for (int i=0; i<N_EQUIPOS ; i++) {
		int pipe_status = pipe(pipes[i]);
		if (pipe_status < 0) {
			perror("(pipe) No se pudo inicializar pipe del simulador");
			return;
		}
		close(pipes[i][0]);
	}

	/////////////////////
	// Empieza a jugar //
	/////////////////////
	
	while (sigue_jugando) {

		printf("Simulador: Nuevo TURNO\n");
		// Rutina de turnos aqui
		sleep(1);	
	}


	
}


/*
 * @brief
 */
void init_mapa() {
	printf("Inicializando el mapa\n");
	for (int i=0; i<MAPA_MAXY; i++) {
		for (int j=0; j<MAPA_MAXX; j++) {
			tipo_casilla new_casilla;
			new_casilla.simbolo = '.';
			new_casilla.equipo = -1;
			new_casilla.numNave = -1;
			mapa->casillas[i][j] = new_casilla;
		}	
	}
	
	for (int i=0; i<N_EQUIPOS; i++) {
		//mapa_set_num_naves(mapa, i, N_NAVES);
	}
}


 /*
 * @brief Inicializar la simulación
 * @return 0 si todo ha sido bien, -1 en el caso de error
 */
int init() {

	struct sigaction act;
	int fd_shm;
	
	// Inicializar memoria compartida para la mapa
	printf("Simulador gestionando SHM\n");
	fd_shm = shm_open(SHM_MAP_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd_shm < 0) {
		perror("(shm_open) No se pudo inicializar la memoria compartida");
		return -1;
	}
	recurso_shared_memory = 1;
	
	// Resize shared memory
	if (ftruncate(fd_shm, sizeof(mapa)) == -1) {
		perror("(ftruncate) failed");
		return -1;
	}

	// Map la memoria compartida para el uso de este proceso
	printf("Simulador inicializando mapa\n");  
	mapa = mmap(NULL, sizeof(tipo_mapa), PROT_WRITE | PROT_READ, MAP_SHARED, fd_shm, 0);
	if (mapa == MAP_FAILED) {
		perror("(mmap) No se pudo mapear la memoria compartid de mapa");
		return -1;
	}
	recurso_mmap = 1;
	
	
	// Inicializar mapa
	init_mapa();
	
	// Inicializar el manejador para SIGINT
	printf("Simulador gestionando senales\n");
	act.sa_handler = manejador_SIGINT;
	int error = sigaction(SIGINT, &act, NULL);
	if (error < 0) {
		perror("No se pudo agregar el manejador a SIGINT");
#define MQ_NAME "/mq_simulador"

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
			ejecutar_jefe(i);
		} else {
			// Simulador (proceso padre)
			
		}
	}
	proceso_simulador();

	// Espera a todos los jefes
	for (int i=0; i<N_EQUIPOS; i++) {
		wait(NULL);
	}

	return 0;
}



int main() {
	// Inicializar juego
	if (init() < 0) {
		perror("Simulador ha producido un error inesperado\n");
	}
	
	// Finalmente librar todos los recursos
	librar_recursos_proceso_simulador();
	
	if (recurso_shared_memory) {
		shm_unlink(SHM_MAP_NAME);
	}
	if (recurso_mqueue) {
		mq_unlink(MQ_NAME);
	}
	return 0;
}
