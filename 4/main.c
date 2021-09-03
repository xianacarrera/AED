/* Práctica 4: AmongETSE (versión 2) */

/*
 * Autora: Xiana Carrera
 * Este programa genera los datos iniciales de una partida de AmongETSE, incluyendo la asignación de 5 tareas
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h> //para utilizar la semilla aleatoria time(NULL)
#include "abb.h"
#include "FuncionesAmongETSE.h"

//Función que imprime el menú de opciones
//Se encuentra en el main y no en FuncionesAmongETSE porque su funcionalidad es intrínseca al primero
void mostrarMenu();

int main(int argc, char** argv) {
    abb arbolJug;           //Árbol global de jugadores
    char opcion;

    srand((unsigned int) time(NULL));   //Semilla para generar números aleatorios
    
    //Crear el árbol de jugadores
    crear(&arbolJug);
    
    //Leer el archivo de disco
    leerArchivo(&arbolJug);
    
    //Menú
    printf("\n\nBienvenid@ a AmongETSE");       //Solo se muestra cuando empieza el programa
    do {
        mostrarMenu();
        scanf(" %c", &opcion);

        switch (opcion){
            case 'A':
            case 'a':
                altaJugador(&arbolJug);
                break;
            case 'B':
            case 'b':
                bajaJugador(&arbolJug);
                break;
            case 'F':
            case 'f':
                guardarArchivo(arbolJug);
                break;
            case 'G':
            case 'g':
                generarPartida(&arbolJug);
                break;
            case 'H':
            case 'h':
                consultarPorHabitacion(arbolJug);
                break;
            case 'L':
            case 'l':
                listadoJugadores(arbolJug);
                break;
            case 'U':
            case 'u':
                consultarJugador(arbolJug);
                break;
            case 'S':
            case 's':
                printf("Cerrando AmongETSE...");
                break;
            default:
                printf("Opción incorrecta\n");
        }
    } while(opcion != 's' && opcion != 'S');

    //Destruir el árbol al finalizar
    destruir(&arbolJug);

    return (EXIT_SUCCESS);
}

void mostrarMenu(){
    printf("\n----------------------- Menú -----------------------");
    printf("\n\ta) Alta de jugador@");
    printf("\n\tb) Baja de jugador@");
    printf("\n\tf) Guardar archivo");
    printf("\n\tg) Generar datos partida");
    printf("\n\th) Consulta jugador@s por habitación");
    printf("\n\tl) Listado de jugador@s por orden alfabético");
    printf("\n\tu) Consulta por jugador@ de la última tarea asignada");
    printf("\n\ts) Salir de la aplicación");
    printf("\n----------------------------------------------------\n");
    printf("\nOpción: ");
}

