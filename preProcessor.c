#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "headers/constants.h"
#include "headers/stringUtils.h"
#include "headers/errors.h"
#include "headers/operations.h"
#include "headers/assembler.h"
#include "headers/errors.h"
#include "headers/memory.h"
#include "headers/macros.h"


/*Description: This file is dedicated to the pre processing stage of the assembler where macros are found in the source code and 
  the source code is copied to an output file without the macro declarations as well as replacing any reference
  to a macro in the source code with the code of the macro.*/


static int lineNumber;
static char* currentFileName;

/*Gets the name of the current file being iterated through.*/
char* getPreProcessorFileName(){
    return currentFileName;
}


/*Gets current line number of the source file.*/
int getPreProcessorLineNumber(){
    return lineNumber;
}

/*Used to close source and output files as well as delete the outputfile if there is an error during the 
  preProcessor phase.*/
void breakPreProcessor(FILE* sourceFile, FILE* outputFile, char* outputFilePath){
    fclose(sourceFile);
    fclose(outputFile);
    remove(outputFilePath);
}


/*Checks if there are extra tokens at the end of a macro declaration or at the end of a macro. endMacro variable
  acts as a boolean that tells the function whether to check for the error at the macro declaration or at the end of a macro.*/
int checkForExtraTokensInMacro(char** splitLine, int endMacro){
    if (endMacro && splitLine[1] != NULL){
        /*A valid end of macro should consist of only one token in the array*/
        raiseExtraMacroTokens(endMacro);
        return 0;
    }
    if (!endMacro && splitLine[2] != NULL){
        /*A valid end of macro should consist of only two tokens in the array*/
        raiseExtraMacroTokens(endMacro);
        return 0;
    }
    return 1; 
}


/*This function receives a string which supposed to be a path to a source file and the macroTable. If it is able to open the file,
  the function iterates through the file line by line looking for a macro declaration. It then stores the macros and their 
  respective code in the macro table while deleting the macro declaration from the file as well as replacing references 
  to a macro's name with the appropriate code.*/
int preProcessor(char* fileName){
    FILE *sourceFile, *outputFile;
    char** splitLine; /*Will hold the current line of code split by whitespace*/
    char* firstToken; /*Holds the first token of the current line in file*/
    char* macroName = ""; /*The name of the macro if it is found in code*/
    char* sourceFilePath = malloc((strlen(fileName)  + strlen(SOURCE_FILETYPE)) * sizeof(char) + 1);
    char* outputFilePath = malloc((strlen(fileName)  + strlen(POST_PREPROCESSOR_FILETYPE)) * sizeof(char) + 1);

    char* macroContents = malloc(sizeof(char)); /*Will hold the contents of a certain macro*/
    int isMacro = 0; /*Acts as boolean flag that symbolizes if currently iterating through a macro*/
    int writeLineToOutput = 1; /*Acts as boolean flag that tells the program to write a line to output file or not*/
    char line[MAX_STATEMENT_LENGTH + 1];
    lineNumber = 1;
    currentFileName = fileName;
    
    sprintf(sourceFilePath, "%s%s", fileName, SOURCE_FILETYPE);
    sprintf(outputFilePath, "%s%s", fileName, POST_PREPROCESSOR_FILETYPE);
    sourceFile = fopen(sourceFilePath, "r");
    outputFile = fopen(outputFilePath, "w"); /*Creating am file to be written to.*/

     if (sourceFile == NULL){
        raiseFileNotFound(fileName);
		fclose(outputFile);
		remove(outputFilePath);
        return 0;

    }

    /*This section iterates through source file line by line, finds macros and writes code to the output file, skipping over
      macro declarations and replacing references to macros in the source file with their code in the output file.*/
    while (fgets(line, MAX_STATEMENT_LENGTH, sourceFile) != NULL){
        trimWhitespace(line);

        if (strlen(line) == 1 && line[0] == '\n'){
            /*Found empty line, can skip to next iteration*/
            lineNumber++;
            continue;
        }

        splitLine = splitLineByWhitespace(line);
        firstToken = splitLine[0];
        writeLineToOutput = 1;

        if (isMacro){
            writeLineToOutput = 0;
            if (strcmp(firstToken, END_MACRO_ID) == 0){
                /*Reached end of macro declaration*/
                if (checkForExtraTokensInMacro(splitLine, 1) != 0 && isValidMacroName(macroName) != 0){
                    /*There are no extra tokens at the end of the macro and the macro name is valid*/
                    /*store the macro ID and contents in macroTable*/
                   /*htInsert(macroTable, macroName, macroContents);*/
                   
                    enterMacro(macroName, macroContents);
                    isMacro = 0; /*no longer iterating through macro*/
                }
                else{
                    /*Too many extra tokens at the end of the macro or invalid macro name*/
                    breakPreProcessor(sourceFile, outputFile, outputFilePath);
                    break;
                }
            }
            else{
                /*Reallocate memory for contents of the current macro*/
                macroContents = realloc(macroContents, strlen(macroContents) + strlen(line) + 1);
                strcat(macroContents, line); /*current line is added to the current macro*/
            }
        }

        if (getMacroContents(firstToken) != NULL){
            /*Found reference to a known macro ID, write the macroContents from macroTable to output file*/
            writeLineToOutput = 0;
            fputs(getMacroContents(firstToken), outputFile);
        }

        if (strcmp(firstToken, MACRO_ID) == 0){
            /*A macro declaration has been found*/
            if (checkForExtraTokensInMacro(splitLine, 0) != 0){
                /*There are no extra tokens in the macro declaration line*/
                isMacro = 1;
                writeLineToOutput = 0;
                macroName = splitLine[1]; /*The name of the macro is the next token in the line*/
                macroContents[0] = '\0'; /*Reset macroContents*/
            }
            else{
                /*There are too many tokens in the macro declaration*/
                breakPreProcessor(sourceFile, outputFile, outputFilePath);
                break;
            }
        }
        
        if (writeLineToOutput)
            fputs(line, outputFile);

        free(splitLine);
        lineNumber++;
    }
    
    /*Free all dynamically allocated memory and close files.*/
    free(sourceFilePath);
    free(outputFilePath);
    free(macroContents);
    fclose(sourceFile);
    fclose(outputFile);
    return 1;
}
