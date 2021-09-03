#include "tabla_hash_recolocacion.h"

/* TABLA HASH CON RECOLOCACION */

void InicializarTablaHash(TablaHash t) {
    int i;

    for (i = 0; i < Tam; i++)
        t[i].clave[0]=VACIO;            // Inicialmente todas las posiciones están vacías

}


/******* FUNCIONES HASH *******/

/* FUNCION HASH 1 */
/*
int Hash(char *cad) {
    int valor;
    unsigned char *c;

    for (c = cad, valor = 0; *c; c++)
        valor += (int) *c;

    return (valor % Tam);
}
*/

/* FUNCION HASH 2 */
/*
int Hash (char *cad){
    int i,suma=0;
    for (i=strlen(cad)-1;i>=0;i--){
        suma=(suma*256+cad[i])%Tam;
    }
    return suma;
}
*/

/* FUNCION HASH 3 */

  int Hash (char *cad){
     int i,suma=0;
     int K=500;
     for (i=strlen(cad)-1;i>=0;i--){
         suma=(suma*K+cad[i])%Tam;
     }
     return suma;
 }


/* RECOLOCACION LINEAL: depende del valor de la constante a*/

/* Función que localiza la posición del elemento cuando buscamos*/
int _PosicionBuscar(TablaHash t, char *cad, int *adicionales) {
    /* Devuelve el sitio donde está la clave cad, o donde debería estar. */
    /* No tiene en cuenta los borrados para avanzar.                     */

    int ini, i, aux, a = 1;

    ini = Hash(cad);

    for (i = 0; i < Tam; i++) {
        aux = (ini + a * i) % Tam;          // Recolocación lineal en función de a
        if (t[aux].clave[0] == VACIO) {     // El dato no está en la tabla
            *adicionales = i;           // Se devuelve el número de pasos adicionales que se han tenido que dar
            return aux;
        }
        if (!strcmp(t[aux].clave, cad)) {       // Se ha encontrado el dato
            *adicionales = i;
            return aux;
        }
    }

    // Se ha recorrido toda la tabla, pero no se ha encontrado el elemento.
    *adicionales = i - 1;       // El número de pasos adicionales será Tam - 1, para no contar la primera búsqueda
    return ini;
}

/* Función que localiza la posicion para insertar un elemento */
int _PosicionInsertar(TablaHash t, char *cad, int *colisiones, int *adicionales) {
    // Devuelve el sitio donde podriamos poner el elemento de clave cad

    int ini, aux, i, a = 1;

    ini = Hash(cad);

    // Se comprueba si ya se insertó un elemento anteriormente en la posición ini para determinar si hay colisión
    aux = ini % Tam;
    if (t[aux].clave[0] == VACIO || t[aux].clave[0] == BORRADO || !strcmp(t[aux].clave, cad)){
        *colisiones = 0;
        *adicionales = 0;
        return aux;         // Se devuelve la posición en la que irá el elemento
    }

    // Si la posición no está desocupada, hubo una colisión. Falta por calcular los pasos adicionales
    *colisiones = 1;

    for (i = 1; i < Tam; i++) {
        aux = (ini + a*i) % Tam;            // Posición a comprobar
        if (t[aux].clave[0] == VACIO || t[aux].clave[0] == BORRADO){        // Se ha encontrado un hueco
            *adicionales = i;               // Se han necesitado i intentos para ubicar el dato
            return aux;
        }
        if (!strcmp(t[aux].clave, cad)){
            // El valor ya está en la tabla
            *adicionales = i;               // De igual forma, se ha encontrado la ubicación del dato tras i intentos
            return aux;
        }
    }

    // Se ha recorrido toda la tabla. El contador de pasos adicionales valdrá Tam - 1
    *adicionales = i - 1;           // El -1 se debe a que el primer intento de inserción no se cuenta como paso extra
    return ini;
}


/* RECOLOCACION CUADRATICA */

/*
int _PosicionBuscar(TablaHash t, char *cad, int *adicionales) {
    // Devuelve el sitio donde esta cad  o donde deberia  estar.
    // No tiene en cuenta los borrados.

    int ini, i, aux;

    ini = Hash(cad);

    for (i = 0; i < Tam; i++) {
        aux = (ini + i*i) % Tam;
        if (t[aux].clave[0] == VACIO) {         // Hay una posición vacía. Implica que el elemento no está en la tabla
            *adicionales = i;           // Se devuelve el número de pasos adicionales que se han tenido que dar
            return aux;
        }
        if (!strcmp(t[aux].clave, cad)) {       // Se ha encontrado el dato
            *adicionales = i;
            return aux;
        }
    }

    // Se ha recorrido toda la tabla, pero no se ha encontrado el elemento.
    *adicionales = i - 1;       // El número de pasos adicionales será Tam - 1, para no contar la primera búsqueda
    return ini;
}

int _PosicionInsertar(TablaHash t, char *cad, int *colisiones, int *adicionales) {
   //Devuelve el sitio donde podriamos poner cad (recolocacion cuadratica)
    int ini, i, aux;

    ini = Hash(cad);

    // Se comprueba si ya se insertó un elemento anteriormente en la posición ini para determinar si hay colisión
    aux = ini % Tam;
    if (t[aux].clave[0] == VACIO || t[aux].clave[0] == BORRADO){
        *colisiones = 0;
        *adicionales = 0;
        return aux;         // Se devuelve la posición en la que irá el elemento
    }

    // Si la posición no está desocupada, hubo una colisión. Falta por calcular los pasos adicionales
    *colisiones = 1;

    for (i = 1; i < Tam; i++) {
        aux = (ini + i*i) % Tam;            // Posición a comprobar
        if (t[aux].clave[0] == VACIO || t[aux].clave[0] == BORRADO){        // Se ha encontrado un hueco
            *adicionales = i;               // Se han necesitado i intentos para ubicar el dato
            return aux;
        }
        if (!strcmp(t[aux].clave, cad)){
            // El valor ya está en la tabla
            *adicionales = i;               // De igual forma, se ha encontrado la ubicación del dato tras i intentos
            return aux;
        }
    }

    // Se ha recorrido toda la tabla. El contador de pasos adicionales valdrá Tam - 1
    *adicionales = i - 1;           // El -1 se debe a que el primer intento de inserción no se cuenta como paso extra
    return ini;
}
*/

/* Funcion que indica si un elemento está o no en la tabla */
/* Devuelve 1 si el elemento de clave cad está en la tabla, 0 si no está */
int MiembroHash(TablaHash t, char *cad) {

    int nCol, colision, cont = 0;    //Aunque cont no se usa, la función necesita un *int pasado como tercer argumento
    int pos = _PosicionBuscar(t,cad, &cont);

    if (t[pos].clave[0] == VACIO)
        return 0;
    else
        return (!strcmp(t[pos].clave, cad));
}

/* BUSCA UN ELEMENTO CON LA CLAVE INDICADA EN LA TABLA HASH, Y LO DEVUELVE,
 * ADEMAS DE INDICAR CON 1 QUE EXISTE EL ELEMENTO, Y CON 0 QUE NO ESTA EN LA TABLA */
int Busqueda(TablaHash t, char *clavebuscar, tipo_jugador *e, int *adicionales) {
    int pos = _PosicionBuscar(t,clavebuscar, adicionales);

    if (t[pos].clave[0] == VACIO)
        //El contador de pasos adicionales se mantiene sin cambios
        return 0;
    else {
        if (strcmp(t[pos].clave, clavebuscar) == 0) {
            *e = t[pos];
            //El contador se devolverá con el número de colisiones que se encontraron
            return 1;
        } else return 0;            //El contador se mantiene sin cambios
    }
}

/* Función que inserta un elemento en la tabla */
/* Devuelve 1 si hubo una colisión y 0 en caso contrario */
int InsertarHash(TablaHash t, tipo_jugador e, int *adicionales) {
    int pos, colisiones;

    pos = _PosicionInsertar(t, e.clave, &colisiones, adicionales);

    if (t[pos].clave[0] == VACIO || t[pos].clave[0] == BORRADO) {      // Se ha encontrado una ubicación adecuada
        t[pos] = e;             // Se guarda el dato
    }
    else if (strcmp(t[pos].clave, e.clave)){
        printf("No se encuentra posicion libre.\n");
    }
    //La otra opción es que el elemento ya estuviera insertado. En ese caso, no se modifica

    return colisiones;
}


/* Función que elimina un elemento de la tabla */
void BorrarHash(TablaHash t, char *cad) {
    int cont = 0;        //Aunque cont no se utiliza, la función necesita que se le pase un *int como tercer argumento
    int pos = _PosicionBuscar(t,cad, &cont);

    if (t[pos].clave[0] != VACIO && t[pos].clave[0] != BORRADO) {
        if (!strcmp(t[pos].clave, cad)) {
            t[pos].clave[0]= BORRADO;
            t[pos].clave[1]= VACIO;
        }
    }
}

