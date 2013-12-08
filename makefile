compile: main.o lexical.o syntax.o symbol.o gencode.o interpretrun.o
	gcc -o compile main.o lexical.o syntax.o symbol.o gencode.o interpretrun.o
main.o:main.c
	gcc -c main.c -o main.o
lexical.o:lexical.c
	gcc -c lexical.c -o lexical.o
syntax.o:syntax.c
	gcc -c syntax.c -o syntax.o
symbol.o:symbol.c
	gcc -c symbol.c -o symbol.o
gencode.o:gencode.c
	gcc -c gencode.c -o gencode.o
interpretrun.o:interpretrun.c
	gcc -c interpretrun.c -o interpretrun.o

clean:
	rm -f compile *.o
