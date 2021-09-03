//
// Created by Xiana on 09/12/2020.
//

#include "backtracking.h"

/************************* FUNCIONES GENÉRICAS DEL ESQUEMA DE BACKTRACKING **************************************/

// Da lugar a una tupla candidata a solución a partir de un nuevo nodo del nivel actual.
void Generar(int B[][N], int nivel, int s[], int *bact){
    s[nivel]++;        // Se toma la siguiente tarea posible (cuyo índice está entre 0 y N - 1)
    if (!s[nivel])     // Previamente, en la posición no se estaba usando ninguna tarea
        // Se añade el beneficio correspondiente a la persona del nivel y a la tarea s[nivel]
        *bact = *bact + B[nivel][s[nivel]];
    else               // s[nivel] > 0, lo que implica que (s[nivel] - 1) > -1, es decir, había una tarea anteriormente
        // Además de añadir el nuevo beneficio, se resta el remanente correspondiente a la anterior asignación
        *bact = *bact + B[nivel][s[nivel]] - B[nivel][s[nivel] - 1];
}

// Comprueba si la tupla de s cumple los requisitos necesarios para que pueda llegar a ser una solución
int Criterio(int nivel, int s[], int *cumplenCriterio, int *numPasos){
    int i;

    // Se comprueba si en cualquier posición anterior de s hay la misma tarea que la última asignada (s[nivel])
    for (i = 0; i < nivel; i++){
        // El contador del número de pasos se actualiza en cada iteración por si el bucle se termina antes de tiempo
        (*numPasos)++;
        if (s[nivel] == s[i]) return 0;     // No se cumple el criterio
    }
    (*cumplenCriterio)++;                   // Se actualiza el número de nodos que pasan Criterio
    return 1;                               // Se cumple el criterio
}

// Determina si la asignación de tareas a personas realizada es una solución al problema
int Solucion(int nivel, int s[], int *cumplenCriterio, int *numPasos){
    /*
     * Se comprueba que el nivel actual sea N - 1, el de la última persona, esto es, que se haya llegado a los
     * nodos hoja. Además, se llama a Criterio para saber si la tupla s es válida. Esto es necesario porque en el
     * esquema algorítmico se llama a Solucion antes que a Criterio, por lo que no se ha determinado todavía si la
     * última generación cumple los requisitos establecidos.
     */

    // Como se verifica antes nivel == N - 1 que Criterio, cumplenCriterio solo se actualiza para los nodos hoja
    return (nivel == N - 1 && Criterio(nivel, s, cumplenCriterio, numPasos));
}

// Comprueba si quedan nodos sin estudiar que tengan el mismo padre que el nodo actual. Es decir, comprueba si quedan
// combinaciones por probar manteniendo fijos los nivel elementos anteriores en s.
int MasHermanos(int nivel, int s[]){
    // Las tareas tienen asignados índices en el rango [0, N). De un nodo padre salen N tareas, desde 0 hasta N - 1
    return s[nivel] < N - 1;
}

// Deshace los cambios provocados por la última asignación. Se vuelve al nivel anterior del árbol.
void Retroceder(int B[][N], int *nivel, int s[], int *bact){
    *bact = *bact - B[*nivel][s[*nivel]];           // Se elimina el beneficio añadido por la última asignación
    s[*nivel] = -1;                                 // La persona del nivel actual pasa a no tener tarea asignada
    (*nivel)--;                                     // Se vuelve al nivel anterior
}


/***************************** FUNCIONES CON VARIANTE EN CRITERIO PARA OPTIMIZACIÓN *******************************/

// Da lugar a una tupla candidata a solución a partir de un nuevo nodo del nivel actual.
void GenerarUsada(int usada[], int B[][N], int nivel, int s[], int *bact){
    if (s[nivel] != -1)         // La persona tenía ya asignada una tarea, por lo que usada[s[nivel]] era mayor que 0
        usada[s[nivel]]--;      // Se desasignará la tarea, por lo que se usa una vez menos en s
    s[nivel]++;                 // Se toma la siguiente tarea posible (cuyo índice está entre 0 y N - 1)
    usada[s[nivel]]++;          // Se registra el uso de esa nueva tarea (aparece una vez más en s)

    if (!s[nivel])     // Previamente, en la posición no se estaba usando ninguna tarea
        // Se añade el beneficio correspondiente a la persona del nivel y a la tarea s[nivel]
        *bact = *bact + B[nivel][s[nivel]];
    else               // s[nivel] > 0, lo que implica que (s[nivel] - 1) > -1, es decir, había una tarea anteriormente
        // Además de añadir el nuevo beneficio, se resta el remanente correspondiente a la anterior asignación
        *bact = *bact + B[nivel][s[nivel]] - B[nivel][s[nivel] - 1];
}

// Comprueba si la tupla de s cumple los requisitos necesarios para que pueda llegar a ser una solución
int CriterioUsada(int usada[], int nivel, int s[], int *cumplenCriterio, int *numPasos){
    if (usada[s[nivel]] == 1)       // La tarea solo se usa una vez
        (*cumplenCriterio)++;       // Si se cumple el criterio, se añade 1 al contador de nodos válidos

    // Se aumenta el contador del número de pasos (no se incluyen aquellas operaciones relativas a cumplenCriterio)
    (*numPasos) += 1;

    // Devuelve 1 si la tarea del nivel actual solo se usó una vez, y 0 si aparece más veces en la tupla solución
    return usada[s[nivel]] == 1;
}

// Determina si la asignación de tareas a personas realizada es una solución al problema
// Aunque tiene la misma implementación que Solucion, se distinguen entre si llaman a Criterio o CriterioUsada
int SolucionUsada(int *usada, int nivel, int *s, int *cumplenCriterio, int *numPasos){
    /*
     * Se comprueba que el nivel actual sea N - 1, el de la última persona, esto es, que se haya llegado a los
     * nodos hoja. Además, se llama a Criterio para saber si la tupla s es válida. Esto es necesario porque en el
     * esquema algorítmico se llama a Solucion antes que a Criterio, por lo que no se ha determinado todavía si la
     * última generación cumple los requisitos establecidos.
     */

    // Como se comprueba antes nivel == N - 1 que Criterio, cumplenCriterio solo se actualiza para los nodos hoja
    return (nivel == N - 1 && CriterioUsada(usada, nivel, s, cumplenCriterio, numPasos));
}

// Deshace los cambios provocados por la última asignación. Se vuelve al nivel anterior del árbol.
void RetrocederUsada(int usada[], int B[][N], int *nivel, int s[], int *bact){
    *bact = *bact - B[*nivel][s[*nivel]];    // Se elimina el beneficio añadido por la última asignación
    usada[s[*nivel]]--;                      // Se deja de usar la tarea que había. Aparecerá una vez menos en s
    s[*nivel] = -1;                          // La persona del nivel actual pasa a no tener tarea asignada
    (*nivel)--;                              // Se vuelve al nivel anterior
}