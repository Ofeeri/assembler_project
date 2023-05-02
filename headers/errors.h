void raiseFileNotFound(char* filename);
void raiseExtraMacroTokens(int endMacro);
void raiseInvalidMacroName(char* str, int operationName);
void raiseInvalidLabelSyntax(char* str);
void raiseLabelIsOpName(char* str);
void raiseLabelIsRegisterName(char* str);
void raiseLabelAlreadyExists(char* str);
void raiseCommaAtStart();
void raiseConsecutiveCommas();
void raiseCommaAtEnd();
void raiseNoCommasBetween();
void raiseInvalidCharInData();
void raiseStrayTokenError();
void raiseNoQuotesError();
void raiseTooManyParams();
void raiseTooFewParams();
void raiseNoSpaceAfterOp();
void raiseMissingOperand();
void raiseTooManyOperands();
void raiseInvalidSourceType();
void raiseInvalidDestinationType();
void raiseSpaceInJumpOperand();
void raiseMissingParenthesesInJumpOperand();
void raiseUndeclaredLabelReference(char* labelName);
void raiseTooManyParentheses();
void raiseInvalidEntryLabel(char* labelName);
void raiseDataOverFlow();
void raiseUnidentifiedStatement();