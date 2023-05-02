# assembler_project
Project that simulates the operation of an assembler on a fictional assembly language.

Built as a project in a C coding course in university.

Operation of program:

operated from the command line.
receives file names that are supposed to contain fake assembly code and, according to various rules given by the professors who assigned the project, converts the assembly code into binary representation that is seen in according output files.
does not assume input is valid and there are extensive error checks and handling.
like a real compiler/assembler, program does not stop after finding error but rather raises all errors and there location in the input file
Stages of assembler: 1: pre processing stage- receives input file with assembly code and finds all declarations of macros, replacing each instance of the macro in the assembly code with the code in the macro declaration. 2: first pass: iterates through assembly code line by line and converts each operation, variable, and operand into binary representation based on the requirements of the assignment. 3: iterates through assembly code a second time in order to properly encode the "address" of variables that were declared in the assembly code. (this is because during the first pass there is no way of knowing if a variable that is referenced has been declared later on in the code). This stage also creates appropriate output files if no errors were found during run time.
