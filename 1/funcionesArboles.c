#include <stdio.h>
#include <stdlib.h>
#include "abin.h"

/////////////////////PROTOTIPOS DE FUNCIONES PRIVADAS
// no visibles para usuario
//////ESTAS FUNCIONES NO APARECEN EL EL FICHERO funcionesArboles.h

// Funcion auxiliar para calcular altura 
//Se utiliza en la función AlturaArbol()
int _maxAltura(int a, int b);

//Funcion auxiliar para contar nodos
//Se utiliza en NumeroNodos()
void _auxContador(abin nodo, int *c);

//Función auxiliar para sumar nodos
//Se utiliza en SumaElementos()
void _auxSuma(abin A, int *suma);

//////////////////////////////////FUNCIONES PÚBLICAS
//////////////////APARECEN EN EL FICHERO funcionesArboles.h

//Función que calcula la altura del árbol A
int AlturaArbol(abin A) {
    int altura;
    if (A == NULL)
        altura = 0;
    else
        /* Función auxiliar privada */
        altura = 1 + _maxAltura(AlturaArbol(izq(A)), AlturaArbol(der(A)));
    return altura;
}

//Función que calcula el número de nodos de un árbol A
int NumeroNodos(abin A) {
    int contador = 0;
    _auxContador(A, &contador); /* Función auxiliar privada */
    return contador;
}

//Función que suma los elementos de un árbol A
//Esta función debe llamar a la función recursiva PRIVADA, que es la que va
//acumulando la suma
//Su propósito es inicializar el valor de la suma a 0 y devolver el resultado
//al programa
int SumaElementos(abin A){
    int suma=0;
    _auxSuma(A,&suma); 
    //suma se pasa por referencia para que las llamadas recursivas acumulen
    return suma;
}

/* Función que localiza el nodo del árbol A que contiene el elemento inf.
 * Requiere que *aux esté igualado a NULL.
 * Si el elemento no está en el árbol, *aux permanece como NULL */
void buscar(abin A, tipoelem inf, abin *aux){
    tipoelem E;
    if (!esVacio(A)){   //Si el árbol es vacío, no hay ningún nodo correcto
        //Este if permite parar la búsqueda recursiva por subárboles 
        
        leer(A, &E);
        if (E == inf){  //Se comprueba si el nodo contiene el elemento a buscar
            *aux = A;
            //Se coloca *aux en el nodo del árbol con el elemento
        }
        
        if (*aux == NULL){      //Aún no se encontró un nodo con inf
            buscar(izq(A), inf, aux);   //Se busca por el subárbol izquierdo
        }
        
        if (*aux == NULL){      //Aún no se encontró un nodo con inf 
            buscar(der(A), inf, aux);   //Se busca por el subárbol derecho
        }
    }
}


//////////////////////////////////FUNCIONES PRIVADAS
//////////////////NO APARECEN EN EL FICHERO funcionesArboles.h

/* Función auxiliar para calcular altura */
int _maxAltura(int a, int b) {
    if (a > b) return a;
    else return b;
}

/* Función auxiliar para contar nodos. Función recursiva de recorrido en
   preorden, el proceso es aumentar el contador */
void _auxContador(abin nodo, int *c) {
    if (!esVacio(nodo)){
	(*c)++; /* Otro nodo */
    	/* Continuar recorrido */
    	_auxContador(izq(nodo), c);
    	_auxContador(der(nodo), c);
    }
}
//Función auxiliar recursiva para sumar los nodos
void _auxSuma(abin A, int *suma){
    tipoelem E;
    if (!esVacio(A)){
        leer(A,&E);
        *suma=*suma+E;
        _auxSuma(izq(A),suma);
        _auxSuma(der(A),suma);
    }
}
