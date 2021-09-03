#include <stdio.h>
#include <stdlib.h>

#include "abin.h"

//Recorrido recursivo inorden: I-R-D
void inorden(abin A){
	tipoelem E;
	if(!esVacio(A)){
	  inorden(izq(A));
	  leer(A,&E); printf("%c ",E);
	  inorden(der(A));
	}	
}

//Recorrido recursivo preorden: R-I-D
void preorden(abin A){
	tipoelem E;
	if(!esVacio(A)){
	  leer(A,&E); printf("%c ",E);
	  preorden(izq(A));	
	  preorden(der(A));
	}	
}

//Recorrido recursivo postorden: I-D-R
void postorden(abin A){
	tipoelem E;
	if(!esVacio(A)){
	  postorden(izq(A));	
	  postorden(der(A));
	  leer(A,&E); printf("%c ",E);
	}	
}
