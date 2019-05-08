#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>

#include "simulador.h"
#include "nave.h"

/*
 * @brief 
 * @param i el numero identificador del jefe
 */
void ejecutar_jefe(int num_jefe, int sim_pipe[2]) {
	pid_t pid;

	int pipes[N_NAVES][2];

	for (int i=0; i<N_NAVES ; i++) {
		pid = fork();
		if (pid < 0) {
			perror("(fork) No se pudo inicializar proceso nave");
			exit(EXIT_FAILURE);
		} else if (pid == 0) {
			printf("CREANDO NAVE %d %d\n", num_jefe, i);
			//ejecutar_nave(num_jefe, i, pipes[i]);	
			exit(EXIT_SUCCESS);
		} else {
			// Inicializar tuberias para comunicar con las naves
			int pipe_status = pipe(pipes[i]);
			if (pipe_status < 0) {
				perror("(pipe) No se pudo inicializar pipe de la nave");
				exit(EXIT_FAILURE);
			}
			close(pipes[i][0]);

		}
	}
	
	sem_t *sem_simjefe;
	if ((sem_simjefe = sem_open(SEM_SYNC_SIMJEFE, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
		perror("(sem_open) No se pudo abrir semafor");
		exit(EXIT_FAILURE);
	}

	close(sim_pipe[1]);
	char msg_sim[80];
	// La rutina del jefe
	while (1) {
		printf("Sim Jefe %d: leyendo siguente mensaje del PIPE\n", num_jefe);
		sem_wait(sem_simjefe);	
		read(sim_pipe[0], msg_sim, sizeof(msg_sim));
		printf("Jefe: %s\n", msg_sim);
		if (strcmp(msg_sim, "TURNO") == 0) {
			// Enviar MOVER_ALEATORIO y ATACAR
			printf("JEFE TURNO !\n");
		} else if (strcmp(msg_sim, "FIN") == 0) {
			// Enviar FIN y acabar este proceso
			
			exit(EXIT_SUCCESS);
		}
		sleep(1);

	}
	exit(EXIT_FAILURE); // No deberia que llegar hasta aqui
}

