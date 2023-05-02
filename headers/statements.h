
typedef enum {
    EMPTY, COMMENT, INSTRUCTION, COMMAND, UNIDENTIFIED

} Statement_type;

typedef enum {
    DATA, STRING, ENTRY, EXTERN, NONE

} Instruction_type;



Instruction_type getCurrentInstructionType();
char* getInstruction(char* statement);
int isPossibleInstructionstatement(char* statement);
Statement_type getStatementType(char* statement);
int checkDataInstructionSyntax(char* statement);
void checkStringInstructionSyntax(char* statement);
void checkCommandSyntax(char* statement, char* pointer, void* currentOperation);
void checkJumpOperandSyntax(char* statement, char* pointer); /*DELETE*/