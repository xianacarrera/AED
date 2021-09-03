#include <stdio.h>
#include <stdlib.h> //necesaria para rand()
#include <string.h> //para comparar cadenas
#include <math.h> //para calcular el número de impostores con round()

#include "abb.h"

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
    registro->descripcionTarea[0] = '\0';       //Se coloca el final de la cadena en el primer elemento
    registro->lugarTarea[0] = '\0';             //Se coloca el final de la cadena en el primer elemento
}

//Función privada que imprime los datos de un jugador
//Usada en la función consultarJugador()
void _imprimirJugador(tipoelem E) {
    printf("\tNombre de Jugador: %s\n", E.nombreJugador);
    // Si el jugador tiene una tarea asignada (con descripción y lugar), se imprime
    if (E.descripcionTarea[0] != '\0' && E.lugarTarea[0] != '\0'){
        printf("\tÚltima tarea realizada: |%s| en |%s|\n", E.descripcionTarea, E.lugarTarea);
    } else {      //Al menos uno de los campos no tiene información. Se interpreta que no hay tarea
        printf("\tNinguna tarea asignada\n");
    }
}

//Función privada que imprime los nombres, roles y ubicaciones de los jugadores
//Usada en generarPartida() para mostar el resultado de la creación completa del árbol de jugadores
void _estadoJugadores(abb A){
    tipoelem E;
    if (!es_vacio(A)){
        leer(A, &E);
        printf("%-15s", E.nombreJugador);       //Nombre alineado a la izquierda en un campo de 15 caracteres
        if (E.rol != ' '){                       //Si existe rol, se imprime
            printf(" Rol: %c", E.rol);
        }
        if (E.lugarTarea[0] != '\0'){           //Si existe lugar de la tarea, se imprime
            printf(" situado en: |%-15s|", E.lugarTarea);
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
    if (!es_vacio(A)){
        leer(A, &E);                //Se guarda la información del nodo en E
        _inicializarJugador(&E);    //Los campos rol, descripcionTarea y lugarTarea se "vacían"
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

//Función privada que asigna roles a los jugadores de un árbol
//Se emplea en generarPartida()
void _asignarRolesAleat(abb A, int *contador, int nImpostores){
    tipoelem E;
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
        modificar(A, E);    //Se guardan los cambios en el árbol. Se modifica el nodo con el mismo nombreJugador que E

        _asignarRolesAleat(izq(A), contador, nImpostores);
        _asignarRolesAleat(der(A), contador, nImpostores);
    }
}

//Función privada que asigna una tarea, es decir, una descripción de la misma y un lugar, a un determinado jugador
//Empleada en _asignarTareas()
void _darTarea(tipoelem *E){
    switch (_aleatorio(1,8)){       //Se genera un entero aleatorio entre 1 y 8, ambos incluidos
        //Dependiendo del resultado, se copia una determinada descripción y su lugar asociado
        case 1:
            strncpy(E->descripcionTarea, "Alinear la salida del motor", 28);
            //Aunque la cadena solo tiene 27 caracteres, se copian 28 para que termine en '\0'
            strncpy(E->lugarTarea, "Motores", 8);
            break;
        case 2:
            strncpy(E->descripcionTarea, "Calibrar distribuidor", 22);
            //Cuando hay tildes, se debe añadir un carácter extra para cada una (ocupan 2 caracteres)
            strncpy(E->lugarTarea, "Electricidad", 13);
            break;
        case 3:
            strncpy(E->descripcionTarea, "Descargar datos/subir datos", 28);
            switch(_aleatorio(1, 5)){   // Varios lugares posibles
                case 1:
                    strncpy(E->lugarTarea, "Cafetería", 11);
                    break;
                case 2:
                    strncpy(E->lugarTarea, "Comunicaciones", 15);
                    break;
                case 3:
                    strncpy(E->lugarTarea, "Armería", 9);
                    break;
                case 4:
                    strncpy(E->lugarTarea, "Electricidad", 13);
                    break;
                case 5:
                    strncpy(E->lugarTarea, "Navegación", 12);
            }
            break;
        case 4:
            strncpy(E->descripcionTarea, "Desviar energía", 17);
            switch(_aleatorio(1, 7)){
                case 1:
                    strncpy(E->lugarTarea, "Navegación", 12);
                    break;
                case 2:
                    strncpy(E->lugarTarea, "O2", 3);
                    break;
                case 3:
                    strncpy(E->lugarTarea, "Seguridad", 10);
                    break;
                case 4:
                    strncpy(E->lugarTarea, "Armería", 9);
                    break;
                case 5:
                    strncpy(E->lugarTarea, "Comunicaciones", 15);
                    break;
                case 6:
                    strncpy(E->lugarTarea, "Escudos", 8);
                    break;
                case 7:
                    strncpy(E->lugarTarea, "Motores", 8);
            }
            break;
        case 5:
            strncpy(E->descripcionTarea, "Encender escudos", 17);
            strncpy(E->lugarTarea, "Escudos", 8);
            break;
        case 6:
            strncpy(E->descripcionTarea, "Estabilizar dirección", 23);
            strncpy(E->lugarTarea, "Navegación", 12);
            break;
        case 7:
            strncpy(E->descripcionTarea, "Limpiar el filtro O2", 21);
            strncpy(E->lugarTarea, "O2", 3);
            break;
        case 8:
            strncpy(E->descripcionTarea, "Mapa de navegación", 20);
            strncpy(E->lugarTarea, "Navegación", 12);
    }
}

//Función privada que asigna tareas a los jugadores de un árbol
//Se utiliza en generarPartida()
void _asignarTareas(abb A){
    tipoelem E;
    if (!es_vacio(A)){
        leer(A, &E);            //Se almacenan los datos del nodo en el tipoelem E
        _darTarea(&E);          //Se copia en E una tarea aleatoria (con descripción y lugar)
        modificar(A, E);        //Se guardan los cambios en el nodo del árbol
        _asignarTareas(izq(A));
        _asignarTareas(der(A));
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
    if (!es_vacio(Arbol)){
        leer(Arbol, &E);        //Se guardan los datos del nodo en el tipoelem E
        if (strcmp(E.lugarTarea, hab) == 0){    //Si strcmp devuelve 0, las cadenas coinciden
            printf("%s: Tarea |%s| en |%s|\n", E.nombreJugador, E.descripcionTarea, E.lugarTarea);
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
    printf("\n\t0. Volver al menú");
    printf("}\nSeleccione habitación: ");
}

//Función privada que actualiza los elementos de un árbol con los datos de otro
//Necesaria para generarPartida()
void _copiarCambios(abb destino, abb origen){
    //Se lee cada uno de los nodos del origen y se copian sus datos en el nodo de destino que comparte clave con él
    tipoelem E;
    if (!es_vacio(origen)){
        leer(origen, &E);           //Se toman los datos de un nodo de origen
        modificar(destino, E);      //Se modifica el nodo de destino con el mismo nombreJugador
        //Se siguen copiando nodos hasta recorrer el árbol por completo
        _copiarCambios(destino, izq(origen));
        _copiarCambios(destino, der(origen));
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
        printf("\t%s", E.nombreJugador);    //Se imprime el nombre del jugador
        if (E.rol != ' '){                  //Si tiene un rol asignado, se imprime
            printf("\t\t\tRol: %c", E.rol);
        }
        if (E.descripcionTarea[0] != '\0'){     //Si tiene una tarea guardada, se imprime
            printf("\tTarea: |%-15s|", E.descripcionTarea);
            //Tarea alineada a la izquierda en un espacio de 15 caracteres
        }
        if (E.lugarTarea[0] != '\0'){               //Habitación en la que se encuentra
            printf(" en |%-15s|", E.lugarTarea);    //Lugar alineado a la izquierda en un espacio de 15 caracteres
        }
        printf("\n");
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
        listadoJugadores(*A);
        printf("}\n");
    } else {
        printf("Error en la lectura del archivo\n");
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
    listadoJugadores(jugadores);
    printf("}\n");


    //Asignación de roles y tareas -------------------------------------------------------------------------------

    //Se reutiliza contador
    contador = 0;       //Necesario para entrar en el while
    nImpostores = round(nJugadores / 5.0);
    while (contador < nImpostores){      // Controla que el número de Impostores no sea menor al deseado
        contador = 0;
        _asignarRolesAleat(jugadores, &contador, nImpostores);      //Se asignan los roles al azar
    }

    _asignarTareas(jugadores);      //A continuación, se reparten tareas
    //No es necesario llamar a _limpiarDatos antes ya que las tareas se reparten una vez los roles son definitivos

    printf("\nNúmero de impostores: %d\n", nImpostores);

    printf("\nEstado final de los jugadores:\n");
    _estadoJugadores(jugadores);

    _copiarCambios(*Arbol, jugadores);      //El árbol global es actualizado con los nuevos roles y tareas

    destruir(&jugadores);           //Es necesario liberar la memoria reservada para el árbol
}


//Función pública que imprime los datos de un usuario dado
void consultarJugador(abb Arbol) {
    tipoelem E;

    printf("Introduzca el usuario: ");
    scanf("%s", E.nombreJugador);

    if (!es_miembro(Arbol, E)){
        printf("El usuario no existe\n");
        return;     // Se devuelve al usuario al menú
    }

    buscar_nodo(Arbol, E.nombreJugador, &E);   //Se busca el nodo del jugador dado y se guardan sus datos en E
    _imprimirJugador(E);                       //Se imprime su información
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
                printf("Lista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Armería");
                printf("}\n");
                break;
            case 2:
                printf("Lista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Cafetería");
                printf("}\n");
                break;
            case 3:
                printf("Lista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Comunicaciones");
                printf("}\n");
                break;
            case 4:
                printf("Lista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Electricidad");
                printf("}\n");
                break;
            case 5:
                printf("Lista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Escudos");
                printf("}\n");
                break;
            case 6:
                printf("Lista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Motores");
                printf("}\n");
                break;
            case 7:
                printf("Lista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Navegación");
                printf("}\n");
                break;
            case 8:
                printf("Lista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "O2");
                printf("}\n");
                break;
            case 9:
                printf("Lista de jugadores:\n{\n");
                _buscarPorHabitacion(Arbol, "Seguridad");
                printf("}\n");
                break;
            default:
                printf("Opción incorrecta\n");
        }
    } while (opcion != 0);      //Salir y volver al menú
    return;
}