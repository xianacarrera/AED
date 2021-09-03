#include <stdio.h>
#include <time.h>
#include "tabla_hash_encadenamiento.h"

/*
 * Práctica 8 - Análisis de distintas implementaciones de tablas hash
 *
 * Xiana Carrera Alonso
 */

int main() {
    int i, colisiones = 0, adicionales, adicionalesTot = 0;         // Contadores
    tipo_jugador jugador;                                           // Elemento leído
    FILE *fp;                                                       // Archivo
    TablaHash t;
    char desechable1[15], desechable2[15];                          // Datos no utilizados
    clock_t inicio, fin;                                            // Medición de tiempos de búsqueda
    double tiempoConsumido;

    //Creación de la tabla
    InicializarTablaHash(t);

    //Lectura de los datos
    fp = fopen("jugadores_amongETSE.txt", "rt");

    if (fp) {
        for (i = 1; i <= 10000; i++) {
            // Se lee un elemento y se guardan todos sus datos en un jugador
            fscanf(fp, "%s %s %s - %s - %s", jugador.nombre, jugador.apellido1, jugador.apellido2,
                   jugador.clave, jugador.correo);
            colisiones += InsertarHash(&t, jugador);        // Se inserta y se actualizan las colisiones
        }

        printf("---------- Inserción ----------\n");
        printf("Colisiones totales: %d\n", colisiones);

        rewind(fp);             // Se vuelve al inicio del archivo
        adicionalesTot = 0;

        inicio = clock();           // Empieza la medición del tiempo de la búsqueda
        for (i = 1; i <= 10000; i++){       // Se leen todos los datos de nuevo y se buscan en la tabla
            fscanf(fp, "%s %s %s - %s - %s", jugador.nombre, jugador.apellido1, jugador.apellido2,
                   jugador.clave, jugador.correo);
            Busqueda(t, jugador.clave, &jugador, &adicionales);
            adicionalesTot += adicionales;
        }

        fin=clock();
        tiempoConsumido = ((double)(fin-inicio))/CLOCKS_PER_SEC;            // Tiempo en segundos
        printf("\n---------- Búsqueda ----------\n");
        printf("Tiempo consumido: %lf s\n", tiempoConsumido);
        printf("Número de pasos adicionales: %d\n", adicionalesTot);

        fclose(fp);

    } else {
        printf("Error en la apertura del archivo\n");
    }

    // Se destruye la tabla
    DestruirTablaHash(t);

    return 0;
}