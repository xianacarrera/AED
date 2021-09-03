#ifndef FUNCIONESAMONGETSE_H
#define FUNCIONESAMONGETSE_H

#include "abb.h"

/**
 * lee un archivo de jugadores de un fichero
 * @param A
 */
void leerArchivo(abb *A);

/**
 * Guarda la lista de usuarios en el fichero jugadores.txt
 * @param A
 */
void guardarArchivo(abb A);

/**
 * Inserta un nuevo jugador en el árbol
 * @param A
 */
void altaJugador(abb *A);

/**
 * Elimina un jugador en el árbol
 * @param A
 */
void bajaJugador(abb *A);

/**
 * Imprime en inorden (alfabético) los nodos del árbol
 * @param A
 */
void listadoJugadores(abb A);

/**
 * Ejecuta una partida de AmongETSE
 * @param A
 */
void jugarPartida(abb *A);

/**
 * Consulta los datos de un jugador
 * @param A
 */
void consultarJugador(abb A);

/**
 * Consulta los jugadores que hay en una habitación
 * @param A
 */
void consultarPorHabitacion(abb A);


#endif	// FUNCIONESAMONGETSE_H

