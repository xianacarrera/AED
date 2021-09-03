//
// Created by Xiana on 09/12/2020.
//

#ifndef VUELTAATRAS_BACKTRACKING_H
#define VUELTAATRAS_BACKTRACKING_H

#define N 3             // Orden de la matriz = Número de personas = Número de tareas = Número de niveles del árbol
#define NINF 0          // Representa -infinito

/************************* FUNCIONES GENÉRICAS DEL ESQUEMA DE BACKTRACKING **************************************/

// Función que utiliza un nuevo nodo del nivel actual para dar lugar a una nueva asignación personas - tareas.
void Generar(int B[][N], int nivel, int s[], int *bact);

// Función que comprueba si la tupla s podría llegar a ser solución.
int Criterio(int nivel, int s[], int *cumplenCriterio, int *numPasos);

// Función que verifica si la tupla s es una solución válida al problema.
int Solucion(int nivel, int s[], int *cumplenCriterio, int *numPasos);

// Función que comprueba si quedan más nodos hermanos del actual por estudiar.
int MasHermanos(int nivel, int s[]);

// Función que vuelve atrás un nivel en el árbol, deshaciendo los cambios de las asignaciones del nivel actual.
void Retroceder(int B[][N], int *nivel, int s[], int *bact);


/***************************** FUNCIONES CON VARIANTE EN CRITERIO PARA OPTIMIZACIÓN *******************************/

// Función que utiliza un nuevo nodo del nivel actual para dar lugar a una nueva asignación personas - tareas.
// Se utiliza un array, usada[], para registrar qué tareas han sido ya repartidas.
void GenerarUsada(int usada[], int B[][N], int nivel, int s[], int *bact);

// Función que comprueba si la tupla s podría llegar a ser solución.
// Se utiliza un array, usada[], para registrar qué tareas han sido ya repartidas.
int CriterioUsada(int usada[], int nivel, int s[], int *cumplenCriterio, int *numPasos);

// Función que verifica si la tupla s es una solución válida al problema.
// Se utiliza un array, usada[], para registrar qué tareas han sido ya repartidas.
int SolucionUsada(int *usada, int nivel, int *s, int *cumplenCriterio, int *numPasos);

// Función que vuelve atrás un nivel en el árbol, deshaciendo los cambios de las asignaciones del nivel actual.
// Se utiliza un array, usada[], para registrar qué tareas han sido ya repartidas.
void RetrocederUsada(int usada[], int B[][N], int *nivel, int s[], int *bact);

#endif //VUELTAATRAS_BACKTRACKING_H
