#include <stdio.h>
#include <stdlib.h>
#include "funciones.h"

/*
 * Práctica 6 (G1) - AEMaps V1
 * Xiana Carrera Alonso
 */

/*
 * Programa que permite manejar un mapa a través del uso de un TAD grafo de números enteros. Se dan las opciones de
 * ampliar el mapa, reducirlo, mostrarlo por pantalla y leerlo y guardarlo en un archivo.
 */

void imprimirMenu();

int main(int argc, char** argv) {
    //Grafo de números enteros
    grafo G;                //Grafo
    char opcion;            //Manejo del menú
    int guardar;            //Elección sobre si se desea guardar los cambios del mapa o no

    //Creo el grafo
    crear_grafo(&G);

    //Se leen los datos desde fichero
    leer_archivo(&G);

    printf("\n\nBienvenido a AmongETSE");
    do {
        imprimirMenu();
        scanf(" %c", &opcion); 

        switch (opcion) {
            case 'a': case'A':
                introducir_habitacion(&G);
                break;
            case 'b': case 'B':
                eliminar_habitacion(&G);
                break;
            case 'c': case 'C':
                nuevo_arco(&G);
                break;
            case 'd': case 'D':
                eliminar_arco(&G);
                break;
            case 'i': case 'I':
                imprimir_grafo(G);
                break;
            case 's': case 'S':
                opcion='s';
                break;
            default:
                printf("Opción errónea\n");
        }
    } while (opcion != 's');

    printf("Introduzca 1 para guardar mapa: ");
    scanf("%d", &guardar);
    if (guardar) {
        guardar_archivo(G);
    }

    printf("Cerrando programa...");

    //Al salir, liberamos la memoria del TAD, lo destruimos
    borrar_grafo(&G);

    return (EXIT_SUCCESS);
}

void imprimirMenu(){
    printf("\n\033[0;35m----------------------- Menu -----------------------\n");
    printf("a. Añadir habitacion\n");
    printf("b. Clausurar habitacion\n");
    printf("c. Crear comunicacion\n");
    printf("d. Eliminar comunicacion\n");
    printf("i. Imprimir mapa\n");
    printf("s. Salir");
    printf("\n----------------------------------------------------\033[0m\n");
    printf("Opcion: ");
}

