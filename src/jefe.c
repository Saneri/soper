#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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
			printf("CREANDO NAVE\n");

			// Inicializar tuberias para comunicar con las naves
			int pipe_status = pipe(pipes[i]);
			if (pipe_status < 0) {
				perror("(pipe) No se pudo inicializar pipe de la nave");
				return;
			}
			close(pipes[i][0]);

			//crear_nave(num_jefe, i);
			exit(EXIT_SUCCESS);
		} else {
			
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

	printf("JEFE LISTO");
	exit(EXIT_SUCCESS);
}

