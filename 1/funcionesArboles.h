#ifndef FUNCIONESARBOLES_H
#define FUNCIONESARBOLES_H

//Función que calcula la altura del árbol A
// @param A Arbol binario
int AlturaArbol(abin A);

//Función que calcula el número de nodos de un árbol A
// @param A Arbol binario
int NumeroNodos(abin A);

//Función que suma los elementos de un árbol A
// @param A Arbol binario
int SumaElementos(abin A);

//Función que busca un nodo que contenga el elemento inf
// @param A Arbol binario
// @param inf Elemento que se desea buscar
// @param *aux Puntero a árbol donde se guardará el nodo. Debe ser nulo
void buscar(abin A, tipoelem inf, abin *aux);

#endif /* FUNCIONESARBOLES_H */
