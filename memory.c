#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "headers/constants.h"
#include "headers/stringUtils.h"
#include "headers/utils.h"
#include "headers/labels.h"
#include "headers/errors.h"


/*Description: this file contains all functions and datatypes that have to do with storing information from the
  source code into memory. Including the instruction array, data array and entries array. Furthermore, 
  the function that write the memory to the output files (objects, externs, entries are also found here).*/


static char instructionArray[MEMORY_SIZE][MAX_LABEL_LENGTH]; /*holds binary encoding of machine commands in source file*/
static char dataArray[MEMORY_SIZE][wordSize+1]; /*holds binary encoding of data in source file (.data/.string)*/
static int IC; /*Instruction counter-points to next available index in instructionArray*/
static int DC; /*Data counter points to next available index in dataArray*/

static const char registerNames[NUMBER_OF_REGISTERS][4] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

static char** entriesArray; /*Will hold addresses of all entry labels declared in the source code*/
static int entriesArraySize; /*current size of entriesArray*/
static int entryCount; /*Current number of entries in entriesArray*/


/*Initialize entries array*/
void initEntriesArray(){
    entriesArraySize = INITIAL_TABLE_SIZE;
    entryCount = 0;
    entriesArray = malloc(entriesArraySize * sizeof(char*));
}


/*Receives declaration address of entry label and writes it to the entriesArray. If 
  there are more entries than the size of the entries array, entries array is enlarged 
  dynamically.*/
void enterEntry(char* name){
    char* entryName;
    if (entryCount >= entriesArraySize){
        entriesArraySize += INITIAL_TABLE_SIZE;
        entriesArray = realloc(entriesArray, entriesArraySize * sizeof(int));
    }
    entryName = malloc(strlen(name) * sizeof(char));
    strcpy(entryName, name);

    entriesArray[entryCount] = entryName;
    entryCount++;
}


/*Free entries array*/
void freeEntriesArray(){
    int i;
    for (i=0; i < entryCount; i++){
        free(entriesArray[i]);
    }
    free(entriesArray);
}


/*Receives a register name and returns the number of the register (0-7)*/
int getRegisterNumber(char* registerName){
    int i;
    for (i=0; i< NUMBER_OF_REGISTERS; i++){
        if (strcmp(registerName, registerNames[i]) == 0)
            return i;
    }
    return -1;
}


/*Receives string and returns 1 if the string is the name of a register in the cpu. Returns 0 if not.*/
int isRegisterName(char* str){
    int i;
    if (strlen(str) > 2)
        /*str is too long to be register name*/
        return 0;
    for (i=0; i < NUMBER_OF_REGISTERS; i++){
        if (strcmp(str, registerNames[i]) == 0)
            return 1;
    }
    return 0;
}


/*Checks if the amount of words in memory is larger than the memory size*/
void checkMemoryOverFlow(){
    if ((IC + DC) > MEMORY_SIZE)
        raiseDataOverFlow();
}

/*Initializes instruction counter.*/
void initIC(){
    IC = MEMORY_START;
}

/*Initializes data counter.*/
void initDC(){
    DC = 0;
}

/*Returns current value of instruction counter.*/
int getIC(){
    return IC;
}

/*Returns current value of data counter.*/
int getDC(){
    return DC;
}

/*Increments data counter by one.*/
void incrementDataCounter(){
    DC++;
    checkMemoryOverFlow();
}

/*Increments instruction counter by one.*/
void incrementInstructionCounter(){
    IC++;
    checkMemoryOverFlow();
}


/*Inserts a word into the instruction array at the current instruction counter index.*/
void writeToInstructionArray(char* binaryWord){
    binaryWord[wordSize] = '\0';
    strcpy(instructionArray[IC], binaryWord);
}


/*Inserts a word into the instruction array at the current data counter index.*/
void writeToDataArray(char* binaryWord){
    binaryWord[wordSize] = '\0';
    strcpy(dataArray[DC], binaryWord);
}


/*Iterates through the instruction array and replaces the name of each label found with its binary encoding in the 
  instruction array. If the label has not been declared, an error is raised.*/
void encodeLabelsSecondPass(){
    int i;
	Label* label;
    Encoding_Type encodingType;
    char* bin = malloc((sizeof(char) * wordSize) + 1); /*will hold binary representation of the word*/

	for (i=MEMORY_START; i <= getIC(); i++){
		label = getSymbol(instructionArray[i]);
		if (label != NULL){
            encodingType = label->type;

            encodeLabelAddress(bin, label->value, (int)encodingType); /*encode label*/
			strcpy(instructionArray[i], bin); /*write to instruction array*/
            bin[0] = '\0'; /*reset bin*/
		}
        /*check if a label name is in the instruction array that has not been encoded*/
        if (isValidLabelName(instructionArray[i])){
            /*this means there is a label referenced in the input that has not been declared*/
            raiseUndeclaredLabelReference(instructionArray[i]);
            
        }   
	}
    free(bin);
}


/*Receives the name of a source file. Creates an objects file and writes the contents
  of memory to the objects fils in the correct format.*/
int writeMemoryToObjectsFile(char* filename){
    FILE* objectsFile;
    int i;
    char currentLine[wordSize * sizeof(int)];
    char address[sizeof(int) * ADDRESS_LENGTH];
    char dataSize[sizeof(int) * DATA_LENGTH]; /*will hold size of instruction array and size of data array to be written in file*/
    char* objectsPath = malloc((strlen(filename) + strlen(OBJECT_FILETYPE)) * sizeof(char));

    sprintf(objectsPath, "%s%s", filename, OBJECT_FILETYPE);
    sprintf(dataSize, "\t\t%d %d\n", (IC - MEMORY_START), getDC());
    objectsFile = fopen(objectsPath, "w");

    if (objectsFile == NULL)
        return 0;
    
    fputs(dataSize, objectsFile);

    /*writing instruction array to file*/
    for (i=MEMORY_START; i < IC; i++){
        sprintf(address, "0%d", i);
        sprintf(currentLine, "%s\t%s\n", address, instructionArray[i]);
        fputs(currentLine, objectsFile);
    }

    /*writing data array to memory*/
    for (i=0; i < DC; i++){
        sprintf(address, "0%d", i + IC);
        sprintf(currentLine, "%s\t%s\n", address, dataArray[i]);
        fputs(currentLine, objectsFile);
    }
    free(objectsPath);
    fclose(objectsFile);
    return 1;
}


/*Receives a filename and writes the names and addresses of all references to external labels 
  into the externals file*/
int writeToExternsFile(char* filename){
    FILE* externsFile;
    Label* currentLabel;
    int i;
    char address[sizeof(int) * ADDRESS_LENGTH];
    char lineToWrite[MAX_LABEL_LENGTH + (sizeof(int) * ADDRESS_LENGTH) + 1]; /*will hold label name and address*/

    char* externPath = malloc((strlen(filename) + strlen(EXTERNALS_FILETYPE)) * sizeof(char));
    sprintf(externPath, "%s%s", filename, EXTERNALS_FILETYPE);
    externsFile = fopen(externPath, "w");

    if (externsFile == NULL)
        return 0;

    /*Iterate through instruction array and look for label names, if they are external, write them to externals file*/
    for (i=MEMORY_START; i < IC; i++){
        if (isValidLabelName(instructionArray[i])){
            currentLabel = getSymbol(instructionArray[i]);
            if (currentLabel != NULL && currentLabel->type == EXTERNAL){
                /*write to externals file*/
                sprintf(address, "%d", i); /*formatting output*/
                sprintf(lineToWrite, "%s\t%s\n", instructionArray[i], address); /*formatting output*/
                fputs(lineToWrite, externsFile);
            }
        }
    }
    free(externPath);
    fclose(externsFile);
    return 1;
}


/*Receives a filename and writes all labels listed in the entries array into the entries file.*/
int writeToEntriesFile(char* fileName){
    FILE* entriesFile;
    int i;
    char address[sizeof(int) * ADDRESS_LENGTH];
    Label* currentLabel;
    char lineToWrite[MAX_LABEL_LENGTH + (sizeof(int) * ADDRESS_LENGTH) + 1]; /*will hold label name and address*/

    char* entriesPath = malloc((strlen(fileName) + strlen(ENTRIES_FILETYPE)) * sizeof(char));
    sprintf(entriesPath, "%s%s", fileName, ENTRIES_FILETYPE); /*formatting output*/
    entriesFile = fopen(entriesPath, "w");

    if (entriesFile == NULL)
        return 0;

    for (i=0; i < entryCount; i++){
        currentLabel = getSymbol(entriesArray[i]);
        if (currentLabel != NULL){
            sprintf(address, "%d", currentLabel->value);
            sprintf(lineToWrite, "%s\t%s\n", currentLabel->name, address);
            fputs(lineToWrite, entriesFile);
        }
        else{
            /*label listed as entry has not been declared*/
            raiseInvalidEntryLabel(entriesArray[i]);
        }

    }
    free(entriesPath);
    fclose(entriesFile);
    return 1;
}
