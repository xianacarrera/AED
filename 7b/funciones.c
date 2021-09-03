#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

#define INFINITY 1000      //Valor mayor que todos los utilizados. Representa el infinito

//Estructura privada para representar las matrices de vértice previo
struct camino {
    int verticeprevio;     //Entero con el vértice previo
    char mapa;             //T o I
};


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// FUNCIONES PRIVADAS

//Función privada que permite leer un grafo guardado como texto en un formato predefinido
void _leerGrafo(FILE *fp, grafo *G) {
    char linea[45];          //20 por cada una de las 2 habitaciones + 5 por los símbolos extra
    char codigos[2];         //Caracteres que separan la habitacion 1 de valor
    int valor;
    tipovertice v1, v2;      //Vértices que almacenan la información leída

    //Formato: Habitacion1%%valor-Habitacion2, donde %% = .. o %% = --

    fscanf(fp, "%s", v1.habitacion);               //Se lee una línea
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


/* Función privada que imprime todas las habitaciones por las que se tiene que pasar para ir de un punto a otro
 * por el camino más corto posible*/
void _imprimir_camino(struct camino P[][MAXVERTICES], int i, int j, tipovertice *V) {
    if (i != j) {      //Antes de imprimir nada, se tiene que llegar al inicio del recorrido, donde i coincide con j
        _imprimir_camino(P, i, P[i][j].verticeprevio, V);
    } else {
        //Llamando recursivamente a _impimir_camino se ha llegado al punto en el que i y j son iguales
        printf("\033[0;36m%s\033[0m", V[j].habitacion); //Habitación inicial del recorrido. Es la primera en imprimirse
        return;
    }

    if (P[i][j].mapa == 'T') {                  //Arco de tripulantes
        printf(" \033[0;36m--%s\033[0m", V[j].habitacion);
    } else {                                    //Arco de impostores
        printf(" \033[0;36m..%s\033[0m", V[j].habitacion);
    }
}


/* Función privada que utiliza el algoritmo de Floyd-Warshall para encontrar la matriz de caminos y la matriz de
 * vértice previo del mapa, que después son utilizadas para hallar la ruta más corta entre dos habitaciones*/
void _floyd(grafo G, int origen, int destino, char tipo) {
    int D[MAXVERTICES][MAXVERTICES];                //Matriz de distancias
    struct camino P[MAXVERTICES][MAXVERTICES];
    //Matriz de vértice previo (incluyendo para cada camino si es de tripulantes o de impostores)
    tipovertice *VERTICES;                          //Vector de vértices
    int i, j, k, nVertices = num_vertices(G);       //Contadores, número de vértices del grafo

    //Inicialización de las matrices
    for (i = 0; i < nVertices; i++) {
        for (j = 0; j < nVertices; j++) {
            if (i == j) {            //Diagonal (comunicaciones de las habitaciones a sí mismas)
                D[i][i] = 0;                        //Acceso inmediato
                P[i][i].verticeprevio = i;          //El vértice previo es la propia habitación
            } else if ((tipo == 'I' || tipo == 'i') && (distancia_T(G, i, j) != distancia_I(G, i, j))) {
                /* Comparar el tipo descarta esta opción si se está calculando la ruta para tripulantes.
                 * A continuación, se comprueba si existe una rejilla de ventilación. Para ello tiene que ocurrir que
                 * distancia_I sea distinta de 0 y, al mismo tiempo, distinta de distancia_T. distancia_T != distancia_I
                 * verifica directamente que se cumpla la segunda condición, y la primera está implícita: si
                 * distancia_T == 0, para que haya rejilla tiene que ocurrir que distancia_I > 0 y, como las
                 * distancias no pueden ser negativas, es suficiente comprobar distancia_I != distancia_T. Si
                 * distancia_T != 0 y se cumple distancia_T != distancia_I, sabemos que eso implica que
                 * distancia_I < distancia_T (pues los caminos de impostores siempre son más rápidos), y no puede darse
                 * que distancia_I sea 0, por construcción de la función nuevo_arco y eliminar_arco*/
                D[i][j] = distancia_I(G, i, j);
                P[i][j].verticeprevio = i;
                P[i][j].mapa = 'I';                     //Acceso a través de una rejilla, exclusiva de impostores
            } else if (distancia_T(G, i, j)) {
                /* En esta opción se incluyen dos casos:
                 *      1. Se está buscando la ruta mínima para un tripulante y hay una comunicación entre i y j
                 *      2. Se está buscando la ruta mínima para un impostor y no hay una rejilla entre i y j, pero sí
                 *          un camino de tripulantes */
                D[i][j] = distancia_T(G, i, j);
                P[i][j].verticeprevio = i;
                P[i][j].mapa = 'T';
            } else {                            //No hay camino entre los dos vértices
                D[i][j] = INFINITY;             //Distancia infinita
                P[i][j].verticeprevio = i;      //Se guarda la propia habitación
            }
        }
    }

    //Búsqueda de la distancia mínima
    for (k = 0; k < nVertices; k++) {            //Se analiza la matriz Dk
        for (i = 0; i < nVertices; i++) {        //Arco i->k
            for (j = 0; j < nVertices; j++) {    //Arco k->j
                if (D[i][j] > D[i][k] + D[k][j]) {   //El camino a través del vértice k es más rápido que el que había
                    D[i][j] = D[i][k] + D[k][j];    //Se actualiza la distancia
                    P[i][j] = P[k][j];              //Se actualiza el vértice previo al que había anterior al arco k-j
                }
            }
        }
    }

    //Camino más corto entre el par de vértices (origen, destino)
    VERTICES = array_vertices(G);            //Se guardan los vértices del grafo
    printf("La distancia total desde %s hasta %s es de \033[0;32m%d\033[0m\n",
           VERTICES[origen].habitacion, VERTICES[destino].habitacion, D[origen][destino]);
    printf("\tRuta:   ");
    _imprimir_camino(P, origen, destino, VERTICES);    //Se imprimen las habitaciones de la ruta
    printf("\n");
}


/* Función privada que recorre hacia atrás el camino calculado por Floyd-Warshall para encontrar la próxima habitación
 * a la que se debe dirigir un determinado jugador*/
void _buscar_sig_hab(struct camino P[][MAXVERTICES], int i, int j, tipovertice *V, char *habitacionSiguiente){
    if (i != j){
        /* Condición de parada. Nótese que nunca ocurrirá que i y j coincidan nada más empezar porque en ese caso
         * no se llegaría a llamar a floyd() */
        if (P[i][j].verticeprevio == i){        //La habitación actual es la inmediatamente siguiente a la inicial
            strcpy(habitacionSiguiente, V[j].habitacion);
            return;         //No hace falta seguir: ya se ha cumplido el cometido de la función
        }
        _buscar_sig_hab(P, i, P[i][j].verticeprevio, V, habitacionSiguiente);
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// FUNCIONES PÚBLICAS

//Introducir un vertice en el grafo
void introducir_habitacion(grafo *G) {
    tipovertice v1;
    printf("\nIntroduce habitacion: ");
    scanf("%s", v1.habitacion);
    if (existe_vertice(*G, v1))
        printf("Esa habitacion ya esta en el grafo\n");
    else
        insertar_vertice(G, v1);
}


//Eliminar un vértice del grafo
void eliminar_habitacion(grafo *G) {
    tipovertice v1;
    printf("\nIntroduce habitacion: ");
    scanf("%s", v1.habitacion);
    if (existe_vertice(*G, v1))
        borrar_vertice(G, v1);
    else
        printf("Esa habitacion no existe en el grafo\n");
}


//Crear una relación entre dos vértices
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
    while (!existe_vertice(*G, v2) || strcmp(v2.habitacion, v1.habitacion) == 0) {
        if (!existe_vertice(*G, v2)) {
            printf("La habitacion %s no existe en el mapa\n", v2.habitacion);
        } else {
            /* Se ha añadido la condición de que las habitaciones tienen que ser distintas para evitar que se
             * introduzcan valores no nulos en la diagonal de las matrices de adyacencia */
            printf("No es posible crear una comunicación de una habitación a sí misma\n");
        }
        printf("Introduce habitacion destino: ");
        scanf("%s", v2.habitacion);
    }

    //Valor del arco (grafo valorado)
    printf("Introduce distancia: ");
    scanf("%d", &valor);
    if (valor <= 0) {
        printf("Las distancias tienen que ser mayores que 0\n");
        //Verificar que no se introduzcan ni distancias negativas ni nulas previene de errores posteriores
        printf("Introduce distancia: ");
        scanf("%d", &valor);
    }

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


//Eliminar una relación entre dos vértices
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
                crear_arco_I(G, posicion(*G, v1), posicion(*G, v2),
                             distancia_T(*G, posicion(*G, v1), posicion(*G, v2)));
                //Se borra la rejilla, que es sustituida por el valor del arco de tripulantes
            } else {
                printf("La comunicación indicada no existe\n");
            }
    }
}


//Imprimir el grafo
//Función que imprime el grafo utilizando num_vertices para saber cuántos vértices tiene
//y array_vertices para recuperar el vector de vértices y recorrerlo
void imprimir_grafo(grafo G) {
    tipovertice *VECTOR; //Para almacenar el vector de vértices del grafo
    int N; //número de vértices del grafo

    //Para recorrerla, simplemente vamos a recorrer las matrices de adyacencia
    N = num_vertices(G);
    VECTOR = array_vertices(G);

    int i, j;
    printf("El mapa actual es:\n");
    for (i = 0; i < N; i++) {
        //Imprimo el vértice
        printf("\033[0;32mHabitacion(%d): %s\033[0m\n", i, VECTOR[i].habitacion);
        //Chequeo sus arcos
        for (j = 0; j < N; j++) {
            if (distancia_T(G, i, j))
                printf("\t---%-20s(distancia=\033[0;36m%d\033[0m)\n", VECTOR[j].habitacion, distancia_T(G, i, j));
            if (distancia_I(G, i, j) != distancia_T(G, i, j))
                printf("\t...%-20s(distancia=\033[0;36m%d\033[0m)\n", VECTOR[j].habitacion, distancia_I(G, i, j));
        }
    }
}


/* Función que utiliza el algoritmo de Floyd-Warshall para encontrar la matriz de caminos y la matriz de
 * vértice previo del mapa, que después son utilizadas para hallar la ruta más corta entre dos habitaciones
 * Devuelve una cadena de caracteres con la próxima habitación a la que se debería ir siguiendo la ruta óptima */
char* floyd(grafo G, int origen, int destino, char tipo){
    int D[MAXVERTICES][MAXVERTICES];                //Matriz de distancias
    struct camino P[MAXVERTICES][MAXVERTICES];
    //Matriz de vértice previo (incluyendo para cada camino si es de tripulantes o de impostores)
    tipovertice *VERTICES;                          //Vector de vértices
    int i, j, k, nVertices = num_vertices(G);       //Contadores, número de vértices del grafo
    char* habitacionSiguiente = (char*) malloc (L * sizeof(char));
    //Se reserva memoria para poder devolver la cadena de caracteres

    //Inicialización de las matrices
    for (i = 0; i < nVertices; i++) {
        for (j = 0; j < nVertices; j++) {
            if (i == j) {            //Diagonal (comunicaciones de las habitaciones a sí mismas)
                D[i][i] = 0;                        //Acceso inmediato
                P[i][i].verticeprevio = i;          //El vértice previo es la propia habitación
            } else if ((tipo == 'I' || tipo == 'i') && (distancia_T(G, i, j) != distancia_I(G, i, j))) {
                /* Comparar el tipo descarta esta opción si se está calculando la ruta para tripulantes.
                 * A continuación, se comprueba si existe una rejilla de ventilación. Para ello tiene que ocurrir que
                 * distancia_I sea distinta de 0 y, al mismo tiempo, distinta de distancia_T. distancia_T != distancia_I
                 * verifica directamente que se cumpla la segunda condición, y la primera está implícita: si
                 * distancia_T == 0, para que haya rejilla tiene que ocurrir que distancia_I > 0 y, como las
                 * distancias no pueden ser negativas, es suficiente comprobar distancia_I != distancia_T. Si
                 * distancia_T != 0 y se cumple distancia_T != distancia_I, sabemos que eso implica que
                 * distancia_I < distancia_T (pues los caminos de impostores siempre son más rápidos), y no puede darse
                 * que distancia_I sea 0, por construcción de la función nuevo_arco y eliminar_arco*/
                D[i][j] = distancia_I(G, i, j);
                P[i][j].verticeprevio = i;
                P[i][j].mapa = 'I';                     //Acceso a través de una rejilla, exclusiva de impostores
            } else if (distancia_T(G, i, j)) {
                /* En esta opción se incluyen dos casos:
                 *      1. Se está buscando la ruta mínima para un tripulante y hay una comunicación entre i y j
                 *      2. Se está buscando la ruta mínima para un impostor y no hay una rejilla entre i y j, pero sí
                 *          un camino de tripulantes */
                D[i][j] = distancia_T(G, i, j);
                P[i][j].verticeprevio = i;
                P[i][j].mapa = 'T';
            } else {                            //No hay camino entre los dos vértices
                D[i][j] = INFINITY;             //Distancia infinita
                P[i][j].verticeprevio = i;      //Se guarda la propia habitación
            }
        }
    }

    //Búsqueda de la distancia mínima
    for (k = 0; k < nVertices; k++) {            //Se analiza la matriz Dk
        for (i = 0; i < nVertices; i++) {        //Arco i->k
            for (j = 0; j < nVertices; j++) {    //Arco k->j
                if (D[i][j] > D[i][k] + D[k][j]) {   //El camino a través del vértice k es más rápido que el que había
                    D[i][j] = D[i][k] + D[k][j];    //Se actualiza la distancia
                    P[i][j] = P[k][j];              //Se actualiza el vértice previo al que había anterior al arco k-j
                }
            }
        }
    }

    //Camino más corto entre el par de vértices (origen, destino)
    VERTICES = array_vertices(G);           //Se guardan los vértices del grafo
    //Se guarda en habitacionSiguiente el nombre del próximo vértice a recorrer
    _buscar_sig_hab(P, origen, destino, VERTICES, habitacionSiguiente);

    return habitacionSiguiente;
}


//Calcular las conexiones mínimas que mantienen conectadas las habitaciones
void arbol_expansion(grafo G) {
    int numArcos = 0;                                   //Contador del número de arcos seleccionados
    int distanciaTotal = 0;                             //Valor suma del árbol de expansión resultante
    int minimo;                                         //Arco de valor mínimo encontrado en una iteración
    int i, j, N = num_vertices(G);                      //Variables de control de bucles, número de vértices del grafo
    int vx, vy;                                         //Vértices unidos por un arco
    tipovertice *VERTICES = array_vertices(G);          //Array con todos los vértices del grafo
    int Selected[N];                                    //Array donde se guardan los vértices que fueron seleccionados
    char rol, rolCamino;                                //Selección del rol, rol de un arco guardado

    printf("¿Para tripulantes o para impostores? (T/I): ");
    scanf(" %c", &rol);
    while (rol != 'T' && rol != 't' && rol != 'I' && rol != 'i') {
        printf("El rol debe ser T o I\n");
        printf("Introduce rol (T/I): ");
        scanf(" %c", &rol);
    }

    //Inicialización del conjunto de vértices seleccionados
    for (i = 0; i < N; i++) {
        Selected[i] = 0;            //Ninguno de los vértices ha sido seleccionado todavía
    }

    //Selección del primer vértice
    Selected[0] = 1;                //Se empieza en el vértice 0

    //Construcción del árbol de expansión
    while (numArcos < (N - 1)) {     //El árbol final tendrá exactamente N - 1 arcos, uniendo los N vértices sin ciclos
        minimo = INFINITY;          //Cualquier arco existente tendrá un valor menor
        vx = 0;
        vy = 0;

        //Búsqueda del arco x-y con valor mínimo, donde Selected[vx] = 1 y Selected[vy] = 0
        for (i = 0; i < N; i++) {
            if (Selected[i] == 1) {
                //Un arco solo se puede añadir si uno de sus vértices ya pertenece al conjunto seleccionado
                for (j = 0; j < N; j++) {
                    if (Selected[j] != 1 && (rol == 'I' || rol == 'i') &&
                        distancia_I(G, i, j) != distancia_T(G, i, j)) {         //Comunicación para impostores
                        /* Selected[j] != 1 --> El otro vértice correspondiente al arco no puede pertenecer al conjunto
                         *      de vértices seleccionados (se formaría un ciclo).
                         * rol == 'I' || rol == 'i' --> Se está comprobando la existencia de una rejilla exclusiva de
                         *      impostores.
                         * distancia_I(G, i, j) != distancia_T(G, i, j) --> Se verifica que exista una rejilla en ese
                         *      camino (la explicación completa de esta condición puede leerse en _floyd).
                         * */
                        if (minimo > distancia_I(G, i, j)) {    //Nos quedamos con el arco de distancia mínima
                            minimo = distancia_I(G, i, j);
                            vx = i;                 //Vértice ya seleccionado
                            vy = j;                 //Vértice que se añadirá a Selected
                            rolCamino = 'I';        //Necesario saber el tipo del camino para mostrarlo por pantalla
                        }
                    } else if (Selected[j] != 1 && distancia_T(G, i, j)) {      //Comunicación para tripulantes
                        /* El otro vértice correspondiente al arco no puede pertenecer al conjunto de vértices
                         * seleccionados (se formaría un ciclo), y tal comunicación entre i y j tiene que existir*/
                        if (minimo > distancia_T(G, i, j)) {    //Nos quedamos con el arco de distancia mínima
                            minimo = distancia_T(G, i, j);
                            vx = i;                 //Vértice ya seleccionado
                            vy = j;                 //Vértice que se añadirá a Selected
                            rolCamino = 'T';        //Necesario saber el tipo del camino para mostrarlo por pantalla
                        }
                    }
                }
            }
        }
        Selected[vy] = 1;           //Se ha seleccionado vy como el vértice a añadir
        numArcos++;                 //Variable de control del bucle
        if (rolCamino == 'T') {      //El camino seleccionado puede ser recorrido por tripulantes e impostores
            printf("\033[0;36m%15s -- %-15s\033[0m : \033[0;32m%-10d\033[0m\n",
                   VERTICES[vx].habitacion, VERTICES[vy].habitacion, distancia_T(G, vx, vy));
            distanciaTotal = distanciaTotal + distancia_T(G, vx, vy);           //Se actualiza la distancia total
        } else {                    //El camino seleccionado es exclusivo de impostores
            printf("\033[0;36m%15s .. %-15s\033[0m : \033[0;32m%-10d\033[0m\n",
                   VERTICES[vx].habitacion, VERTICES[vy].habitacion, distancia_I(G, vx, vy));
            distanciaTotal = distanciaTotal + distancia_I(G, vx, vy);           //Se actualiza la distancia total
        }
    }

    //La distancia total es la suma del valor de todos los arcos que han sido seleccionados
    printf("Distancia total del árbol de expansión de coste mínimo = \033[0;32m%-10d\033[0m\n", distanciaTotal);
}


//Calcular la ruta más corta entre dos habitaciones dadas
void ruta_minima(grafo G) {
    tipovertice orig, dest;
    char rol;

    printf("\nIntroduce habitacion origen: ");
    scanf("%s", orig.habitacion);
    if (!existe_vertice(G, orig)) {
        printf("La habitacion %s no existe en el mapa\n", orig.habitacion);
        return;
    }

    printf("Introduce habitacion destino: ");
    scanf("%s", dest.habitacion);
    if (!existe_vertice(G, dest)) {
        printf("La habitacion %s no existe en el mapa\n", dest.habitacion);
        return;
    }

    printf("Introduce rol (T/I): ");
    scanf(" %c", &rol);
    while (rol != 'T' && rol != 't' && rol != 'I' && rol != 'i'){
        printf("El rol debe ser T o I\n");
        printf("Introduce rol (T/I): ");
        scanf(" %c", &rol);
    }

    //Camino más corto entre el par de vértices (orig, dest)
    if (strcmp(orig.habitacion, dest.habitacion) == 0) {
        printf("La habitación de origen y la de destino coinciden. La distancia total es 0\n");
        printf("\tRuta: %s", dest.habitacion);
    } else {
        _floyd(G, posicion(G, orig), posicion(G, dest), rol);
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
