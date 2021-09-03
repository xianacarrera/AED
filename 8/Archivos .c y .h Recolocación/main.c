#include <stdio.h>
#include <time.h>
#include "tabla_hash_recolocacion.h"

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
        for (i = 1; i <= 10000; i++) {              // Podemos usar un for al saber el número de datos
            fscanf(fp, "%s %s %s - %s - %s", jugador.nombre, desechable1, desechable2,
                   jugador.clave, jugador.correo);      // Los apellidos no se guardan para ahorrar memoria
            colisiones += InsertarHash(t, jugador, &adicionales);      // Se inserta y se registran la posible colisión
            adicionalesTot += adicionales;                             // Se guardan los pasos adicionales
        }

        printf("---------- Resultados ----------\n");
        printf("Colisiones totales: %d\n", colisiones);
        printf("Pasos adicionales totales: %d\n", adicionalesTot);


        rewind(fp);             // Se vuelve al inicio del archivo
        adicionalesTot = 0;

        // Se empieza a contar el tiempo de búsqueda
        inicio = clock();
        for (i = 1; i <= 10000; i++){
            fscanf(fp, "%s %s %s - %s - %s", jugador.nombre, desechable1, desechable2,
                   jugador.clave, jugador.correo);
            Busqueda(t, jugador.clave, &jugador, &adicionales);         // Se busca el dato
            adicionalesTot += adicionales;             // Se guarda el número de pasos adicionales necesarios
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

    return 0;
}