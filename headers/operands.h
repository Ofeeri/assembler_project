typedef enum {IMMEDIATE, DIRECT, JUMP, DIRECT_REGISTER, NO_TYPE} Assignment_Type;

int isPossibleJumpOperand(char* statement);
Assignment_Type getAssignmentType(char* operand, int firstJumpCheck);
int convertOperandToInt(char* operand);
char* getJumpLabel(char* operand);
char* getJumpSourceOperand(char* operand);
char* getJumpDestinationOperand(char* operand);
char* getSingleOperand(char* pointer);
char* getSecondOperand(char* pointer);
char* getJumpOperand(char* pointer);
