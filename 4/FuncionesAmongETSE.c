#include <stdio.h>
#include <stdlib.h> //necesaria para rand()
#include <string.h> //para comparar cadenas
#include <math.h> //para calcular el número de impostores con round()

#include "abb.h"
#include "cola.h"

#define NTAREAS 5   // Constante. Cada jugador tiene 5 tareas asignadas al empezar la partida.


//////////////////////////////////////////////////////////////////////
///////////////// FUNCIONES PRIVADAS


//Función privada que genera un número aleatorio entre inf y sup
//Usada en las funciones _darRol(), _darTarea() y generarPartida()
unsigned int _aleatorio(int inf, int sup) {
    return (rand() % (sup - inf + 1)) +inf;
}

//Función privada para inicializar los datos de un jugador, necesaria en varias funciones públicas
//Usada en las funciones _limpiarDatos(), leerArchivo(), altaJugador() y generarPartida()
void _inicializarJugador(tipoelem * registro) { //inicializa los campos rol,descripcionTarea y lugarTarea
    registro->rol = ' ';
    crear_cola(&registro->tareas);          //Se crea una nueva cola de tareas
}


//Función privada que imprime las tareas de una cola dada (descripción y lugar)
//Usada en la función _imprimirJugador()
void _imprimirTareas(cola *c){
    cola aux;                   //Cola auxiliar
    tipoelemCola EC;            //Tarea

    crear_cola(&aux);           //Inicialización de la cola auxiliar

    printf("\n\t\t\tTareas asignadas:\n\t\t\t{\n");

    while (!es_vacia_cola(*c)){         //Bucle que vacía la cola pasada en aux
        EC = primero(*c);               //Se lee el elemento del frente de la cola
        printf("\t\t\t\tTarea: |%-30s| en |%-15s|\n", EC.descripcionTarea, EC.lugarTarea);
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
    printf("\tNombre de Jugador: %-15s\n", E.nombreJugador);
    //Si el jugador tiene un rol asignado, se imprime
    if (E.rol != ' '){
        printf("\t\t\tRol: %c", E.rol);
    }
    //Si el jugador tiene una tarea asignada (con descripción y lugar), se imprime
    if (E.tareas != NULL && !es_vacia_cola(E.tareas)){
        //Si bien en esta versión del programa nunca se va a dar que E.tareas sea NULL, no está de más comprobarlo
        //Si E.tareas fuera NULL, no se podría llamar a es_vacia_cola()

        _imprimirTareas(&E.tareas);
    } else {      //No hay tareas
        printf("\tNo jugó a AmongETSE\n\n");
    }
}

//Función privada que imprime los nombres, roles y ubicaciones de los jugadores
//Usada en generarPartida() para mostar el resultado de la creación completa del árbol de jugadores
void _estadoJugadores(abb A){
    tipoelem E;
    tipoelemCola EC;

    if (!es_vacio(A)){
        leer(A, &E);
        printf("%-15s", E.nombreJugador);       //Nombre alineado a la izquierda en un campo de 15 caracteres
        if (E.rol != ' '){                       //Si existe rol, se imprime
            printf(" Rol: %c", E.rol);
        }
        if (E.tareas != NULL && !es_vacia_cola(E.tareas)){
            //Si bien en esta versión del programa nunca se va a dar que E.tareas sea NULL, no está de más comprobarlo
            //Si E.tareas fuera NULL, no se podría llamar a es_vacia_cola()

            EC = primero(E.tareas);
            printf(" situado en: |%-15s|", EC.lugarTarea);
            //Lugar alineado a la izquierda en un campo de 15 caracteres (se rellena con espacios si es necesario)
        }
        printf("\n");
        _estadoJugadores(izq(A));
        _estadoJugadores(der(A));
    }
}

//Función privada que borra los datos guardados de los jugadores de un árbol (rol, descripcionTarea y lugarTarea)
//Se utiliza en generarPartida()
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
//Utilizada en generarPartida()
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
//Se emplea en generarPartida()
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

//Función privada que asigna una tarea, es decir, una descripción de la misma y un lugar, a un tipoelemCola
//Empleada en _asignarTareas()
void _darTarea(tipoelemCola *EC){
    //Modificación: ahora la función recibe un tipoelemCola, que representa una tarea

    switch (_aleatorio(1,8)){       //Se genera un entero aleatorio entre 1 y 8, ambos incluidos
        //Dependiendo del resultado, se copia una determinada descripción y su lugar asociado
        case 1:
            strncpy(EC->descripcionTarea, "Alinear la salida del motor", 28);
            //Aunque la cadena solo tiene 27 caracteres, se copian 28 para que termine en '\0'
            strncpy(EC->lugarTarea, "Motores", 8);
            break;
        case 2:
            strncpy(EC->descripcionTarea, "Calibrar distribuidor", 22);
            //Cuando hay tildes, se debe añadir un carácter extra para cada una (ocupan 2 caracteres)
            strncpy(EC->lugarTarea, "Electricidad", 13);
            break;
        case 3:
            strncpy(EC->descripcionTarea, "Descargar datos/subir datos", 28);
            switch(_aleatorio(1, 5)){   // Varios lugares posibles
                case 1:
                    strncpy(EC->lugarTarea, "Cafetería", 11);
                    break;
                case 2:
                    strncpy(EC->lugarTarea, "Comunicaciones", 15);
                    break;
                case 3:
                    strncpy(EC->lugarTarea, "Armería", 9);
                    break;
                case 4:
                    strncpy(EC->lugarTarea, "Electricidad", 13);
                    break;
                case 5:
                    strncpy(EC->lugarTarea, "Navegación", 12);
            }
            break;
        case 4:
            strncpy(EC->descripcionTarea, "Desviar energía", 17);
            switch(_aleatorio(1, 7)){
                case 1:
                    strncpy(EC->lugarTarea, "Navegación", 12);
                    break;
                case 2:
                    strncpy(EC->lugarTarea, "O2", 3);
                    break;
                case 3:
                    strncpy(EC->lugarTarea, "Seguridad", 10);
                    break;
                case 4:
                    strncpy(EC->lugarTarea, "Armería", 9);
                    break;
                case 5:
                    strncpy(EC->lugarTarea, "Comunicaciones", 15);
                    break;
                case 6:
                    strncpy(EC->lugarTarea, "Escudos", 8);
                    break;
                case 7:
                    strncpy(EC->lugarTarea, "Motores", 8);
            }
            break;
        case 5:
            strncpy(EC->descripcionTarea, "Encender escudos", 17);
            strncpy(EC->lugarTarea, "Escudos", 8);
            break;
        case 6:
            strncpy(EC->descripcionTarea, "Estabilizar dirección", 23);
            strncpy(EC->lugarTarea, "Navegación", 12);
            break;
        case 7:
            strncpy(EC->descripcionTarea, "Limpiar el filtro O2", 21);
            strncpy(EC->lugarTarea, "O2", 3);
            break;
        case 8:
            strncpy(EC->descripcionTarea, "Mapa de navegación", 20);
            strncpy(EC->lugarTarea, "Navegación", 12);
    }
}

/* Función privada que genera NTAREAS = 5 tareas aleatorias y las inserta en la cola de un jugador. No obstante, el
 * jugador está "duplicado" (se encuentra tanto en el árbol de jugadores como en el de usuarios), por lo que se debe
 * pasar la cola correspondiente a cada uno de los dos árboles, registrando la misma información para ambos */
//Se utiliza en _asignarTareas()
void _insertarTareaColaTareas(cola *c, cola *d){
    tipoelemCola EC;
    int i;

    for (i = 0; i < NTAREAS; i++){
        _darTarea(&EC);                 //EC recibe una tarea aleatoria (descripción y lugar)
        insertar_cola(c, EC);           //El tipoelemCola se añade a la cola del nodo del árbol de jugadores
        insertar_cola(d, EC);           //El tipoelemCola se añade a la cola del nodo del árbol de usuarios
    }
}


//Función privada que asigna tareas a los jugadores de un árbol
//Se utiliza en generarPartida()
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
//Se usa en generarPartida()
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
            //Si bien en esta versión del programa nunca se va a dar que E.tareas sea NULL, no está de más comprobarlo
            //Si E.tareas fuera NULL, no se podría llamar a es_vacia_cola()
            //Tampoco se puede llamar a primero() si la cola estuviese vacía (daría error)

            EC = primero(E.tareas);                  //Elemento del frente de la cola (tarea actual)
            if (strcmp(EC.lugarTarea, hab) == 0){    //Si strcmp devuelve 0, las cadenas coinciden
                printf("\t%-15s: Tarea |%-30s| en |%-15s|\n", E.nombreJugador, EC.descripcionTarea, EC.lugarTarea);
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
//Se utiliza en generarPartida() y leerArchivo()
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

///////////////////////////////////////////////////////////////////////////////////////
///////////// FUNCIONES PÚBLICAS

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
        printf("Error en la lectura del archivo\n");
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

    //El nombre es válido. El rol y la cola de tareas se inicializan y el elemento se inserta
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

//Función pública que genera los datos de una partida: jugadores, roles y tareas
void generarPartida(abb *Arbol) {
    /* Arbol se pasa por referencia para dar la opción al usuario de insertar un nuevo elemento cuando selecciona
     * los jugadores manualmente y este no se encuentra entre los disponibles*/

    abb jugadores;                                      //Árbol de los jugadores de la partida
    tipoelem jugador;                                   //Variable temporal para un jugador
    int nJugadores, nImpostores, nUsuarios;             //Valores de referencia
    int numElegido, nInsertados = 0, contador;          //Contadores y variables dependientes del usuario
    char opcion;                                        //Selección

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
     * se inicializan, creándose una nueva cola para cada uno de ellos)*/
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

    printf("\nNúmero de impostores: %d\n", nImpostores);

    printf("\nEstado final de los jugadores:\n");
    _estadoJugadores(jugadores);

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


    printf("\tNombre de Jugador: %-15s\n", E.nombreJugador);
    // Si el jugador tiene una tarea asignada (con descripción y lugar), se imprime
    if (E.tareas != NULL && !es_vacia_cola(E.tareas)){
        EC = primero(E.tareas);
        printf("\tTarea a realizar: |%-30s| en |%-15s|\n", EC.descripcionTarea, EC.lugarTarea);
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
    return;
}