all: Captura

Captura: http.o libHTTP.so Basedades.o Funciones.o 
	gcc Basedades.o Funciones.o -o Captura -lsqlite3 -L. -lHTTP -lwiringPi
	LD_LIBRARY_PATH=. ./Captura -i 172.28.16.16 -n config.txt -r /home/pi/Desktop/ADSTR_2019/src/captura/captura.db

main.o: Basedades.c
	gcc -c Basedades.c

Funciones.o: Funciones.c
	gcc -c Funciones.c 

libHTTP.so: http.o
	gcc -shared -fPIC -o libHTTP.so http.o

http.o: http.c
	gcc -fPIC -c -o http.o  http.c

clean:
	rm -rf *o *gch Captura

