#include <stdio.h>

#include "headers/assembler.h"
#include "headers/operations.h"
#include "headers/preProcessor.h" 


/*Description: this file contains all the errors of the assembler. Each time an error is raised, the changeOutputStatus 
  function is called which signals to the assembler to not create any output files.*/


void raiseFileNotFound(char* filename){
    changeOutputStatus();
    fprintf(stdout, "Error: Failed to open %s for processing.\nExiting program.", filename);
}


void raiseExtraMacroTokens(int endMacro){
    changeOutputStatus();
    if (endMacro)
        fprintf(stdout, "Error at line %d in %s.as: Extra characters after endmcr.\n", 
            getPreProcessorLineNumber(), getPreProcessorFileName());
    else fprintf(stdout, "Error at line %d in %s.as: Extra characters at end of macro declaration.\n", 
        getPreProcessorLineNumber(), getPreProcessorFileName());
}

void raiseInvalidMacroName(char* str, int operationName){
    changeOutputStatus();
    if (operationName)
        fprintf(stdout , "Error at line %d in %s.as: %s is an invalid macro name because it is the name of an operation", 
        getPreProcessorLineNumber(), getPreProcessorFileName(), str);
    else fprintf(stdout , "Error at line %d in %s.as: %s is an invalid macro name because it is the name of a register", 
    getPreProcessorLineNumber(), getPreProcessorFileName(), str);
}

void raiseInvalidLabelSyntax(char* str){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: label %s has invalid syntax. First letter should be a letter followed by a series of alphanumeric characters and should be ended with ':' with no spaces.\n", 
    getLineNumber(), getFileName(), str);
}

void raiseLabelIsOpName(char* str){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: label %s is invalid because it is the name of an operation.\n", 
    getLineNumber(), getFileName(), str);
}

void raiseLabelIsRegisterName(char* str){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: label %s is invalid because it is the name of a register.\n", 
    getLineNumber(), getFileName(), str);
}

void raiseLabelAlreadyExists(char* str){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: label %s is already declared somewhere else.\n", 
    getLineNumber(), getFileName(), str);
}


/*comma errors*/

void raiseCommaAtStart(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: comma at start of token.\n", getLineNumber(), getFileName());
}

void raiseConsecutiveCommas(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: consecutive commas.\n", getLineNumber(), getFileName());
}

void raiseCommaAtEnd(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: comma at end of token.\n", getLineNumber(), getFileName());
}

void raiseNoCommasBetween(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: no commas between tokens.\n", getLineNumber(), getFileName());
}

/*Statement syntax errors*/

void raiseInvalidCharInData(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: invalid char in data.\n", getLineNumber(), getFileName());
}

void raiseStrayTokenError(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: stray token.\n", getLineNumber(), getFileName());
}

void raiseNoQuotesError(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: given string should begin and end with quotation mark.\n", getLineNumber(), getFileName());
}

void raiseTooManyParams(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: too many parameters given.\n", getLineNumber(), getFileName());
}

void raiseTooFewParams(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: too few parameters given.\n", getLineNumber(), getFileName());
}

void raiseNoSpaceAfterOp(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: no space between operation name and rest of statement.\n", getLineNumber(), getFileName());
}

void raiseMissingOperand(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: missing operand.\n", getLineNumber(), getFileName());
}

void raiseTooManyOperands(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: too many operands given.\n", getLineNumber(), getFileName());
}

void raiseInvalidSourceType(){
    changeOutputStatus();
    fprintf(stdout, "Error: at line %d in %s.am: the assignment type of the source operand does not match with the assignment types of the operation.\n",
     getLineNumber(), getFileName());
}

void raiseInvalidDestinationType(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: the assignment type of the destination operand does not match with the assignment types of the operation.\n", 
    getLineNumber(), getFileName());
}

void raiseSpaceInJumpOperand(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: there is a space in the jump operand.\n", getLineNumber(), getFileName());
}

void raiseMissingParenthesesInJumpOperand(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: missing parentheses in jump operand.\n", getLineNumber(), getFileName());
}

void raiseUndeclaredLabelReference(char* labelName){
    changeOutputStatus();
    fprintf(stdout, "Error: label %s has been referenced at line/s ", labelName);
    printUndeclaredLabelReferences(getFileName(), labelName);
    fprintf(stdout, "without being declared\n");
}

void raiseTooManyParentheses(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: too many parentheses in jump operand.\n", getLineNumber(), getFileName());
}

void raiseInvalidEntryLabel(char* labelName){
    changeOutputStatus();
    fprintf(stdout, "Error at in %s.am: the label %s being entered does not exist.\n", getFileName(), labelName);
}

void raiseDataOverFlow(){
    changeOutputStatus();
    fprintf(stdout, "Error: code in %s.as is too long, cannot be stored in memory.\n", getFileName());
}

void raiseUnidentifiedStatement(){
    changeOutputStatus();
    fprintf(stdout, "Error at line %d in %s.am: this statement does not match the language syntax.\n", getLineNumber(), getFileName());
}