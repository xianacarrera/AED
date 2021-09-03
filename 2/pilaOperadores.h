#ifndef PILA_OPERADORES_H
#define PILA_OPERADORES_H

//Interfaz TAD pila de operadores
typedef void *pilaOperadores; /*tipos opacos*/

//Contenido de cada elemento de la pila de operadores: char
typedef char tipoelemPilaOperadores;


//Funciones de creacion y destruccion
/**
 * Crea la pila vacia. 
 * @param P Puntero a la pila de operadores. Debe estar inicializada.
 */
void crearPilaOperadores(pilaOperadores *P);

/**
 * Destruye la pila
 * @param P puntero a la pila de operadores
 */
void destruirPilaOperadores(pilaOperadores *P);

//Funciones de informacion
/**
 * Comprueba si la pila esta vacia
 * @param P pila de operadores
 */
unsigned esVaciaPilaOperadores(pilaOperadores P);

/*
 * Recupera la informacion del tope de la pila
 * @param P pila de operadores
 * 
*/
tipoelemPilaOperadores topeOperadores(pilaOperadores P);

//Funciones de insercion/eliminacion
/**
 * Inserta un nuevo nodo en la pila para el elemento E
 * en el tope de la pila de operadores
 * @param P puntero a la pila de operadores
 * @param E Informacion del nuevo nodo. 
 */
void pushOperadores(pilaOperadores *P, tipoelemPilaOperadores E);

/**
 * Suprime el elemento en el tope de la pila
 * @param P puntero a la pila de operadores
 */
void popOperadores(pilaOperadores *P);

#endif	// PILA_OPERADORES_H

