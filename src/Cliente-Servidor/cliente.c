/***************************************************************************
                          main.c  -  client
                             -------------------
    begin                : lun feb  4 16:00:04 CET 2002
    copyright            : (C) 2002 by A. Moreno
    email                : amoreno@euss.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif



#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <time.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

#define REQUEST_MSG_SIZE	1024
#define REPLY_MSG_SIZE		500
#define SERVER_PORT_NUM		5001

void print_usage(); // Func. opciones ayuda GETOPT
void E_R_Datos(char missatge[200],char buffer[200]);
void ImprimirMenu(void);
int callback(void *data, int argc, char **argv, char **azColName);
int openDB(char * name, sqlite3** db); //Crear o obrir la base de dades


 /************************
*
*
* tcpClient
*
*
*/


int main(int argc, char *argv[]){
	
	//Lectura por comandos de IP del servidor para consultas HTTP y ruta y nombre de la base de datos
	int opt= 0;
	char *ip_servidor="127.0.0.1",*ruta_bbdd=".";
	static struct option long_options[] = {
        {"ip_servidor",		required_argument,	0,	'i'},
        {"ruta_bbdd",		required_argument,	0,	'r'},
        {0,									0,	0,	0}
    };
    int long_index =0;	
    while ((opt = getopt_long(argc, argv,"i:r:", 
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'i' : ip_servidor = optarg;
                 break;
             default: print_usage(); 
                 exit(EXIT_FAILURE);
        }
    }

  	sqlite3 *db;
    //Variables consulta sql
	char data[200];
	char sql[200];
	int rc;
	char *zErrMsg = 0;	
 	
 	openDB(".", &db);
	
	// Comprobacion si hay tablas ya creadas en la base de datos
	memset(sql, '\0', sizeof(sql));
	sprintf(sql, "SELECT * FROM Sensors_table");

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);

	// Si no hay tablas, se crean
	if (rc != SQLITE_OK) {
		
		//Creamos Tabla de Lecturas
		memset(sql, '\0', sizeof(sql));
		sprintf(sql,	"CREATE TABLE Lectures_table(" \
						"ID_row		INTEGER PRIMARY KEY AUTOINCREMENT,"\
						"ID						INTEGER 	NOT NULL," \
						"Date_time_lecture		DATE    	NOT NULL," \
						"Value					INT   		NOT NULL);");
		rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
		
		//Creamos Tabla de Sensores
		memset(sql, '\0', sizeof(sql));
		sprintf(sql,	"CREATE TABLE Sensors_table("  \
						"ID						INTEGER 	NOT NULL," \
						"Types					CHAR  		NOT NULL," \
						"Description			CHAR  		NOT NULL);");
		rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
		
		//Creamos Tabla de Alarmas
		memset(sql, '\0', sizeof(sql));
		sprintf(sql,	"CREATE TABLE Alarms_table("  \
						"Date_time_alarm		DATE    NOT NULL," \
						"Alarm_description		CHAR    NOT NULL);");
		rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
    
    printf("La IP del servidor seleccionada es: %s\n",ip_servidor);
	char input,enviat[200],rebut[200];

	ImprimirMenu();
	scanf("%s", &input);

	while (input != 's')
	{
		memset (enviat,'\0',200);
		memset (rebut,'\0',200);	
		switch (input)
		{
			// declaración de variables para bucles dentro del switch (escribir cadenas)
			int i;
			//Limpiamos el espacio de memoria de enviat para que no se cuelen caracteres extraños	

			case '1':
				printf("Heu seleccionat l'opció 1\n");	
				sprintf(enviat,"{A_2020/01/01_12:00:00_30_1}"); //cargem a la variable a enviar les dades
				E_R_Datos(enviat, rebut);   
				printf("\nS'ha rebut el codi d'error %s\n",rebut);
				break;           				
			case '2':
				break;           			
			case '3':
				break;           			
			case '4':
				break;           			
			case '5':
				break;           			
			case '6':
				break;           			
			case 0x0a: //Això és per enviar els 0x0a (line feed) que s'envia quan li donem al Enter
				break;
			default:
				printf("Opció incorrecta\n");	
				printf("He llegit %c \n",input);
				break;
			}
		scanf("%s", &input);
		}

	
	printf("Heu seleccionat la opció sortida\n");
	return 0;
}
}


void ImprimirMenu(void)
{
	printf("\n\nMenu:\n");
	printf("-------------------------------------------------------\n");
	printf("1.-ENVIAR TRAMA 1 CONSULTA SENSORS\n");
	printf("2.-Demanar última mitjana (promig del n-mostres anteriors)\n");
	printf("3.-Demanar màxim\n");
	printf("4.-Demanar mínim\n");
	printf("5.-Reset màxim i mínim\n");
	printf("6.-Demanar comptador\n");
	printf("Premer 's' per sortir \n");
	printf("-------------------------------------------------------\n");
}


void E_R_Datos(char missatge[200],char buffer[200]){

	struct sockaddr_in	serverAddr;
	char	    serverName[] = "127.0.0.1"; //Adreça IP on est� el servidor
	int			sockAddrSize;
	int			sFd;
	int 		result;


	/*Crear el socket*/
	sFd=socket(AF_INET,SOCK_STREAM,0);

	/*Construir l'adreça*/
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero ((char *)&serverAddr, sockAddrSize); //Posar l'estructura a zero
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons (SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = inet_addr(serverName);

	/*Conexió*/
	result = connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
	if (result < 0)
	{
		printf("Error en establir la connexió\n");
		exit(-1);
	}
	//printf("\nConnexió establerta amb el servidor: adreça %s, port %d\n",	inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

	/*Enviar*/
	strcpy(buffer,missatge); //Copiar missatge a buffer
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n",	result, missatge);
	sleep(2);
	/*Rebre*/
	result = read(sFd, buffer, 200);
	printf("Missatge rebut del servidor(bytes %d): %s\n",	result, buffer);
	
	/*Tancar el socket*/
	close(sFd);
}
// -----------------------------GETOPT HELP-------------------------------------
void print_usage() {
    printf(	"\n/---------------------HELP-------------------------------\n"\
			"Uso:	Introducir -i seguido de la IP del servidor \n" \
			"\tIntroducir -n seguido del nombre del archivo config\n"\
			"\tIntroducir -r seguido de la ruta y nombre.db de la base de "\
			"datos\n"\
			"/-----------------------------------------------------------\n\n");
}
// -----------------------------------------------------------------------------
//Crear o obrir la base de dades.
int openDB(char * name, sqlite3** db){
	int rc;
	
	/* Open database */
	rc = sqlite3_open(name, db);
	if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
		return 1;
		
	} else {
		fprintf(stdout, "Opened database successfully\n");
	}
	return 0;
}

int callback(void *data, int argc, char **argv, char **azColName){
	int i;
	
	memset((char *)data, '\0', sizeof(data));
		
	for(i = 0; i<argc; i++) {
		sprintf(data, "%s", argv[i] ? argv[i] : "NULL");
		//printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
	printf("\n");
	return 0;
}
