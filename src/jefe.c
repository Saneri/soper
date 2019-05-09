/*
 * @file jefe.c
 * @author Santeri Suitiala & Roberto Pirck Valdés, grupo 5, practicas 2212
 * @date 9 de Mayo 2019
 * @brief El fichero que maneja los procesos jefes y lanzan procesos naves
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>

#include "simulador.h"
#include "nave.h"

/*
 * @brief La rutina del proceso jefe 
 * @param num_jefe el numero identificador del jefe
 * @param sim_pipe[2] El tuberia que se utiliza para comunicar con el proceso simulador
 */
void ejecutar_jefe(int num_jefe, int sim_pipe[2]) {
	pid_t pid;

	// Inicializar tuberias para comunicar con las naves
	int pipes[N_NAVES][2];
	for (int i=0; i<N_NAVES; i++) {
		int pipe_status = pipe(pipes[i]);
		if (pipe_status < 0) {
			perror("(pipe) No se pudo inicializar pipe de la nave");
			exit(EXIT_FAILURE);
		}

	}

	for (int i=0; i<N_NAVES; i++) {
		pid = fork();
		if (pid < 0) {
			perror("(fork) No se pudo inicializar proceso nave");
			exit(EXIT_FAILURE);
		} else if (pid == 0) {
			ejecutar_nave(num_jefe, i, pipes[i]);	
			exit(EXIT_SUCCESS);
		}
	}
	
	sem_t *sem_simjefe;
	if ((sem_simjefe = sem_open(SEM_SYNC_SIMJEFE, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
		perror("(sem_open) No se pudo abrir semafor");
		exit(EXIT_FAILURE);
	}
	for (int i=0; i<N_NAVES; i++) {
		close(pipes[i][0]);
	}

	// La rutina del jefe
	close(sim_pipe[1]);
	char msg_sim[80];
	char msg_nave_mover[] = "MOVER_ALEATORIO";
	char msg_nave_atacar[] = "ATACAR";
	char msg_nave_fin[] = "FIN";
	while (1) {
		printf("Sim Jefe %d: leyendo siguente mensaje del PIPE\n", num_jefe);
		sem_wait(sem_simjefe);	
		read(sim_pipe[0], msg_sim, sizeof(msg_sim));
		printf("Jefe: %s\n", msg_sim);	
		if (strcmp(msg_sim, "TURNO") == 0) {
			for (int i=0; i<N_NAVES; i++) {
				write(pipes[i][1], msg_nave_mover, sizeof(msg_nave_mover));
				sleep(1);
				write(pipes[i][1], msg_nave_atacar, sizeof(msg_nave_atacar));
			}
		} else if (strcmp(msg_sim, "FIN") == 0) {
			for (int i=0; i<N_NAVES; i++) {
				write(pipes[i][1], msg_nave_fin, sizeof(msg_nave_fin));
			}	
			exit(EXIT_SUCCESS);
		} else {
			for (int i=0; i<N_NAVES; i++) {
                                write(pipes[i][1], msg_nave_fin, sizeof(msg_nave_fin));
                        }
			perror("Jefe ha sacado un mensaje invalido");
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_FAILURE); // No deberia que llegar hasta aqui
}

