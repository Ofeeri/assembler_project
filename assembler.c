#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "headers/constants.h"
#include "headers/utils.h"
#include "headers/memory.h"
#include "headers/stringUtils.h"
#include "headers/statements.h"
#include "headers/errors.h"
#include "headers/labels.h"
#include "headers/operations.h"
#include "headers/operands.h"


/*Description: this file deals with all function that have to do with the actual assembly process.
  Including the first and second pass and all functions that handle statements in the source code.
  Functions that call appropriate encoding functions based on the parameters of each statement are also 
  found here.*/


static int lineNumber; /*Indicated current line number in source file*/
static int outputStatus; /*Acts as bool that indicates whether to create output files and enter */
static int outputExterns; /*Acts as bool that indicates whether to create an externals file or not*/
static int outputEntries; /*Acts as bool that indicates whether to create an entries file or not*/
static char* currentFileName; /*The name of the current file*/


/*Returns name of the current file being assembled*/
char* getFileName(){
	return currentFileName;
}


/*Returns current line number of the sourcefile.*/
int getLineNumber(){
	return lineNumber;
}


/*Changes outputs status to 0, meaning no output files will be created because an error has been found.*/
void changeOutputStatus(){
	if (outputStatus){
		outputStatus = 0;
		outputExterns = 0;
		outputEntries = 0;
	}
}


/*Receives filename and an undefined label name and prints all lines at which the label
  is referenced in the source code.*/
void printUndeclaredLabelReferences(char* fileName, char* labelname){
	FILE* sourceFile;
	char statement[MAX_STATEMENT_LENGTH+1];
	char* sourcePath = malloc((strlen(fileName) + strlen(SOURCE_FILETYPE)) * sizeof(char));
	sprintf(sourcePath, "%s%s", fileName, SOURCE_FILETYPE);
	sourceFile = fopen(sourcePath, "r");
	
	lineNumber = 1;
	while (fgets(statement, MAX_STATEMENT_LENGTH, sourceFile) != NULL){
		trimWhitespace(statement);
		if (strstr(statement, labelname) && getStatementType(statement) != INSTRUCTION)
			printf("%d ",  lineNumber);
		lineNumber++;
	}
	free(sourcePath);
	fclose(sourceFile);
}


/*Receives statement and Checks that there are not too many or too few parameters in
  the .extern instruction. If the given label name is valid, it is entered into the symbol table
  with the external tag.*/
int handleExternInstruction(char* statement){
	char** splitLine = splitLineByWhitespace(statement);
	if (splitLine[2] != NULL){
		/*Should only be .extern and single label given*/
		raiseTooManyParams();
		return 0;
	}
	if (splitLine[1] == NULL){
		/*No label given*/
		raiseTooFewParams();
		return 0;
	}
	if (isValidLabelName(splitLine[1])){
		enterSymbol(splitLine[1], EXTERN_DEFAULT_VALUE, CODETAG, EXTERNAL);
		outputExterns = 1; /*program should output externals file*/
	}
	else return 0;
	return 1;
}


/*Recevies a statement and, if there are no syntax errors, adds the
  label in the .entry statement to the entries array to be stored in .ent
  file later in the program.*/
int handleEntryInstruction(char* statement){
	char** splitLine = splitLineByWhitespace(statement);
	if (splitLine[2] != NULL){
		/*Should only be .entry and single label given*/
		raiseTooManyParams();
		return 0;
	}
	if (splitLine[1] == NULL){
		/*No label given*/
		raiseTooFewParams();
		return 0;
	}
	if (isValidLabelName(splitLine[1])){
		outputEntries = 1; /*program should output externals file*/
		enterEntry(splitLine[1]);
	}
	return 1;
}


/*Receives statement. If a label is declared and valid in the statement, adds label to the symbol table 
  with the correct type (.data) and value (data counter). Then iterates through the statement in order to find the given
  string, converting each char into binary and storing it in the correct place in the instruction array after which the
  data counter DC is incremented. Also adds '\0' to instruction array at the end of the string to symbolize the end of string.*/
int handleStringInstruction(char* statement){
	char* bin; /*binary representation of the current number*/
	int withinString = 0; /*Acts as bool that indicates whether iterating through string or not.*/
	char* pointer = strstr(statement, ".string"); /*points to occurrence of .string in statement*/
	pointer += strlen(".string"); /*Increment pointer to after .string token*/

	/*This section checks for a label and if it finds one, checks its validity in order to insert into symbol table*/
	if (isPossibleLabelDeclaration(statement))
		handleLabelDeclaration(statement, DATATAG);

	checkStringInstructionSyntax(statement);

	/*This section iterates through the statement to add the given string to memory.*/
	if (outputStatus){
		while(*pointer){
			if (withinString && *pointer == '"' && *(pointer+1) == '\n'){
				/*Reached end of line*/
				break;
			}

			if (withinString){
				/*Write current char to memory*/
				bin = convertToBinary(*pointer, wordSize);
				writeToDataArray(bin);
				incrementDataCounter();
				free(bin);
			}
		
			if (*pointer == '"')
				withinString = 1;
			pointer++;
		}
		/*End of string added to memory*/
		bin = convertToBinary('\0', wordSize);
		writeToDataArray(bin);
		incrementDataCounter();
		free(bin);
	}
	return 1;
}

/*Receives statement. If a label is declared and valid in the statement, adds label to the symbol table 
with the correct type (.data) and value (data counter). 
Then iterates through the statement in order to find all numbers after the .data instruction. 
For each number found, it is converted to binary and stored at the correct place in the instruction array 
after which the data counter (DC) is incremented.*/
int handleDataInstruction(char* statement){
	char currentNum[MAX_NUM_LENGTH+1]; /*the current number in the data*/
	char* bin; /*binary representation of the current number*/
	int i = 0; /*used to enter the current digit in data to currenNum*/
	char* pointer = strstr(statement,  ".data"); /*points to occurrence of .data in statement*/
	pointer += strlen(".data"); /*Increment pointer to after .data token*/

	/*This section checks for a label and if it finds one, checks its validity in order to insert into symbol table*/
	if (isPossibleLabelDeclaration(statement))
		handleLabelDeclaration(statement, DATATAG); /*Deal with label declaration*/
			

	checkDataInstructionSyntax(statement);
	
	/*This section iterates through the statement from the .data token and enters each number it finds into memory as binary*/
	if (outputStatus){
		while (*pointer){
			if (isdigit(*pointer) || *pointer == '+' || *pointer == '-'){
				currentNum[i] = *pointer;
				i++; /*only increment i when iterating through a number*/
			}
			if (*pointer == ','){ /*reached end of number in statement*/
				currentNum[i] = '\0';
				bin = convertToBinary(atoi(currentNum), wordSize);
				writeToDataArray(bin);
				incrementDataCounter();
				i = 0; /*reset i for next number*/
				free(bin);
			}

			if (!isdigit(*pointer) && !isspace(*pointer) && *pointer != '+' && *pointer != '-' && *pointer != ','){
				/*there is an invalid char within the data*/
				raiseInvalidCharInData();
			}
			pointer++;
		}
		/*Converts number at end of line*/
		bin = convertToBinary(atoi(currentNum), wordSize);
		writeToDataArray(bin);
		incrementDataCounter();
		free(bin);
	}
	return 1;
}


/*Receives a statement. Based on the current instruction type, calls
  function to deal with the current instruction.*/
int handleInstructionStatement(char* statement){
	Instruction_type currentInstructionType = getCurrentInstructionType();
	if (currentInstructionType == DATA)
		handleDataInstruction(statement);	
	if (currentInstructionType == STRING)
		handleStringInstruction(statement);
	if (currentInstructionType == EXTERN)
		handleExternInstruction(statement);
	if (currentInstructionType == ENTRY)
		handleEntryInstruction(statement);
	return 1;
}


/*Receives binary word pointer, the name of a register and isSource which 
  acts as a bool that indicates if the register is a source operand or not, as well
  as isOnlyOperand which acts as a bool to indicate if this register is the only
  register in this word.*/
void encodeRegisterOperand(char* bin, char* registerName, int isSource, int isOnlyOperand){
	int registerNumber = getRegisterNumber(registerName);
	fillRegisterWord(bin, registerNumber, isSource, isOnlyOperand, 0);	
}


/*Receives binary word pointer,and an operand that is an immediate number. converts the
  number from char* to integer and encodes it accordingly. */
void encodeImmediateNumber(char* bin, char* numberOperand){
	int number = convertOperandToInt(numberOperand);
	fillImmediateNumberWord(bin, number, 0);
}


/*Receives necassary information for encoding the following words of an operation with two operands. Checks
  the type of the operands and encodes them accordingly. After encoding, writes the binary representations
  of each word into the instruction array and increments IC.*/
void encodeTwoOperandFollowingWords(char* bin, char* sourceOperand, char* destinationOperand, Assignment_Type sourceType, 
Assignment_Type destType){

	if (sourceType == IMMEDIATE){
		encodeImmediateNumber(bin, sourceOperand);
		writeToInstructionArray(bin);
		incrementInstructionCounter();
		bin[0] = '\0'; /*reset bin*/
	}

	if (sourceType == DIRECT){
		/*operand is a label and its address is unknown during first pass
		  so only incrementing counter to make space in memory and writing the label
		  name in instruction array and references array*/
		writeToInstructionArray(sourceOperand);
		incrementInstructionCounter(); 
	}

	if (sourceType == DIRECT_REGISTER && destType == DIRECT_REGISTER){
		/*if both operands are registers, encode as a single word*/
		encodeRegisterOperand(bin, sourceOperand, 1, 0); /*encode source*/
		encodeRegisterOperand(bin, destinationOperand, 0, 0); /*encode destination*/
		writeToInstructionArray(bin);
		incrementInstructionCounter();
		return; /*encoded both operands*/
	}

	if (sourceType == DIRECT_REGISTER){
		encodeRegisterOperand(bin, sourceOperand, 1, 1);
		writeToInstructionArray(bin);
		incrementInstructionCounter();
		bin[0] = '\0'; /*reset bin*/

	}

	if (destType == IMMEDIATE){
		/*encode immediate number*/
		encodeImmediateNumber(bin, destinationOperand);
		writeToInstructionArray(bin);
		incrementInstructionCounter();
		bin[0] = '\0'; /*reset bin*/
	}

	if (destType == DIRECT){
		/*operand is a label and its address is unknown during first pass
		  so only incrementing counter to make space in memory and writing
		  label name in memory and writing to references array*/
		writeToInstructionArray(destinationOperand);
		incrementInstructionCounter();
	}

	if (destType == DIRECT_REGISTER){
		encodeRegisterOperand(bin, destinationOperand, 0, 1);
		writeToInstructionArray(bin);
		incrementInstructionCounter();
		bin[0] = '\0'; /*reset bin*/
	}
}


/*Receives necessary information to encode the following words of a jump statement. Checks 
  the types of the operands and encodes them accordingly, each time incrementing IC and adding them to the 
  instruction array.*/
void encodeJumpOperandFollowingWords(char* bin, char* labelName, char* sourceOperand, char* destinationOperand, Assignment_Type sourceType, 
	Assignment_Type destType){
	/*for label, only increment IC because address is unkown*/
	writeToInstructionArray(labelName);
	incrementInstructionCounter();

	/*can encode like a two operand command. Writing to memory and incrementing happens here as well*/
	encodeTwoOperandFollowingWords(bin, sourceOperand, destinationOperand, sourceType,destType);
}


/*Receives necassary information for encoding the following words of an operation with one operand (not jump). Checks
  the type of the operand and encodes it accordingly. After encoding, writes the binary representations
  of the word into the instruction array and increments IC.*/
void encodeOneOperandFollowingWords(char* bin, char* destinationOperand, Assignment_Type destType){
	if (destType == IMMEDIATE){
		/*encode immediate number*/
		encodeImmediateNumber(bin, destinationOperand);
		writeToInstructionArray(bin);
		incrementInstructionCounter();
		return;
	}
	if (destType == DIRECT){
		/*operand is a label and its address is unknown during first pass
		  so only incrementing counter to make space in memory and writing 
		  label name into instruction array and references array*/
		writeToInstructionArray(destinationOperand);
		incrementInstructionCounter();
		return;
	}
	if (destType == DIRECT_REGISTER){
		encodeRegisterOperand(bin, destinationOperand, 0, 1);
		writeToInstructionArray(bin);
		incrementInstructionCounter();
		return;
	}
}


/*Receives necessary information in order to properly encode each section of the first word of
  a two operand command.*/
void encodeFirstWordTwoOperandCommand(char* bin, Operation* currentOperation, Assignment_Type sourceType, Assignment_Type destType){
	fillBits13to10(bin, 0, 0, 0);
	fillBits9to6(bin, currentOperation->opCode);
	fillBits5to4(bin, sourceType, 1);
	fillBits3to2(bin, destType, 1);
	fillBits1to0(bin, 0);
}


/*Receives necessary information in order to properly encode each section of the first word of
  a non-jump one operand command.*/
void encodeFirstWordOneOPerandCommmand(char* bin, Operation* currentOperation, Assignment_Type destType){
	fillBits13to10(bin, 0, 0, 0);
	fillBits9to6(bin, currentOperation->opCode);
	fillBits5to4(bin, 0, 0);
	fillBits3to2(bin, destType, 1);
	fillBits1to0(bin, 0);
}


/**/
void encodeFirstWordJumpOperandCommand(char* bin, Operation* currentOperation, Assignment_Type sourceType, Assignment_Type destType){
	fillBits13to10(bin, 1, sourceType, destType);
	fillBits9to6(bin, currentOperation->opCode);
	fillBits5to4(bin, 0, 0);
	fillBits3to2(bin, JUMP, 1);
	fillBits1to0(bin, 0);

}


/*Receives necesarry information in order to properly the first word of a command with no operands.*/
void encodeFirstWordZeroOperandCommand(char* bin, Operation* currentOperation){
	fillBits13to10(bin, 0, 0, 0);
	fillBits9to6(bin, currentOperation->opCode);
	fillBits5to4(bin, 0, 0);
	fillBits3to2(bin, 0, 0);
	fillBits1to0(bin, 0);	
}


/*Receives operation, command section of a statement and the source and destination operands in the statement.
  Checks the validity of the operands and then encodes the information into binary and stores it into memory.*/
int handleTwoOperandCommand(Operation* currentOperation, char* sourceOperand, char* destinationOperand){
	char* bin = malloc((sizeof(char) * wordSize) + 1);


	Assignment_Type sourceType = getAssignmentType(sourceOperand, 1);
	Assignment_Type destType = getAssignmentType(destinationOperand, 1);

	isValidSourceOperand(currentOperation, sourceOperand);
	isValidDestinationOperand(currentOperation, destinationOperand);


	/*Fills bits of the first word of current command*/
	encodeFirstWordTwoOperandCommand(bin, currentOperation, sourceType, destType);


	writeToInstructionArray(bin);
	incrementInstructionCounter();
	bin[0] = '\0'; /*reset bin*/

	encodeTwoOperandFollowingWords(bin, sourceOperand, destinationOperand, sourceType, destType);
	free(bin);
	return 1;
}


/*Receives operation, command section of a statement and the destination operand in the statement.
  Checks the validity of the operand and then encodes the information into binary and stores it into memory.*/
int handleOneOperandCommand(Operation* currentOperation, char* destinationOperand){
	char* bin = malloc((sizeof(char) * wordSize) + 1); /*will hold binary representation of the word*/
	Assignment_Type destType = getAssignmentType(destinationOperand, 1);

	isValidDestinationOperand(currentOperation, destinationOperand);

	encodeFirstWordOneOPerandCommmand(bin, currentOperation, destType);
	

	writeToInstructionArray(bin);
	incrementInstructionCounter();

	bin[0] = '\0'; /*reset bin*/

	encodeOneOperandFollowingWords(bin, destinationOperand, destType);
	free(bin);
	
	return 1;
}


/*Receives operation, command section of a statement and the destination operand (jump operand) in the statement.
  Checks the validity of the operand and then encodes the information into binary and stores it into memory.*/
int handleJumpOperandCommand(Operation* currentOperation, char* jumpOperand){
	char* bin = malloc((sizeof(char) * wordSize) + 1); /*will hold binary representation of the word*/
	char* jumpLabel;
	char* sourceOperand;
	char* destinationOperand;
	Assignment_Type sourceType;
	Assignment_Type destType;


	isValidDestinationOperand(currentOperation, jumpOperand);
	
	jumpLabel = getJumpLabel(jumpOperand);
	sourceOperand = getJumpSourceOperand(jumpOperand);
	destinationOperand = getJumpDestinationOperand(jumpOperand);
	sourceType = getAssignmentType(sourceOperand, 1);
	destType = getAssignmentType(destinationOperand, 1);
	
	encodeFirstWordJumpOperandCommand(bin, currentOperation, sourceType, destType);
	writeToInstructionArray(bin);
	incrementInstructionCounter();
	bin[0] = '\0'; /*reset bin*/

	encodeJumpOperandFollowingWords(bin, jumpLabel, sourceOperand, destinationOperand, sourceType, destType);
	free(bin);
	return 1;
}


/*Receives operation and command section of the statement. Encodes information into memory.*/
int handleZeroOperandCommand(Operation* currentOperation){
	char* bin = malloc((sizeof(char) * wordSize) + 1); /*will hold binary representation of the word*/

	if (outputStatus){
		encodeFirstWordZeroOperandCommand(bin, currentOperation);
		writeToInstructionArray(bin);
		incrementInstructionCounter();
	}
	free(bin);
	return 1;
}

/*Receives a command statement and, if there is a label declaration, adds it to the symbol table. Then, 
  checks the sntax of the command and gets the operands of the command. Calls appropriate encoding functions.*/
int handleCommandStatement(char* statement){
	Operation* currentOperation;
	char* command; /*will hold section of code that has op name and operands*/
	char* sourceOperand;
	char* destinationOperand;
	char* jumpOperand;

	/*This section checks for a label and if it finds one, checks its validity in order to insert into symbol table*/
	if (isPossibleLabelDeclaration(statement)){
		handleLabelDeclaration(statement, CODETAG);
		command = strchr(statement, ':') + 1; /*increments pointer to after label declaration*/
	}
	
	currentOperation = getOperation(statement);
	command = strstr(statement, currentOperation->opName) + strlen(currentOperation->opName); /*increments pointer to after op name*/
	
	checkCommandSyntax(statement, command, currentOperation);

	if (currentOperation->numberOfOperands == 2){
		sourceOperand = getSingleOperand(command);
		destinationOperand = getSecondOperand(command);
		handleTwoOperandCommand(currentOperation, sourceOperand, destinationOperand);
		free(sourceOperand);
		free(destinationOperand);
	}

	if (currentOperation->numberOfOperands == 1 && !isPossibleJumpOperand(statement)){
		/*Should only have a destination operand*/
		sourceOperand = getSingleOperand(command);
		handleOneOperandCommand(currentOperation, sourceOperand);
		free(sourceOperand);
	}

	if (currentOperation->numberOfOperands == 1 && isPossibleJumpOperand(statement)){
		jumpOperand = getJumpOperand(command);
		handleJumpOperandCommand(currentOperation, jumpOperand);
		free(jumpOperand);
	}

	if (currentOperation->numberOfOperands == 0){
		handleZeroOperandCommand(currentOperation);
	}

	return 1;
}


/*Receives the statement type and calls appropriate function to handle the statement, returns 1 if statement is handled successfully, 0 otherwise.*/
int handleStatement(Statement_type type, char* statement){
	if (type == EMPTY || type == COMMENT)
		return 1;
	if (type == INSTRUCTION){
		handleInstructionStatement(statement);
		return 1;
	}
	if (type == COMMAND){
		handleCommandStatement(statement);
		return 1;
	}
	return 0;
}


/*Carries out first pass of the assembler on the source code*/
int firstPass(char* fileName){
	FILE* sourceFile;
	Statement_type statementType;
	char statement[MAX_STATEMENT_LENGTH+1];
	char* sourcePath = malloc((strlen(fileName)  + strlen(POST_PREPROCESSOR_FILETYPE)) * sizeof(char) + 1);
	sprintf(sourcePath, "%s%s", fileName, POST_PREPROCESSOR_FILETYPE);
	sourceFile = fopen(sourcePath, "r");

	initIC();
	initDC();
	outputStatus = 1;
	outputEntries = 0;
	outputExterns = 0;
	lineNumber = 1;
	currentFileName = fileName;

	while (fgets(statement, MAX_STATEMENT_LENGTH, sourceFile) != NULL){
		trimWhitespace(statement);
		statementType = getStatementType(statement);

		if (statementType == EMPTY || statementType == COMMENT){
			/*assembler skips comments and empty lines*/
			lineNumber++;
			continue;
		}
		
		if (statementType == COMMAND)
			handleCommandStatement(statement);

		if (statementType == INSTRUCTION)
			handleInstructionStatement(statement);
			
		if (statementType == UNIDENTIFIED)
			raiseUnidentifiedStatement();

		lineNumber++;
	}
	free(sourcePath);
	fclose(sourceFile);
	return 1;
}


/*Carries out second pass of the assembler on the source code. If the program should output (no errors)
  then output files are created*/
int secondPass(char* fileName){

	addICToDataValues(); /*increment all data label values by IC*/

	if (outputExterns)
		/*this is done before encoding labels because after encoding, the label names will no longer
		  appear in the instruction array. Also, error can occur during writing to extern file.*/
		writeToExternsFile(fileName);

	encodeLabelsSecondPass(); /*encodes addresses of labels in memory*/

	writeMemoryToObjectsFile(fileName);

	if (outputEntries)
		writeToEntriesFile(fileName);

	if (outputStatus){
		printf("\nProgram complete: You can find the output files for %s in the directory.\n", fileName);
	}
	else {
		printf("\nNo output files created because of error/s in the source code in %s.as.\n", fileName);
		deleteOutputFiles(fileName); /*deleting output files*/
	}
	return 1;
}