#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "headers/constants.h"
#include "headers/operations.h"
#include "headers/memory.h"
#include "headers/errors.h"
#include "headers/stringUtils.h"
#include "headers/operands.h"

/*Description: this file is dedicated to all operations and data types that are related to analyzing statements in the source code.*/


typedef enum {EMPTY, COMMENT, INSTRUCTION, COMMAND, UNIDENTIFIED} Statement_type;
typedef enum {DATA, STRING, ENTRY, EXTERN, NONE} Instruction_type; /*types of instruction statements*/
static Instruction_type currentInstructionType; /*Holds the type of the current instruction (if there is one)*/


/*Returns the instruction type of the current instruction statement.*/
Instruction_type getCurrentInstructionType(){
    return currentInstructionType;
}


/*Receives an instruction statement with .data tag and calls the relevant error 
  checking functions to ensure the statement is valid. Returns 1 if valid, 0 otherwise.*/
void checkDataInstructionSyntax(char* statement){
    char* pointer = strstr(statement, ".data");
    checkForStrayString(statement, ".data");

    pointer += strlen(".data"); /*Increment pointer to after .data*/
    checkForExtraCommas(statement, pointer);
    checkForNoCommas(pointer);
}


/*Receives an instruction statment with .string tag and calls the relevant error
  checking functiions to ensure the statement is valid. Returns 1 if valid, 0 otherwise.*/
void checkStringInstructionSyntax(char* statement){
    char* pointer = strstr(statement, ".string");
    checkForStrayString(statement, ".string");

    pointer += strlen(".string"); /*Increment pointer to after .string*/
    checkForValidString(pointer);
}


/*Receives a statement and gets the instruction that should appear after the '.'. Will return this instruction
  even if it is invalid (will be checked later in the program).*/
char* getInstruction(char* statement){
    char* instruction = malloc(MAX_INSTRUCTION_LENGTH + 1);
    char* iPointer = strchr(statement, '.'); /*Pointer to start of instruction*/
    int i = 0;

    while (*iPointer && !isspace(*iPointer)){
        /*Iterates through token that starts with '.' until whitespace char*/
        instruction[i] = *iPointer;
        iPointer++;
        i++;
    }
    instruction[i] = '\0';
    return instruction;
}

/*Receives char* which is supposed to be an instruction and if it is, returns its type. OtherWise returns NONE
  meaning no valid instruction was found in the statement.*/
Instruction_type getInstructionType(char* instruction){
    if (strcmp(instruction, ".data") == 0)
        return DATA;
    if (strcmp(instruction, ".string") == 0)
        return STRING;
    if (strcmp(instruction, ".extern") == 0)
        return EXTERN;
    if (strcmp(instruction, ".entry") == 0)
        return ENTRY;
    return NONE;
}


/*This function receives a statement from the source code and checks if it is a possible command statement by looking for '.'
  in the statement. Returns 1 if a '.' is found, 0 otherwise.*/
int isPossibleInstructionstatement(char* statement){
    if (strchr(statement, '.') != NULL){
        /*Found possible instruction declaration*/
        return 1;
    }
    return 0;
}


/*Receives a statement and a pointer to the command section of the statement. Checks the syntax of the statement 
  and raises errors if needed.*/
void checkTwoOperandSyntax(char* statement, char* pointer){
    char* firstOperand;
    char* secondOperand;

    checkForExtraCommas(statement, pointer);
    checkForNoCommas(pointer);
    firstOperand = getSingleOperand(pointer);
    secondOperand = getSecondOperand(pointer);
    
    if (firstOperand == NULL || secondOperand == NULL){
        raiseMissingOperand();
        return;
    }

    if (strcmp(firstOperand, secondOperand) != 0)
        checkForExtraOperands(statement, secondOperand); /*Looks for extra operands after second operand*/
}


/*Receives a statement and a pointer to the command section of the statement. Checks the syntax of the statement 
  and raises errors if needed.*/
void checkOneOperandSyntax(char* statement, char* pointer){
    char* operand;

    checkForExtraCommas(statement, pointer);
    operand = getSingleOperand(pointer);

    if (operand == NULL){
        raiseMissingOperand();
        return;
    }
    
    checkForExtraOperands(statement, operand); /*Looks for extra operands after first operand*/
}


/*Receives a statement and a pointer to the command section of the statement. Checks the syntax of the statement 
  and raises errors if needed.*/
void checkJumpOperandSyntax(char* statement, char* pointer){
    char* jumpOperand;

    jumpOperand = getJumpOperand(pointer);

    if (jumpOperand == NULL)
        raiseMissingOperand();
    
    checkForExtraOperands(statement, jumpOperand);

}

/*Receives a statement, a pointer to the section of the statement after the operation name, and 
  a pointer to the current operation in the statement and does syntax checks accordingly.*/
void checkCommandSyntax(char* statement, char* pointer, Operation* currentOperation){
    
    int operandNumber = currentOperation->numberOfOperands;
    if (!isspace(*pointer)) 
        /*Checks if there is a whitespace immediately after op name (because pointer has been incremented to after op name)*/
        raiseNoSpaceAfterOp();

    checkForStrayString(statement, currentOperation->opName); /*Finds stray string between start of statement and opName*/

    if (operandNumber == 2){ 
        /*2 operand operation checks*/
        checkTwoOperandSyntax(statement, pointer);
    }

    if (operandNumber == 1 && isPossibleJumpOperand(statement)){
        /*jump operand checks*/
        checkJumpOperandSyntax(statement, pointer);
    }

    if (operandNumber == 1 && isPossibleJumpOperand(statement) == 0){
        /*Single operand operation checks*/
        checkOneOperandSyntax(statement, pointer);
        
    }
    if (operandNumber == 0){
        /*No operand operation checks*/
        checkForExtraOperands(statement, currentOperation->opName);
    }

}


/*This function receives a statement from the source code and analyzes it to determine its type. Also updates the 
currentInstructionType variable.*/
Statement_type getStatementType(char* statement){
	if (statement[0] == COMMENT_ID){
		/*Found comment statement*/
		return COMMENT;
    }
	if (strlen(statement) == 1 && statement[0] == '\n'){
		/*Found empty line*/
		return EMPTY;
    }
    if (isPossibleInstructionstatement(statement)){
        char* instruction = getInstruction(statement);
        currentInstructionType = getInstructionType(instruction);
        if (currentInstructionType != NONE){ /*instruction type is valid*/
            /*Found instruction statement*/
            return INSTRUCTION;
        }
    }
    if (getOperation(statement) != NULL){
        /*Found command statement*/
        return COMMAND;
    }
	return UNIDENTIFIED;
}