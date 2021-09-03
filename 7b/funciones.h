#ifndef FUNCIONES_H
#define FUNCIONES_H

#include "grafo.h"

///// FUNCIONES PARA EL MAPA

//Introducir un vértice (habitación) en el grafo
void introducir_habitacion(grafo *G);

//Eliminar un vértice (habitación) del grafo
void eliminar_habitacion(grafo *G);

//Crear una relación entre dos vértices
void nuevo_arco(grafo *G);

//Eliminar una relación entre dos vértices
void eliminar_arco(grafo *G);

//Leer un archivo con los datos de un grafo
void leer_archivo(grafo *G);

//Guardar datos del grafo en un archivo
void guardar_archivo(grafo G);

//Imprimir el grafo
void imprimir_grafo(grafo G);

//Calcular menor subconjunto del grafo con todos los vértices conectados y los arcos de menor valor
void arbol_expansion(grafo G);

//Calcular camino más corto entre dos vértices introducidos por el usuario
void ruta_minima(grafo G);

//Función que calcula el camino más corto entre origen y destino
char *floyd(grafo G, int origen, int destino, char tipo);


#endif	/* FUNCIONES_H */

