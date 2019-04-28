#include "simulador.h"
#include "nave.h"

void ejecutar_jefe(int num_jeve) {
	pid_t pid;

	for (int i=0; i<N_NAVES ; i++) {
		pid = fork();
		if (pid < 0) {
			perror("(fork) No se pudo inicializar proceso nave");
			exit(EXIT_FAILURE);
		} else if (pid == 0) {
			crear_nave(num_jefe, i);
			exit(EXIT_SUCCESS);
		} else {
			
		}
	}

	exit(EXIT_SUCCESS);
}
