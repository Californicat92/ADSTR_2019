/**
 * @file			informe.c
 * @brief			Trabajar una base da datos para generar un informe
 * @package			ADSTR
 * @version			1.0
 * @author			Adrián, Fran, Christopher, Joan, Ruben, Estefan
 * @copyright (C)	2019 Alumnes EUSS ADSTR 2019
 * \n
 * \n
 * @license        GNU/GPL, see COPYING
 * This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
 *\n
 */




/*
* cd Escriptori
* gcc -fPIC -c -o enviar.o enviar.c
* gcc -shared -fPIC -o libenviar.so enviar.o
* gcc -c -o prueba.o prueba.c -I/home/alumne/Escriptori/enviar.h
* gcc -o prueba prueba.o -L/home/alumne/Escriptori/ libenviar.so -lsqlite3
* export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/alumne/Escriptori/libenviar.so
*./prueba
*/

#include <stdio.h>
#include <stdlib.h>
#include "enviar.h"
#include <sqlite3.h>
#include <string.h>
#include <getopt.h>
#include <sqlite3.h>

FILE * fp;

void print_usage() {
    printf(	"\n/---------------------HELP------------------------------------------\n"\
			"Uso:	Introducir -m seguido de la direccion de correo a la cual queremos enviar el informe \n" \
			"	Introducir -s seguido del tema del correo \n"\
			"	Introducir -f seguido del correo desde el cual se envia(el que aparecera en el mensaje) \n"\
			"	Introducir -t para indicar la direccion de correo que lo recibira\n"\
			"	Introducir -i seguido del contenido informe(variable en la que lo almacenamos) \n"\
			"/-------------------------------------------------------------------\n\n");
}
static int getValues(void *data, int argc, char **argv, char **azColName)
{
	int i;

	memset((char *)data, '\0', sizeof(data));

	for (i = 0; i < argc; i++) {
		sprintf(data, "%s", argv[i] ? argv[i] : "NULL");
		//~ printf("%s", (char *)data);
	}
	return 0;
}

int funcioinforme (char *informe){
	sqlite3 *db;
	char sql[120];
	char *zErrMsg = 0;
	int rc;
	int nSen;
	int nAlarm;
	char data[200];
	char texto[400];
	char *fecha, *hora;
	
	/** Para trabajar Bases de datos usamos el comando sqlite3_open
	 * y se añade la ruta donde esta guardada la base de datos, en este
	 * caso /ADSTR_2019/db/bbdd.db\n
	**/
	/* Open database */
	rc = sqlite3_open("/home/alumne/Escriptori/captura.db", &db);

	if (rc)
		fprintf(stderr, "Can't open database: %s\n",
		sqlite3_errmsg(db));

	/* Open document */
	fp = fopen("informe.txt", "w");
	if (fp == NULL)
		printf("Could not open file");

	/**
	 * Después se obtiene el número máximos de sensores.
	**/
	/* Lectura nº sensores */
	memset(data, '\0', sizeof(data));
	sprintf(sql, "SELECT MAX(ID) FROM Sensors_table");

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	nSen = atoi(data);
	/**
	 * A continuación, se obtiene toda la información incluida en el informe 
	 * (mirar variable fp) y se imprime en el fichero .txt, con el formato
	 * deseado
	**/
	int n;
	for (n = 1; n <= nSen; n++) {
		//Sensor
		sprintf(texto, "\nInforme de sensor %d:\n", n);
		fprintf(fp, "%s", texto);
		strcat(informe,texto);
		memset(texto, '\0', sizeof(texto));

		//Fecha/hora inicial
		sprintf(sql, "SELECT MIN(Date_time_lecture)"\
			" FROM Lectures_table WHERE ID = %d", n);
		rc = sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
		fecha = strtok(data, " ");
		hora = strtok(hora, " ");
		sprintf(texto, "\tLa lectura empieza el dia %s a las %s\n",
			fecha, hora);
		fprintf(fp, "%s", texto);
		strcat(informe,texto);
		memset(texto, '\0', sizeof(texto));

		//Fecha/hora final
		sprintf(sql, "SELECT MAX(Date_time_lecture) FROM "\
			"Lectures_table WHERE ID = %d", n);
		sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
		fecha = strtok(data, " ");
		hora = strtok(hora, " ");
		sprintf(texto, "\tLa lectura acaba el dia %s a las %s\n",
			fecha, hora);
		fprintf(fp, "%s", texto);
		strcat(informe,texto);
		memset(texto, '\0', sizeof(texto));

		//Valor máximo
		sprintf(sql, "SELECT MAX(Value) FROM "\
			"Lectures_table WHERE ID = %d", n);
		sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
		sprintf(texto, "\tEl valor máximo de tensión registrado: "
			"%s V\n", data);
		fprintf(fp, "%s", texto);
		strcat(informe,texto);
		memset(texto, '\0', sizeof(texto));

		//Valor mínimo
		sprintf(sql, "SELECT MIN(Value) FROM Lectures_table WHERE "\
			"ID = %d", n);
		sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
		sprintf(texto, "\tEl valor mínimo de tensión registrado: "\
			"%s V\n", data);
		fprintf(fp, "%s", texto);
		strcat(informe,texto);
		memset(texto, '\0', sizeof(texto));

		//Valor medio
		sprintf(sql, "SELECT AVG(Value) FROM Lectures_table WHERE "\
			"ID = %d", n);
		sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
		sprintf(texto, "\tEl valor medio de tensión obtenido: "\
			"%s V\n\n", data);
		fprintf(fp, "%s", texto);
		strcat(informe,texto);
		memset(texto, '\0', sizeof(texto));
	}
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	/** Para obtener el número de alarmas máximo usamos el comando SQL:
	* SELECT Count(*) FROM Alarms_table
	**/
	// Obtener número de alarmas de la BBDD en SQL
	sprintf(sql, "SELECT Count(*) FROM Alarms_table");
	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	nAlarm = atoi(data);

	/**
	 * Imprimimos el listado de alarmas, obteniendo por separado cuando se
	 * produjo la incidencia y el motivo de esta. Después se separa la fecha
	 * y la hora en variables separadas para poderlas meter en el string
	**/
	// Imprimir alarmas
	int x;
	fprintf(fp, "\n\nListado de alarmas:\n");
	for (x = 1; x <= nAlarm; x++) {
		sprintf(sql, "SELECT Date_time_alarm FROM Alarms_table WHERE "\
			"rowid = %d", x);
		sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
		fecha = strtok(data, " ");
		hora = strtok(data, " ");
		sprintf(texto, "\t%d.- Alarma ocurrida el día %s a las %s\n",
		x, fecha, hora);
		fprintf(fp, "%s", texto);
		strcat(informe,texto);
		memset(texto, '\0', sizeof(texto));
		sprintf(sql, "SELECT Alarm_description FROM Alarms_table "\
			"WHERE rowid = %d", x);
		sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
		sprintf(texto, "\tDescripción de la alarma: %s\n\n", data);
		fprintf(fp, "%s", texto);
		strcat(informe,texto);
		memset(texto, '\0', sizeof(texto));
	}

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("\nEjecución correcta, revise el documento "\
			"informe.txt\n\n");
	}

	fclose(fp);
	sqlite3_close(db);
	return 0;
}
	
	
	
	
	

int main(int argc, char *argv[]){
	
	
	
//Lectura por comandos de IP del servidor para consultas HTTP y ruta y nombre de la base de datos
	int opt= 0;
	char *mail_receptor=0,*subject=0,*froms=0,*tos=0;
	char informe[10000];
	funcioinforme(informe);

	static struct option long_options[] = {
        
        {"mail_receptor",	required_argument,	0,	'm'},
        {"subject",	required_argument,	0,	's'},
		{"froms",	required_argument,	0,	'f'},
		{"tos",	required_argument,	0,	't'},
        {0,								0,	0,	0}
    };
    int long_index =0;	
    while ((opt = getopt_long(argc, argv,"m:s:f:t:i:", 
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'm' : mail_receptor = optarg;
                 break;
             case 's' : subject = optarg;
                 break;
             case 'f' : froms = optarg;
                 break;
             case 't' : tos = optarg;
                 break;
             default: print_usage(); 
                 exit(EXIT_FAILURE);
        }
    }
    
	

    
    printf("La direccion de correo seleccionada es: %s\n",mail_receptor);
	printf("El subject del mensaje es: %s\n",subject);
	printf("La direccion de correo emisora es : %s\n",froms);
	printf("La direccion de correo receptora es: %s\n",tos);
	printf("El contenido del informe es: %s\n",informe);

	char		cap[] = "HELO host\n";
	char		from[] = "MAIL FROM: 1468244@campus.euss.org\n";
	char to[50];
	sprintf(to,"RCPT TO: %s\n",mail_receptor);
	char 		data[] = "DATA\n";
	char sub[10000];
	sprintf(sub,"Subject: %s\n"\
				"From: %s\n"\
				"To: %s\n"\
				"%s\n.\n",subject,froms,tos,informe);
									
	funcioenviar(cap, from, to, data, sub);
	
}
