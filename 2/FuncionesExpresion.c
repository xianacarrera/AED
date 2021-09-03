#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "abin.h"
#include "pilaOperadores.h"
#include "pilaOperandos.h"

///////////////////////////PROTOTIPO FUNCIÓN PRIVADA
//no visible para el usario, no aparece en FuncionesExpresion.h

/* Función auxiliar que extrae un operador de la pila de operadores, crea un árbol con este como raíz y los dos primeros
 * operandos de la pila de operandos como subárboles y, finalmente, inserta el árbol resultante en la pila de operandos.
 */
//Se utiliza en arbolExpresion()
void _extraerOperador(pilaOperadores *operadores, pilaOperandos *operandos);



//////////////////////////FUNCIONES PÚBLICAS
// sí aparecen en FuncionesExpresion.h

//Devuelve la prioridad del operador dentro de la pila.
//Prioridad('(')=0
int prioridadDentro(char op) {
    int prior;
    switch (op) {
        case '^': prior = 3;
            break;
        case '*': case '/': prior = 2;
            break;
        case '+': case '-': prior = 1;
            break;
        case '(': prior = 0; //nunca va a salir de la pila (hasta que llegue ')')
            break;
    }
    return prior;
}

//Devuelve la prioridad del operador fuera de la pila.
//Prioridad('(')=4
int prioridadFuera(char op) {
    int prior;
    switch (op) {
        case '^': prior = 3;
            break;
        case '*': case '/': prior = 2;
            break;
        case '+': case '-': prior = 1;
            break;
        case '(': prior = 4; //para que SIEMPRE entre en la pila
            break;
    }
    return prior;
}

//Devuelve 1 si c es un operador: +, -, /, *, ^, ()
unsigned esOperador(char c) {
    unsigned ope;
    switch (c) {
        case '^': case '*': case '/': case '+': case '-': case '(':
            ope = 1;
            break;
        default:
            ope = 0;
    }
    return ope;
}

//Devuelve 1 si c es un operando: números, mayúsculas y minúsculas
unsigned esOperando(char c) {
    if ((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122) )
        return 1;
    else
        return 0;
}

//Recibe una cadena de caracteres y devuelve el árbol de expresión
abin arbolExpresion(char *expr_infija) {
    abin expr, aux;                 //expr guardará el árbol de expresión final, aux es un árbol auxiliar
    pilaOperadores operadores;      //pila de operadores
    pilaOperandos operandos;        //pila de operandos
    char c;                         //variable para almacenar cada carácter de la cadena de caracteres recibida
    int i = 0;                      //contador

    //Se crean las dos pilas y el árbol de expresión
    crearPilaOperadores(&operadores);
    crearPilaOperandos(&operandos);
    crear(&expr);

    //Lectura de la expresión
    while (expr_infija[i] != '\0'){         //Se para al llegar al final de la cadena (carácter '\0')
        c = expr_infija[i];                 //c almacena el carácter que se acaba de leer

        if (esOperando(c)) {

            crear(&aux);                            //Se crea un árbol nuevo
            insizq(&aux, c);                        //c se inserta como raíz
            pushOperandos(&operandos, aux);         //Se mete el árbol aux en la pila de operandos

        } else if (esOperador(c)) {

            while (!esVaciaPilaOperadores(operadores) &&
                    prioridadFuera(c) <= prioridadDentro(topeOperadores(operadores))){
                //Se extraen operadores hasta encontrar alguno con menor prioridad que c o hasta vaciar la pila
                _extraerOperador(&operadores, &operandos);       //Función auxiliar
                /* Función auxiliar: se saca un operador de la pila de operadores y dos operandos de la pila de
                 * operandos, se construye un árbol con los 3 elementos y se introduce este en la pila de operandos
                 */
            }
            pushOperadores(&operadores, c);     //Se introduce c en la pila de operadores

        } else if (c == ')') {

            while (topeOperadores(operadores) != '('){
                // Se extraen operadores hasta encontrar un '('
                _extraerOperador(&operadores, &operandos);
            }
            popOperadores(&operadores);         //Se elimina '('

        } else {
            printf("Símbolo incorrecto en la expresión");     //El carácter no es ninguna de las opciones válidas
            exit(EXIT_FAILURE);                               //Se sale del programa con un mensaje de error
        }

        i++;            // Siguiente carácter
    }

    //Una vez se ha terminado de leer la expresión, se extraen los operadores de la pila de operadores hasta vaciarla
    while (!esVaciaPilaOperadores(operadores)){
        _extraerOperador(&operadores, &operandos);
    }

    destruirPilaOperadores(&operadores);        //Se destruye la pila de operadores
    expr = topeOperandos(operandos);            //Se guarda el árbol de expresión en expr
    destruirPilaOperandos(&operandos);          //Se destruye la pila de operandos

    return expr;
}

///////////////////////////FUNCIÓN PRIVADA
//no aparece en FuncionesExpresion.h

// Función privada en favor de la modularidad
/* Saca un operador de la pila de operadores, crea un nuevo árbol con el operador como raíz y los dos primeros
*  operandos de la pila de operandos como subárboles y, por último, inserta el árbol en la pila de operandos.
*/
void _extraerOperador(pilaOperadores *operadores, pilaOperandos *operandos) {
    abin aux;           //Árbol auxiliar
    char caux;          //Para guardar el operador

    //Se desapila el primer operador de la pila de operadores y se guarda en caux
    caux = topeOperadores(*operadores);
    popOperadores(operadores);

    crear(&aux);                    //Nuevo árbol
    insizq(&aux, caux);             //El operador se introduce como raíz
    insArbolder(&aux, topeOperandos(*operandos));
    //Se introduce el primer elemento de la pila de operandos como subárbol izquierdo
    popOperandos(operandos);        //Se desapila
    insArbolizq(&aux, topeOperandos(*operandos));
    //Se introduce el siguiente elemento de la pila de operandos como subárbol izquierdo
    popOperandos(operandos);        //Se desapila

    pushOperandos(operandos, aux);      //Se introduce el árbol resultante en la pila de operandos
}