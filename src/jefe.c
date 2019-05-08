#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "simulador.h"
#include "nave.h"

/*
 * @brief 
 * @param i el numero identificador del jefe
 */
void ejecutar_jefe(int num_jefe) {
	pid_t pid;

	int pipes[N_NAVES][2];

	for (int i=0; i<N_NAVES ; i++) {
		pid = fork();
		if (pid < 0) {
			perror("(fork) No se pudo inicializar proceso nave");
			exit(EXIT_FAILURE);
		} else if (pid == 0) {
			printf("CREANDO NAVE %d %d\n", num_jefe, i);
			//ejecutar_nave(num_jefe, i);	
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

	// Iniciar tuberia para escuchar a simulador (el proceso padre)
	int fd[2];
	int pipe_status = pipe(fd);
	if (pipe_status < 0) {
		perror("(pipe) No se pudo inicializar pipe del jefe");
		exit(EXIT_FAILURE);
	}
	close(fd[1]); // Cierra la salida del pipe
	
	msg_simulador msg_sim;

	// La rutina del jefe
	while (1) {
		printf("Sim Jefe %d: leyendo siguente mensaje del PIPE", num_jefe);
		read(fd[0], (char*) &msg_sim, sizeof(msg_simulador));
		if (strcmp(msg_sim.msg, "TURNO") == 0) {
			// Enviar MOVER_ALEATORIO y ATACAR
		} else if (strcmp(msg_sim.msg, "FIN") == 0) {
			// Enviar FIN y acabar este proceso
			
			exit(EXIT_SUCCESS);
		}

	}
	exit(EXIT_FAILURE); // No deberia que llegar hasta aqui
}

