//
// Created by Xiana on 16/12/2020.
//

#include "ramificacion_poda.h"

// Realiza una asignación voraz. A cada persona le da la tarea no usada con mayor beneficio
// Devuelve el beneficio acumulado final
// Se utiliza en el cálculo de la cota inferior precisa
int _AsignacionVoraz(tipoelem x, int M[][N]){
    int bacum = 0;       // Beneficio acumulado: total de la asignación voraz
    int maxCol;          // Para cada fila, valor del beneficio máximo entre las tareas no usadas
    int i, j;            // Contadores
    int k;               // Para cada nivel, columna de la tarea no usada de mayor beneficio

    for (i = x.nivel + 1; i < N; i++){      // Se recorren todos los niveles entre el siguiente al actual y el último
        maxCol = -1;
        for (j = 0; j < N; j++){            // Se comprueban todas las tareas
            if (!x.usadas[j] && M[i][j] > maxCol){          // Se selecciona la no usada con mayor beneficio
                maxCol = M[i][j];               // Se almacena su beneficio
                k = j;                          // Se guarda también su índice
            }
        }
        x.usadas[k] = 1;        // Como el nodo original se pasa por valor, no sufre cambios
        // No es necesario actualizar la tupla. Lo que necesitamos saber en la asignación es qué tareas se usaron
        bacum += maxCol;        // Se añade el beneficio obtenido en la asignación al acumulado
    }

    return bacum;
}

// Realiza una asignación completa, dándole a cada persona la tarea con mayor beneficio
// Las tareas no se pueden repetir con respecto a las usadas hasta el momento, pero sí entre los niveles siguientes
// Devuelve el beneficio acumulado final
// Se utiliza en el cálculo de la cota superior precisa
int _MaximosTareas(tipoelem x, int B[][N]){
    int bacum = 0;      // Beneficio acumulado
    int maxCol;         // Beneficio máximo para una determinada fila entre las tareas no usadas previamente
    int i, j;           // Contadores

    for (i = x.nivel + 1; i < N; i++){      // Se estudian todos los niveles que siguen al actual
        maxCol = -1;
        for (j = 0; j < N; j++){        // Se comprueban todas las tareas
            if (!x.usadas[j] && B[i][j] > maxCol){    // Se toma la tarea no previamente usada de mayor beneficio
                maxCol = B[i][j];       // Se guarda su correspondiente beneficio
            }
        }
        // No se actualiza x.usadas. Las asignaciones de _MaximosTareas no "bloquean columnas" entre sí
        bacum += maxCol;                // Se actualiza el beneficio acumulado con el de la tarea seleccionada
    }

    return bacum;
}

// Devuelve el máximo entre dos valores
// Se usa en AsignacionRyP_trivial y en AsignacionRyP_precisa
float _max(float a, float b){
    if (a >= b) return a;
    else return b;
}

// Inserta un nodo en la lista de nodos vivos de forma ordenada. La lista va de mayor a menor beneficio
// Se emplea en AsignacionRyP_trivial y en AsignacionRyP_precisa
void _inserta_LIFO(lista *LNV, tipoelem m){
    tipoelem n;
    posicion p = primero(*LNV);

    while (p != fin(*LNV)) {        // Se recorre la lista
        recupera(*LNV, p, &n);      // Se extrae un nodo
        /*
         * Se está usando una estrategia LIFO. Por consiguiente, el nodo tiene que quedar colocado después de todos
         * los que tengan un beneficio estimado superior a él, pero antes de todos los que tengan uno igual o menor.
         */
        if (n.BE <= m.BE) break;    // Se ha encontrado la posición de inserción
        p = siguiente(*LNV, p);     // Se sigue recorriendo la lista
    }

    inserta(LNV, p, m);         // Se inserta el nodo antes del primer elemento con BE menor o igual que él
}

// Si un nodo tiene x.CI == x.CS, se conoce la solución exacta que generará. Se calcula por una asignación voraz
// Se devuelve el nodo con la solución (es decir, s y bact actualizados)
// Se utiliza en AsignacionRyP_precisa
tipoelem _SolAsignacionVoraz(tipoelem x, int M[][N]){
    int maxCol;          // Para cada fila, valor del beneficio máximo entre las tareas no usadas
    int i, j;            // Contadores
    int k;               // Para cada nivel, columna de la tarea no usada de mayor beneficio

    for (i = x.nivel + 1; i < N; i++){          // Se recorren todos los niveles siguientes al actual
        maxCol = -1;
        for (j = 0; j < N; j++){       // Se comprueban las tareas del nivel en busca de la no usada de mayor beneficio
            if (!x.usadas[j] && M[i][j] > maxCol){
                maxCol = M[i][j];       // Se guarda el beneficio asociado a la asignación
                k = j;                  // Se registra el índice de la tarea elegida
            }
        }
        x.tupla[i] = k;           // La tarea correspondiente a la persona del nivel será la de índice k
        x.usadas[k] = 1;                // La tarea k fue usada
        x.bact += maxCol;               // Se actualiza el beneficio acumulado para el nodo
    }

    x.nivel = N - 1;        // Necesario para que después pueda verificarse como solución
    return x;               // Se devuelve el nodo hoja con la solución
}

// Devuelve el mayor elemento de una matriz de enteros
int maxMatriz(int B[][N]) {
    int i, j;
    int max = -1;       // Menor a todos los valores (la matriz solo tiene elementos positivos)

    for (i = 0; i < N; i++) {               // Filas
        for (j = 0; j < N; j++) {           // Columnas
            if (B[i][j] > max)
                max = B[i][j];
        }
    }

    return max;
}

// Calcula el beneficio estimado de un nodo como media entre su cota superior y su cota inferior
void BE(tipoelem *x){
    x->BE = (x->CI + x->CS)/2;
}

// Calcula la cota inferior trivial de un nodo como su beneficio acumulado
void CI_trivial(tipoelem *x) {
    x->CI = x->bact;
}

// Calcula la cota superior trivial de un nodo
void CS_trivial(tipoelem *x, int maxB){
    // A bact se le suma el máximo de la matriz, previamente calculado, por el número de niveles no recorridos
    x->CS = x->bact + (N - 1 - x->nivel) * maxB;
}

// Calcula la cota inferior precisa de un nodo
void CI_precisa(tipoelem *x, int B[][N]){
    // Se suma el beneficio correspondiente a una asignación voraz a partir de x con su beneficio acumulado
    x->CI = x->bact + _AsignacionVoraz(*x, B);
}

// Calcula la cota superior precisa de un nodo
void CS_precisa(tipoelem *x, int B[][N]){
    // Se suma el beneficio obtenido por el cálculo de _MaximosTareas con el beneficio acumulado de x
    x->CS = x->bact + _MaximosTareas(*x, B);
}

// Comprueba si un nodo es solución, esto es, si es un nodo hoja
int Solucion(tipoelem x){
    return x.nivel == N - 1;
}

// Selecciona un candidato de la lista de nodos vivos siguiendo una estrategia MB-LIFO
tipoelem Seleccionar(lista LNV){
    tipoelem x;

    // LNV está ordenada de mayor a menor BE, con estrategia LIFO. Por consiguiente, basta sacar el primer elemento
    recupera(LNV, primero(LNV), &x);
    return x;
}

// Se realiza una asignación de las tareas a las personas por ramificación y poda con estimaciones triviales
// Se devuelve el beneficio total de la solución óptima
int AsignacionRyP_trivial(int B[][N], int s[], int *nodosExplorados){
    lista LNV;                  // Lista de nodos vivos
    tipoelem raiz, x, y;        // Nodos
    int maxB, i, j;             // Elemento máximo de la raíz y contadores
    float C;                   // Variable de poda
    int sBact = -1;             // Beneficio acumulado de la solución (se inicializa a un valor menor que cualquiera)

    maxB = maxMatriz(B);        // Se guarda el máximo de la matriz de antemano, pues maxMatriz tiene orden cuadrático

    /*** Inicialización de valores para la raíz ***/

    raiz.bact = 0;              // No se han realizado asignaciones. El beneficio acumulado es 0
    raiz.nivel = -1;            // Primer nivel del árbol
    for (i = 0; i < N; i++){
        raiz.tupla[i] = -1;         // La tupla de la solución para la raíz se llena con -1 (ninguna tarea asignada)
        raiz.usadas[i] = 0;         // Todas las tareas están sin usar
    }
    raiz.CI = 0;                    // Con estimaciones triviales, la cota inferior de la raíz valdrá siempre 0
    CS_trivial(&raiz, maxB);        // Se calcula la cota superior correspondiente a la raíz
    C = 0;                          // C = raiz.CI

    (*nodosExplorados)++;            // Se cuenta la raíz como un nodo explorado, al haberse calculado su CI, CS y BE

    /*** Preparación de la lista de nodos vivos ***/

    crea(&LNV);
    inserta(&LNV, primero(LNV), raiz);      // Se guarda la raíz como primer elemento de la lista


    /*** Algoritmo de ramificación y poda ***/

    while(!esvacia(LNV)){           // Condición de parada
        x = Seleccionar(LNV);                // Se saca de LNV un nodo siguiendo la estrategia MB-LIFO
        suprime(&LNV, primero(LNV));         // El nodo se elimina de la lista
        if (x.CS > C){                 // Estrategia de poda. Si no se cumple, no se generan los hijos del nodo
            y.nivel = x.nivel + 1;     // Se puede realizar fuera del for. Los hijos tendrán un nivel más que su padre
            for (i = 0; i < N; i++){            // Se generan los N posibles hijos, uno por tarea
                for (j = 0; j < N; j++) {     // Se copian las tuplas con la solución parcial y tareas usadas del padre
                    y.tupla[j] = x.tupla[j];
                    y.usadas[j] = x.usadas[j];
                }

                if (!x.usadas[i]){          // El nodo es válido. La tarea no fue usada anteriormente
                    y.tupla[y.nivel] = i;       // A la persona correspondiente al nivel se le asigna la tarea i
                    y.usadas[i] = 1;
                    y.bact = x.bact + B[y.nivel][i];        // Se suma el beneficio correspondiente a la asignación
                } else {
                    continue;       // No es necesario realizar más operaciones. Se comprueba la siguiente tarea
                }

                // El nodo es válido. Se calcula su cota inferior, cota superior y beneficio estimado
                CI_trivial(&y);
                CS_trivial(&y, maxB);
                BE(&y);

                (*nodosExplorados)++;                       // Se han calculado las estadísticas del nodo

                if (Solucion(y) && y.bact > sBact){         // El nodo es una solución mejor que la óptima actual
                    for (j = 0; j < N; j++){
                        s[j] = y.tupla[j];                  // Se guarda la nueva solución
                    }

                    sBact = y.bact;                         // Se actualiza el valor óptimo para las soluciones
                    C = _max(C, sBact);                     // Si sBact supera a C, esta se actualiza a sBact

                } else if (!Solucion(y) && y.CS > C){       // El nodo no es solución, pero es prometedor
                    _inserta_LIFO(&LNV, y);                 // Se guarda en la lista de nodos vivos
                    C = _max(C, y.CI);                      // Se actualiza C a la CI del nodo si es necesario
                }
            }
        }
    }

    return sBact;           // Beneficio acumulado de la solución óptima
}

// Se realiza una asignación de las tareas a las personas por ramificación y poda con estimaciones precisas
// Se devuelve el beneficio total de la solución óptima
int AsignacionRyP_precisa(int B[][N], int s[], int *nodosExplorados){
    lista LNV;                  // Lista de nodos vivos
    tipoelem raiz, x, y;        // Nodos
    int i, j;                   // Contadores
    float C;                   // Variable de poda
    int sBact = -1;             // Beneficio acumulado de la solución (se inicializa a un valor menor que cualquiera)


    /*** Inicialización de valores para la raíz ***/

    raiz.bact = 0;              // No se han realizado asignaciones. El beneficio acumulado es 0
    raiz.nivel = -1;            // Primer nivel del árbol
    for (i = 0; i < N; i++){
        raiz.tupla[i] = -1;         // La tupla de la solución para la raíz se llena con -1 (ninguna tarea asignada)
        raiz.usadas[i] = 0;         // Todas las tareas están sin usar
    }
    // Se calculan las cotas de la raíz y se inicializa C a la cota inferior de la misma
    CI_precisa(&raiz, B);
    CS_precisa(&raiz, B);
    /*
     * Aunque C se debería inicializar a raiz.CI, es más seguro hacer C = 0, para asegurar que se generen los hijos
     * de la raíz en el bucle principal. En caso contrario, la función terminaría y no se habría explorado ningún nodo.
     * Esta situación puede darse si raiz.CI y raiz.CS coinciden, una situación común para matrices de tamaño 2x2, por
     * ejemplo. Si el orden elegido es mayor, es bastante improbable que se de este problema.
     */
    C = raiz.CI;

    (*nodosExplorados)++;            // Se cuenta la raíz como un nodo explorado, al haberse calculado su CI, CS y BE

    /*** Preparación de la lista de nodos vivos ***/

    crea(&LNV);
    inserta(&LNV, primero(LNV), raiz);      // Se guarda la raíz como primer elemento de la lista


    /*** Algoritmo de ramificación y poda ***/

    while(!esvacia(LNV)){           // Condición de parada
        x = Seleccionar(LNV);                // Se saca de LNV un nodo siguiendo la estrategia MB-LIFO
        suprime(&LNV, primero(LNV));         // El nodo se elimina de la lista
        if (x.CS > C){                 // Estrategia de poda. Si no se cumple, no se generan los hijos del nodo
            for (i = 0; i < N; i++){            // Se generan los N posibles hijos, uno por tarea
                y.nivel = x.nivel + 1;          // Se realiza dentro del for por las posibles asignaciones voraces
                for (j = 0; j < N; j++) {     // Se copian las tuplas con la solución parcial y tareas usadas del padre
                    y.tupla[j] = x.tupla[j];
                    y.usadas[j] = x.usadas[j];
                }

                if (!x.usadas[i]){          // El nodo es válido. La tarea no fue usada anteriormente
                    y.tupla[y.nivel] = i;       // A la persona correspondiente al nivel se le asigna la tarea i
                    y.usadas[i] = 1;
                    y.bact = x.bact + B[y.nivel][i];        // Se suma el beneficio correspondiente a la asignación
                } else {
                    continue;       // No es necesario realizar más operaciones. Se comprueba la siguiente tarea
                }

                // El nodo es válido. Se calcula su cota inferior, cota superior y beneficio estimado
                CI_precisa(&y, B);
                CS_precisa(&y, B);
                BE(&y);

                (*nodosExplorados)++;                       // Se han calculado las estadísticas del nodo

                if (!Solucion(y) && y.CS >= C && y.CS == y.CI) {
                    /*
                     * Se obtiene inmediatamente el valor de la solución mediante una asignación voraz.
                     * Aunque se cumpla y.CS == y.CI, si y.CS < C, el nodo es podado igualmente.
                     * Además, si y es solucion, tampoco se entra en el if, porque la llamada a _SolAsignacionVoraz no
                     * es necesaria.
                     *
                     * Este caso no es posible en AsignacionRyP_trivial, porque si el beneficio acumulado (la cota
                     * inferior) es igual al máximo de la matriz * N (la cota superior), entonces necesariamente se
                     * está en un nodo hoja. Esto es debido a que, en el mejor caso, que sería aquel en el que todas
                     * las asignaciones realizadas hasta el nivel actual dieron beneficio igual al máximo de la matriz,
                     * bact sería igual a dicho valor * el nivel actual + 1. Para que coincida con la cota superior,
                     * necesariamente el nivel es igual a N - 1, el último del árbol.
                     */

                    (*nodosExplorados) += N - 1 - y.nivel;      // Número de niveles restantes por explorar
                    y = _SolAsignacionVoraz(y, B);
                }

                if (Solucion(y) && y.bact > sBact) {         // El nodo es una solución mejor que la óptima actual
                    for (j = 0; j < N; j++) {
                        s[j] = y.tupla[j];                  // Se guarda la nueva solución
                    }

                    sBact = y.bact;                         // Se actualiza el valor óptimo para las soluciones
                    C = _max(C, sBact);                     // Si sBact supera a C, esta se actualiza a sBact

                } else if (!Solucion(y) && y.CS > C){       // El nodo no es solución, pero es prometedor
                    _inserta_LIFO(&LNV, y);                 // Se guarda en la lista de nodos vivos
                    C = _max(C, y.CI);                      // Se actualiza C a la CI del nodo si es necesario
                }
            }
        }
    }

    return sBact;           // Beneficio acumulado de la solución óptima
}