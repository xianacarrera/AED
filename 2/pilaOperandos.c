#include <stdlib.h>
#include "abin.h"

//Implementacion TAD pila de operandos

//Contenido de cada elemento de la pila de operandos: abin
typedef abin tipoelemPilaOperandos;
///////////////////////////////////////////////////////

//Definicion del tipo de datos pila de operandos
struct tipo_celda_operandos {
    tipoelemPilaOperandos elemento;
    struct tipo_celda_operandos * sig;
};

typedef struct tipo_celda_operandos * pilaOperandos;
/////////////////////////////////////////////////////////

void crearPilaOperandos(pilaOperandos *P) {
    *P = NULL;
}

void destruirPilaOperandos(pilaOperandos *P) {
    pilaOperandos aux;
    aux = *P;
    while (aux != NULL) {
        aux = aux->sig;
        free(*P);
        *P = aux;
    }
}

unsigned esVaciaPilaOperandos(pilaOperandos P) {
    return P == NULL;
}


tipoelemPilaOperandos topeOperandos(pilaOperandos P) {
    if (!esVaciaPilaOperandos(P)) /*si pila no vacia*/
        return P->elemento;
}

void pushOperandos(pilaOperandos *P, tipoelemPilaOperandos E) {
    pilaOperandos aux;
    aux = (pilaOperandos) malloc(sizeof (struct tipo_celda_operandos));
    aux->elemento = E;
    aux->sig = *P;
    *P = aux;
}

void popOperandos(pilaOperandos *P) {
    pilaOperandos aux;
    if (!esVaciaPilaOperandos(*P)) /*si pila no vacia*/ {
        aux = *P;
        *P = (*P)->sig;
        free(aux);
    }
}
