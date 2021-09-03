//
// Created by Xiana on 16/12/2020.
//

#ifndef LISTA_H
#define LISTA_H

/**************************** NUEVOS ELEMENTOS PARA RAMIFICACIÓN Y PODA ******************************/

#define N 6     // Número de niveles = Número de tareas = Número de personas

typedef struct{
    int tupla[N];           // Solución parcial correspondiente al nodo
    int nivel;              // Nivel del nodo
    int bact;               // Beneficio acumulado hasta ese punto
    float CI, BE, CS;      // Cota inferior, beneficio estimado, cota superior
    int usadas[N];          // Vector de tareas usadas (cada posición tiene 0 o 1)
} Nodo;

typedef Nodo tipoelem;


/******************************** TAD LISTA ORIGINAL (SIN CAMBIOS) ************************************/

struct celda
{
tipoelem elemento;
struct celda *sig;
};

typedef struct celda * posicion;

struct l
{
posicion inicio;
unsigned longitud;
posicion fin;
};

typedef struct l * lista;

void crea(lista *l);
void destruye(lista *l);
posicion primero(lista l);
posicion siguiente(lista l, posicion p);
posicion fin(lista l);
posicion anterior(lista l, posicion p);
unsigned existe(lista l);
unsigned esvacia(lista l);
void recupera(lista l, posicion p, tipoelem *e);
unsigned longitud(lista l);
void inserta (lista *l, posicion p, tipoelem e);
void suprime (lista *l, posicion p);
void modifica (lista *l, posicion p, tipoelem e);
posicion posinser (lista l, tipoelem e);


#endif //LISTA_H