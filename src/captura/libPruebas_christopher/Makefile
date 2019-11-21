all: Captura

Captura: Basedades.o Funciones.o
	gcc Basedades.o Funciones.o -o Captura -lsqlite3

main.o: Basedades.c
	gcc -c Basedades.c

factorial.o: Funciones.c
	gcc -c Funciones.c 

clean:
	rm -rf *o *db *gch Captura

