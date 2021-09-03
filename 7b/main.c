/* Práctica 7b: AmongETSE */

/*
 * Autora: Xiana Carrera
 * Este programa permite jugar a AmongETSE, integrando sus funciones básicas con la estructura de un mapa y
 * posibilitando el movimiento de los jugadores a través del mismo.
 */

/*
 * Actualización:
 *
 * Se ha cambiado el sistema de sabotajes para adaptarlo a las características de la ampliación. La condición de
 * derrota cuando ocurren 4 sabotajes seguidos ya no tiene sentido, pues el hecho de que los impostores deban moverse
 * de una habitación a otra constantemente disminuye significativamente la probabilidad de que esta situación tenga
 * lugar.
 *
 * Con los cambios introducidos, un sabotaje se efectúa si en un turno un impostor se encuentra en una habitación y
 * está solo. En ese caso, se lleva la cuenta de que esta situación ha tenido lugar. Si la próxima vez que un impostor
 * se halle en una habitación (no en tránsito) también está solo, se suma 1 a la cuenta. Puede ocurrir que este
 * segundo impostor sea distinto del primero que cumplió lo establecido. Además, no sería relevante que otro impostor
 * esté en una habitación con tripulantes en ese mismo turno: basta con que tan solo uno verifique la condición.
 *
 * Si dicha cuenta llega a 4, la victoria es para los impostores. Si la cadena se rompe debido a que todos los
 * impostores se encuentran acompañados o en tránsito en un turno, la cuenta se reinicia. Nótese que para que la cadena
 * se quiebre, al menos uno de ellos no debe estar en tránsito.
 */

/*
 * Práctica 3 (descripción original):
 *
 * Se ha desarollado un sistema de sabotajes, que crea una nueva condición de victoria para los impostores e introduce
 * un nuevo elemento estratégico. Asimismo, se le ha dado una nueva funcionalidad a los jugadores ya muertos.
 *
 * Con respecto al sistema de sabotajes, este entra en funcionamiento cuando un impostor se encuentra solo en una
 * habitación (o bien acompañado únicamente de otros impostores). En ese caso, efectúa un sabotaje, añadiendo una tarea
 * a todos los jugadores vivos (tripulantes e impostores), lo que implica que el juego podrá durar un turno más.
 * Pueden ocurrir varios sabotajes en un mismo turno si no hay ningún tripulante en la habitación de varios impostores.
 * En caso de que tenga lugar al menos uno durante 4 turnos seguidos, la victoria será para los impostores, ante
 * la incapacidad de los tripulantes de gestionar la emergencia.
 *
 * El sistema de sabotajes está implementado en las funciones:
 * void _sabotear(abb A, abb B);  --- Añade una tarea a los tripulantes e impostores
 * void _contarTripulantesHabitacion(abb A, char *hab, int *num); --- Función necesaria para contar el número de
 *          tripulantes de una habitación dada. Se le debe pasar el rol a comprobar (en este caso, 'T').
 * void _ejecutarTarea(abb A, abb B, int *contador, int nImpostores, int *sabotaje); --- Cuando el número de
 *          tripulantes en la habitación de un impostor es 0, llama a la función _sabotear y cambia *sabotaje a 1.
 * void jugarPartida(abb *Arbol);  --- Verifica si han ocurrido 4 sabotajes seguidos y, en caso afirmativo, da la
 *          victoria automáticamente a los impostores. Además, añade un turno al juego cada vez que ocurre un sabotaje.
 *
 *
 * Por otro lado, se introduce un mecanismo de salvación, mediante el cual cada uno de los jugadores muertos, es decir,
 * los que han sido asesinados por un impostor (los expulsados no cuentan) tiene una posibilidad de un 10% de evitar
 * la muerte de un tripulante vivo si este iba a morir en el turno. La generación de los resultados se da cada vez que
 * un impostor se dispone a matar a alguien, lo que implica que puede haber más de una ayuda de los fantasmas por turno.
 * Si al menos uno de los fantasmas tiene éxito, el jugador será salvado, independientemente de los resultados del
 * resto. El usuario es notificado de lo que ha ocurrido.
 * Para esta mecánica se introduce un nuevo rol, 'E', que se le asigna a aquellos jugadores que hayan sido expulsados,
 * para distinguirlos de los que hayan sido asesinados.
 *
 * Estos añadidos se implementan en:
 * void _contarJugadoresRol(abb A, char rol, int *cuenta); --- Necesaria para saber el número de fantasmas. Al pasarle
 *          un rol, devuelve el número de jugadores con él en el árbol.
 *  void _ejecutarTarea(abb A, abb B, int *contador, int nImpostores, int *sabotaje); --- Comprueba si la generación
 *          de números aleatorios tiene éxito y, en tal caso, impide una muerte.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h> //para utilizar la semilla aleatoria time(NULL)
#include "abb.h"
#include "FuncionesAmongETSE.h"
#include "grafo.h"
#include "funciones.h"

//Función que imprime el menú de opciones
//Se encuentra en el main y no en FuncionesAmongETSE porque su funcionalidad es intrínseca al primero
void mostrarMenu();

int main(int argc, char** argv) {
    abb arbolJug;           //Árbol global de jugadores
    grafo G;                //Mapa
    char opcion;
    int guardar;

    srand((unsigned int) time(NULL));   //Semilla para generar números aleatorios
    
    //Crear el árbol de jugadores
    crear(&arbolJug);

    //Creo el grafo
    crear_grafo(&G);
    
    //Leer el archivo de disco
    leerArchivo(&arbolJug);

    //Se leen los datos del mapa desde fichero
    leer_archivo(&G);

    //Menú
    printf("\n\nBienvenid@ a AmongETSE");       //Solo se muestra cuando empieza el programa
    do {
        mostrarMenu();
        scanf(" %c", &opcion);

        switch (opcion){
            case 'A':case 'a':
                altaJugador(&arbolJug);
                break;
            case 'B':case 'b':
                bajaJugador(&arbolJug);
                break;
            case 'F':case 'f':
                guardarArchivo(arbolJug);
                break;
            case 'H':case 'h':
                consultarPorHabitacion(arbolJug);
                break;
            case 'J':case 'j':
                jugarPartida(&arbolJug, G);
                break;
            case 'L':case 'l':
                listadoJugadores(arbolJug);
                break;
            case 'M':case 'm':
                introducir_habitacion(&G);
                break;
            case 'N':case 'n':
                eliminar_habitacion(&G);
                break;
            case 'O':case 'o':
                nuevo_arco(&G);
                break;
            case 'P':case 'p':
                eliminar_arco(&G);
                break;
            case 'Q':case 'q':
                imprimir_grafo(G);
                break;
            case 'R':case 'r':
                ruta_minima(G);
                break;
            case 'T':case 't':
                arbol_expansion(G);
                break;
            case 'U':case 'u':
                consultarJugador(arbolJug);
                break;
            case 'S':case 's':
                break;
            default:
                printf("Opcion incorrecta\n");
        }
    } while(opcion != 's' && opcion != 'S');

    printf("Introduzca 1 para guardar mapa: ");
    scanf("%d", &guardar);
    if (guardar == 1) {              //Se guarda el archivo si y solo si se ha introducido 1
        guardar_archivo(G);
    }

    printf("Cerrando AmongETSE...");

    //Destruir el árbol al finalizar
    destruir(&arbolJug);

    //Destruir el grafo
    borrar_grafo(&G);

    return (EXIT_SUCCESS);
}

void mostrarMenu(){
    printf("\n\033[0;35m----------------------- Menu -----------------------");
    printf("\n\ta) Alta de jugador@");
    printf("\n\tb) Baja de jugador@");
    printf("\n\tf) Guardar archivo de usuari@s");
    printf("\n\th) Consulta jugador@s por habitación");
    printf("\n\tj) Jugar partida");
    printf("\n\tl) Listado de jugador@s por orden alfabético");
    printf("\n\tm) Añadir habitacion");
    printf("\n\tn) Clausurar habitacion");
    printf("\n\to) Crear comunicacion");
    printf("\n\tp) Eliminar comunicacion");
    printf("\n\tq) Imprimir mapa");
    printf("\n\tr) Ruta mas rapida entre dos habitaciones");
    printf("\n\tt) Arbol de expansion de coste minimo");
    printf("\n\tu) Consulta por jugador@ de la última tarea asignada");
    printf("\n\ts) Salir de la aplicacion");
    printf("\n----------------------------------------------------\033[0m\n");
    printf("\nOpcion: ");
}

