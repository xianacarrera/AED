#include <stdio.h>
#include <stdlib.h> //necesaria para rand()
#include <string.h> //para comparar cadenas
#include <math.h> //para calcular el número de impostores con round()

#include "abb.h"
#include "cola.h"
#include "funciones.h"

#define NTAREAS 5   // Constante. Cada jugador tiene 5 tareas asignadas al empezar la partida.


////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// FUNCIONES PRIVADAS


//Función privada que genera un número aleatorio entre inf y sup
//Usada en las funciones _darRol(), _darTarea() y jugarPartida()
unsigned int _aleatorio(int inf, int sup) {
    return (rand() % (sup - inf + 1)) +inf;
}

//Función privada para inicializar los datos de un jugador, necesaria en varias funciones públicas
//Usada en las funciones _limpiarDatos(), leerArchivo(), altaJugador() y jugarPartida()
void _inicializarJugador(tipoelem * registro) {
    registro->rol = ' ';
    crear_cola(&registro->tareas);          //Se crea una nueva cola de tareas
}


//Función privada que imprime las tareas de una cola dada (descripción y lugar para cada una)
//Usada en la función _imprimirJugador()
void _imprimirTareas(cola *c){
    cola aux;                   //Cola auxiliar
    tipoelemCola EC;            //Tarea

    crear_cola(&aux);           //Inicialización de la cola auxiliar

    printf("\t\t\tTareas asignadas:\n\t\t\t{\n");

    while (!es_vacia_cola(*c)){         //Bucle que vacía la cola pasada en aux
        EC = primero(*c);               //Se lee el elemento del frente de la cola
        printf("\t\t\t\tTarea: \033[0;36m|%s|\033[0m en \033[0;36m|%s|\033[0m\n", EC.descripcionTarea, EC.lugarTarea);
        insertar_cola(&aux, EC);        //Se introduce el elemento en la cola auxiliar
        suprimir_cola(c);               //Se elimina EC de la cola c, pasando el frente al siguiente elemento
    }

    while(!es_vacia_cola(aux)){         //Bucle para volver a insertar todos los nodos en la cola c
        EC = primero(aux);              //Se lee el primer elemento de aux por el frente
        insertar_cola(c, EC);           //Se introduce en c por el final
        suprimir_cola(&aux);            //Se suprime dicho elemento de aux, pasando el frente al siguiente elemento
    }

    printf("\t\t\t}\n\n");

    destruir_cola(&aux);                //aux está vacía. No hay riesgo de eliminar información en la cola c
}

//Función privada que imprime los datos de un jugador
//Usada en la función listadoJugadores()
void _imprimirJugador(tipoelem E) {
    printf("\tNombre de Jugador: \033[0;35m%s\033[0m\n", E.nombreJugador);
    // Si el jugador tiene una tarea asignada (con descripción y lugar), se imprime
    if (E.tareas != NULL && !es_vacia_cola(E.tareas)){
        //Antes de llamar a es_vacia_cola se comprueba que tal cola exista
        _imprimirTareas(&E.tareas);
    } else {      //No hay tareas
        printf("\tNo jugó a AmongETSE\n\n");
    }
}

//Función privada que imprime los nombres y ubicaciones de los jugadores, y avisa si han muerto o sido expulsados
//Usada en jugarPartida()
void _estadoJugadores(abb A){
    tipoelem E;

    if (!es_vacio(A)){
        leer(A, &E);            //Se lee el nodo
        printf("\033[0;35m%-15s\033[0m", E.nombreJugador);
        //Nombre alineado a la izquierda en un campo de 15 caracteres

        /* Si el jugador está vivo o ha muerto en este turno, se imprimirá la habitación en la que se encuentra.
         * La condición se comprueba con E.tareas != NULL. No se puede usar los roles, debido a que estos
         * ya han sido actualizados y permiten distinguir si un jugador está vivo o muerto, pero no si su asesinato
         * ha sucedido en este turno. En cambio, la cola de tareas se destruye en _siguienteTarea(), igualándose a NULL,
         * y sí permite separar estas situaciones.*/
        if (E.tareas != NULL) {
            printf(" situado en: \033[0;36m|%-15s|\033[0m", E.habitacionActual);
            //Lugar alineado a la izquierda en un campo de 15 caracteres (se rellena con espacios si es necesario)
        }

        //Si un jugador ha muerto o fue expulsado (en este turno o en uno anterior), se muestra
        if (E.rol == 'K') {
            printf("\033[0;31m ha muerto\033[0m");
        } else if (E.rol == 'E'){
            printf("\033[0;35m ha sido expulsado\033[0m");
        }

        printf("\n");

        //Función recursiva
        _estadoJugadores(izq(A));
        _estadoJugadores(der(A));
    }
}

//Función privada que borra los datos guardados de los jugadores de un árbol (rol, descripcionTarea y lugarTarea)
//Se utiliza en jugarPartida()
void _limpiarDatos(abb A){
    tipoelem E;
    if (!es_vacio(A)){              //Función recursiva
        leer(A, &E);                //Se guarda la información del nodo en E
        _inicializarJugador(&E);    //El campo rol se vacía y se crea una nueva cola de tareas
        modificar(A, E);            //Se guardan los cambios en el nodo cuyo nombreJugador coincide con el de E
        _limpiarDatos(izq(A));
        _limpiarDatos(der(A));
    }
}

//Función privada que cuenta el número de nodos de un árbol
//Utilizada en jugarPartida()
void _contarNodos(abb A, int *c){
    if (!es_vacio(A)){
        (*c)++;   //Se registra la existencia del nodo actual aumentando el contador en 1
        //Se sigue recorriendo el árbol
        _contarNodos(izq(A), c);
        _contarNodos(der(A), c);
    }
}

//Función privada que devuelve un rol al alzar: tripulante (T) o impostor (I)
//Se emplea en _asignarRolesAleat()
int _darRol(char *rol){
    int rand;

    rand = _aleatorio(0, 1);        //rand solo puede valer 0 o 1

    switch (rand){
        case 0:
            *rol = 'T';
            break;
        case 1:
            *rol = 'I';
    }

    return rand;    //Se devuelve la información sobre cuál fue el valor generado: 0 <-> T o 1 <-> I
}

//Función privada que asigna roles a los jugadores de los árboles
//Se emplea en jugarPartida()
void _asignarRolesAleat(abb A, abb B, int *contador, int nImpostores){
    //Los árboles de usuarios y jugadores no comparten nodos (con el objetivo de que no tengan la misma cola de tareas)
    //En consecuencia, se debe registrar el mismo rol en ambos

    tipoelem E, F;

    if (!es_vacio(A)){
        leer(A, &E);        //Se almacenan los datos del nodo en el tipoelem E
        if (*contador < nImpostores) {          //Aún no se han generado todos los impostores requeridos
            *contador += _darRol(&E.rol);       //Se le da a E un rol al azar
            //Si el rol es de impostor, _darRol devuelve 1 y el valor de contador aumenta
            //Si el rol es de tripulante, _darRol devuelve 0 y el valor de contador no es alterado
        } else {
            //Si ya se han asignado todos los impostores necesarios, el resto de jugadores serán tripulantes
            E.rol = 'T';
        }

        buscar_nodo(B, E.nombreJugador, &F);        //Se guarda el nodo del árbol de usuarios con el mismo nombre en F
        F.rol = E.rol;                              //Se registra el mismo rol para nodo del árbol de usuarios

        //Se guardan los cambios en los árboles. Se modifican los nodos con el mismo nombreJugador que E y F
        modificar(A, E);
        modificar(B, F);

        _asignarRolesAleat(izq(A), B, contador, nImpostores);
        _asignarRolesAleat(der(A), B, contador, nImpostores);
    }
}

//Función privada que asigna una tarea, es decir, una descripción de la misma y un lugar, a un determinado jugador
//Empleada en _asignarTareas()
void _darTarea(tipoelemCola *EC){
    //Modificación: ahora la función recibe un tipoelemCola, que representa una tarea

    switch (_aleatorio(1,8)){       //Se genera un entero aleatorio entre 1 y 8, ambos incluidos
        //Dependiendo del resultado, se copia una determinada descripción y su lugar asociado
        case 1:
            strncpy(EC->descripcionTarea, "Alinear la salida del motor", 28);
            //Aunque la cadena solo tiene 27 caracteres, se copian 28 para que termine en '\0'
            switch(_aleatorio(1, 2)){
                case 1:
                    strncpy(EC->lugarTarea, "MotorSuperior", 14);
                    break;
                case 2:
                    strncpy(EC->lugarTarea, "MotorInferior", 14);
            }
            break;
        case 2:
            strncpy(EC->descripcionTarea, "Calibrar distribuidor", 22);
            strncpy(EC->lugarTarea, "Electricidad", 13);
            break;
        case 3:
            strncpy(EC->descripcionTarea, "Descargar datos/subir datos", 28);
            switch(_aleatorio(1, 5)){   // Varios lugares posibles
                case 1:
                    strncpy(EC->lugarTarea, "Cafeteria", 10);
                    //Se ha elegido no usar tildes para facilitar la lectura y el guardado en ficheros
                    break;
                case 2:
                    strncpy(EC->lugarTarea, "Comunicacion", 13);
                    break;
                case 3:
                    strncpy(EC->lugarTarea, "Armeria", 8);
                    break;
                case 4:
                    strncpy(EC->lugarTarea, "Electricidad", 13);
                    break;
                case 5:
                    strncpy(EC->lugarTarea, "Navegacion", 11);
            }
            break;
        case 4:
            strncpy(EC->descripcionTarea, "Desviar energia", 16);
            switch(_aleatorio(1, 8)){
                case 1:
                    strncpy(EC->lugarTarea, "Navegacion", 11);
                    break;
                case 2:
                    strncpy(EC->lugarTarea, "O2", 3);
                    break;
                case 3:
                    strncpy(EC->lugarTarea, "Seguridad", 10);
                    break;
                case 4:
                    strncpy(EC->lugarTarea, "Armeria", 8);
                    break;
                case 5:
                    strncpy(EC->lugarTarea, "Comunicacion", 13);
                    break;
                case 6:
                    strncpy(EC->lugarTarea, "Escudos", 8);
                    break;
                case 7:
                    strncpy(EC->lugarTarea, "MotorSuperior", 14);
                    break;
                case 8:
                    strncpy(EC->lugarTarea, "MotorInferior", 14);
            }
            break;
        case 5:
            strncpy(EC->descripcionTarea, "Encender escudos", 17);
            strncpy(EC->lugarTarea, "Escudos", 8);
            break;
        case 6:
            strncpy(EC->descripcionTarea, "Estabilizar direccion", 22);
            strncpy(EC->lugarTarea, "Navegacion", 11);
            break;
        case 7:
            strncpy(EC->descripcionTarea, "Limpiar el filtro O2", 21);
            strncpy(EC->lugarTarea, "O2", 3);
            break;
        case 8:
            strncpy(EC->descripcionTarea, "Mapa de navegacion", 19);
            strncpy(EC->lugarTarea, "Navegacion", 11);
    }
}


//Función privada que genera NTAREAS = 5 tareas aleatorias y las inserta en la cola de un jugador
//Se utiliza en _asignarTareas() y en _sabotear()
void _insertarTareaColaTareas(cola *c, cola *d){
    tipoelemCola EC;
    int i;

    /* El jugador está "duplicado" (se encuentra tanto en el árbol de jugadores como en el de usuarios), por lo que se
     * debe pasar la cola correspondiente a cada uno de los dos árboles, registrando la misma información para ambos*/

    for (i = 0; i < NTAREAS; i++){
        _darTarea(&EC);                 //EC recibe una tarea aleatoria (descripción y lugar)
        insertar_cola(c, EC);           //El tipoelemCola se añade a la cola del nodo del árbol de jugadores
        insertar_cola(d, EC);           //El tipoelemCola se añade a la cola del nodo del árbol de usuarios
    }
}


//Función privada que asigna tareas a los jugadores de un árbol
//Se utiliza en jugarPartida()
void _asignarTareas(abb A, abb B){
    tipoelem E, F;

    if (!es_vacio(A)){
        leer(A, &E);                                //Se almacenan los datos del nodo en el tipoelem E
        buscar_nodo(B, E.nombreJugador, &F);        //Se guarda el nodo análogo del árbol de usuarios en F

        //Los nodos reciben cada uno una cola de tareas, pero estas contienen la misma información
        _insertarTareaColaTareas(&E.tareas, &F.tareas);

        modificar(A, E);        //Se actualizan los cambios en el nodo del árbol de jugadores
        modificar(B, F);        //Se actualizan los cambios en el nodo del árbol de usuarios

        //Función recursiva
        _asignarTareas(izq(A), B);
        _asignarTareas(der(A), B);
    }
}

//Función privada que busca un elemento con un determinado índice siguiendo preorden en un árbol
//Se usa en jugarPartida()
void _buscarPorIndice(abb A, int index, int *contador, tipoelem *E){
    if (!es_vacio(A)){
        (*contador)++;
        //Como los jugadores se numeran desde el 1, se aumenta el contador nada más empezar en la raíz

        if (*contador == index){        //Se ha encontrado el jugador
            leer(A, E);                 //Se guardan sus datos en el tipoelem
        }

        //Si contador coincide con el índice, se para de buscar
        if (*contador != index){
            _buscarPorIndice(izq(A), index, contador, E);
        }

        //Si contador coincide con el índice, se para de buscar
        if (*contador != index){
            _buscarPorIndice(der(A), index, contador, E);
        }
    }
}

//Función privada que imprime los perfiles de los jugadores que se encuentren en una habitación dada
//Se emplea en la función consultarPorHabitacion()
void _buscarPorHabitacion(abb Arbol, char *hab){
    //*hab es una cadena de caracteres con el nombre de la habitación
    tipoelem E;
    tipoelemCola EC;

    if (!es_vacio(Arbol)){
        leer(Arbol, &E);        //Se guardan los datos del nodo en el tipoelem E
        if (E.tareas != NULL && !es_vacia_cola(E.tareas)){
            //No se puede llamar a primero() si la cola está vacía, pues daría error

            EC = primero(E.tareas);                  //Elemento del frente de la cola (tarea actual)
            if (strcmp(EC.lugarTarea, hab) == 0){    //Si strcmp devuelve 0, las cadenas coinciden
                printf("\t\033[0;35m%s\033[0m: Tarea \033[0;36m|%s|\033[0m en \033[0;36m|%s|\033[0m\n",
                        E.nombreJugador, EC.descripcionTarea, EC.lugarTarea);
            }
        }
        //Se continúa recorriendo el árbol en búsqueda de más coincidencias
        _buscarPorHabitacion(izq(Arbol), hab);
        _buscarPorHabitacion(der(Arbol), hab);
    }
}

//Función privada que imprime un menú de selección de habitaciones
//Utilizada en consultarPorHabitacion()
void _mostrarHabitaciones(){
    printf("Lista de habitaciones:\n{");
    printf("\n\t1. Armería");
    printf("\n\t2. Cafetería");
    printf("\n\t3. Comunicaciones");
    printf("\n\t4. Electricidad");
    printf("\n\t5. Escudos");
    printf("\n\t6. Motores");
    printf("\n\t7. Navegación");
    printf("\n\t8. O2");
    printf("\n\t9. Seguridad");
    printf("\n\t0. Volver al menú\n");
    printf("}\nSeleccione habitación: ");
}

//Función privada que imprime únicamente los nombres de los miembros de un árbol en orden alfabético
//Se utiliza en jugarPartida() y leerArchivo()
void _listaJugadores(abb A){
    tipoelem E;
    //Para que el resultado aparezca en orden alfabético, se debe utilizar inorden
    if (!es_vacio(A)){
        _listaJugadores(izq(A));           //Se recorre el subárbol izquierdo
        leer(A, &E);                        //Se lee el nodo
        printf("\t%s\n", E.nombreJugador);    //Se imprime el nombre del jugador
        _listaJugadores(der(A));           //Se recorre el subárbol derecho
    }
}

//Función privada que recorre el árbol de usuarios guardando los nombres de los jugadores en un fichero
//Es necesaria para la función pública guardarArchivo()
void _guardarUsuarios(FILE *fp, abb A){
    //El archivo de texto se pasa ya abierto
    tipoelem E;

    //Para que el resultado aparezca en orden alfabético, se debe utilizar inorden
    if (!es_vacio(A)){
        _guardarUsuarios(fp, izq(A));               //Se recorre el subárbol izquierdo
        leer(A, &E);                                //Se lee el nodo
        fprintf(fp, "\t%s\n", E.nombreJugador);     //Se imprime el nombre del jugador
        _guardarUsuarios(fp, der(A));               //Se recorre el subárbol derecho
    }
}

//Función privada que guarda en el parámetro num el número de tripulantes que hay en una habitación dada
//Se utiliza en _ejecutarTarea()
void _contarTripulantesHabitacion(abb A, char *hab, int *num){
    tipoelem E;

    if (!es_vacio(A)){
        leer(A, &E);                      //Se guardan los datos del nodo en el tipoelem E
        if (E.rol == 'T' && strcmp(E.habitacionActual, hab) == 0){
            //Si strcmp devuelve 0, las cadenas coinciden
            //Con la condición E.rol == 'T' se evita que se cuente a los impostores
            (*num)++;           //Se aumenta el contador
        }

        //Se continúa recorriendo el árbol en búsqueda de más coincidencias
        _contarTripulantesHabitacion(izq(A), hab, num);
        _contarTripulantesHabitacion(der(A), hab, num);
    }
}

//Función privada que cuenta el número de jugadores que tienen un determinado rol
//Se llama desde jugarPartida() y desde _ejecutarTarea()
void _contarJugadoresRol(abb A, char rol, int *cuenta){
    tipoelem E;

    if (!es_vacio(A)){
        leer(A, &E);
        if (E.rol == rol){      //Si el rol es el pasado a la función, se aumenta el contador
            (*cuenta)++;
        }
        _contarJugadoresRol(izq(A), rol, cuenta);
        _contarJugadoresRol(der(A), rol, cuenta);
    }
}

//Función privada que simula el asesinato de un tripulante por parte de un impostor
//Es empleada en _ejecutarTarea()
void _kill(abb A, char *hab, int elegido, int *num){
    tipoelem E;

    if (!es_vacio(A)) {
        leer(A, &E);                        //Se guardan los datos del nodo en el tipoelem E
        if (E.rol == 'T' && strcmp(E.habitacionActual, hab) == 0) {
            //Si strcmp devuelve 0, las cadenas coinciden
            //Con la condición E.rol == 'T' se impone que los únicos jugadores que pueden ser asesinados
            //son los tripulantes
            (*num)++;           //Aumenta el contador
            if (*num == elegido) {           //El contador ha llegado al número que fue elegido aleatoriamente
                E.rol = 'K';                //El jugador se marca como muerto, pero aún no se limpian sus tareas
                modificar(A, E);            //Se guardan los cambios
            }
        }


        //Se continúa recorriendo el árbol solo si aún no se ha encontrado al tripulante elegido
        if (*num < elegido) {
            _kill(izq(A), hab, elegido, num);
        }

        if (*num < elegido) {
            _kill(der(A), hab, elegido, num);
        }
    }
}

//Función privada que añade una tarea más a todos los jugadores
//Se usa en _ejecutarTarea()
void _sabotear(abb A, abb B){
    tipoelemCola EC;
    tipoelem E, F;

    //Se modifica también el árbol de usuarios para mantener un registro actualizado de cara a las opciones del menú

    if (!es_vacio(A)){
        leer(A, &E);                                //Se almacenan los datos del nodo en el tipoelem E
        buscar_nodo(B, E.nombreJugador, &F);        //Se guarda el nodo análogo del árbol de usuarios en F

        if (E.rol != 'K' && E.rol != 'E') {         //Solo se añade una tarea a los impostores y a los tripulantes
            //Se crea una tarea aleatoria y se añade a la lista de tareas del nodo correspondiente en ambos árboles
            _darTarea(&EC);
            insertar_cola(&E.tareas, EC);
            insertar_cola(&F.tareas, EC);

            modificar(A, E);        //Se actualizan los cambios en el nodo del árbol de jugadores
            modificar(B, F);        //Se actualizan los cambios en el nodo del árbol de usuarios
        }

        //Función recursiva
        _sabotear(izq(A), B);
        _sabotear(der(A), B);
    }
}

//Función privada que simula el proceso de ejecución de una tarea, y posibilita una ocasión de asesinato
//Es necesaria para la función jugarPartida()
void _ejecutarTarea(abb A, abb B, abb jugadores, int *contador, int nImpostores, int *sabotaje){
    tipoelem E;
    int i, num = 0, elegido, intervencion = 1, nFantasmas = 0;

    if (!es_vacio(A)){
        //En primer lugar, se recorre el árbol en busca de los impostores
        leer(A, &E);

        if (E.rol == 'I'){
            (*contador)++;          //Se ha encontrado a un impostor
            if (strcmp(E.habitacionActual, "En transito") != 0){
                _contarTripulantesHabitacion(jugadores, E.habitacionActual, &num);
                //No se puede pasar A, porque es un subárbol de jugadores, y se necesita comprobar el árbol completo
            } else {
                num = -1;
                /* Si se dejara num = 0, tendría lugar un sabotaje, pero la condición para este es que un impostor
                 * esté solo en una habitación. Que esté moviéndose no es válido.*/
                if (*sabotaje == 0) {       //*sabotaje = -1 solo tiene prioridad sobre *sabotaje = 0
                    *sabotaje = -1;   //Se indica que el impostor estaba en tránsito. No se puede producir un sabotaje
                }
            }

            if(num > 0){        //Hay al menos un tripulante
                if (*sabotaje == 0 || *sabotaje == -1) {            //Se sobreescribe *sabotaje
                    *sabotaje = 2;      //No ha ocurrido un sabotaje, porque el impostor está acompañado
                }
                _contarJugadoresRol(A, 'K', &nFantasmas);       //Se cuenta el número de jugadores muertos
                for (i = 0; i < nFantasmas; i++){               //Para cada fantasma, se genera un número aleatorio
                    if (_aleatorio(1, 100) <= 10){
                        /* Si al menos uno de los números aleatorios generados (entre 1 y 100, ambos inclusive), es
                         * menor o igual que 10 (10% de posibilidades), ocurre una intervención y el tripulante que iba
                         * a morir es salvado*/
                        printf("\033[0;32mHa ocurrido una intervención de ultratumba\033[0m\n");
                        intervencion = 0;
                        break;
                    }
                }

                if (intervencion) {         //Los fantasmas no han intervenido. Un tripulante de la habitación muere
                    elegido = _aleatorio(1, num);   //Se selecciona uno de los tripulantes de la habitación al azar
                    num = 0;                            //Se reutiliza num
                    _kill(jugadores, E.habitacionActual, elegido, &num);     //El impostor mata al jugador elegido
                }

            } else if (num == 0){        //num == 0, el impostor está solo
                _sabotear(jugadores, B);
                *sabotaje = 1;      //Se indica que ha ocurrido un sabotaje (ahora los jugadores tendrán 1 tarea más)
            }
        }

        //Se sigue recorriendo el árbol solo si aún no se ha encontrado a todos los impostores
        //nImpostores se pasa a la función por este motivo: agiliza el recorrido del árbol
        if (*contador < nImpostores){
            _ejecutarTarea(izq(A), B, jugadores, contador, nImpostores, sabotaje);
        }

        if (*contador < nImpostores){
            _ejecutarTarea(der(A), B, jugadores, contador, nImpostores, sabotaje);
        }

    }
}

//Función privada que ofrece al usuario la posibilidad de adivinar quién es el impostor
//See llama desde jugarPartida()
void _adivinarJugador(abb A, int *nImpostores){
    tipoelem votado;

    printf("Vote por un impostor (skip para votar en blanco): ");
    scanf("%s", votado.nombreJugador);

    if (strcmp(votado.nombreJugador, "skip") == 0 || strcmp(votado.nombreJugador, "Skip") == 0){
        printf("\n");
        return;         //Se da la opción de votar en blanco
    }

    if (es_miembro(A, votado)){             //Si el jugador existe, se busca para comprobar que no esté muerto
        buscar_nodo(A, votado.nombreJugador, &votado);
    }

    while (!es_miembro(A, votado) || votado.rol == 'K'){
        if (!es_miembro(A, votado)) {                   //El nombre no es válido (no está en el árbol de jugadores)
            printf("No es un jugador de la partida\n");
        } else {                                        //El nombre no es válido, porque el jugador debe estar vivo
            printf("El jugador ha muerto\n");
        }
        printf("Vote por un impostor (skip para votar en blanco): ");
        scanf("%s", votado.nombreJugador);
        if (strcmp(votado.nombreJugador, "skip") == 0 || strcmp(votado.nombreJugador, "Skip") == 0){
            printf("\n");
            return;         //Se vuelve a dar la opción de votar en blanco
        }
        if (es_miembro(A, votado)){             //Si existe, se comprobará que el jugador no esté muerto
            buscar_nodo(A, votado.nombreJugador, &votado);
        }
    }

    if (votado.rol == 'T'){
        printf("%s no era el/la impostor@\n\n", votado.nombreJugador);
    } else {
        printf("\033[0;32m%s era el/la impostor@\033[0m\n\n", votado.nombreJugador);
        (*nImpostores)--;           //Queda un impostor menos en el juego
    }

    votado.rol = 'E';           //Fuera o no el impostor, el jugador es expulsado
    modificar(A, votado);       //Se guardan los cambios
}

//Función privada que pasa a la siguiente tarea o destruye la cola de tareas si el jugador ha muerto o fue expulsado
//Es necesaria para jugarPartida()
void _siguienteTarea(abb A){
    tipoelem E;

    /* Los cambios en las colas solo se realizan en el árbol de jugadores, con el fin de preservar la información para
     * consultas desde el menú después de la partida*/

    if (!es_vacio(A)){
        leer(A, &E);
        if (E.tareas != NULL && !es_vacia_cola(E.tareas)){
            //Para suprimir un elemento o destruir la cola, esta debe ser no vacía (y existir)
            if ((E.rol == 'T' || E.rol == 'I') && !strcmp(E.habitacionActual, primero(E.tareas).lugarTarea)){
                //Solo se suprime una tarea si esta se ha realizado en el turno actual
                suprimir_cola(&E.tareas);           //Se elimina una tarea
            } else if (E.rol == 'K' || E.rol == 'E'){
                //El jugador ha muerto o fue expulsado en este turno (su cola de tareas aún no se había destruido)
                destruir_cola(&E.tareas);
                E.tareas = NULL;
            }
        }
        modificar(A, E);        //Se guardan los cambios

        //Se sigue recorriendo el árbol
        _siguienteTarea(izq(A));
        _siguienteTarea(der(A));
    }
}

//Función privada, llamada al empezar la partida, que coloca a todos los jugadores en Cafetería
//Es usada en jugarPartida()
void _irACafeteria(abb A){
    tipoelem E;

    if (!es_vacio(A)){
        leer(A, &E);                                //Se almacenan los datos del nodo en el tipoelem E

        strcpy(E.habitacionActual, "Cafeteria");

        modificar(A, E);        //Se actualizan los cambios en el nodo del árbol de jugadores

        //Función recursiva
        _irACafeteria(izq(A));
        _irACafeteria(der(A));
    }
}

//Función privada que mueve a los jugadores desde su posición hasta la de su próxima tarea por el camino más corto
//Es llamada desde jugarPartida()
void _moverse(abb A, grafo G){
    tipoelem E;
    tipovertice v1, v2, proximo;
    char *habSiguiente;

    if (!es_vacio(A)) {
        leer(A, &E);

        if (E.tareas != NULL && !es_vacia_cola(E.tareas) && (E.rol == 'T' || E.rol == 'I')) {
            //Solo los impostores y tripulantes vivos y con tareas pendientes se mueven
            if (E.distanciaDestino == 0) {    //El jugador está en una habitación. Debe determinarse a dónde irá
                strncpy(v1.habitacion, E.habitacionActual, L);   //Se almacena en un tipovertice la habitación actual
                strncpy(v2.habitacion, primero(E.tareas).lugarTarea, L);            //Habitación de destino

                if (strcmp(v1.habitacion, v2.habitacion) != 0) {
                    //Si la habitación de origen y la de destino coinciden, no hace falta llamar a floyd()

                    //Se guarda en habSiguiente la próxima en el camino hasta la de destino
                    habSiguiente = floyd(G, posicion(G, v1), posicion(G, v2), E.rol);
                    strncpy(proximo.habitacion, habSiguiente, L);       //Se guarda habSiguiente en un vértice
                    free(habSiguiente);     //Se puede liberar la memoria que fue reservada en floyd()

                    strncpy(E.habitacionSiguiente, proximo.habitacion, L);   //Se guarda la información para el jugador
                    switch (E.rol) {            //Se calcula la distancia a E.habitacionSiguiente
                        case 'T':
                            E.distanciaDestino = distancia_T(G, posicion(G, v1), posicion(G, proximo));
                            break;
                        case 'I':
                            E.distanciaDestino = distancia_I(G, posicion(G, v1), posicion(G, proximo));
                    }
                    strncpy(E.habitacionActual, "En transito", L);      //El jugador está moviéndose
                }

            } else {            //No ha llegado a su destino aún
                E.distanciaDestino--;
                if (E.distanciaDestino == 0){       //Tras reducir en 1 la distancia, sí ha llegado
                    strncpy(E.habitacionActual, E.habitacionSiguiente, L);      //Se actualiza la habitación actual
                }
            }

            modificar(A, E);        //Se guardan los cambios
        }

        //Función recursiva
        _moverse(izq(A), G);
        _moverse(der(A), G);
    }
}

//Función privada que comprueba si quedan tripulantes vivos con tareas por realizar (condicionante para seguir jugando)
//Se utiliza en jugarPartida()
void _comprobarTareas(abb A, int *exist){
    tipoelem E;

    if (!es_vacio(A)){
        _comprobarTareas(izq(A), exist);
        leer(A, &E);
        if (E.rol == 'T' && E.tareas != NULL && !es_vacia_cola(E.tareas)){
            *exist = 1;
        }
        _comprobarTareas(der(A), exist);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// FUNCIONES PÚBLICAS

//Función pública que imprime los jugadores por orden alfabético
void listadoJugadores(abb A) {
    tipoelem E;
    //Para que el resultado aparezca en orden alfabético, se debe utilizar inorden
    if (!es_vacio(A)){
        listadoJugadores(izq(A));           //Se recorre el subárbol izquierdo
        leer(A, &E);                        //Se lee el nodo
        _imprimirJugador(E);                //Se imprime el jugador (función privada)
        listadoJugadores(der(A));           //Se recorre el subárbol derecho
    }
}

//Función pública para leer el archivo de disco
void leerArchivo(abb *A) {
    tipoelem registro;
    FILE *fp;
    fp = fopen("ETSErsG3.txt", "rt");
    if (fp) {
        fscanf(fp, " %s", registro.nombreJugador);      //Se almacena el nombre del jugador
        while (!feof(fp)) {
            _inicializarJugador(&registro);             //El resto de campos simplemente se inicializan
            insertar(A, registro);                      //Se inserta el elemento en el árbol
            fscanf(fp, " %s", registro.nombreJugador);
        }
        fclose(fp);
        printf("Lectura del archivo:\n{\n");            //Se imprime el árbol
        _listaJugadores(*A);
        printf("}\n");
    } else {
        printf("Error en la lectura del archivo de jugadores\n");
    }
}


//Función pública que guarda la lista de usuarios en un archivo de texto
void guardarArchivo(abb A){
    FILE *fp;
    fp = fopen("jugadores.txt", "wt");      //Se abre el archivo en modo escritura (w) con texto (t)

    if (fp) {               //fopen no ha dado error
        fprintf(fp, "Lista de usuarios registrados en AmongETSE:\n");
        fprintf(fp, "{\n");
        _guardarUsuarios(fp, A);         //Función privada que recorre el árbol registrando los usuarios
        fprintf(fp, "}");

        printf("Archivo guardado\n");           //Aviso por pantalla
        fclose(fp);                             //Se cierra el archivo
    } else {           //El modo escritura crea el archivo si este no existe, pero fopen puede dar error igualmente
        printf("Error en la creación del archivo\n");
    }
}

//Función pública que añade un jugador al árbol
void altaJugador(abb *Arbol) {
    tipoelem jugador;

    printf("\nIntroduzca el jugador a dar de alta (@xxx): ");
    scanf("%s", jugador.nombreJugador);

    if (jugador.nombreJugador[0] != '@'){
        printf("El nombre de los jugadores debe empezar por '@'\n");
        return;     //Se devuelve al usuario al menú
    } else if (es_miembro(*Arbol, jugador)) {
        printf("El jugador ya ha sido registrado\n");
        return;     //Se devuelve al usuario al menú
    }

    //El nombre es válido. El resto de campos se inicializan y el elemento se inserta
    _inicializarJugador(&jugador);
    insertar(Arbol, jugador);
}

//Función pública que elimina un jugador del árbol
void bajaJugador(abb *Arbol) {
    tipoelem jugador;

    printf("\nIntroduzca el jugador a dar de baja: ");
    scanf("%s", jugador.nombreJugador);

    if (!es_miembro(*Arbol, jugador)){
        printf("El jugador no ha sido registrado\n");
        return;     //Se devuelve al usuario al menú
    }

    //El elemento existe en el árbol. Se elimina
    suprimir(Arbol, jugador);
}

//Función pública que da paso a un juego de AmongETSE
void jugarPartida(abb *Arbol, grafo G) {
    /* Arbol se pasa por referencia para dar la opción al usuario de insertar un nuevo elemento cuando selecciona
     * los jugadores manualmente y este no se encuentra entre los disponibles*/

    abb jugadores;                                          //Árbol de los jugadores de la partida
    tipoelem jugador;                                       //Variable temporal para un jugador
    int nJugadores, nImpostores, nUsuarios;                 //Valores de referencia
    int numElegido, nInsertados = 0, contador;              //Contadores y variables dependientes del usuario
    int exist, sabotaje, controlSabotajes = 0;              //Contadores y variables dependientes del usuario
    char opcion, victoria = ' ';                            //Selección y condición de victoria

    crear(&jugadores);          //Se crea el árbol

    nUsuarios = 0;     //A la función _contarNodos se le debe pasar un contador inicializado a 0
    _contarNodos(*Arbol, &nUsuarios);

    if (nUsuarios < 4){
        printf("\nNúmero de usuarios no suficiente para una partida. Es necesario dar de alta más jugadores");
        return;     //Se vuelve al menú
    }

    //Se deben limpiar los datos del árbol de usuarios de anteriores partidas
    _limpiarDatos(*Arbol);
    //Con esto se garantiza que los nodos que se copian en el árbol de jugadores solo contienen el nombre

    //Lectura de datos -------------------------------------------------------------------------------------

    printf("\nIntroduzca el número de jugadores (4-10): ");
    scanf("%d", &nJugadores);

    while (nJugadores < 4 || nJugadores > 10 || nJugadores > nUsuarios) {
        printf("El número de jugadores debe estar entre 4 y 10 y ser menor o igual que el número de usuarios"
               " (%d)", nUsuarios);
        printf("\nIntroduzca el número de jugadores: ");
        scanf("%d", &nJugadores);
    }

    printf("\nIntroduzca 0 para elegir los jugadores o 1 para seleccionarlos aleatoriamente: ");
    scanf(" %c", &opcion);

    while (opcion != '0' && opcion != '1') {
        printf("Opción no válida");
        printf("\nIntroduzca 0 para elegir los jugadores o 1 para seleccionarlos aleatoriamente: ");
        scanf(" %c", &opcion);
    }

    //Selección de jugadores ------------------------------------------------------------------------------

    switch (opcion) {
        case '0':
            //Se imprimen los jugadores para poder comprobar los disponibles
            printf("Listado de jugadores disponibles:\n");
            listadoJugadores(*Arbol);
            while (nInsertados < nJugadores) {      //Se para al alcanzar el número introducido anteriormente
                printf("\nIntroduzca el nombre de un jugador de la lista u otro nuevo: ");
                scanf("%s", jugador.nombreJugador);

                if (!es_miembro(*Arbol, jugador) && jugador.nombreJugador[0] == '@') {
                    //El jugador no se encuentra en la lista, pero el nombre es válido
                    printf("El jugador no ha sido registrado. Introduza 1 si quiere añadirlo: ");
                    scanf(" %c", &opcion);

                    switch (opcion) {
                        case '1':
                            _inicializarJugador(&jugador);
                            insertar(Arbol, jugador);       //El jugador se inserta en el árbol de usuarios
                            break;
                        default:        //Si no se introduce un 1, se interpreta como una respuesta negativa
                            printf("El jugador no ha sido insertado\n");
                            continue;       //Se reinicia el bucle sin aumentar nInsertados
                    }
                } else if (!es_miembro(*Arbol, jugador) && jugador.nombreJugador[0] != '@') {
                    printf("Jugador no válido\n");
                    continue;               //Se reinicia el bucle sin aumentar nInsertados
                }

                insertar(&jugadores, jugador);      //El jugador se añade a la partida
                nInsertados++;
                printf("Jugador añadido\n");
            }
            printf("\n");
            break;
        case '1':
            while (nInsertados < nJugadores) {      //Se repite hasta alcanzar el número de jugadores deseado

                numElegido = _aleatorio(1, nUsuarios);      //Nodo al azar de Arbol
                contador = 0;               //A _buscarPorIndice se le debe pasar un contador inicializado a 0
                _buscarPorIndice(*Arbol, numElegido, &contador, &jugador);
                //jugador contiene ahora un nodo al azar

                if (!es_miembro(jugadores, jugador)) {  //Se comprueba que no estuviera ya en el árbol de jugadores
                    insertar(&jugadores, jugador);
                    nInsertados++;
                }
            }
            break;

    }

    //En este momento, el árbol creado ya contiene a todos los jugadores reales
    //Se imprimen los nombres de los jugadores
    printf("\nJugadores seleccionados:\n{\n");
    _listaJugadores(jugadores);
    printf("}\n");

    /* En este momento, los nodos del árbol de jugadores y del de usuarios comparten las colas de tareas
     * Llamando a _limpiarDatos() para los usuarios, se asegura que los nodos tienen colas independientes (pues
     * se inicializan, creándose una nueva cola para cada uno de ellos*/
    _limpiarDatos(*Arbol);


    //Asignación de roles y tareas -------------------------------------------------------------------------------

    //Se reutiliza contador
    contador = 0;       //Necesario para entrar en el while
    nImpostores = round(nJugadores / 5.0);
    while (contador < nImpostores) {      // Controla que el número de Impostores no sea menor al deseado
        contador = 0;
        _asignarRolesAleat(jugadores, *Arbol, &contador, nImpostores);      //Se asignan los roles al azar
        //Se pasan los dos árboles para mantener la información de los nodos de forma independiente
    }

    _asignarTareas(jugadores, *Arbol);      //A continuación, se reparten tareas
    //De nuevo, se pasan los dos árboles. Aunque en las colas se va a registrar la misma información, son distintas

    _irACafeteria(jugadores);       //La habitación actual de todos los jugadores pasa a ser cafetería

    printf("\nNúmero de impostores: %d\n", nImpostores);

    printf("\nEstado final de los jugadores:\n");
    _estadoJugadores(jugadores);



    //Ejecución de la partida -------------------------------------------------------------------------------------

    exist = 1;
    printf("\n\n\033[0;35m------------------- Comienza el juego -------------------------\033[0m\n\n");
    while(exist) {
        //Se avisa del número de impostores que quedan en la partida
        if (nImpostores == 1){
            printf("Queda \033[0;31m1 impostor\033[0m entre los jugadores\n");
        } else {
            printf("Quedan \033[0;31m%d impostores\033[0m entre los jugadores\n", nImpostores);
        }

        _moverse(jugadores, G);

        contador = 0;
        sabotaje = 0;
        //Se llama a _ejecutarTarea, que posibilita que los impostores maten a un jugador si se dan las condiciones
        _ejecutarTarea(jugadores, *Arbol, jugadores, &contador, nImpostores, &sabotaje);
        /* Es necesario pasar Arbol por los sabotajes de los impostores, de forma que se actualicen las tareas.
         * Si ocurre un sabotaje, _ejecutarTarea devolverá sabotaje como 1.
         * Se envían dos copias de jugadores. Una será la que se recorrerá, y otra permite hacer comprobaciones sobre
         * el árbol completo (pues la primera no serviría, al irse dividiendo en subárboles) */

        //Control de los sabotajes
        if (sabotaje == 1){
            printf("\033[0;31m¡Ha ocurrido un sabotaje!\033[0m\n");
            controlSabotajes++;         //1 sabotaje más en cadena
        } else if (sabotaje == 2){
            /* Esta condición ha pasado de un else a un else if, pues solo se tienen en cuenta los turnos en los cuales
             * los impostores no están en tránsito, en cuyo caso sabotaje valdría -1 */
            controlSabotajes = 0;       //Se reinicia (no hubo sabotajes "seguidos")
        }

        //Si hay 4 sabotajes seguidos, no se da la opción de adivinar: se pierde automáticamente
        if (controlSabotajes == 4){
            printf("Los jugadores no han conseguido gestionar la emergencia\n");
            victoria = 'I';
            break;
        }

        _estadoJugadores(jugadores);
        //Se imprime la habitación en la que se encuentra cada jugador o un aviso si ha muerto/ha sido expulsado

        _adivinarJugador(jugadores, &nImpostores);      //El jugador tiene la oportunidad de adivinar el impostor

        contador = 0;
        _contarJugadoresRol(jugadores, 'T', &contador);        //Se cuenta el número de tripulantes
        if (nImpostores == 0){
            printf("Los impostores han sido eliminados\n");
            victoria = 'T';
            break;
        } else if (contador <= nImpostores){
            printf("Los impostores superan o igualan en número a los tripulantes\n");
            victoria = 'I';
            break;
        }

        //Se suprime la tarea actual si se ha realizado y se destruye la cola de los jugadores muertos y eliminados
        _siguienteTarea(jugadores);

        //Se comprueba si los tripulantes han llevado a cabo todas las tareas
        exist = 0;
        _comprobarTareas(jugadores, &exist);
    }

    switch (victoria){
        case 'I':
            printf("\033[0;31mVictoria de los impostores\033[0m\n");
            break;
        case 'T':       //Se cumplen las condiciones de victoria de los tripulantes
            printf("\033[0;32m¡Victoria de los tripulantes!\033[0m\n\n");
            break;
        default:
            //Se ha llegado al final del bucle sin que se cumpla ninguna condición (se han ejecutado todas las tareas)
            //Ganan los tripulantes
            printf("Los tripulantes han logrado terminar sus tareas\n");
            printf("\033[0;32m¡Victoria de los tripulantes!\033[0m\n\n");
    }

    destruir(&jugadores);           //Es necesario liberar la memoria reservada para el árbol
    //No comparten las colas de tareas con las de los usuarios, por lo que los datos no se borran
}

//Función pública que imprime los datos de un usuario dado
void consultarJugador(abb Arbol) {
    tipoelem E;
    tipoelemCola EC;

    printf("Introduzca el usuario: ");
    scanf("%s", E.nombreJugador);

    if (!es_miembro(Arbol, E)){
        printf("El usuario no existe\n");
        return;     // Se devuelve al usuario al menú
    }

    buscar_nodo(Arbol, E.nombreJugador, &E);   //Se busca el nodo del jugador dado y se guardan sus datos en E

    printf("\tNombre de Jugador: \033[0;35m%s\033[0m\n", E.nombreJugador);
    // Si el jugador tiene una tarea asignada (con descripción y lugar), se imprime
    if (E.tareas != NULL && !es_vacia_cola(E.tareas)){
        EC = primero(E.tareas);
        printf("\tTarea a realizar: \033[0;36m|%s|\033[0m en \033[0;36m|%s|\033[0m\n",
                EC.descripcionTarea, EC.lugarTarea);
    } else {      //La cola está vacía
        printf("\tNinguna tarea asignada\n");
    }
}

//Función pública que imprime todos los usuarios que están en una habitación determinada
void consultarPorHabitacion(abb Arbol) {
    int opcion;

    do{
        _mostrarHabitaciones();     //Se llama a una función privada para imprimir el menú de selección
        scanf("%d", &opcion);

        //Se busca en la habitación correspondiente a la opción elegida
        switch (opcion){
            case 1:
                printf("\nLista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Armería");
                printf("}\n\n");
                break;
            case 2:
                printf("\nLista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Cafetería");
                printf("}\n\n");
                break;
            case 3:
                printf("\nLista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Comunicaciones");
                printf("}\n\n");
                break;
            case 4:
                printf("\nLista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Electricidad");
                printf("}\n\n");
                break;
            case 5:
                printf("\nLista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Escudos");
                printf("}\n\n");
                break;
            case 6:
                printf("\nLista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Motores");
                printf("}\n\n");
                break;
            case 7:
                printf("\nLista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Navegación");
                printf("}\n\n");
                break;
            case 8:
                printf("\nLista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "O2");
                printf("}\n\n");
                break;
            case 9:
                printf("\nLista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Seguridad");
                printf("}\n\n");
                break;
            case 0:
                break;
            default:
                printf("\nOpción incorrecta\n\n");
        }
    } while (opcion != 0);      //Salir y volver al menú
}