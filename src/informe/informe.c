Images/**
 * @file			informe.c
 * @brief			Trabajar una base da datos para generar un informe
 * @package			ADSTR
 * @version			1.0
 * @author			Adrián, Fran & Christopher
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
 * Formato bases de datos a usar:
 *
 *	· Sensors_table
 *		- ID
 *		- Types (Magnitud)
 *		- Description
 *
 *	· Lectures_table
 *		- Date_time_lecture
 *		- ID
 *		- Value
 *
 *	· Alarms_table
 *		- Date_time_alarm
 *		- Alarm_description
 */

/*
 * gcc Informe3.c -o informe -lsqlite3
 * arm-linux-gnueabihf-gcc informe.c -o informe -lsqlite3
 *./scripts/checkpatch.pl -f /home/cali/Desktop/ADSTR_2019/src/informe/informe.c
 * scp nom_executable pi@192.168.11.10X:/tmp
 * (on X és el número de dispositiu si es conecta a través de la xarxa
 * LAN i /tmp és el directori destí).
 * pi@192.168.11.20X:/tmp para WIFI
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
/**
 * Llamadas a archivo. 
 */
/**
 * Archivo de texto donde se escribirán los datos de los sensores y las alarmas\n
 * Formato:\n\n
 * X Sensor:
 * 	- Fecha/hora inicio
 * 	- Fecha/hora final
 *	- Valor máximo
 * 	- Valor mínimo
 * 	- Valor medio
 *
 * Listado de alarmas
 **/
FILE * fp;

/**
 * Esta función devuelve los valores que recibe de la base de datos y los pone
 * en la variable data, que es un puntero.
 */
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

int main(int argc, char *argv[]){

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
	rc = sqlite3_open("captura.db", &db);

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
		memset(texto, '\0', sizeof(texto));

		//Valor máximo
		sprintf(sql, "SELECT MAX(Value) FROM "\
			"Lectures_table WHERE ID = %d", n);
		sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
		sprintf(texto, "\tEl valor máximo de tensión registrado: "
			"%s V\n", data);
		fprintf(fp, "%s", texto);
		memset(texto, '\0', sizeof(texto));

		//Valor mínimo
		sprintf(sql, "SELECT MIN(Value) FROM Lectures_table WHERE "\
			"ID = %d", n);
		sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
		sprintf(texto, "\tEl valor mínimo de tensión registrado: "\
			"%s V\n", data);
		fprintf(fp, "%s", texto);
		memset(texto, '\0', sizeof(texto));

		//Valor medio
		sprintf(sql, "SELECT AVG(Value) FROM Lectures_table WHERE "\
			"ID = %d", n);
		sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
		sprintf(texto, "\tEl valor medio de tensión obtenido: "\
			"%s V\n\n", data);
		fprintf(fp, "%s", texto);
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
		memset(texto, '\0', sizeof(texto));
		sprintf(sql, "SELECT Alarm_description FROM Alarms_table "\
			"WHERE rowid = %d", x);
		sqlite3_exec(db, sql, getValues, (void *)data, &zErrMsg);
		sprintf(texto, "\tDescripción de la alarma: %s\n\n", data);
		fprintf(fp, "%s", texto);
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
