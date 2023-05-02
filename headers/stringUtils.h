char** splitLineByWhitespace(char* str);
void trimWhitespace(char* inputStr);
int checkForExtraCommas(char* statement, char* token);
int checkForNoCommas(char* pointer);
int checkForStrayString(char* statement, char* token);
void checkForValidString(char* string);
void checkForExtraOperands(char* statement, char* str);
void checkTooManyParentheses(char* str);
