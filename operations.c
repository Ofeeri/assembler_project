#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "headers/constants.h"
#include "headers/operands.h"
#include "headers/errors.h"


/*Description: this file contains all data types and functions dealing with operations in the assembly language and
  in the source code. There is a static list of operations containing an instance of all 16 operations. Also
  contains functions that check the validity of a given operand based on the operand types of the given operation*/


typedef struct Operation{
    char opName[5]; /*name of operation*/
    int opCode; /*The operation code of the operation*/
    int numberOfOperands; /*Number of operands operation can get*/
    Assignment_Type sourceOperandTypes[4]; /*Valid source operand types*/
    Assignment_Type destOperandTypes[4]; /*Valid destination operand types*/
} Operation;


/*static list of operations and their attributes*/
Operation operations[NUMBER_OF_OPERATIONS + 1] = {
    /*{name, opcode, number of operations, source types, destination types}*/
    {"mov", 0, 2, {IMMEDIATE, DIRECT, DIRECT_REGISTER}, {DIRECT, DIRECT_REGISTER}},
    {"cmp", 1, 2, {IMMEDIATE, DIRECT, DIRECT_REGISTER}, {IMMEDIATE, DIRECT, DIRECT_REGISTER}},
    {"add", 2, 2, {IMMEDIATE, DIRECT, DIRECT_REGISTER}, {DIRECT, DIRECT_REGISTER}}, 
    {"sub", 3, 2, {IMMEDIATE, DIRECT, DIRECT_REGISTER}, {DIRECT, DIRECT_REGISTER}},
    {"not", 4, 1, {NO_TYPE}, {DIRECT, DIRECT_REGISTER}},
    {"clr", 5, 1, {NO_TYPE}, {DIRECT, DIRECT_REGISTER}},
    {"lea", 6, 2, {DIRECT}, {DIRECT, DIRECT_REGISTER}},
    {"inc", 7, 1, {NO_TYPE}, {DIRECT, DIRECT_REGISTER}},
    {"dec", 8, 1, {NO_TYPE}, {DIRECT, DIRECT_REGISTER}},
    {"jmp", 9, 1, {NO_TYPE}, {DIRECT, JUMP, DIRECT_REGISTER}},
    {"bne", 10, 1, {NO_TYPE}, {DIRECT, JUMP, DIRECT_REGISTER}},
    {"red", 11, 1, {NO_TYPE}, {DIRECT, DIRECT_REGISTER}},
    {"prn", 12, 1, {NO_TYPE}, {IMMEDIATE, DIRECT, DIRECT_REGISTER}},
    {"jsr", 13, 1, {NO_TYPE}, {DIRECT, JUMP, DIRECT_REGISTER}},
    {"rts", 14, 0, {NO_TYPE}, {NO_TYPE}},
    {"stop", 15, 0, {NO_TYPE}, {NO_TYPE}}
};


/*Receives string and returns 1 if the string is the name of an operation in the assembly language. Returns 0 if not.*/
int isOperationName(char* str){
    int i;
    for (i=0; i < NUMBER_OF_OPERATIONS; i++){
        if (strcmp(str, operations[i].opName) == 0)
            return 1;
    }
    return 0;
}


/*Receives statement and looks for an operation name in the statement. If one is found returns a pointer 
  to the appropriate operation.*/
Operation* getOperation(char* statement){
    int i;
    char opNameAsLabel[6];
    Operation* currentOp;
    
    currentOp = operations;
    for (i=0; i < NUMBER_OF_OPERATIONS; i++, currentOp++){
        /*this section skips over opName if it is wrongfully declared as a label*/
        sprintf(opNameAsLabel, "%s:", currentOp->opName);
        if (strstr(statement, opNameAsLabel)){
            statement = strchr(statement, ':');
        }

        if (strstr(statement, currentOp->opName)){
            return currentOp;
        }
   }
    return NULL;
}


/*Receives operation pointer and an operand and checks if the operands assignment type is valid
  for the given operation as a source operand.*/
int isValidSourceOperand(Operation* operation, char* operand){
    Assignment_Type operandType = getAssignmentType(operand, 1);
    int i;
    for (i=0; i < MAX_ASSIGNMENT_TYPES - 1; i++){
        if (operation->sourceOperandTypes[i] == operandType)
            return 1;
    }
    raiseInvalidSourceType();
    return 0;
}


/*Receives operation pointer and an operand and checks if the operands assignment type is valid
  for the given operation as a destination operand.*/
int isValidDestinationOperand(Operation* operation, char* operand){
    Assignment_Type operandType = getAssignmentType(operand, 1);
    int i;
    for (i=0; i < MAX_ASSIGNMENT_TYPES - 1; i++){
        if (operation->destOperandTypes[i] == operandType)
            return 1;
    }
    raiseInvalidDestinationType();
    return 0;
}