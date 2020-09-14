#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
    FILE *f;
    char comentario[200], tag[100], nombre_fichero[] = "fichero.txt";
    f = fopen(nombre_fichero, "r");
    if(f == NULL){
        return EXIT_FAILURE;
    }
    fscanf(f, " %s #%s", comentario, tag);
    while(!feof(f)){
        printf("Comentario: %s con hashtag %s\n", comentario, tag);
        fscanf(f, " %s %s", comentario, tag);
    }
    fclose(f);

    return EXIT_SUCCESS;
}
