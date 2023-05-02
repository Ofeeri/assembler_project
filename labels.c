#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "headers/constants.h"
#include "headers/operations.h"
#include "headers/memory.h"
#include "headers/errors.h"
#include "headers/stringUtils.h"


/*Description: this file contains all functions and data types that have to do with checking, storing, and getting labels 
  throughout the assembly process.*/


static int symbolTableSize; /*holds the max size of the symbol table (dynamically allocated)*/
static int labelCount; /*current number of labels in symbol table*/
typedef enum {CODETAG, DATATAG} Label_Tag; /*type of label*/
typedef enum {ABSOLUTE, EXTERNAL, RELOCATABLE} Encoding_Type; /*encoding type of labels*/


typedef struct Label{
    char* name; /*label name*/
    int value; /*current IC or DC value (label's address)*/
    Label_Tag tag; /*code or data*/
    Encoding_Type type; /*relocatable or external*/
} Label;


static Label** symbolTable; /*Will hold all the labels that are found during the first pass*/


/*Initializes symbol table.*/
void initSymbolTable(){
    symbolTableSize = INITIAL_TABLE_SIZE;
    labelCount = 0;
    symbolTable = malloc(symbolTableSize * sizeof(Label));
}


/*Receives name, value, tag, and type creates a new instance of a Label struct and stores
 it in the symbol table. If the label count is higher than the symbol table size, 
 the symbol table is enlarged dynamically.*/
void enterSymbol(char* name, int value, Label_Tag tag, Encoding_Type type){
    Label* currentSymbol;
    if (labelCount >= symbolTableSize){
        symbolTableSize += INITIAL_TABLE_SIZE;
        symbolTable = realloc(symbolTable, symbolTableSize * sizeof(Label));
    }

    currentSymbol = malloc(sizeof(Label));
    currentSymbol->name = malloc(MAX_LABEL_LENGTH * sizeof(char));

    strcpy(currentSymbol->name, name);
    currentSymbol->value = value;
    currentSymbol->tag = tag;
    currentSymbol->type = type;

    symbolTable[labelCount] = currentSymbol;
    labelCount++;
}


/*Receives a string and, if it is the name of an existing label, returns the Label struct.
  Otherwise returns NULL.*/
Label* getSymbol(char* name){
    int i;
    Label* currentSymbol;
    for (i=0; i < labelCount; i++){
        currentSymbol = symbolTable[i];
        if (strcmp(currentSymbol->name, name) == 0){
            return currentSymbol;
        }
    }
    return NULL; /*symbol not found*/
}

/*Receives a value (address) of a label and returns the label with this value. 
  If none is found, returns NULL.*/
Label* getSymbolByValue(int value){
    int i;
    Label* currentSymbol;
    for (i=0; i < labelCount; i++){
        currentSymbol = symbolTable[i];
        if (currentSymbol->value == value){
            return currentSymbol;
        }
    }
    return NULL; /*symbol not found*/
}


/*Receives string and, if it is the name of an existing label, returns 1. Otherwise, 
  returns 0 and raises error.*/
int checkIfLabelExists(char* name){
    int i;
    Label* currentSymbol;
    for (i=0; i < labelCount; i++){
        currentSymbol = symbolTable[i];
        if (strcmp(currentSymbol->name, name) == 0){
            raiseLabelAlreadyExists(name);
            return 0;
        }
    }
    return 1;
}


/*Frees symbol table from memory*/
void freeSymbolTable(){
    int i;
    Label* currentSymbol;
    for (i=0; i < labelCount; i++){
        currentSymbol = symbolTable[i];
        free(currentSymbol->name);
        free(currentSymbol);
    }
     free(symbolTable);   
}


/*Adds value of IC to the value of all data labels.*/
void addICToDataValues(){
    int i;
    Label* currentSymbol;
    for (i=0; i < labelCount; i++){
        currentSymbol = symbolTable[i];
        if (currentSymbol->tag == DATATAG)
            currentSymbol->value += getIC();
    }
}


/*Checks if label name is a register or operation name. If it is, raises appropriate error and returns
 0. Otherwise returns 1, meaning it is valid.*/
int isValidLabelName(char* labelName){
    int i;

    if (isOperationName(labelName)){
        raiseLabelIsOpName(labelName);
        return 0;
    }
    if (isRegisterName(labelName)){
        raiseLabelIsRegisterName(labelName);
        return 0;
    }

    if (!isalpha(labelName[0])){ /*First char of label must be alphabetical*/
        return 0;
    }

    i = 1;
    while (i < strlen(labelName)){
        if (!isalnum(labelName[i])){ /*Label name can only be made up of alphanumeric chars*/
            return 0;
        }
        i++;
    }
    
    return 1;
}


/*Identical to isValidLabelName but does not raise errors. This is used when checking labels 
  that are operands and not being declared.*/
int isValidLabelNameNoError(char* labelName){
    int i;

    if (isOperationName(labelName) || isRegisterName(labelName) || !isalpha(labelName[0])){
        return 0;
    }

    i = 1;
    while (i < strlen(labelName)){
        if (!isalnum(labelName[i])){ /*Label name can only be made up of alphanumeric chars*/
            return 0;
        }
        i++;
    }

    return 1;
}


/*Receives a statement from the source file and finds the label name declared in the statement
  by iterating through the beginning of a statement (label name should be at beginning) until reaching
  ':' if one exists and returns the label name. If label syntax is invalid, returns NULL.*/
char* getLabelName(char* statement){
    int i = 0;
    char* labelName = malloc(MAX_LABEL_LENGTH + 1);
    if (isalpha(statement[0])){ /*First char of label must be alphabetical*/

        while (statement[i] != ':'){
            if (isalnum(statement[i])) /*Label name can only be made up of alphanumeric chars*/
                labelName[i] = statement[i];
            else return NULL;

            i++;
        }
        labelName[i] = '\0'; /*Terminate label name*/
    }
    else return NULL;

    return labelName;
}


/*Receives a statement from the source code and checks if it could be a label declaration by looking for ':' 
  in the statement. Returns 1 if is a possible label declaration, 0 otherwise.*/
int isPossibleLabelDeclaration(char* statement){
    if (strchr(statement, ':') != NULL){
        /*Found possible label declaration*/
        return 1;
    }    
    return 0;
}


int handleLabelDeclaration(char* statement, Label_Tag tag){
	char* labelName = getLabelName(statement);
	if (labelName == NULL){
		/*this means that the label name either has a space or invalid characters in it*/
		raiseInvalidLabelSyntax(labelName);
		return 0;
	}
	if (isValidLabelName(labelName) && (checkIfLabelExists(labelName))){
		/*label name is valid and does not already exist*/
        if (tag == DATATAG){
            /*Value is DC for data labels*/
		    enterSymbol(labelName, getDC(), tag, RELOCATABLE);
            free(labelName);
        }
        if (tag == CODETAG){
            /*Value is IC for data labels*/
            enterSymbol(labelName, getIC(), tag, RELOCATABLE);
            free(labelName);
        }
    }
	else{
        free(labelName);
        return 0;
    }
	return 1;
}
