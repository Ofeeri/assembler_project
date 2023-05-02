#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "header_data.h"



void assemble(char* filename){
    initMacroTable();
    initSymbolTable();
    initEntriesArray();

    if (preProcessor(filename) != 0){
        /*Only calls these if pre processor was successful*/
        firstPass(filename);

        secondPass(filename);
    }


    freeMacroTable();
    freeSymbolTable();
    freeEntriesArray();
}


int main(int argc, char** argv){
    int i;
    for (i=1; i<argc; i++){
        assemble(argv[i]);
    }

    
    return 1;
}