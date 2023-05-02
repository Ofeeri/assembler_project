#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "headers/constants.h"
#include "headers/errors.h"
#include "headers/operations.h"


/*Description: this file contains utility functions that handle strings.*/


/*This function receives a string and returns an array of all the tokens in the string split by white space.*/
char** splitLineByWhitespace(char* str){
    int i = 0; /*Iterates through str*/
    int j = 0; /*used to add tokens to splitString*/
    int k = 0; /*Used to add chars to current pointer*/

    char** splitString = malloc(sizeof(char*));
    splitString[0] = NULL;

    while (i < strlen(str)) {
        /*Skip whitespace characters*/
        while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') {
            i++;
        }

        splitString[j] = malloc(MAX_STATEMENT_LENGTH * sizeof(char));

        k = 0;
        while (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\0') {
            /*Current characters are not whitespace*/
            splitString[j][k] = str[i];
            k++;
            i++;
        }
        splitString[j][k] = '\0'; /*Terminates current pointer*/
        j++;
        splitString = realloc(splitString, (j + 1) * sizeof(char*));
        splitString[j] = NULL;
    }
    splitString[j-1] = NULL; /*Ensures that array is null terminated at the correct spot*/
    return splitString;
}


/*This function receives a string and removes all leading and trailind whitespace (except for \n at the end)*/
void trimWhitespace(char* str){
    /*Remove leading whitespace*/
    int count = 0;
    int i = 0;
    while (str[count] == ' ' || str[count] == '\t'){
        count++;
    }

    for (i=0; (i + count) < strlen(str); i++){
        str[i] = str[i + count];
    }
    str[i] = '\0';


    /*Remove trailing whitespace*/
    i = strlen(str) - 1;

    while (i >= 0){
        if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
            i--;
        else break;
    }

    str[i+1] = '\n'; /*Adds newline back because it is removed along with other whitespace*/
    str[i+2] = '\0';
}


/*Receives a statement and a pointer. Gets pointer to place in statement and checks 
  if there are commas between the end of the token pointed to by pointer and the beginning of the next token and checks
  if there are consecutive commas with no chars between them within the next token. 
  Also check if there are commas at the end of the statement. Returns 1 if there are no invalid commas, 0 otherwise.*/
int checkForExtraCommas(char* statement, char* pointer){
    int lookForStartComma = 0; /*Acts as bool that indicates if to look for comma before next pointer*/
    int withinNextToken = 0; /*Acts as bool that indicates if iterating within the pointer after the given pointer*/
    int lookForConsecutiveComma = 0; /*Acts as bool that indicated if looking for two consecutive commas*/
    while (*pointer){
        if (isspace(*pointer) && !withinNextToken){
            /*start looking for comma between pointer and next token*/
            lookForStartComma = 1;
        }
        if (lookForStartComma && *pointer == ','){
            raiseCommaAtStart(pointer);
            return 0;
        }
        if (lookForStartComma && !isspace(*pointer)){
            lookForStartComma = 0;
            withinNextToken = 1;
        }
        if (withinNextToken && lookForConsecutiveComma && *pointer == ','){
            raiseConsecutiveCommas(pointer);
            return 0;
        }
        if (withinNextToken && *pointer == ','){
            lookForConsecutiveComma = 1;
        }
        if (withinNextToken && lookForConsecutiveComma && *pointer != ',' && !isspace(*pointer)){
            lookForConsecutiveComma = 0;
        }
        pointer++;
    }
    if (statement[strlen(statement)-2] == ','){ /*Checks for comma at end of statement. -2 because last char is '\n'*/
        raiseCommaAtEnd(statement);
        return 0;
    }
    /*No invalid commas*/
    return 1;
}

/*Receives string and checks if there are two consecutive tokens without a comma between them. Returns 1 if 
  there are commas between each token, 0 otherwise.*/
int checkForNoCommas(char* pointer){
    int lookForComma = 0;
    while (*pointer){
        if (lookForComma && *pointer && !isspace(*pointer) && *pointer != ','){
            raiseNoCommasBetween(pointer);
            return 0;
        }
        if (lookForComma && *pointer == ',')
            lookForComma = 0;
        if (!isspace(*pointer) && *pointer != ',' && isspace(*(pointer+1)))
            /*only looks for comma if the current char is not a space and not a comma and the next char is a space*/
            lookForComma = 1;
        pointer++;
    }
    /*Commas between each token*/
    return 1;
}


/*Receives a pointer to a char in a larger string and iterates backwards in string until reaching start of string or label
declaration. If any stray token is found, raises error and returns 0. Returns 1 otherwise.*/
int checkForStrayString(char* statement, char* token){
    char* statementCopy = malloc(strlen(statement) + 1);
    char* pointer = strstr(statement, token);

    statementCopy = strcpy(statementCopy, statement);
    while (*pointer){
        pointer--;

        if (!isspace(*pointer) && *pointer != ':' && (strcmp(pointer, "") != 0)){
            /*found character that is not empy character or ':'.*/
            raiseStrayTokenError();
            return 0;
        }
        if (*pointer == ':')
            /*Reached label declaration*/
            break;
    }
    return 1;
}


/*Used to check if the string given in a .string statement is valid.*/
void checkForValidString(char* string){
    trimWhitespace(string);
    if (string[0] != '"' || string[strlen(string) - 2] != '"')
        raiseNoQuotesError();
    
}


/*Receives statement and str and checks if there are any stray tokens (separated by whitespace) after str.
  Raises appropriate error if there are.*/
void checkForExtraOperands(char* statement, char* str){
    char* pointer = strstr(statement, str) + strlen(str); /*Increment to after str*/
    
    while (*pointer){
        
        if (!isspace(*pointer)){
            /*Found non whitespace character where there shouldn't be one*/
            raiseTooManyOperands();
            break;
        }
        pointer++;
    }
}


/*Receives a string and checks if there are too many parentheses in it.*/
void checkTooManyParentheses(char* str){
    char* pointer = str;
    int parenthesesCount = 0;
    while (*pointer){
        if (*pointer == '(' || *pointer == ')')
            parenthesesCount++;
        if (parenthesesCount > 2){
            raiseTooManyParentheses();
            break;
    }
    pointer++;
    }
}