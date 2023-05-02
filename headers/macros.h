void initMacroTable();
void enterMacro(char* name, char* contents);
char* getMacroContents(char* name);
int isValidMacroName(char* str);
void freeMacroTable();