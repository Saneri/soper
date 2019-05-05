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

	for (int i=0; i<N_NAVES ; i++) {
		pid = fork();
		if (pid < 0) {
			perror("(fork) No se pudo inicializar proceso nave");
			exit(EXIT_FAILURE);
		} else if (pid == 0) {
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

	exit(EXIT_SUCCESS);
}

