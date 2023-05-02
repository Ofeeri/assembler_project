#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "headers\\all_headers.h"


static int macroTableSize;
static int macroCount;


typedef struct Macro{
    char* name;
    char* contents;
} Macro;

static Macro** macroTable; /*Will hold the name and contents of each macro that is found in pre processor stage*/


/*Initializes macroTable.*/
void initMacroTable(){
    macroTableSize = INITIAL_TABLE_SIZE;
    macroCount = 0;
    macroTable = malloc(macroTableSize * sizeof(Macro));
}



/*Receives a macro name and its contents and enters them as a Macro struct into the next available space in the macroTable.*/
void enterMacro(char* name, char* contents){
    Macro* currentMacro;
    if (macroCount >= macroTableSize){
        macroTableSize += INITIAL_TABLE_SIZE;
        macroTable = realloc(macroTable, macroTableSize * sizeof(Macro));
    }

    currentMacro = malloc(sizeof(Macro));
    currentMacro->name = malloc(strlen(name) * sizeof(char) + 1);
    currentMacro->contents = malloc(strlen(contents) * sizeof(char) + 1);

    strcpy(currentMacro->name, name);
    strcpy(currentMacro->contents, contents);

    macroTable[macroCount] = currentMacro;
    macroCount++;
}


/*Receives a string and, if it is the name of an existing macro, returns its contents.*/
char* getMacroContents(char* name){
    int i;
    Macro* currentMacro;
    for (i=0; i < macroCount; i++){
        currentMacro = macroTable[i];
        if (strcmp(currentMacro->name, name) == 0)
            return currentMacro->contents;
    }
    return NULL;
}

/*Frees macro table from memory*/
void freeMacroTable(){
    int i;
    Macro* currentMacro;
    for (i=0; i < macroCount; i++){
        currentMacro = macroTable[i];
        free(currentMacro->name);
        free(currentMacro->contents);
        free(currentMacro);
    }
    free(macroTable);
}


/*Receives string which is supposed to be a macro name. Returns 1 if the name is valid, 0 otherwise.*/
int isValidMacroName(char* str){
    if (isOperationName(str)){
        raiseInvalidMacroName(str, 1);
        return 0;
    }
    if (isRegisterName(str)){
        raiseInvalidMacroName(str, 0);
        return 0;
    }
    return 1;
}
