all: Captura //Fita 2

exec:
	LD_LIBRARY_PATH=. ./Captura -i 192.168.11.175 -n config.txt -r /home/pi/Desktop/fita3/ADSTR_2019/src/captura/Fita2_grupB/captura.db

Captura: http.o libHTTP.so Basedades.o Funciones.o 
	gcc Basedades.o Funciones.o -o Captura -lsqlite3 -L. -lHTTP -lwiringPi
	LD_LIBRARY_PATH=. ./Captura -i 192.168.1.71 -n config.txt -r /home/pi/Desktop/fita3/ADSTR_2019/src/captura/Fita2_grupB/captura.db

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

