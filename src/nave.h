#ifndef SRC_NAVE_H_
#define SRC_NAVE_H_



// funcion para cambiar posicion de la nave
int nave_cambiarposicion(tipo_nave *nave, int posy, int posx);
// Intenta a atacar a una posicion con la nave elegido
int nave_atacar(tipo_mapa *mapa, tipo_nave *nave, int targety, int targetx);

// Mover nave a la casilla indicada
int nave_mover(tipo_mapa *mapa, tipo_nave *nave, int targety, int targetx);

// Mover nave aleatoriamente a las casilla adyacentes
int nave_mover_aleatorio(tipo_nave *nave);

// Destruye una nave librando sus recursos
int nave_destruir(tipo_nave *nave);
#endif /* SRC_NAVE_H_ */
