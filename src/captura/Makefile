all: Appcaptura

Appcaptura:
	gcc Basedades.c -o Basedades -lsqlite3
	./Basedades

libreria: mainXungo.o http.o libHTTP.so
	gcc -o mainXungo mainXungo.o -L/home/pi/Desktop/ADSTR_2019/src/captura/ -lHTTP

Basedades.o: mainXungo.c
	gcc -c -o mainXungo.o mainXungo.c -I/home/pi/Desktop/ADSTR_2019/src/captura/

http.o: http.c
	gcc -fPIC -c -o http.o  http.c

libHTTP.so: http.o
	gcc -shared -fPIC -o libHTTP.so http.o

exec: libreria
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/pi/Desktop/ADSTR_2019/captura/ ./mainXungo

clean:
	rm -rf *o *db Basedades -r doc

