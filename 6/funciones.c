#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

//FUNCIONES DEL PROGRAMA DE PRUEBA DE GRAFOS

//Función privada que permite leer un grafo guardado como texto en un formato predefinido
void _leerGrafo(FILE *fp, grafo *G) {
    char linea[45];          //20 por cada una de las 2 habitaciones + 5 por los símbolos extra
    char codigos[2];         //Caracteres que separan la habitacion 1 de valor
    int valor;
    tipovertice v1, v2;      //Vértices que almacenan la información leída

    //Formato: Habitacion1%%valor-Habitacion2, donde %% = .. o %% = --

    fscanf(fp, "%s", v1.habitacion);      //Se lee una línea
    while (strcmp(v1.habitacion, "*") != 0) {      //Vértices iniciales
        //No hace falta verificar que no se haya llegado al final del archivo, porque antes tendrá que aparecer *
        if (!existe_vertice(*G, v1)) {             //El vértice no ha sido previamente insertado
            insertar_vertice(G, v1);
        }
        fscanf(fp, "%s", v1.habitacion);           //Siguiente línea
    }

    fscanf(fp, "%s", linea);      //Se lee una nueva línea
    while (!feof(fp)) {           //No se ha llegado al final del archivo. Líneas con información sobre los arcos
        sscanf(linea, "%[^-.]%[-.]%d-%s", v1.habitacion, codigos, &valor, v2.habitacion);
        //Se leen los caracteres iniciales hasta encontrar un . o un - y se almacenan en v1.habitacion
        //Los dos caracteres que indican si el arco es tripulantes (--) o de impostores (..) se guardan en codigos
        //A continuación aparece un dígito, el valor del arco
        //Por último, después de un guion que no se almacena, se encuentra la cadena correspondiente a v2.habitacion

        if (!existe_vertice(*G, v1) || !existe_vertice(*G, v2)) {
            printf("Se ha intentado crear un arco para una habitacion inexistente\n");
            exit(0);
        }

        switch (codigos[0]) {
            case '-':           //Todos los jugadores. Se inserta en las matrices AT y AI, sin sobreescrituras
                if (!distancia_T(*G, posicion(*G, v1), posicion(*G, v2))) {
                    //Si el arco leído ya se había guardado, se desecha la línea del fichero
                    crear_arco_T(G, posicion(*G, v1), posicion(*G, v2), valor);

                    if (!distancia_I(*G, posicion(*G, v1), posicion(*G, v2))) {
                        //Se inserta también el arco en la matriz de impostores
                        crear_arco_I(G, posicion(*G, v1), posicion(*G, v2), valor);
                    }
                    //Si los impostores ya tenían una rejilla de ventilación en esa comunicación, no se sobreescribe
                }
                //Si el arco ya existía previamente en AT y, por tanto, también en AI, no se sobreescribe
                break;
            case '.':           //Impostores
                if (!distancia_I(*G, posicion(*G, v1), posicion(*G, v2)) ||
                    (distancia_I(*G, posicion(*G, v1), posicion(*G, v2)) ==
                     distancia_T(*G, posicion(*G, v1), posicion(*G, v2)) && valor <
                                                                            distancia_I(*G, posicion(*G, v1),
                                                                                        posicion(*G, v2))))
                    /* Solo se añadirá una rejilla si no había ningún arco (distancia 0) o si el que existía previamente
                     * era una comunicación de tripulantes e impostores, evitando sobreescrituras de los datos propios
                     * de AI. En este último caso, se exige además que el nuevo valor sea menor que el antiguo, ya que
                     * los impostores no pueden tardar más que los tripulantes en moverse*/
                    crear_arco_I(G, posicion(*G, v1), posicion(*G, v2), valor);
                break;
            default:
                printf("Archivo con errores de formato\n");
                exit(0);
        }

        fscanf(fp, "%s", linea);      //Se lee una línea
    }
}


//Función privada que guarda los datos del grafo como texto usando un formato predefinido
void _guardarGrafo(FILE *fp, grafo G) {
    tipovertice *array_habitaciones = array_vertices(G);            //Array con los vértices del grafo
    int i, j, nVertices = num_vertices(G);

    //Se guardan los vértices
    for (i = 0; i < nVertices; i++) {
        fprintf(fp, "%s\n", array_habitaciones[i].habitacion);      //Nombre de cada habitación
    }

    //Separación entre vértices y arcos
    fprintf(fp, "*\n");

    //Se guardan los arcos
    for (i = 0; i < nVertices; i++) {               //Filas de las matrices
        for (j = 0; j < nVertices; j++) {           //Columnas
            if (i > j) {      //Solo se consultan los elementos debajo de la diagonal (para no imprimirlos dos veces)
                if (distancia_T(G, i, j)) {             //Hay un arco de tripulantes
                    fprintf(fp, "%s--%d-%s\n", array_habitaciones[i].habitacion, distancia_T(G, i, j),
                            array_habitaciones[j].habitacion);
                }
                if (distancia_I(G, i, j) != distancia_T(G, i, j)) {
                    //El arco de impostores solo se guarda si este es una rejilla (distinto del camino de tripulantes)
                    fprintf(fp, "%s..%d-%s\n", array_habitaciones[i].habitacion, distancia_I(G, i, j),
                            array_habitaciones[j].habitacion);
                }
            }
        }
    }
}


//Opción a del menú, introducir un vertice en el grafo
void introducir_habitacion(grafo *G) {
    tipovertice v1;
    printf("\nIntroduce habitacion: ");
    scanf("%s", v1.habitacion);
    if (existe_vertice(*G, v1))
        printf("Esa habitacion ya esta en el grafo\n");
    else
        insertar_vertice(G, v1);
}

//Opción b del menú, eliminar un vértice del grafo
void eliminar_habitacion(grafo *G) {
    tipovertice v1;
    printf("\nIntroduce habitacion: ");
    scanf("%s", v1.habitacion);
    if (existe_vertice(*G, v1))
        borrar_vertice(G, v1);
    else
        printf("Esa habitacion no existe en el grafo\n");
}

//Opción c del menú, crear una relación entre dos vértices
void nuevo_arco(grafo *G) {
    tipovertice v1, v2;
    char m;                 //Distinguimos entre dos opciones, una para cada matriz
    int valor;              //El grafo es valorado

    //Insertamos una nueva relación pidiendo los datos al usuario controlando que existan los vértices

    //Vértice origen del arco
    printf("\nIntroduce habitacion origen: ");
    scanf("%s", v1.habitacion);
    while (!existe_vertice(*G, v1)) {
        printf("La habitacion %s no existe en el mapa\n", v1.habitacion);
        printf("Introduce habitacion origen: ");
        scanf("%s", v1.habitacion);
    }

    //Vértice destino del arco
    printf("Introduce habitacion destino: ");
    scanf("%s", v2.habitacion);
    while (!existe_vertice(*G, v2)) {
        printf("La habitacion %s no existe en el mapa\n", v2.habitacion);
        printf("Introduce habitacion destino: ");
        scanf("%s", v2.habitacion);
    }

    //Valor del arco (grafo valorado)
    printf("Introduce distancia: ");
    scanf("%d", &valor);

    //Se le pide al usuario que indique quiénes serán los usuarios del arco
    printf("¿Se trata de una comunicacion para los tripulantes o para los impostores? (T/I) ");
    scanf(" %c", &m);
    while (m != 'T' && m != 'I' && m != 't' && m != 'i') {
        printf("Opción incorrecta\n");
        printf("¿Se trata de una comunicacion para los tripulantes o para los impostores? (T/I) ");
        scanf(" %c", &m);
    }

    //Creación del arco
    switch (m) {
        case 't':
        case 'T':
            if (!distancia_T(*G, posicion(*G, v1), posicion(*G, v2))) {
                //Si no existía arco (la distancia era 0) se crea uno nuevo
                crear_arco_T(G, posicion(*G, v1), posicion(*G, v2), valor);

                if (!distancia_I(*G, posicion(*G, v1), posicion(*G, v2))) {
                    //Los impostores comparten los arcos de los tripulantes. Se debe insertar la comunicación en AI
                    //Se comprueba la distancia de nuevo: los impostores podrían tener una rejilla previa en el camino
                    crear_arco_I(G, posicion(*G, v1), posicion(*G, v2), valor);
                }
                //Si los impostores ya tenían una rejilla de ventilación en esa comunicación, no se sobreescribe

            } else {
                //Si el arco ya existía previamente en AT y, por tanto, también en AI, no se sobreescribe
                printf("No se permite la sobreescritura de datos\n");
            }
            break;

        case 'i':
        case 'I':    //Comunicación exclusiva para impostores
            if (!distancia_I(*G, posicion(*G, v1), posicion(*G, v2)) ||
                (distancia_I(*G, posicion(*G, v1), posicion(*G, v2)) ==
                 distancia_T(*G, posicion(*G, v1), posicion(*G, v2)) && valor <
                                                                        distancia_I(*G, posicion(*G, v1),
                                                                                    posicion(*G, v2)))) {
                /* Se añadirá una rejilla si no había ningún arco (distancia 0) o si el que existía previamente era
                * una comunicación de tripulantes e impostores, evitando sobreescrituras de los datos propios de AI. En
                * este último caso, se exige además que el nuevo valor sea menor que el antiguo, ya que los impostores
                * no pueden tardar más que los tripulantes en moverse*/
                crear_arco_I(G, posicion(*G, v1), posicion(*G, v2), valor);
            } else {
                printf("No se permite la sobreescritura de datos\n");
            }
    }
}


//Opción d del menú, eliminar una relación entre dos vértices
void eliminar_arco(grafo *G) {
    tipovertice v1, v2;
    char m;

    //Eliminamos una relación pidiendo los datos al usuario controlando que existan los vértices

    //Vértice origen del arco
    printf("\nIntroduce habitacion origen: ");
    scanf("%s", v1.habitacion);
    while (!existe_vertice(*G, v1)) {
        printf("La habitacion %s no existe en el mapa\n", v1.habitacion);
        printf("Introduce habitacion origen: ");
        scanf("%s", v1.habitacion);
    }

    //Vértice destino del arco
    printf("Introduce habitacion destino: ");
    scanf("%s", v2.habitacion);
    while (!existe_vertice(*G, v2)) {
        printf("La habitacion %s no existe en el mapa\n", v2.habitacion);
        printf("Introduce habitacion destino: ");
        scanf("%s", v2.habitacion);
    }

    //El usuario debe indicar si desea borrar el arco para los impostores (I) o para impostores y tripulantes (T)
    printf("¿Se trata de un arco para los tripulantes o para los impostores? (T/I) ");
    scanf(" %c", &m);
    while (m != 'T' && m != 'I' && m != 't' && m != 'i') {
        printf("Opción incorrecta\n");
        printf("¿Se trata de un arco para los tripulantes o para los impostores? (T/I) ");
        scanf(" %c", &m);
    }

    //Eliminación del arco
    switch (m) {
        case 't':
        case 'T':
            if (distancia_T(*G, posicion(*G, v1), posicion(*G, v2))) {           //El arco existía
                /* Si el arco existe en AT, existe también en AI. No obstante, se debe comprobar que no se borre una
                 * rejilla de ventilación, exclusiva para impostores. Esta existirá si el arco en AI no coincide con el
                 * de AT, es decir, su distancia es menor*/
                if (distancia_I(*G, posicion(*G, v1), posicion(*G, v2)) ==
                    distancia_T(*G, posicion(*G, v1), posicion(*G, v2)))        //No hay rejilla de ventilación
                    borrar_arco_I(G, posicion(*G, v1), posicion(*G, v2));

                //El arco en AT se borra ahora para poder hacer la comparación anterior
                borrar_arco_T(G, posicion(*G, v1), posicion(*G, v2));
            } else {
                printf("La comunicación indicada no existe\n");
            }
            break;

        case 'i':
        case 'I':
            /*
             * Casos posibles:
             *      1. No existe ningún arco en ninguna matriz. Implica que la distancia en AI es 0. No se hace nada.
             *      2. Existe un arco en AI, pero no en AT. La distancia en AT es 0, en AI no es 0. Se elimina.
             *      3. Existe un arco en AI y otro en AT y son iguales. Se trata de una comunicación de tripulantes e
             *              impostores. No se hace nada.
             *      4. Existe un arco en AI y otro en AT y son diferentes. La distancia en ambos no es 0 y la de AI es
             *              menor que la de AT. Se elimina.
             * Por lo tanto, se elimina si y solo si las distancias en AT y AI no coinciden.
             */
            if (distancia_I(*G, posicion(*G, v1), posicion(*G, v2)) != distancia_T(*G,
                                                                                   posicion(*G, v1),
                                                                                   posicion(*G, v2))) {
                borrar_arco_I(G, posicion(*G, v1), posicion(*G, v2));
                crear_arco_I(G, posicion(*G, v1), posicion(*G, v2), distancia_T(*G, posicion(*G, v1), posicion(*G,v2)));
                //Se borra la rejilla, que es sustituida por el valor del arco de tripulantes
            } else {
                printf("La comunicación indicada no existe\n");
            }
    }
}


//Opción i del menú, imprimir el grafo
//Función que imprime el grafo utilizando num_vertices para saber cuántos vértices tiene
//y array_vertices para recuperar el vector de vértices y recorrerlo
void imprimir_grafo(grafo G) {
    tipovertice *VECTOR; //Para almacenar el vector de vértices del grafo
    int N; //número de vértices del grafo

    //Para recorrerla, simplemente vamos a recorrer las matrices de adyacencia
    N = num_vertices(G);
    VECTOR = array_vertices(G);

    int i, j;
    printf("\nEl grafo actual es:\n");
    for (i = 0; i < N; i++) {
        //Imprimo el vértice
        printf("\033[0;32mVertice(%d): %s\033[0m\n", i, VECTOR[i].habitacion);
        //Chequeo sus arcos
        for (j = 0; j < N; j++) {
            if (distancia_T(G, i, j))
                printf("\t%s---%s\n", VECTOR[i].habitacion, VECTOR[j].habitacion);
            if (distancia_I(G, i, j) != distancia_T(G, i, j))
                printf("\t%s...%s\n", VECTOR[i].habitacion, VECTOR[j].habitacion);
        }
    }
}


//Leer un archivo con los datos de un grafo. Ejecución automática inicial
void leer_archivo(grafo *G) {
    FILE *fp;
    fp = fopen("Mapa.txt", "rt");       //Se abre el archivo en modo lectura (r) con texto (t)

    if (fp) {
        _leerGrafo(fp, G);          //Función privada que carga los datos
        printf("Mapa cargado\n\n");
        imprimir_grafo(*G);         //Se muestra el grafo
        fclose(fp);
    } else {
        printf("Error en la lectura del archivo\n");
    }
}

//Guardar datos del grafo en un archivo. Ejecución automática final
void guardar_archivo(grafo G) {
    FILE *fp;
    fp = fopen("Mapa.txt", "wt");      //Se abre el archivo en modo escritura (w) con texto (t)

    if (fp) {               //fopen no ha dado error
        _guardarGrafo(fp, G);         //Función privada que recorre el grafo registrando sus vértices y arcos
        printf("Archivo guardado\n");           //Aviso por pantalla
        fclose(fp);                             //Se cierra el archivo
    } else {           //El modo escritura crea el archivo si este no existe, pero fopen puede dar error igualmente
        printf("Error en la creación del archivo\n");
    }
}
