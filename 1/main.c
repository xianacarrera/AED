/* Practica 1: Prueba de manejo de Arboles Binarios  */

/*
 * Autora: Xiana Carrera
 * Este programa sirve como prueba de diferentes funciones de los Árboles 
 * Binarios. En concreto, se crea un árbol, se imprime diversa información 
 * sobre él y se elimina un subárbol de un nodo.
 */

#include <stdlib.h>
#include <stdio.h>

#include "abin.h"
#include "recorridos.h"
#include "funcionesArboles.h"

int main(int argc, char** argv) {

    abin Arbol, aux;    //Árbol y árbol auxiliar
    tipoelem inf;       //char para la información de la función buscar
    char comp;          //char usado para decidir qué subárbol borrar

    //En primer lugar, se crea el árbol:
    crear(&Arbol);
    printf("Arbol creado");
    

    //---------------------- Inserción de nodos -----------------------------
    //Se inserta la raíz:
    insizq(&Arbol, 'R');
    //Se inserta el nodo con el elemento 'T' a la derecha de la raíz:
    insder(&Arbol, 'T');
    //Se inserta el nodo con el elemento 'S' a la izquierda de 'T'
    //Para ello, se coloca el árbol auxiliar en el nodo 'T'
    aux = der(Arbol);
    insizq(&aux, 'S');
    //Se inserta el nodo con el elemento 'V' a la derecha de 'T'
    insder(&aux, 'V');
    //Se inserta el nodo con el elemento 'U' a la izquierda de 'V'
    //Para ello, se mueve el árbol auxiliar a la derecha de donde estaba
    aux = der(aux);
    insizq(&aux, 'U');
    
    //De vuelta a la raíz, se inserta un nodo a su izquierda
    insizq(&Arbol, 'P');
    //Se inserta otro nodo a la izquierda de 'P' con el elemento 'M'
    //Para ello, aux se desplaza a la izquierda de la raíz (a 'P')
    aux = izq(Arbol);
    insizq(&aux, 'M');
    //A continuación, se inserta 'Q' a la derecha de 'P'
    insder(&aux, 'Q');
    //Se inserta el nodo con el elemento 'A' a la izquierda de 'M'
    //Para ello, aux se desplaza una posición a su izquierda (de 'P' a 'M')
    aux = izq(aux);
    insizq(&aux, 'A');
    //Finalmente, se inserta 'O' a la derecha de 'M'
    insder(&aux, 'O');
    
    
    //---------------------- Datos del árbol ------------------------------

    //Se imprime la altura del árbol resultante (4)
    printf("\nAltura del arbol: %d", AlturaArbol(Arbol));

    //Se imprimen los elementos del árbol con un recorrido en anchura
    printf("\nRecorrido en anchura (usa colas):\n"); 
    anchura(Arbol);
    
    
    //------------------------- Eliminación -------------------------------

    /* Se muestra aquí el código para eliminar el subárbol izquierdo de 'P' sin 
     * buscarlo. Para ello, se mueve aux a la izquierda de la raíz, donde 
     * está 'P'. Después, se suprime su subárbol izquierdo.
     * 
     * aux = izq(Arbol);  
     * supizq(&aux);
     *
     * Se imprime un mensaje de aviso:
     * printf("\nArbol izquierdo de P eliminado");
     */
     
    
    //Se procede a eliminar un subárbol de un nodo dado

    //aux apunta al nodo 'M'. Se reinicia a NULL (necesario para buscar)
    aux = NULL;
    
    //Se pide el nodo del que se quiere suprimir el subárbol
    printf("\nIntroduzca el elemento deseado: ");
    scanf(" %c", &inf);
    
    //Se busca el nodo con el elemento introducido en el árbol
    buscar(Arbol, inf, &aux);
    //Si el nodo fue encontrado, aux le estará apuntando 
    
    //Si aux existe (elemento encontrado), se suprime su subárbol izquierdo
    if (aux != NULL){
        printf("Introduzca i para eliminar subarbol izquierdo, d para eliminar "
                "subarbol derecho: ");
        scanf(" %c", &comp);
        
        switch(comp){
            case 'i':
                supizq(&aux);
                //Mensaje de aviso:
                printf("Arbol izquierdo de %c eliminado", inf);   
                break;
            case 'd':
                supder(&aux);
                //Mensaje de aviso:
                printf("Arbol derecho de %c eliminado", inf);   
                break;
            default:
                printf("Opcion no valida");
        }
        
    } else {
        //El elemento introducido es erróneo
        printf("El elemento no se encuentra en el arbol");
    }
    
    //Se imprime el árbol resultante en preorden
    printf("\nRecorrido PreOrden (RID):\n");     
    preorden(Arbol);
    //Se imprime la altura del árbol
    //Al solo eliminarse un nodo, AlturaArbol(Arbol) dará 4 en cualquier caso
    printf("\nAltura del arbol: %d", AlturaArbol(Arbol));
    //Se imprime el número de nodos del árbol (7, si se eliminó 'P')
    printf("\nNumero de nodos: %d", NumeroNodos(Arbol));
    

    //---------------------- Liberación de memoria -------------------------
    
    //Se destruye el árbol (y, con ello, se libera también aux)
    destruir(&Arbol);

    return (EXIT_SUCCESS);
}
