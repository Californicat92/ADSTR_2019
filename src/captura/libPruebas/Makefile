all: Appcaptura

Appcaptura:
	gcc Basedades.c -o Basedades -lsqlite3
	./Basedades

libreria: FuncADSTR.o libFuncADSTR.so
	gcc -c -o Basedades.o Basedades.c -I.
	gcc -o Basedades Basedades.o -L. -llibFuncADSTR.so

FuncADSTR.o: FuncADSTR.c
	gcc -fPIC -c -o FuncADSTR.o  FuncADSTR.c

libFuncADSTR.so: FuncADSTR.o
	gcc -shared -fPIC -o libFuncADSTR.so FuncADSTR.o
	
clean:
	rm -rf *o *db Basedades -r doc

