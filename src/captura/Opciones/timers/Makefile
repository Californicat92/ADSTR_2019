all: Captura //Fita 2

exec:
	LD_LIBRARY_PATH=. ./Captura -i 192.168.11.175 -n config.txt -r /home/fran/Escritorio/ADSTR_2019/src/captura/Opciones/timers/captura.db

Captura: http.o libHTTP.so Basedades_version1.o Funciones_con_timer.o 
	gcc Basedades_version1.o Funciones_con_timer.o -lrt -lpthread -o Captura -lsqlite3 -L. -lHTTP
	LD_LIBRARY_PATH=. ./Captura -i 192.168.11.175 -n config.txt -r /home/fran/Escritorio/ADSTR_2019/src/captura/Opciones/timers/captura.db

main.o: Basedades_version1.c
	gcc -c Basedades_version1.c

Funciones.o: Funciones_con_timer.c
	gcc -c Funciones_con_timer.c 

libHTTP.so: http.o
	gcc -shared -fPIC -o libHTTP.so http.o

http.o: http.c
	gcc -fPIC -c -o http.o  http.c

clean:
	rm -rf *o *gch Captura

