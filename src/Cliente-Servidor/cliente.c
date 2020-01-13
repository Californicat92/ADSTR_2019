//Compilar gcc cliente.c -o cliente -lsqlite3
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
int insert_Sensors_table(sqlite3* db,int ID, char* Tipo_sensor,char* Unitat_de_mesura, char* Descripcio_sensor, char* Localitzacio_sensor,int intervalo, char* IP);
int insert_Resum_table(sqlite3* db,int ID2, char* Data_inici,int Increment_de_temps, float Maxim,char* Data_maxim,float Minim,char* Data_minim,float Mitjana);
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
	char *ip_servidor="127.0.0.1",*ruta_bbdd="bbdd";
	static struct option long_options[] = {
        {"ip_servidor",		required_argument,	0,	'i'},
        {"ruta_bbdd",		required_argument,	0,	'r'},
        {0,									0,	0,	0}
    };
    int long_index =0;	
    while ((opt = getopt_long(argc, argv,"i:r:", 
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'i' : 
				ip_servidor = optarg;
				printf("La IP del servidor seleccionada es: %s\n",ip_servidor);
				break;
             case 'r' : 
				ruta_bbdd = optarg;
				printf("La ruta de la base de dades seleccionada es: %s\n",ruta_bbdd);
				break;
             default: print_usage(); 
                 exit(EXIT_FAILURE);
        }
    }
    //Variables comunicación 
   	char input,enviat[200],rebut[200];
	//Variables sensores
	int ID=0,intervalo=0;
	char Tipo_sensor[50],Unitat_de_mesura[10],Descripcio_sensor[100],Localitzacio_sensor[50],IP[15];
  	sqlite3 *db;
    //Variables consulta sql
	char data[400];
	char sql[400];
	int rc;
	char *zErrMsg = 0;	
 	int max_sensores=0;
 	char data_inici[200];
 	openDB(ruta_bbdd, &db);
	const char 	s[2] = "#";
	char *Data_inici,*Data_Maxim,*Data_Minim;
	int ID2, Increment_de_temps;
	float Maxim,Minim,Mitjana;
	// Comprobacion si hay tablas ya creadas en la base de datos
	memset(sql, '\0', sizeof(sql));
	sprintf(sql, "SELECT * FROM Sensors");

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);

	// Si no hay tablas, se crean
	if (rc != SQLITE_OK) {
		printf("Creación de BBDD \n");
		//Creamos Tabla de sensores
		memset(sql, '\0', sizeof(sql));
		sprintf(sql,	"CREATE TABLE Sensors(" \
						"ID_row		INTEGER PRIMARY KEY AUTOINCREMENT,"\
						"ID						INTEGER 	NOT NULL," \
						"Tipo_sensor			CHAR    	NOT NULL," \
						"Unitat_de_mesura		CHAR    	NOT NULL," \
						"Descripcio_sensor		CHAR    	NOT NULL," \
						"Localitzacio_sensor	CHAR    	NOT NULL," \
						"intervalo				INTEGER    	NOT NULL," \
						"IP						CHAR   		NOT NULL);");
		rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
		
		//Creamos Tabla Resumen lecturas
		memset(sql, '\0', sizeof(sql));
		sprintf(sql,	"CREATE TABLE Resum("  \
						"ID						INTEGER 	NOT NULL," \
						"Data_inici				DATE  		NOT NULL," \
						"Increment_de_temps		INTEGER 	NOT NULL," \
						"Maxim					FLOAT	 	NOT NULL," \
						"Data_maxim				DATE  		NOT NULL," \
						"Minim					FLOAT	 	NOT NULL," \
						"Data_minim				DATE  		NOT NULL," \
						"Mitjana				FLOAT  		NOT NULL);");
		rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
		
		//Creamos Tabla de Alarmas
		memset(sql, '\0', sizeof(sql));
		sprintf(sql,	"CREATE TABLE Alarmas("  \
						"ID						INTEGER		NOT NULL," \
						"Data					DATE		NOT NULL," \
						"Descripcio				CHAR    	NOT NULL," \
						"Valor					FLOAT  		NOT NULL);");
		rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
		
		//Creación de sensores debido a la falta de base de datos con datos sensores
		ID=1;
		sprintf(Unitat_de_mesura,"Volts");
		sprintf(Tipo_sensor,"Sensor de tensión");
		sprintf(Descripcio_sensor,"Sensor que muestra la tension en bornes de la batería");
		sprintf(Localitzacio_sensor,"Bateria");
		intervalo=30;
		sprintf(IP,"192.168.11.207");
		insert_Sensors_table(db,ID,Tipo_sensor,Unitat_de_mesura,Descripcio_sensor,Localitzacio_sensor,intervalo,IP);
		ID=2;
		sprintf(Unitat_de_mesura,"Ampers");
		sprintf(Tipo_sensor,"Sensor corriente");
		sprintf(Descripcio_sensor,"Sensor que muestra la lectura de corriente de la batería");
		sprintf(Localitzacio_sensor,"Bateria");
		intervalo=30;
		sprintf(IP,"192.168.11.207");
		insert_Sensors_table(db,ID,Tipo_sensor,Unitat_de_mesura,Descripcio_sensor,Localitzacio_sensor,intervalo,IP);
		ID=1;
		sprintf(Unitat_de_mesura,"Humitat");
		sprintf(Tipo_sensor,"Sensor humitat");
		sprintf(Descripcio_sensor,"Sensor que mostra la humitat a la placa");
		sprintf(Localitzacio_sensor,"Placa solar");
		intervalo=30;
		sprintf(IP,"192.168.11.211");
		insert_Sensors_table(db,ID,Tipo_sensor,Unitat_de_mesura,Descripcio_sensor,Localitzacio_sensor,intervalo,IP);
		ID=2;
		sprintf(Unitat_de_mesura,"ºC");
		sprintf(Tipo_sensor,"Sensor de temperatura");
		sprintf(Descripcio_sensor,"Sensor que muestra la temperatura a la placa");
		sprintf(Localitzacio_sensor,"Placa solar");
		intervalo=30;
		sprintf(IP,"192.168.11.211");
		insert_Sensors_table(db,ID,Tipo_sensor,Unitat_de_mesura,Descripcio_sensor,Localitzacio_sensor,intervalo,IP);
		ID=3;
		sprintf(Unitat_de_mesura,"º");
		sprintf(Tipo_sensor,"Sensor inclinació");
		sprintf(Descripcio_sensor,"Sensor que muestra la orientació de la placa");
		sprintf(Localitzacio_sensor,"Placa solar");
		intervalo=30;
		sprintf(IP,"192.168.11.211");
		insert_Sensors_table(db,ID,Tipo_sensor,Unitat_de_mesura,Descripcio_sensor,Localitzacio_sensor,intervalo,IP);
		ID=3;
		sprintf(Unitat_de_mesura,"º");
		sprintf(Tipo_sensor,"Sensor inclinació");
		sprintf(Descripcio_sensor,"Sensor que muestra la orientació de la placa");
		sprintf(Localitzacio_sensor,"Placa solar");
		intervalo=30;
		sprintf(IP,"192.168.11.211");
		insert_Sensors_table(db,ID,Tipo_sensor,Unitat_de_mesura,Descripcio_sensor,Localitzacio_sensor,intervalo,IP);
	} else printf("Tabla ya creada \n");

	ImprimirMenu();
	scanf("%s", &input);
	while (input != 's')
	{
		memset (enviat,'\0',200);
		memset (rebut,'\0',200);	
		switch (input)
		{
			case '1':
				printf("Heu seleccionat l'opció 1\n");
				// Buscamos el valor máximo de sensores que tenemos
				memset(sql, '\0', sizeof(sql));
				sprintf(sql, "SELECT MAX(ID_row) FROM Sensors");

				/* Execute SQL statement */
				rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
				max_sensores = atoi(data);
				for (int i = 0; i < max_sensores; i++)
				{	

					//Buscamos la ultima fecha guardada
					sprintf(sql, "SELECT MAX(Data_inici) FROM Resum WHERE ID_row = %i;",i);
					rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
					sprintf(data_inici,"%s",data);
					int ret = strncmp("2019/12/31_00:00:00", data_inici, 10	);
					if(ret > 0)
					{
						sprintf(data_inici,"2020/01/01_00:00:00");
					}
					sprintf(sql, "SELECT intervalo FROM Sensors WHERE ID_row = %i;",i);
					rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
					intervalo = atoi(data);
					sprintf(sql, "SELECT ID FROM Sensors WHERE ID_row = %i;",i);
					rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
					ID = atoi(data);
					sprintf(enviat,"{A_%s_%i_%i}",data_inici, intervalo, ID); //cargem a la variable a enviar les dades
					E_R_Datos(enviat, rebut);
					printf("\nS'ha rebut el codi: %s\n",rebut);
					ret = strncmp("{ERROR_PARAMETRO INICIAL}", rebut, 10	);
					if(ret > 0)
					{
					ID2 = atoi(strtok(rebut, s));
					Data_inici = strtok(NULL, s);
					Increment_de_temps = atoi(strtok(NULL, s));
					Maxim = atof(strtok(NULL, s));
					Data_Maxim = strtok(NULL, s);
					Minim = atof(strtok(NULL, s));
					Data_Minim = strtok(NULL, s);
					Mitjana = atof(strtok(NULL, s));
					insert_Resum_table(db,ID2,Data_inici,Increment_de_temps,Maxim,Data_Maxim,Minim,Data_Minim,Mitjana);
					}
					memset (enviat,'\0',200);
					memset (rebut,'\0',200);
				}

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
		ImprimirMenu();
		scanf("%s", &input);
		}

	
	printf("Heu seleccionat la opció sortida\n");
	return 0;
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
	//~ printf("\n");
	return 0;
}

//Insertar en tabla sensor
int insert_Sensors_table(sqlite3* db,int ID, char* Tipo_sensor,char* Unitat_de_mesura, char* Descripcio_sensor, char* Localitzacio_sensor, int intervalo, char* IP){
	int rc;
	char sql[500];
	char *zErrMsg = 0;
	

	/* Insercion de valores Tabla Sensors_table*/
	sprintf(sql,"INSERT INTO Sensors (ID,Tipo_sensor,Unitat_de_mesura,Descripcio_sensor,Localitzacio_sensor,intervalo,IP) VALUES (%i,'%s','%s','%s','%s',%i,'%s');", ID, Tipo_sensor, Unitat_de_mesura, Descripcio_sensor, Localitzacio_sensor, intervalo, IP);
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);

		return 1;
			
		}
	return 0;	   
}
//Insertar en tabla sensor
int insert_Resum_table(sqlite3* db,int ID2, char* Data_inici,int Increment_de_temps, float Maxim,char* Data_maxim,float Minim,char* Data_minim,float Mitjana){
	int rc;
	char sql[500];
	char *zErrMsg = 0;
	

	/* Insercion de valores Tabla Sensors_table*/
	sprintf(sql,"INSERT INTO Resum (ID,Data_inici,Increment_de_temps,Maxim,Data_maxim,Minim,Data_minim,Mitjana) VALUES (%i,'%s',%i,%f,'%s',%f,'%s',%f);", ID2, Data_inici, Increment_de_temps, Maxim,Data_maxim,Minim,Data_minim,Mitjana);
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);

		return 1;
			
		}
	return 0;	   
}
