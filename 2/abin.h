#ifndef ABIN_H
#define ABIN_H

//Interfaz del TAD abin preparado para los arboles de expresion
typedef void *abin; //TIPO OPACO, no sabemos como esta construido

//contenido de cada nodo del arbol: arbol de letras
//ESTA DEFINICION LA TENEMOS QUE REPETIR EN abin.c, exactamente igual
typedef char tipoelem;

/////////////////////////////// FUNCIONES

//NUEVAS FUNCIONALIDADES PARA INSERTAR UN SUBARBOL a la izquierda 
//o a la derecha NECESARIAS PARA CREAR LOS ARBOLES DE EXPRESION
void insArbolizq(abin *A, abin aux);
void insArbolder(abin *A, abin aux);

/*
 * Crea el arbol vacio. 
 * @param A Puntero al arbol. Debe estar inicializado.
 */
void crear(abin *A);

/*
 * Destruye el arbol recursivamente
 * @param A El arbol que queremos destruir
 */
void destruir(abin *A);

/**
 * Comprueba si el arbol esta vacio
 * @param A El arbol binario
 */
unsigned esVacio(abin A);

/**
 * Devuelve el subarbol izquierdo de A
 * @param A - Arbol original
 */
abin izq(abin A);

/**
 * Devuelve el subarbol derecho de A
 * @param A - Arbol original
 */
abin der(abin A);

/**
 * Recupera la informacion de la celda de la raiz del arbol
 * @param A
 * @param E Puntero al nodo que apuntara a la informacion
 */
void leer(abin A, tipoelem *E);

/**
 * Inserta un nuevo nodo en el arbol para el elemento E
 * a la izquierda de A
 * @param A Arbol binario
 * @param E Informacion del nuevo nodo. 
 */
void insizq(abin *A, tipoelem E);

/**
 * Inserta un nuevo nodo en el arbol para el elemento E
 * a la derecha de A
 * @param A Arbol binario
 * @param E Informacion del nuevo nodo. 
 */
void insder(abin *A, tipoelem E);

/**
 * Suprime el subarbol a la izquierda de A
 * @param A Arbol binario
 */
void supizq(abin *A);

/**
 * Suprime el subarbol a la derecha de A
 * @param A Arbol binario
 */
void supder(abin *A);

/**
 * Modifica el contenido del nodo apuntado por A con el valor de E
 * @param A
 * @param E
 */
void modificar(abin A, tipoelem E);

#endif	// ABIN_H