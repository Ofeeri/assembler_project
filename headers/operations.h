#include "constants.h"

typedef struct Operation{
    char opName[5];
    int opCode; /*The operation code of the operation*/
    int numberOfOperands;
} Operation;


static const Operation operations[NUMBER_OF_OPERATIONS];

int isOperationName(char* str);
Operation* getOperation(char* statement);
int isValidSourceOperand(Operation* operation, char* operand);
int isValidDestinationOperand(Operation* operation, char* operand);
