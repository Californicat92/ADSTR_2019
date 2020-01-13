
/*
 * server_main1.c
 * 
 * Copyright 2020 MARC <1457636@Ccampus.euss.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

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

#define SERVER_PORT_NUM		5001
#define SERVER_MAX_CONNECTIONS	4
#define REQUEST_MSG_SIZE	1024
#define BUFFERRECIVE_TOTAL		200

int comunicacio (char buffer_rebut[], char buffer_enviat[]);
int database_media(char *media, char *fecha_hora, char *id, char *fecha_hora_despues);
int database_maximo(char *maxima, char *fecha_hora, char *id, char *fecha_hora_despues);
int database_minimo( char *minimo, char *fecha_hora, char *id, char *fecha_hora_despues);

//~ int after_timer( char *dia, char *mes, char *year ,char *hora, char *min, char *seg, char *contador, char *fecha_hora_despues);

int callback_all(void *data, int argc, char **argv, char **azColName);

int main(int argc, char **argv){
    
	struct 		sockaddr_in	serverAddr;
	struct 		sockaddr_in	clientAddr;
	int			sockAddrSize;
	int			sFd;
	int			newFd;
	int			nRead;
	int 		result;
	char		buffer_rebut[BUFFERRECIVE_TOTAL];
	char		buffer_enviat[BUFFERRECIVE_TOTAL];
	
	
	

	/*Preparar l'adreçsa local*/
	sockAddrSize=sizeof(struct sockaddr_in);
	bzero ((char *)&serverAddr, sockAddrSize); //Posar l'estructura a zero
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/*Crear un socket*/
	sFd=socket(AF_INET, SOCK_STREAM, 0);
	
	/*Nominalitzar el socket*/
	result = bind(sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
	
	/*Crear una cua per les peticions de connexió*/
	result = listen(sFd, SERVER_MAX_CONNECTIONS);
	
	/*Bucle d'acceptació de connexions*/
		while(1){
		printf("\nServidor esperant connexions\n");
		newFd=accept(sFd, (struct sockaddr *) &clientAddr, &sockAddrSize);
		printf("Connexió acceptada del client: adreça %s, port %d\n",inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		memset( buffer_rebut, 0, BUFFERRECIVE_TOTAL );
		memset( buffer_enviat, 0, BUFFERRECIVE_TOTAL );

		/*Rebre*/
		result = read(newFd, buffer_rebut, BUFFERRECIVE_TOTAL);
		printf("Missatge rebut del client(bytes %d): %s\n",	result, buffer_rebut);

		/*Fer procés necessari amb els casos*/
	
		comunicacio (buffer_rebut, buffer_enviat);
		
		
		/*Enviar*/
		result = write(newFd, buffer_enviat, strlen(buffer_enviat)+1); //+1 per enviar el 0 final de cadena
		printf("Missatge enviat a client(bytes %d): %s\n",	result, buffer_enviat);

		
		
		}//sortida de while
	close(newFd);
	
	return 0;
}

int comunicacio(char buffer_rebut[], char buffer_enviat[]){
	
		
	char 		year[4];
	char		mes[2];
	char 		dia[2];
	char		hora[2];
	char		min[2];
	char		seg[2];
	char 		contador[2];
	char		id[1];
	char 		media[512];
	char 		maxima[512];
	char 		minimo[512];
	char 		fecha_hora_actual[48];
	char 		fecha_hora_despues[48];
	
	
		year[0] = buffer_rebut[3];
		year[1] = buffer_rebut[4];
		year[2] = buffer_rebut[5];
		year[3] = buffer_rebut[6];
		
		mes[0] = buffer_rebut[8];
		mes[1] = buffer_rebut[9];
		
		dia[0] = buffer_rebut[11];
		dia[1] = buffer_rebut[12];
		
		hora[0] = buffer_rebut[14];
		hora[1] = buffer_rebut[15];
		
		min[0] = buffer_rebut[17];
		min[1] = buffer_rebut[18];
		
		seg[0] = buffer_rebut[20];
		seg[1] = buffer_rebut[21];
		
		contador[0] = buffer_rebut[23];
		contador[1] = buffer_rebut[24];
		
		id[0] = buffer_rebut[25];
		
			if (buffer_rebut[0] == '{') {
											printf("1\n\n");	
				if((buffer_rebut[1] == 'A') || (buffer_rebut[1] ='B')){
												printf("2\n\n");	
					if(buffer_rebut[27] == '}'){
													printf("3\n\n");	
						switch (buffer_rebut[1]){
							case 'A':
														printf("4\n\n");	
							sprintf(fecha_hora_actual, "%c%c/%c%c/%c%c%c%c %c%c:%c%c:%c%c", dia[0], dia[1], mes[0], mes[1], year[0], year[1], year[2], year[3], hora[0], hora[1], min[0], min[1], seg[0], seg[1]); 
							 //~ after_timer(dia, mes, year, hora, min, seg, contador, fecha_hora_despues);
							 //database_media(media, fecha_hora_actual, id, fecha_hora_despues);
							 //database_maximo(maxima,fecha_hora_actual,id, fecha_hora_despues);
							 //database_minimo(minimo,fecha_hora_actual,id, fecha_hora_despues);
							 
							//~ sprintf(buffer_enviat,"{C%s}",fecha_hora_actual);
							sprintf(buffer_enviat,"{A#01#%s#30#10.10#%s#1.50#%s#5.55}",fecha_hora_actual,fecha_hora_actual,fecha_hora_actual);
							//sprintf(buffer_enviat,"{C%s/%s/%s_%s:%s:%s_%s_%2.2s_%s_%s}", year,mes,dia,hora,min,seg, contador, media,maximo,minimo);
											

							break;
							
							case 'B':
							
							memset (fecha_hora_actual,'\0',48);
							sprintf(fecha_hora_actual, "%c%c/%c%c/%c%c%c%c %c%c:%c%c:%c%c", dia[0], dia[1], mes[0], mes[1], year[0], year[1], year[2], year[3], hora[0], hora[1], min[0], min[1], seg[0], seg[1]); 
							
							//~ after_timer(dia, mes, year, hora, min, seg, contador, fecha_hora_despues);
							
							
							
						
						
							break;
							
						}
					
					}else{
						memset (buffer_enviat,'\0',BUFFERRECIVE_TOTAL);
						sprintf(buffer_enviat,"{ERROR_PARAMETRO INICIAL}");
					}
				}else{
					memset (buffer_enviat,'\0',BUFFERRECIVE_TOTAL);
					sprintf(buffer_enviat,"{ERROR_PARAMETRO}");
				}
			}else{
				memset (buffer_enviat,'\0',BUFFERRECIVE_TOTAL);
				sprintf(buffer_enviat,"{ERROR_PARAMETRO FINAL}");
			}
			
	
	return 0;
}

int database_media(char *media, char *fecha_hora, char *id, char *fecha_hora_despues){
	
	sqlite3 *db;
	int rc;
	int rf;
	char sql[512];
	char data[512];
	char *zErrMsg = 0;
	int int_id = 0;
	
	int_id = *id - '0';
	rc = sqlite3_open("basededatos1.db", &db);
	if(rc){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	}else{
		fprintf(stderr, "Opened database succesfully\n");
	}
	if (int_id == 1){
		
		  
		
		
		sprintf(sql, "SELECT AVG(Temperatura),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rc = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(media,"%s",data);
	}
	if(int_id == 2){
		sprintf(sql, "SELECT AVG(Humedad),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rc = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(media,"%s",data);
	}
	if(int_id == 3){
		sprintf(sql, "SELECT AVG(Accelerometro),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rc = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(media,"%s",data);
	}
	

	rf = sqlite3_open("basededatos2.db", &db);
	if(rf){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	}else{
		fprintf(stderr, "Opened database succesfully\n");
	}
	if(int_id == 4){
		sprintf(sql, "SELECT AVG(Voltage),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rc = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(media,"%s",data);
	}
	
	if(int_id == 5){
		sprintf(sql, "SELECT AVG(Intensidad),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rc = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(media,"%s",data);
	}
	return 0;
}

int database_maximo(char *maxima, char *fecha_hora, char *id,char *fecha_hora_despues){
	
	sqlite3 *db;
	int rc;
	int rf;
	char sql[512];
	char data[512];
	char *zErrMsg = 0;
	int int_id = 0;
	
	int_id = *id - '0';
	rc = sqlite3_open("basededatos1.db", &db);
	if(rc){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	}else{
		fprintf(stderr, "Opened database succesfully\n");
	}
	if (int_id == 1){
		sprintf(sql, "SELECT AVG(Temperatura),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' BETWEEN '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rc = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(maxima,"%s",data);
	}
	if(int_id == 2){
		sprintf(sql, "SELECT * MAX(Humedad),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rc = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(maxima,"%s",data);
	}
	if(int_id == 3){
		sprintf(sql, "SELECT * MAX(Accelerometro),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rc = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(maxima,"%s",data);
	}
	
	
	rf = sqlite3_open("basededatos2.db", &db);
	if(rf){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	}else{
		fprintf(stderr, "Opened database succesfully\n");
	}
	if(int_id == 4){
		sprintf(sql, "SELECT * MAX(Voltage),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rf = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rf != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(maxima,"%s",data);
	}
	
	if(int_id == 5){
		sprintf(sql, "SELECT * MAX(Intensidad),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rf = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rf != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(maxima,"%s",data);
	}

	return 0;
}
int database_minimo( char *minimo, char *fecha_hora, char *id, char *fecha_hora_despues){
	
	sqlite3 *db;
	int rc;
	int rf;
	char sql[512];
	char data[512];
	char *zErrMsg = 0;
	int int_id = 0;
	
	int_id = *id - '0';
	rc = sqlite3_open("basededatos1.db", &db);
	if(rc){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	}else{
		fprintf(stderr, "Opened database succesfully\n");
	}
	if (int_id == 1){
		sprintf(sql, "SELECT * MIN(Temperatura),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rc = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(minimo,"%s",data);
	}
	if(int_id == 2){
		sprintf(sql, "SELECT * MIN(Humedad),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rc = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(minimo,"%s",data);
	}
	if(int_id == 3){
		sprintf(sql, "SELECT * MIN(Accelerometro),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rc = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(minimo,"%s",data);
	}
	
	rf = sqlite3_open("basededatos2.db", &db);
	if(rf){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	}else{
		fprintf(stderr, "Opened database succesfully\n");
	}
	if(int_id == 4){
		sprintf(sql, "SELECT * MIN(Voltage),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rf = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rf != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(minimo,"%s",data);
	}
	
	if(int_id == 5){
		sprintf(sql, "SELECT * MIN(Intensidad),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID='%s' AND '%s' > Fecha_hora_lectura AND '%s' < Fecha_hora_lectura", id, fecha_hora, fecha_hora_despues);
	
		rf = sqlite3_exec(db, sql, callback_all, (void*)data, &zErrMsg);
		if (rf != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
		fprintf(stdout, "Records created succesfully\n");
		}
	sprintf(minimo,"%s",data);
	}
	
	
	return 0;
}

//~ int after_timer( char *dia, char *mes, char *year ,char *hora, char *min, char *seg, char *contador, char *fecha_hora_despues){
	
	
	//~ int segundos_segundos = 0;
	//~ char hora_caracter[50];
	//~ char fecha_caracter[50];
	
	//~ int horas = atoi(hora);
	//~ int minutos = atoi(min);
	//~ int segundos = atoi(seg);
	//~ int cont = atoi(contador);

	//~ segundos_segundos = segundos + cont;

	//~ if(segundos_segundos >=60){
		//~ segundos_segundos = segundos_segundos - 60;
		//~ minutos = minutos + 1;
	//~ }
	//~ if(minutos >= 60){
	//~ minutos = minutos - 60;
	//~ horas = horas + 1;
		
	//~ }
	//~ if(horas >= 24){
	//~ horas = 0;
	//~ }	
	
	//~ sprintf(hora_caracter, "%d:%d:%d", horas, minutos, segundos_segundos);
	//~ sprintf(fecha_caracter, "%s/%s/%s", dia, mes, year);
	//~ sprintf(fecha_hora_despues, "%s %s", fecha_caracter, hora_caracter);
	
	//~ return 0;
//~ }


	
int callback_all(void *data, int argc, char **argv, char **azColName){
int i;

memset((char *)data, '\0', sizeof(data));

for(i = 0; i<argc; i++) {
sprintf(data, "%s", argv[i] ? argv[i] : "NULL");

//printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
printf("\n");
return 0;
}

