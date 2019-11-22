all: Captura

Captura: http.o libHTTP.so Basedades.o Funciones.o 
	gcc Basedades.o Funciones.o -o Captura -lsqlite3 -L. -lHTTP
	LD_LIBRARY_PATH=. ./Captura -i 127.0.0.1 -n config.txt -r /home/cali/Desktop/ADSTR_2019/src/captura/captura.db

main.o: Basedades.c
	gcc -c Basedades.c

Funciones.o: Funciones.c
	gcc -c Funciones.c 

libHTTP.so: http.o
	gcc -shared -fPIC -o libHTTP.so http.o

http.o: http.c
	gcc -fPIC -c -o http.o  http.c

clean:
	rm -rf *o *db *gch Captura

