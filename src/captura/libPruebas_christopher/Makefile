all: prova

prova: Basedades.o Funciones.o
	gcc Basedades.o Funciones.o -o prova -lsqlite3

main.o: Basedades.c
	gcc -c Basedades.c

factorial.o: Funciones.c
	gcc -c Funciones.c 

clean:
	rm -rf *o *db prova

