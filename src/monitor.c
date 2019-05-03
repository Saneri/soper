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

#include <simulador.h>
#include <gamescreen.h>
#include <mapa.h>


void mapa_print(tipo_mapa *mapa)
{
	int i,j;

	for(j=0;j<MAPA_MAXY;j++) {
		for(i=0;i<MAPA_MAXX;i++) {
			tipo_casilla cas=mapa_get_casilla(mapa,j, i);
			//printf("%c",cas.simbolo);
			screen_addch(j, i, cas.simbolo);
		}
		//printf("\n");
	}
	screen_refresh();
}

tipo_mapa* open_map() {
	tipo_mapa *mapa;
	int fd_shm = shm_open(SHM_MAP_NAME, O_RDONLY, S_IRUSR | S_IWUSR);
	if (fd_shm < 0) {
		perror("(shm_open) No se pudo abrir la memoria compartida");
		exit(EXIT_FAILURE);
	}
	mapa = mmap(NULL, sizeof(tipo_mapa), PROT_WRITE | PROT_READ, MAP_SHARED, fd_shm, 0);
	return mapa;
}

int main() {
	// Inicializacion
	tipo_mapa* mapa = open_map();
	screen_init();
	
	// La rutina de mostrar mapa
	mapa_print(mapa);

	// Finalizacion
	screen_end();


	exit(EXIT_SUCCESS);
}
