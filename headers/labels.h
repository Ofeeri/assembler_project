#define EXTERN_DEFAULT_VALUE 0
#define ENTRY_DEFAULT_VALUE -2

typedef enum {ABSOLUTE, EXTERNAL, RELOCATABLE} Encoding_Type;
typedef enum {CODETAG, DATATAG} Label_Tag;

typedef struct Label{
    char* name; /*label name*/
    int value; /*current IC or DC value*/
    Label_Tag tag; /*code or data*/
    Encoding_Type type; /*relocatable or external*/
} Label;


int handleLabelDeclaration(char* statement, Label_Tag tag);
int isValidLabelName(char* labelName);
char* getLabelName(char* statement);
int isPossibleLabelDeclaration(char* statement);
int isValidLabelNameNoError(char* labelName);
void initSymbolTable();
void enterSymbol(char* name, int value, Label_Tag tag, Encoding_Type type);
Label* getSymbol(char* name);
Label* getSymbolByValue(int value);
void addICToDataValues();
void freeSymbolTable();
