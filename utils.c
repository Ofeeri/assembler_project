#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "headers/constants.h"


/*Description: this file contains utility functions that are used throughout the assembler. Mostly contains 
  functions that convert data to binary and encode data to binary.*/


char* convertToBinary(unsigned int operand, int length);
void fillBits1to0(char* word,  int encodingType);


/*Receives a word that is supposed to be written to memory and fills bits 13-10 of the word accordingly.
  isJump is used as a bool to indicate whether the word refers to a jump operand or not.*/
void fillBits13to10(char* word, int isJump, int sourceType, int destType){
	int i;
	if (!isJump){
		/*Fills bits 13-10 with zeros because they are only relevant to jump operation*/
		for (i = 0; i < 4; i++){
			word[i] = BIN_ZERO;
		}
	}
	else{
		/*fill bits 13-10 for jump operation*/
		char* sourceBin = convertToBinary(sourceType, 2);
		char* destBin = convertToBinary(destType, 2);

		/*source operand*/
		word[0] = sourceBin[0];
		word[1] = sourceBin[1];
		/*destination operand*/
		word[2] = destBin[0];
		word[3] = destBin[1];

		free(sourceBin);
		free(destBin);
	}
	
}


/*Receives a word that is supposed to be written to memory and fills bits 9-6 (opcode) of the word accordingly.*/
void fillBits9to6(char* word, int opCode){
	char* opCodeBin = convertToBinary(opCode, 4); /*4 because this is used to fill 4 bits*/

	/*Copies the encoding of the opcode to bits 9-6 of the word*/
	word[4] = opCodeBin[0];
	word[5] = opCodeBin[1];
	word[6] = opCodeBin[2];
	word[7] = opCodeBin[3];

	free(opCodeBin);
}


/*Receives a word that is supposed to be written to memory and fills bits 5-4 (source operand type)
  of the word accordingly. hasSource acts as a bool to indicate if the operation of the word has a source operand.*/
void fillBits5to4(char* word, int assignmentType, int hasSource){

	if (!hasSource){
		/*there is no source operand so fill with zeros*/
		word[8] = BIN_ZERO;
		word[9] =  BIN_ZERO;
	}
	else{
		char* sourceBin = convertToBinary(assignmentType, 2); /*2 because this is used to fill 2 bits*/
		word[8] = sourceBin[0];
		word[9] =  sourceBin[1];
		free(sourceBin);
	}
}

/*Receives a word that is supposed to be written to memory and fills bits 3-2 (destination operand type)
  of the word accordingly. hasDest acts as a bool to indicate if the operation of the word has a destination operand.*/
void fillBits3to2(char* word, int assignmentType, int hasDest){
	if (!hasDest){
		/*there is no destination operand so fill with zeros*/
		word[10] = BIN_ZERO;
		word[11] = BIN_ZERO;
	}
	else{
		char* destBin = convertToBinary(assignmentType, 2); /*2 because this is used to fill 2 bits*/
		word[10] = destBin[0];
		word[11] = destBin[1];
		free(destBin);
	}
}


/*Receives a word that is supposed to be written to memory and fills bits 1-0 (encoding type)
  with the given encoding type.*/
void fillBits1to0(char* word,  int encodingType){
	/*TODO: wait for answer in forum to know how to handle labels in the word*/

	if (encodingType == 0){
		/*Absolute encoding*/
		word[12] = BIN_ZERO;
		word[13] = BIN_ZERO;
	}

	if (encodingType == 1){
		/*External encoding*/
		word[12] = BIN_ZERO;
		word[13] = BIN_ONE;
	}

	if (encodingType == 2){
		/*Relocatable encoding*/
		word[12] = BIN_ONE;
		word[13] = BIN_ZERO;
	}
}


/*Receives a register operand and encodes it accordingly. isSource acts as bool 
  to indicate if the register is a source operand or not. isOnlyOperand acts as bool
  to indicate if the register is alone on the word or shares it with another register.*/
void fillRegisterWord(char* word, int registerNum, int isSource, int isOnlyOperand, int ARE){
	int i;
	char* registerBin = convertToBinary(registerNum, 6); /*6 is the length of bits 13-8*/
	if (isSource){
		/*fill bits 13 to 8*/
		for (i=0; i < 6; i++){
			word[i] = registerBin[i];
		}
		if (isOnlyOperand){
			/*only fiils with zeros if this register is the only operand in this word*/
			for (i=6; i < 12; i++){
				word[i] = BIN_ZERO;
			}
		}
	}

	else{
		if (isOnlyOperand){
			/*only fiils with zeros if this register is the only operand in this word*/
			for (i=0; i < 6; i++){
				word[i] = BIN_ZERO;
			}
		}
		/*fill rest with zeros*/
		for (i=6; i < 12; i++){
			word[i] = registerBin[i-strlen(registerBin)];
		}
	}
	fillBits1to0(word, ARE);
	free(registerBin);	
}


/*Receives a number and encodes it accordingly.*/
void fillImmediateNumberWord(char* word, int number, int ARE){
	int i;
	char* numberBin = convertToBinary(number, 12); /*fills first 12 bits of word with number*/

	for (i=0; i < 12; i++){
		word[i] = numberBin[i];
	}
	fillBits1to0(word, ARE);
	free(numberBin);
}


/*Receives label's address and its encoding type and encodes it accordingly*/
void encodeLabelAddress(char* word, int labelAddress, int encodingType){
	int i;
	char* addressBin = convertToBinary(labelAddress, 12);
	for (i=0; i < 12; i++){
		word[i] = addressBin[i];
	}
	fillBits1to0(word, encodingType);
	free(addressBin);
}


/*This function receives an operand that can be an int or a char and the desired length of the binary output. It then
  converts the given operand into its binary representation (using the unique binary language), returning it as a string.*/
char* convertToBinary(unsigned int operand, int length){
	int c;
	int i = 0;
	char* bin = malloc(sizeof(char) * length + 1);

	while (i < length){
		c = operand % 2;
		if (c == 0)
			bin[i] = BIN_ZERO;
		else
			bin[i] = BIN_ONE;

		operand = operand / 2;
		i++;
	}
	bin[i] = '\0';
	bin = strrev(bin); /*Reverse bin so binary goes from right to left*/
    return bin;
}


/*Receives filename and deletes all output files that were created during the assembly process.*/
void deleteOutputFiles(char* filename){
	char* filepath = malloc((strlen(filename) * sizeof(char)) + strlen(EXTERNALS_FILETYPE));
	sprintf(filepath, "%s%s", filename, OBJECT_FILETYPE);
	remove(filepath); /*remove.ob file*/
	sprintf(filepath, "%s%s", filename, EXTERNALS_FILETYPE);
	remove(filepath); /*remove.ext file*/
	sprintf(filepath, "%s%s",filename, ENTRIES_FILETYPE);
	remove(filepath); /*remove.ent file*/

	free(filepath);
}