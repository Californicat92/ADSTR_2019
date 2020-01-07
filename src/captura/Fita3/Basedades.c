//Fita 3

/*
  * Program: Basedades
  * 
  * Version: 1.0
  * 
  * Author: Joan Olivé,Estefan Molina,Ruben Uceda,Jacobo Blanco,Christopher Juvé
  * 
  * Copyright (C) 2019 
  * 
  * EUSS 2019-Alumnes_EUSS, GRUP B1
  * 
  * License GNU/GPL, see COPYING
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  * You should have received a copy of the GNU General Public License
  * along with this program. If not, see http://www.gnu.org/licenses/.
*/

//Libreria con las demás librerias, declaraciones de variables y funciones
#include "Func.h"

/* Secuencia SQL --> No se donde ponerla
 *  SELECT Date_time_lecture FROM Lectures_table WHERE ID = 2 AND Value = (SELECT MIN(Value) FROM Lectures_table)
 */

int main(int argc, char* argv[]) {
	
	//Lectura por comandos de IP del servidor para consultas HTTP y ruta y nombre de la base de datos
	int opt= 0;
	char *ip_servidor="172.28.16.16",*ruta_bbdd=".",*nombre_archivo="config.txt";
	static struct option long_options[] = {
        {"ip_servidor",		required_argument,	0,	'i'},
        {"nombre_archivo",	required_argument,	0,	'n'},
        {"ruta_bbdd",		required_argument,	0,	'r'},
        {0,									0,	0,	0}
    };
    int long_index =0;	
    while ((opt = getopt_long(argc, argv,"i:n:r:", 
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'i' : ip_servidor = optarg;
                 break;
             case 'n' : nombre_archivo = optarg;
                 break;
             case 'r' : ruta_bbdd = optarg;
                 break;
             default: print_usage(); 
                 exit(EXIT_FAILURE);
        }
    }
    printf("La IP del servidor seleccionada es: %s\n",ip_servidor);
    printf("La ruta de la base de datos es: %s\n",ruta_bbdd);
    //Variables consulta sql
	char data[200];
	char sql[200];
	int rc;
	char *zErrMsg = 0;
	
	
	char date[100];
	char date_alarm[100];
	int cont_alarma=0,iteraciones=0;
	char Alarm_description[100];
	char *fecha_alarma, *valAlarm, *dateAlarm;
	const char 	s[2] = "|";
	int ret = 0;
	int value_int;
	int value_data;
	float value_volts,value_amps;
	int seg_lectura=10, min_lectura=5;
	char types[50], sensor_description[100];
	int id;
	sqlite3 *db;

	//Adquirimos datos de cofiguración del archivo config.txt
	http(ip_servidor, nombre_archivo, &min_lectura, &seg_lectura);
	printf("\nHem rebut %i min %i seg\n\n",min_lectura,seg_lectura);
	
	openDB(ruta_bbdd, &db);
	
	// Comprovacion si hay tablas ya creadas en la base de datos
	memset(sql, '\0', sizeof(sql));
	sprintf(sql, "SELECT * FROM Sensors_table");

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);

	// Si no hay tablas, se crean
	if (rc != SQLITE_OK) {
		
		//Creamos Tabla de Lecturas
		memset(sql, '\0', sizeof(sql));
		sprintf(sql,	"CREATE TABLE Sensor_table(" \
						"ID_row		INTEGER PRIMARY KEY AUTOINCREMENT,"\
						"ID						INTEGER 	NOT NULL," \
						"Type_sensor			CHAR	 	NOT NULL," \
						"Units					CHAR 		NOT NULL," \
						"Descrip_sensor			CHAR 	 	NOT NULL," \
						"Location_sensor		CHAR 	NOT NULL," \
						"IP						INTEGER 	NOT NULL," );
		rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
		
		//Creamos Tabla de Sensores
		memset(sql, '\0', sizeof(sql));
		sprintf(sql,	"CREATE TABLE Resum_table("  \
						"ID_row		INTEGER PRIMARY KEY AUTOINCREMENT,"\
						"ID						INTEGER 	NOT NULL," \
						"Date_start				DATE  	NOT NULL," \
						"Value_Max				FLOAT 	NOT NULL," \
						"Value_Min				FLOAT 	NOT NULL," \
						"Avg					INTEGER  	NOT NULL," \
						"Increase				INTEGER  		NOT NULL);");
		rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
		
		//Creamos Tabla de Alarmas
		memset(sql, '\0', sizeof(sql));
		sprintf(sql,	"CREATE TABLE Alarms_table("  \
						"Date_time_alarm		DATE    NOT NULL," \
						"Alarm_description		CHAR    NOT NULL);"
						"Value_alarm			INT   NOT NULL);");
		rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);

		printf("Creada Base de datos, tablas y sensores\n");
	}
	cont_alarma = min_lectura * 60;
	cont_alarma = cont_alarma / seg_lectura; //cada X iteraciones buscaremos las alarmas
	//~ printf("El numero de iteraciones ha de ser: %d\n",cont_alarma);
	
	while(1){
		//Estructura de fecha y hora
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		sprintf(date,"%d-%d-%d %d:%d:%d", tm.tm_mday, tm.tm_mon + 1,
		tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
		
		//----Lectura de sensor tensión en bornes de la batería---------
		ret = spiadc_config_transfer(SINGLE_ENDED_CH2, &value_int);

		//printf("voltatge %.3f V\n", value_volts);
		//printf("valor llegit (0-1023) %d\n", value_int);
		//fprintf(stdout, "%lu\n", (unsigned long)t);
		
		value_volts=24*value_int/1023; //Conversión a voltaje real
		id=1;

		insert_Lectures_table(db,date,value_volts,id);


		//----Lectura de sensor intensidad de carga a la batería--------
		ret = spiadc_config_transfer(DIFERENTIAL_CH0_CH1, &value_int);
		
		value_amps=value_int*4.65; //Conversión a voltaje real	
		id = 2;
		insert_Lectures_table(db,date,value_amps,id);

		printf("Tension:%0.3f V - Intensidad:%0.f mA\n", value_volts, value_amps);

		//showTable(db);
		
		/*Alarms*/
		if (iteraciones >= cont_alarma)
		{
			if (tm.tm_min < min_lectura){
				sprintf(date_alarm,"%d-%d-%d %d:%d:%d", tm.tm_mday, tm.tm_mon + 1,
				tm.tm_year + 1900, tm.tm_hour - 1, tm.tm_min + (60 - min_lectura), tm.tm_sec);
			} else{
				sprintf(date_alarm,"%d-%d-%d %d:%d:%d", tm.tm_mday, tm.tm_mon + 1,
				tm.tm_year + 1900, tm.tm_hour, tm.tm_min - min_lectura, tm.tm_sec);
			}
			
			// Buscamos el valor máximo de los valores del sensor 1 recogidos en los 5 min anteriores
			memset(sql, '\0', sizeof(sql));
			sprintf(sql, "SELECT MAX(Value) FROM Lectures_table " \
			"WHERE ID = 1 AND Date_time_lecture > %s",date_alarm);

			/* Execute SQL statement */
			rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
			value_data = atoi(data);
			if(value_data > 2.7){
				sprintf(Alarm_description,"Exceso de tension");
				insert_Alarms_table(db, date, Alarm_description);
			}
			
			// Buscamos el valor máximo de los valores del sensor 2 recogidos en los 5 min anteriores
			memset(sql, '\0', sizeof(sql));
			sprintf(sql, "SELECT MAX(Value) FROM Lectures_table " \
			"WHERE ID = 2 AND Date_time_lecture > %s",date_alarm);

			/* Execute SQL statement */
			rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);	
			value_data = atoi(data);	
			if(value_data > 1){
				sprintf(Alarm_description,"Peligro, hay una fuga de corriente");
				insert_Alarms_table(db, date, Alarm_description);
			}
			
			// Buscamos el valor mínimo de los valores del sensor 2 recogidos en los 5 min anteriores
			memset(sql, '\0', sizeof(sql));
			sprintf(sql, "SELECT MIN(Value),Date_time_lecture FROM Lectures_table " \
			"WHERE ID = 2 AND Date_time_lecture > '%s'",date_alarm);
			/* Execute SQL statement */
			rc = sqlite3_exec(db, sql, alarmas_vals, (void *)data, &zErrMsg);

			valAlarm = strtok(data, s);
			dateAlarm = strtok(NULL, s);
			value_data = atoi(valAlarm);
			
			printf("\n\n%s  %s\n\n", valAlarm, dateAlarm);
				
			if(value_data <= 0){
				sprintf(Alarm_description,"Batería desconectada");
				insert_Alarms_table(db, dateAlarm, Alarm_description);
			}
			iteraciones = 0; // reiniciamos el valor que entra en las alarmas
		}
		iteraciones++;
		blink(	); //Parpadeo LED en Pin BCM17
		sleep(seg_lectura-1);
	}
	
	sqlite3_close(db);
	return 0;
}
