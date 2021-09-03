#ifndef FUNCIONES_H
#define FUNCIONES_H

#include "grafo.h"

//FUNCIONES DEL PROGRAMA DE PRUEBA DE GRAFOS
//Opción a del menú, introducir un vértice (habitación) en el grafo
void introducir_habitacion(grafo *G);

//Opción b del menú, eliminar un vértice (habitación) del grafo
void eliminar_habitacion(grafo *G);

//Opción c del menú, crear una relación entre dos vértices
void nuevo_arco(grafo *G);

//Opción d del menú, eliminar una relación entre dos vértices
void eliminar_arco(grafo *G);

//Opción i del menú, imprimir el grafo
void imprimir_grafo(grafo G);

//Ejecución automática inicial, leer un archivo con los datos de un grafo
void leer_archivo(grafo *G);

//Ejecución automática final, guardar datos del grafo en un archivo
void guardar_archivo(grafo G);


#endif	/* FUNCIONES_H */

