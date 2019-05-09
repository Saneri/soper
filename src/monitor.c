/*
 * @file monitor.c
 * @author Santeri Suitiala & Roberto Pirck Valdés, grupo 5, practicas 2212
 * @date 9 de Mayo 2019
 * @brief El fichero que maneja la interfaz de usuario de este juego
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
#include <semaphore.h>

#include <simulador.h>
#include <gamescreen.h>
#include <mapa.h>

int running = 1;
int recurso_mmap = 0;
int recurso_sem = 1;
sem_t *sem = NULL;
tipo_mapa *mapa;

/*
 * @brief La rutina para mostrar la interfaz de usuario 
 * @param *mapa la mapa que se quiere mostrar
 */
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


/*
 *@brief Para el bucle principal si el usador envia SIGINT (ctrl + C)
 */
void manejador_SIGINT_monitor(int sig) {
	running = 0;
}

/*
 * @brief inicializar monitor y ejecutar la rutina de mostrar el juegoi
 * @return 0 si todo ha sido bien, -1 en el caso de error
 */
int init_monitor() {
	struct sigaction act;
	act.sa_handler = manejador_SIGINT_monitor;
	sigemptyset(&(act.sa_mask));
	act.sa_flags = 0;
	int error = sigaction(SIGINT, &act, NULL);
	if (error < 0) {
		perror("No se pudo agregar el manejador SIGINT");
		return -1;
	}

	if ((sem = sem_open(SEM_SYNC_MONITOR, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
		perror("(sem_open) No se pudo abrir semaforo");
		return -1;
	}
	
	sem_wait(sem);   // Espera a que simulador inicialize
	
	int fd_shm = shm_open(SHM_MAP_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	if (fd_shm < 0) {
		perror("(shm_open) No se pudo abrir la memoria compartida");
		exit(EXIT_FAILURE);
	}
	mapa = mmap(NULL, sizeof(tipo_mapa), PROT_WRITE | PROT_READ, MAP_SHARED, fd_shm, 0);
	if (mapa == MAP_FAILED) {
		perror("(mmap) No se pudo mapear la memoria compartida de mapa");
		return -1;
	}
	recurso_mmap = 1;

	if (ftruncate(fd_shm, sizeof(tipo_mapa)) == -1) {
		perror("(ftruncate) monitor failed to resize memory");
		munmap(mapa, sizeof(tipo_mapa));
		exit(EXIT_FAILURE);
	}
	
	
	screen_init();
	
	// La rutina de mostrar mapa
	while (running) {
		mapa_print(mapa);
		usleep(SCREEN_REFRESH);
	}

	screen_end();
	return 0;
}

int main() {
	
	if (init_monitor() < 0) {
		perror("Monitor ha producido un error inesperado\n");
	}
	
	if (recurso_mmap) {
		munmap(mapa, sizeof(tipo_mapa));
	}
	if (recurso_sem) {
		sem_close(sem);
		sem_unlink(SEM_SYNC_MONITOR);
	}
}
