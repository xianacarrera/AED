#include <stdio.h>
#include <stdlib.h>
#include "backtracking.h"


/*
 * Práctica 9 - Problema de asignación: resolución mediante estrategia de vuelta atrás
 *
 * Xiana Carrera Alonso
 */

/*
 * Este programa resuelve un problema de asignación de N tareas a N personas, donde cada asignación persona - tarea
 * lleva asociado un beneficio. El objetivo será maximizar el beneficio total.
 *
 * Los datos a introducir son el número N de tareas y personas (que se debe cambiar en el archivo backtracking.h) y
 * la matriz de beneficios con los datos del problema, que se puede introducir a mano o desde un archivo, con cada
 * elemento en una línea diferente. Se insertarán de izquierda a derecha en la matriz.
 *
 * Como resultado se dará: la solución óptima, esto es, las asignaciones que le corresponden; su valor asociado o
 * beneficio total, y el número de nodos del árbol permutacional que sirve de representación que verfican la función
 * Criterio.
 */


void leerMatriz(int B[][N]);
void imprimirMatriz(int B[][N]);

int main() {
    int B[N][N];                                // Matriz de beneficios
    int s[N], soa[N], usada[N];                 // Arrays: solución, solución óptima actual, tareas usadas
    int bact, voa, nivel;                       // Beneficio acumulado, valor óptimo actual, nivel actual
    int cumplenCriterio, numPasos;              // Número de nodos que pasan la función Criterio y su número de pasos
    int datos, i, j;                            // Opción para introducir datos, contadores
    char opt;                                   // Opción para usar el array usada[] y optimizar Criterio


    /********************************** LECTURA DE DATOS **************************************/

    /********* Inicialización de variables ******/
    nivel = 0;                   // Se empieza en el nivel correspondiente a la primera persona
    for (i = 0; i < N; i++)      // El array donde se guardan las asignaciones empieza con todas las posiciones a -1
        s[i] = -1;               // Se utiliza -1 commo indicativo de que la persona con índice i no tiene tarea
    voa = NINF;                  // El valor óptimo actual para las soluciones es -infinito (cualquier valor es mejor)
    bact = 0;                    // Beneficio acumulado nulo
    cumplenCriterio = 0;         // Se inicializa el contador de nodos
    numPasos = 0;                // Se inicializa el contador de pasos de Criterio


    /******** Lectura de la matriz de beneficios ****/

    printf("----------- VUELTA ATRÁS -----------\n");
    printf("¿Introducir datos de la matriz a mano (1) o desde fichero (0)?: ");
    scanf("%d", &datos);
    while (datos != 0 && datos != 1) {
        printf("Opción no válida");
        printf("¿Introducir datos de la matriz a mano (1) o desde fichero (0)?\n");
        scanf("%d", &datos);
    }

    if (datos)          // Se introducen los datos a mano
        for (i = 0; i < N; i++)
            for (j = 0; j < N; j++) {
                printf("Introduce beneficio para la persona %d y la tarea %d: ", i, j);
                scanf("%d", &B[i][j]);
            }
    else                // Se lee la matriz desde un fichero
        leerMatriz(B);

    imprimirMatriz(B);          // En cualquier caso, se imprime la matriz resultante


    /****** Elección para emplear usada[] ********/

    printf("¿Optimizar Criterio? (s/n): ");
    scanf(" %c", &opt);
    while (opt != 's' && opt != 'S' && opt != 'n' && opt != 'N'){
        printf("Opción incorrecta");
        printf("¿Optimizar Criterio? (s/n): ");
        scanf(" %c", &opt);
    }

    /********************************** BACTRACKING **************************************/

    switch(opt) {
        case 'n':
        case 'N':                   // No se utiliza usada[]
            while (nivel > -1) {    // Condición de parada
                Generar(B, nivel, s, &bact);        // Se genera una nueva combinación
                if (Solucion(nivel, s, &cumplenCriterio, &numPasos) && bact > voa) {
                    // SolucionUsada llama a CriterioUsada. cumplenCriterio se actualiza si es necesario
                    // Se registra el incremento en pasos totales
                    // Se comprueba si la combinación generada es solución y si esta es mejor que la óptima actual

                    voa = bact;             // Se actualiza el valor óptimo
                    for (i = 0; i < N; i++)
                        soa[i] = s[i];      // Se guarda la nueva solución
                }
                if (nivel < N - 1 && Criterio(nivel, s, &cumplenCriterio, &numPasos)) {
                    // cumplenCriterio se actualiza en la propia función Criterio, si es necesario
                    // Se pasa antes nivel < N - 1 para que cumplenCriterio solo se incremente para los nodos internos
                    // Se actualiza el número de pasos totales
                    // Si el nodo cumple el criterio y no es un nodo hoja, se pasa al siguiente nivel del árbol
                    nivel++;
                }
                while (!MasHermanos(nivel, s) && nivel > -1)    // No quedan más hermanos, el nivel es válido
                    Retroceder(B, &nivel, s, &bact);            // Se retrocede en el árbol
            }
            break;
        case 's':
        case 'S':               // Se utiliza usada[]

            // Inicialización de usada
            for (i = 0; i < N; i++)
                usada[i] = 0;

            while (nivel > -1) {            // Condición de parada
                GenerarUsada(usada, B, nivel, s, &bact);        // Se genera una nueva combinación
                if (SolucionUsada(usada, nivel, s, &cumplenCriterio, &numPasos) && bact > voa) {
                    // SolucionUsada llama a CriterioUsada. cumplenCriterio se actualiza si es necesario
                    // Se registra el incremento en pasos totales
                    // Se comprueba si la combinación generada es solución y si esta es mejor que la óptima actual

                    voa = bact;                 // Se actualiza el valor óptimo
                    for (i = 0; i < N; i++)
                        soa[i] = s[i];          // Se guarda la nueva solución
                }
                if (nivel < N - 1 && CriterioUsada(usada, nivel, s, &cumplenCriterio, &numPasos)) {
                    // cumplenCriterio se actualiza en la propia función Criterio, si es necesario
                    // Se pasa antes nivel < N - 1 para que cumplenCriterio solo se incremente para los nodos internos
                    // Se actualiza también el número de pasos totales
                    // Si el nodo cumple el criterio y no es un nodo hoja, se pasa al siguiente nivel del árbol
                    nivel++;
                }
                while (!MasHermanos(nivel, s) && nivel > -1)         // No quedan más hermanos, el nivel es válido
                    RetrocederUsada(usada, B, &nivel, s, &bact);     // Se retrocede en el árbol
            }
    }

    /********************************** RESULTADOS **************************************/

    printf("\n---------- RESULTADOS ----------\n");
    printf("Valor de la solución óptima: %d\n", voa);
    printf("Solución óptima:\n");
    for (i = 0; i < N; i++)
        printf("Persona %d --> Tarea %d\n", i, soa[i]);
    printf("Número de nodos que cumplen el criterio: %d\n", cumplenCriterio);
    printf("Número de pasos totales efectuados por Criterio: %d", numPasos);


    return(EXIT_SUCCESS);
}

// Función que lee los datos de una matriz desde un fichero
void leerMatriz(int B[][N]) {
    FILE *fp;
    int i, j;

    fp = fopen("Matriz.txt", "rt");

    if (fp)         // Apertura correcta del fichero
        for (i = 0; i < N; i++)
            for (j = 0; j < N; j++)
                fscanf(fp, "%d", &B[i][j]);
    else {
        printf("Error en la lectura del archivo");
        exit(EXIT_FAILURE);         // Se cierra el programa
    }
}

// Función que imprime los datos de una matriz
void imprimirMatriz(int B[][N]){
    int i, j;

    printf("\nMatriz de beneficios:\n");
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++)
            printf("%5d", B[i][j]);     // A cada elemento le corresponden 5 espacios
        printf("\n");
    }
    printf("\n");
}