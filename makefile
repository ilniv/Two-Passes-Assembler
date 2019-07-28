assembler : Exceptions.o IO.o Labels.o main.o Parser.o Instructions.o
	gcc -Wall -ansi -pedantic -g Exceptions.o IO.o Labels.o main.o Parser.o Instructions.o -o assembler

Exceptions.o : Exceptions.c Exceptions.h
	gcc -Wall -ansi -pedantic -g -c Exceptions.c

IO.o : IO.c IO.h
	gcc -Wall -ansi -pedantic -g -c IO.c

Labels.o : Labels.c Labels.h
	gcc -Wall -ansi -pedantic -g -c Labels.c

Parser.o : Parser.c Parser.h
	gcc -Wall -ansi -pedantic -g -c Parser.c

Instructions.o : Instructions.c Instructions.h
	gcc -Wall -ansi -pedantic -g -c Instructions.c

main.o : main.c Exceptions.h Labels.h IO.h Parser.h Instructions.h Object.h
	gcc -Wall -ansi -pedantic -g -c main.c
