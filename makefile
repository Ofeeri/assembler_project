main: main.o assembler.o preProcessor.o stringUtils.o memory.o errors.o operations.o utils.o statements.o labels.o operands.o macros.o
	gcc -ansi -Wall -pedantic -o main main.o assembler.o preProcessor.o stringUtils.o memory.o errors.o operations.o utils.o statements.o labels.o operands.o macros.o

assembler.o: assembler.c
	gcc -ansi -Wall -pedantic -c assembler.c

preProcessor.o: preProcessor.c
	gcc -ansi -Wall -pedantic -c preProcessor.c

stringUtils.o: stringUtils.c
	gcc -ansi -Wall -pedantic -c stringUtils.c

memory.o: memory.c
	gcc -ansi -Wall -pedantic -c memory.c

errors.o: errors.c
	gcc -ansi -Wall -pedantic -c errors.c

operations.o: operations.c
	gcc -ansi -Wall -pedantic -c operations.c

utils.o : utils.c
	gcc -ansi -Wall -pedantic -c utils.c

statements.o: statements.c
	gcc -ansi -Wall -pedantic -c statements.c

labels.o: labels.c
	gcc -ansi -Wall -pedantic -c labels.c

operands.o: operands.c
	gcc -ansi -Wall -pedantic -c operands.c

macros.o : macros.c
	gcc -ansi -Wall -pedantic -c macros.c