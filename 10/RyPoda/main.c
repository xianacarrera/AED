#include <stdio.h>
#include <stdlib.h>
#include "ramificacion_poda.h"

/*
 * Práctica 10 - Problema de asignación: resolución mediante estrategia de ramificación y poda
 *
 * Xiana Carrera Alonso
 *
 * Este programa resuelve un problema de asignación de n tareas a n personas utilizando la estrategia de ramificación
 * y poda. Se compara además el número de nodos que se exploran dependiendo de si las estimaciones usadas son
 * triviales o precisas.
 */


void leerMatriz(int M[][N]);
void imprimirMatriz(int M[][N]);

int main() {
    int B[N][N];                // Matriz de beneficios
    int s[N];                   // Tupla con la solución óptima
    int sBact;                  // Beneficio de la solución óptima
    int datos, i, j;            // Elección para la introducción de datos, contadores
    int nodosExplorados = 0;    // Número de nodos explorados en la ejecución


    /********************************** LECTURA DE DATOS **************************************/

    printf("----------- RAMIFICACION Y PODA -----------\n");
    printf("¿Introducir datos de la matriz a mano (1) o desde fichero (0)?: ");
    scanf("%d", &datos);
    while (datos != 0 && datos != 1) {
        printf("Opcion no valida");
        printf("¿Introducir datos de la matriz a mano (1) o desde fichero (0)?\n");
        scanf("%d", &datos);
    }

    if (datos)          // Introducción de datos a mano
        for (i = 0; i < N; i++)
            for (j = 0; j < N; j++) {
                printf("Introduce beneficio para la persona %d y la tarea %d: ", i, j);
                scanf("%d", &B[i][j]);
            }
    else                // Lectura de datos desde fichero
        leerMatriz(B);

    imprimirMatriz(B);      // Se imprime la matriz creada


    /********************************** RAMIFICACIÓN Y PODA TRIVIAL **************************************/

    // Se modifican s y cumplenCriterio y se guarda el beneficio en sBact
    // nodosExplorados se modificará al número de nodos para los que se hayan calculado las cotas (nodos generados)
    sBact = AsignacionRyP_trivial(B, s, &nodosExplorados);

    printf("\n---------- RESULTADOS CON ESTIMACIONES TRIVIALES ----------\n");
    printf("Solución óptima:\n");
    for (i = 0; i < N; i++)
        printf("Persona %d --> Tarea %d\n", i, s[i]);
    printf("Valor de la solución óptima: %d\n", sBact);
    printf("Número de nodos explorados: %d\n", nodosExplorados);


    /********************************** RAMIFICACIÓN Y PODA PRECISA **************************************/

    nodosExplorados = 0;

    // Se modifican s y cumplenCriterio y se guarda el beneficio en sBact
    // nodosExplorados se modificará al número de nodos para los que se hayan calculado las cotas (nodos generados)
    sBact = AsignacionRyP_precisa(B, s, &nodosExplorados);

    printf("\n---------- RESULTADOS CON ESTIMACIONES PRECISAS ----------\n");
    printf("Solución óptima:\n");
    for (i = 0; i < N; i++)
        printf("Persona %d --> Tarea %d\n", i, s[i]);
    printf("Valor de la solución óptima: %d\n", sBact);
    printf("Número de nodos explorados: %d", nodosExplorados);

    return(EXIT_SUCCESS);
}

// Función que lee los datos de una matriz desde un fichero
void leerMatriz(int B[][N]) {
    FILE *fp;
    int i, j;

    fp = fopen("Matriz2.txt", "rt");

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