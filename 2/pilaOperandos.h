#ifndef PILA_OPERANDOS_H
#define PILA_OPERANDOS_H


//Interfaz TAD pila de operandos
typedef void *pilaOperandos; /*tipo opaco*/

//Contenido de cada elemento de la pila de operandos: abin
typedef abin tipoelemPilaOperandos;


//Funciones de creacion y destruccion
/**
 * Crea la pila vacia. 
 * @param P Puntero a la pila de operandos. Debe estar inicializada.
 */
void crearPilaOperandos(pilaOperandos *P);

/**
 * Destruye la pila
 * @param P puntero a la pila de operandos
 */
void destruirPilaOperandos(pilaOperandos *P);

//Funciones de informacion
/**
 * Comprueba si la pila esta vacia
 * @param P pila de operandos
 */
unsigned esVaciaPilaOperandos(pilaOperandos P);

/*
 * Recupera la informacion del tope de la pila
 * @param P pila de operandos
 * 
*/
tipoelemPilaOperandos topeOperandos(pilaOperandos P);

//Funciones de insercion/eliminacion
/**
 * Inserta un nuevo nodo en la pila para el elemento E
 * en el tope de la pila de operandos
 * @param P puntero a la pila de operandos
 * @param E Informacion del nuevo nodo. 
 */
void pushOperandos(pilaOperandos *P, tipoelemPilaOperandos E);

/**
 * Suprime el elemento en el tope de la pila
 * @param P puntero a la pila de operandos
 */
void popOperandos(pilaOperandos *P);

#endif	// PILA_OPERANDOS_H

