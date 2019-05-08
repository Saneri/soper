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
#include <sys/types.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>

#include "nave.h"
#include "simulador.h"
#include "mapa.h"
#include "jefe.h"

const int INICIO_NAVES[MAPA_MAXY][MAPA_MAXX] = {{0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3}, 
						{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3}, 
						{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

// Los recursos: 0 si no esta inicializado y 1 si hay que librar el recurso
int sigue_jugando = 1;
int recurso_shared_memory = 0; 
int recurso_mmap = 0;
int recurso_mqueue = 0;
int recurso_sem_monitor = 0;
tipo_mapa* mapa;
mqd_t queue;
sem_t *sem_monitor = NULL;
int pipes[N_EQUIPOS][2];


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
	if (recurso_sem_monitor) {
		sem_close(sem_monitor);
		sem_unlink(SEM_SYNC_MONITOR);
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
int proceso_simulador() {

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
		return -1;
	}
	recurso_mqueue = 1;	
	
	for (int i=0; i<N_EQUIPOS; i++) {
		close(pipes[i][0]);
	}	

	sem_post(sem_monitor);

	/////////////////////
	// Empieza a jugar //
	/////////////////////
	
	msg_simulador msg_sim;	
	int num_naves_total = 0;
	while (sigue_jugando) {
		printf("Simulador: Nuevo TURNO\n");
		strcpy(msg_sim.msg, "TURNO");
		// Enviar mensaje TURNO a cada jefe
		for (int i=0; i<N_EQUIPOS; i++) {
			
			write(pipes[i][1], (char*)& msg_sim, sizeof(msg_simulador));  
			num_naves_total += mapa_get_num_naves(mapa, i);
		}
		
		printf("Simulador: eschuchando cola mensajes\n");
		for (int i=0; i<num_naves_total; i++) {
			// Wait until every nave has written to mqueue
		}

		sleep(1);
		// Finalizar el turno 
		mapa_restore(mapa);
		check_winner();	
	}
	
	// Finalizar jefes
	strcpy(msg_sim.msg, "FIN");
	for (int i=0; i<N_EQUIPOS; i++) {
		write(pipes[i][1], (char*)& msg_sim, sizeof(msg_simulador));
	}

	return 0;
}

/*
 * @brief Comprobar si el juego tiene ganador
 */
void check_winner() {
	int n_equipos_vivos = 0;
	int ganador = 0;
	for (int i=0 ; i<N_EQUIPOS ; i++) {
		if (mapa->num_naves[i] > 0) {
		       n_equipos_vivos++;
		       ganador = i;
		}		
	}
	if (n_equipos_vivos < 2) {
		sigue_jugando = 0;
		if (n_equipos_vivos) {
			printf("The winner is team number %d!\n", ganador);
		} else {
			perror("No hay ganador!");
		}
	}
}


/*
 * @brief
 */
int init_mapa() {
	printf("Inicializando el mapa\n");

	for (int i=0; i<N_EQUIPOS; i++) {
		mapa_set_num_naves(mapa, i, 0);
	}

	for (int i=0; i<MAPA_MAXY; i++) {
		for (int j=0; j<MAPA_MAXX; j++) {
			int n_equipo = INICIO_NAVES[i][j];
			if (n_equipo == 0) {
				tipo_casilla new_casilla;
				new_casilla.simbolo = '.';
				new_casilla.equipo = -1;
				new_casilla.numNave = -1;
				mapa->casillas[i][j] = new_casilla;
			}  else if (n_equipo < 0) {
				perror("No se puede asignar un numero negativo para la mapa");
				return -1;
			} else {

				tipo_nave new_nave = crear_nave(n_equipo, mapa_get_num_naves(mapa, n_equipo));
				mapa_set_nave(mapa, new_nave);

				tipo_casilla new_casilla;
				new_casilla.simbolo = n_equipo + '0';
				new_casilla.equipo = n_equipo;
				new_casilla.numNave = mapa_get_num_naves(mapa, n_equipo);
				mapa->casillas[i][j] = new_casilla;

				mapa_set_num_naves(mapa, n_equipo, mapa_get_num_naves(mapa, n_equipo) + 1);
			}
		}	
	}
	return 0;
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
	if (ftruncate(fd_shm, sizeof(tipo_mapa)) == -1) {
		perror("(ftruncate) simulador failed to resize memory");
		return -1;
	}

	// Map la memoria compartida para el uso de este proceso
	printf("Simulador inicializando mapa\n");  
	mapa = mmap(NULL, sizeof(tipo_mapa), PROT_WRITE | PROT_READ, MAP_SHARED, fd_shm, 0);
	if (mapa == MAP_FAILED) {
		perror("(mmap) No se pudo mapear la memoria compartida de mapa");
		return -1;
	}
	recurso_mmap = 1;
	
	
	// Inicializar mapa
	if (init_mapa() < 0) {
		perror("Hay algo mal en la inicializacion de la mapa");
		return -1;
	}
	
	// Inicializar el manejador para SIGINT
	printf("Simulador gestionando senales\n");
	act.sa_handler = manejador_SIGINT;
	sigemptyset(&(act.sa_mask));
	act.sa_flags = 0;
	int error = sigaction(SIGINT, &act, NULL);
	if (error < 0) {
		perror("No se pudo agregar el manejador SIGINT");
		return -1;
	}
	
	// Inicializar semaforo
	printf("Simulador gestionando semaforos\n");
	if ((sem_monitor = sem_open(SEM_SYNC_MONITOR, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
		perror("(sem_open) No se pudo abrir semaforo");
		return -1;
	}
	recurso_sem_monitor = 1;
	
	// Inicializar tuberias para comunicar con jefes
	for (int i=0; i<N_EQUIPOS ; i++) {
		int pipe_status = pipe(pipes[i]);
		if (pipe_status < 0) {
			perror("(pipe) No se pudo inicializar pipe del simulador");
			return -1;
		}
	}

	// Crear procesos jefes
	pid_t pid;
	for (int i=0; i<N_EQUIPOS; i++) {
		pid = fork();
		if (pid < 0) {
			perror("(fork) No se pudo crear nuevo proceso");
			return -1;
		} else if (pid == 0) {	// Jefe (proceso hijo)
			ejecutar_jefe(i, pipes[i]);

		} else {		// Simulador (proceso padre)
			
		}
	}
	int sim_error = proceso_simulador();
	if (sim_error < 0) {
		perror("Proceso simulador no ha ejecutado correctamente");
		return -1;
	}

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
