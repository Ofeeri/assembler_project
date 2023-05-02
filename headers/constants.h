#define MAX_STATEMENT_LENGTH 80
#define wordSize 14 /*word refers to size of a cell of memory, each cell contains 14 bits*/
#define MEMORY_SIZE 256
#define MEMORY_START 100
#define ADDRESS_LENGTH 4
#define NUMBER_OF_OPERATIONS 16
#define NUMBER_OF_REGISTERS 8
#define MAX_INSTRUCTION_LENGTH 7 /*Max size of .data, .string, .entry, and .extern*/
#define NUMBER_OF_INSTRUCTIONS 4
#define BIN_ZERO '.'
#define BIN_ONE '/'
#define MAX_LABEL_LENGTH 30
#define MAX_NUM_LENGTH 6 /*Because largest number with 14 digits in binary is 16,383*/
#define COMMENT_ID ';'
#define MAX_ASSIGNMENT_TYPES 4
#define INITIAL_TABLE_SIZE 10
#define DATA_LENGTH 8

/*file endings*/
#define SOURCE_FILETYPE ".as"
#define POST_PREPROCESSOR_FILETYPE ".am"
#define OBJECT_FILETYPE  ".ob"
#define ENTRIES_FILETYPE ".ent"
#define EXTERNALS_FILETYPE ".ext"

/*Macro declarations*/
#define MACRO_ID "mcr"
#define END_MACRO_ID "endmcr"
