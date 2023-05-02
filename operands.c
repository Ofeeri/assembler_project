#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "headers/labels.h"
#include "headers/memory.h"
#include "headers/constants.h"
#include "headers/errors.h"
#include "headers/stringUtils.h"


/*Description: this file contains all functions and datatypes that have to do with getting and checking operands
  of operations found in source code.*/


typedef enum {IMMEDIATE, DIRECT, JUMP, DIRECT_REGISTER, NO_TYPE} Assignment_Type; /*all types of operands*/

Assignment_Type getAssignmentType(char* operand, int firstJumpCheck);


/*Receives pointer to a section of a statement and returns the jump operand in the statement.
  If no operand is found, returns null.*/
char* getJumpOperand(char* pointer){
    char* operand = malloc(strlen(pointer) + 1);
    int i = 0;
    trimWhitespace(pointer);

    while (*pointer && *pointer != '\n'){
        
        operand[i] = *pointer;
        pointer++;
        i++;
    }
    if (i == 0)
        /*No operand found*/
        return NULL;
    
    operand[i] = '\0';
    return operand;
}


/*Receives pointer to a section of a statement and returns the first operand that is encountered.
  If no operand is found, return null.*/
char* getSingleOperand(char* pointer){
    char* operand = malloc(strlen(pointer) + 1);
    int i = 0;
    trimWhitespace(pointer);

    while (*pointer && !isspace(*pointer) && *pointer != ','){
        /*Iterates until reaching a space or a comma*/
        operand[i] = *pointer;
        pointer++;
        i++;
    }
    if (i == 0)
        /*No operand found*/
        return NULL;
    operand[i] = '\0';
    return operand;
}


/*Receives a pointer to a section of a statement and returns the operand after the first operand.
  If no operand is found, returns null.*/
char* getSecondOperand(char* pointer){
    char* operand = malloc(MAX_INSTRUCTION_LENGTH + 1);
    int lookForNextOperand = 0; /*Bool flag that indicates if to look for second operand*/
    int withinSecondOperand = 0; /*Bool flag that indicates if loop has reached the second operand*/
    int i = 0;

    while (*pointer){

        if (lookForNextOperand && (isalnum(*pointer) || *pointer == '#'))
            /*Found beginning of next operand*/
            withinSecondOperand = 1;

        if (withinSecondOperand && isspace(*pointer))
            break;

        if (withinSecondOperand){
            /*Add chars to second operand if they are not spaces*/
            operand[i] = *pointer;
            i++;
        }

        if (*pointer == ','){
            /*Reached end of first operand*/
            lookForNextOperand = 1;
        }
        pointer++;
    }
    if (i == 0)
        /*No operand found*/
        return NULL;
    operand[i] = '\0';
    return operand;
}


/*Receives a jump operand and returns the label before the parentheses*/
char* getJumpLabel(char* operand){
	int i = 0;
	char* label = malloc(strlen(operand) * sizeof(char));
	char* pointer = operand;
	while (*pointer){
		if (*pointer == '('){
			/*reached end of label in jump operand*/
			label[i] = '\0';
			break;
		}
		label[i] = *pointer;
		i++;
		pointer++;
	}
	return label;
}


/*Receives a jump operand and returns the first operand within the parentheses.*/
char* getJumpSourceOperand(char* operand){
	int i = 0;
	int withinParentheses = 0;
	char* sourceOperand = malloc(strlen(operand) * sizeof(char));
	char* pointer = operand;
	while (*pointer){
		if (*pointer == '(')
			withinParentheses = 1;

		if (withinParentheses && *pointer != '(' && *pointer != ','){ 
			sourceOperand[i] = *pointer;
			i++;
		}
		if (*pointer == ','){
			/*reached end of first operand*/
			sourceOperand[i] = '\0';
			break;
		}
		pointer++;
	}
	return sourceOperand;
}


/*Receives a jump label and returns the second operand within the parentheses.*/
char* getJumpDestinationOperand(char* operand){
	int i = 0;
	int afterComma = 0;
	char* destOperand = malloc(strlen(operand) * sizeof(char));
	char* pointer = operand;

	while (*pointer){
		if (*pointer == ',')
			afterComma = 1;

		if (afterComma && *pointer != ',' && *pointer != ')'){
			destOperand[i] = *pointer;
			i++;
		}
		if (*pointer == ')'){
			/*Reached end of destination operand*/
			destOperand[i] = '\0';
			break;
		}
		pointer++;
	}
	return destOperand;
}


/*Receives a statement and returns 1 if the statement contains a possible jump operand, 
  returns 0 otherwise.*/
int isPossibleJumpOperand(char* statement){
    if (strchr(statement, '(') || strchr(statement, ')'))
        return 1;
    else return 0;
}


/*Receives a possible jump operand (operand with parentheses) and gets the label before the parentheses, 
  and the source and destination operands within the parentheses in order to check the validity of each seperate
  part of the jump operand. Also checks for syntax errors in the operand. Returns 1 if operand is valid, 0 otherwise.
  Because this function is called multiple times on the same operand, firstCheck acts as a bool to indicate whether
  to check for parentheses (this is only needed during the first call)*/
int isValidJumpOperand(char* operand, int firstCheck){
	char* label = malloc(strlen(operand) * sizeof(char));
	char* firstOperand = malloc(strlen(operand) * sizeof(char));
	char* secondOperand = malloc(strlen(operand) * sizeof(char));
	char* pointer = operand;
	int i = 0;

	int withinParentheses = 0; /*Bool to indicate if within parentheses or not*/
	int withinSecondOperand = 0; /*Bool to indicate if loop has reached second operand*/

	checkTooManyParentheses(operand);
	if (firstCheck && (!strchr(operand, '(') || !strchr(operand, ')'))){
		raiseMissingParenthesesInJumpOperand();
		return 0;
	}

	while (*pointer){
		if (isspace(*pointer)){
			/*Should be no spaces in jump operand*/
			raiseSpaceInJumpOperand();
			return 0;
		}

		if (!withinParentheses && *pointer != '('){
			/*add char to label*/
			label[i] = *pointer;
			i++;
		}

		if (withinParentheses && !withinSecondOperand && *pointer != ','){
			/*add char to first operand*/
			firstOperand[i] = *pointer;
			i++;
		}

		if (withinParentheses && withinSecondOperand && *pointer != ')'){
			/*add char to second operand*/
			secondOperand[i] = *pointer;
			i++;
		}

		if (*pointer == '('){
			withinParentheses = 1;
			label[i] = '\0'; /*terminate outside label*/
			i = 0; /*Reset index to add chars to firstOperand*/
		}

		if (*pointer == ','){
			withinSecondOperand = 1;
			firstOperand[i] = '\0'; /*terminte first operand*/
			i = 0;
		}
		pointer++;
	}
	secondOperand [i] = '\0'; /*terminate second operand*/

	/*check validity of internal operands, firstcheck = 0 now*/
	if (getAssignmentType(label, 0) == NO_TYPE || getAssignmentType(firstOperand, 0) == NO_TYPE || getAssignmentType(secondOperand, 0) == NO_TYPE){
		/*One of the operands in the jump operand has invalid syntax*/
		free(label);
		free(firstOperand);
		free(secondOperand);
		return 0;
	}
	free(label);
	free(firstOperand);
	free(secondOperand);
	return 1;
}

/*Receives an operand and checks if it is a valid number to be used as an operand.
  Returns 1 if number is valid operand, 0 otherwise.*/
int isValidNumber(char* operand){
	char* pointer = operand;
	if (operand == NULL)
		return 0;
	if (operand[1] == '-' || operand[1] == '+')
		pointer += 2; /*skips # and + or -*/
	else pointer+= 1; /*skips only #*/
	while (*pointer){
		if (!isdigit(*pointer))
			/*non digit char in operand*/
			return 0;
		pointer++;
	}
	return 1;
}


/*Receives an operand as string and returns it as a valid integer*/
int convertOperandToInt(char* operand){
	char* pointer = operand;
	if (operand[1] == '+')
		pointer += 2; /*skips # and +*/
	else pointer+= 1; /*skips only #*/
	return atoi(pointer);
}


/*Receives an operand and analyzes it to determine its assignment type.
  firstJumpCheck acts as a bool that is needed for the isValidJumpOperand function*/
Assignment_Type getAssignmentType(char* operand, int firstJumpCheck){
	if (operand == NULL)
		return NO_TYPE;
	if (isValidNumber(operand) && operand[0] == '#')
		/*operand is immediate number*/
		return IMMEDIATE;

	if (isValidLabelNameNoError(operand))
		/*operand is a label*/
		return DIRECT;

	if (isPossibleJumpOperand(operand)){
		if (isValidJumpOperand(operand, firstJumpCheck))
			/*operand is a jump operand*/
			return JUMP;
	}

	if (isRegisterName(operand))
		/*operand is a register*/
		return DIRECT_REGISTER;

	return NO_TYPE; /*Operand is not recognizable*/
	}
		
