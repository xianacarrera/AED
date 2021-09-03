//
// Created by Xiana on 16/12/2020.
//


#ifndef RYPODA_RAMIFICACION_PODA_H
#define RYPODA_RAMIFICACION_PODA_H

#include "lista.h"

// Devuelve el elemento máximo de una matriz de enteros
int maxMatriz(int B[][N]);

// Calcula el beneficio estimado para un nodo (tanto para estimaciones triviales como precisas)
// Se modifica el propio nodo para guardar el resultado
void BE(tipoelem *x);

// Calcula la cota inferior trivial para un nodo
// Se modifica el propio nodo para guardar el resultado
void CI_trivial(tipoelem *x);

// Calcula la cota superior trivial para un nodo
// Se modifica el propio nodo para guardar el resultado
void CS_trivial(tipoelem *x, int maxB);

// Calcula la cota inferior precisa para un nodo
// Se modifica el propio nodo para guardar el resultado
void CI_precisa(tipoelem *x, int B[][N]);

// Calcula la cota superior precisa para un nodo
// Se modifica el propio nodo para guardar el resultado
void CS_precisa(tipoelem *x, int B[][N]);

// Determina si un nodo es una solución válida al problema
// Devuelve 1 en caso afirmativo y 0 en caso negativo
int Solucion(tipoelem x);

// Escoge un nodo de la lista de nodos vivos siguiendo una estrategia de ramificación MB-LIFO
// Devuelve dicho nodo
tipoelem Seleccionar(lista LNV);

// Algoritmo de ramificación y poda empleando estimaciones triviales
// Modifica s con la solución óptima y nodosExplorados con el número de nodos para el que se han calculado las cotas.
// Devuelve el valor de la solución óptima
int AsignacionRyP_trivial(int B[][N], int s[], int *nodosExplorados);

// Algoritmo de ramificación y poda empleando estimaciones precisas
// Modifica s con la solución óptima y nodosExplorados con el número de nodos para el que se han calculado las cotas.
// Devuelve el valor de la solución óptima
int AsignacionRyP_precisa(int B[][N], int s[], int *nodosExplorados);

#endif //RYPODA_RAMIFICACION_PODA_H