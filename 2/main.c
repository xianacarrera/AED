/* Práctica 2: Creación de un árbol de expresión */

/*
 * Autora: Xiana Carrera
 * Este programa construye un árbol a partir de una expresión aritmética dada en
 * notación infija.
 */

#include <stdlib.h>
#include <stdio.h>

#include "abin.h"
#include "FuncionesExpresion.h"
#include "recorridos.h"

int main() {
    abin arbol;         //Árbol donde se guardará el árbol de expresión resultante
    char cadena[30];    //Expresión aritmética. Tamaño máximo prefijado en 30

    printf("Introduzca la expresion aritmetica infija: ");
    scanf("%s", cadena);

    arbol = arbolExpresion(cadena);     //Se guarda el árbol de expresión en la variable arbol

    //Se imprime el resultado en preorden y postorden
    printf("La expresion prefija correspondiente es:\n");
    preorden(arbol);
    printf("\nLa expresion postfija correspondiente es:\n");
    postorden(arbol);

    //Se destruye el árbol al finalizar
    destruir(&arbol);

    return (EXIT_SUCCESS);
}
