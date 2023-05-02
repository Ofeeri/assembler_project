int getLineNumber();
int changeOutputStatus();
char* getFileName();
void printUndeclaredLabelReferences(char* filename, char* labelname);

int firstPass(char* fileName);
int secondPass(char* filename);