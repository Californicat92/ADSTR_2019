/*
  * Program: Basedades
  * 
  * Version: 1.0
  * 
  * Author: Joan Olivé,Estefan Molina,Ruben Uceda
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
#include <wiringPi.h>

int verbose = 0;

static char *cntdevice = "/dev/spidev0.0";// definicio pin select (0.1)/CS1

//ADC configurations segons manual MCP3008
#define SINGLE_ENDED_CH0 8
#define SINGLE_ENDED_CH1 9
#define SINGLE_ENDED_CH2 10
#define SINGLE_ENDED_CH3 11
#define SINGLE_ENDED_CH4 12
#define SINGLE_ENDED_CH5 13
#define SINGLE_ENDED_CH6 14
#define SINGLE_ENDED_CH7 15
#define DIFERENTIAL_CH0_CH1 0 //Chanel CH0 = IN+ CH1 = IN-
#define DIFERENTIAL_CH1_CH0 1 //Chanel CH0 = IN- CH1 = IN+
#define DIFERENTIAL_CH2_CH3 2 //Chanel CH2 = IN+ CH3 = IN-
#define DIFERENTIAL_CH3_CH2 3 //Chanel CH2 = IN- CH3 = IN+
#define DIFERENTIAL_CH4_CH5 4 //Chanel CH4 = IN+ CH5 = IN-
#define DIFERENTIAL_CH5_CH4 5 //Chanel CH4 = IN- CH5 = IN+
#define DIFERENTIAL_CH6_CH7 6 //Chanel CH6 = IN+ CH7 = IN-
#define DIFERENTIAL_CH7_CH6 7 //Chanel CH6 = IN- CH7 = IN+

// -----------------------------------------------------------------------------------------------

static void pabort(const char *s)// Indicacion de un error
{
	perror(s); // informa del error
	abort();// cierre del programa
}

// -----------------------------------------------------------------------------------------------

static void spiadc_config_tx( int conf, uint8_t tx[3] )
{
	int i;

	uint8_t tx_dac[3] = { 0x01, 0x00, 0x00 };// las dades de que volem trasnmetre
	uint8_t n_tx_dac = 3;// quantitas de dades que volem transmetre 
	
	for (i=0; i < n_tx_dac; i++) {
		tx[i] = tx_dac[i];
	}
	
// Estableix el mode de comunicació en la parta alta del 2n byte
	tx[1]=conf<<4;
	
	if( verbose ) { 									//Mostra el que s'esta enviant 
		for (i=0; i < n_tx_dac; i++) {
			printf("spi tx dac byte:(%02d)=0x%02x\n",i,tx[i] );
		}
	}
		
}

// -----------------------------------------------------------------------------------------------
static int spiadc_transfer(int fd, uint8_t bits, uint32_t speed, uint16_t delay, uint8_t tx[3], uint8_t *rx, int len )
{
	int ret, value, i;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len*sizeof(uint8_t),
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

	if( verbose ) {

		for (i = 0; i < len; i++) {
			printf("0x%02x ", rx[i]);
		}
		value = ((rx[1] & 0x0F) << 8) + rx[2];
		printf("-->  %d\n", value);
	
	}

	return ret;

}



// -----------------------------------------------------------------------------------------------

static int spiadc_config_transfer( int conf, int *value )
{
	int ret = 0;
	int fd;
	uint8_t rx[3];
	char buffer[255];
	
	/* SPI parameters */
	char *device = cntdevice;
	//uint8_t mode = SPI_CPOL; //No va bé amb aquesta configuació, ha de ser CPHA
	uint8_t mode = SPI_CPHA;
	uint8_t bits = 8;
	uint32_t speed = 500000; //max 1500KHz
	uint16_t delay = 0;
	
	/* Transmission buffer */
	uint8_t tx[3];

	/* open device */
	fd = open(device, O_RDWR);
	if (fd < 0) {
		sprintf( buffer, "can't open device (%s)", device );
		pabort( buffer );
	}

	/* spi mode 	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/* bits per word 	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/* max speed hz  */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	/* build data to transfer */
	spiadc_config_tx( conf, tx );
		
	/* spi adc transfer */
	ret = spiadc_transfer( fd, bits, speed, delay, tx, rx, 3 );
	if (ret == 1)
		pabort("can't send spi message");

	close(fd);

	*value = ((rx[1] & 0x03) << 8) + rx[2];

	return ret;
}

/////////////////////////////////////////////////////////////Comunicacion SPI////////////////////////////////////////////////////////////////////////

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
//Crear o abrir la base de datos.
int openDB(char * name, sqlite3** db){ 
	int rc;
	
	/* Open database */
	rc = sqlite3_open(name, db);
	if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
		return 1;
		
	} 	
		/*else {
			fprintf(stdout, "Opened database successfully\n");		//Comentari tot ok.
		}
		*/
	return 0;
}
//Creacion de la tabla: Lectures_table
int CreateTable(sqlite3* db){ 
	int rc;
	char sql[500];
	char sql1[500];
	char *zErrMsg = 0;
	char *sqlx;
	
	
	    /* Check crear tabla nueva o NO*/
	sprintf(sql1, "SELECT MAX(ID) FROM Lectures_table");	
	rc = sqlite3_exec(db, sql1, callback, 0, &zErrMsg);
	
		if (rc != SQLITE_OK){
			
						/* Creacion Tabla Lectures_table */
			  sqlx = "CREATE TABLE Lectures_table("  \
			  "ID					INTEGER 	PRIMARY KEY	AUTOINCREMENT," \
			  "Date_time_lecture	DATE    	NOT NULL," \
			  "Value				INT   		NOT NULL);";
			  
			strcpy(sql,sqlx); // El valor de sqlx se copia en sql
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
		   
				if( rc != SQLITE_OK ){
				  fprintf(stderr, "SQL error: %s\n", zErrMsg);
				  sqlite3_free(zErrMsg);
				  return 1;
				}  
			   /*
					else {
					fprintf(stdout, "Table created successfully\n");
					}
		   */
		}
	return 0;
}
//Creacion de la tabla: Sensors_table
int CreateTable1(sqlite3* db){
	int rc;
	char sql[500];
	char sql1[500];
	char *zErrMsg = 0;
	char *sqlx;
	
				/* Check crear tabla nueva o NO*/
		sprintf(sql1, "SELECT MAX(ID) FROM Sensors_table");
		rc = sqlite3_exec(db, sql1, callback, 0, &zErrMsg);

		if (rc != SQLITE_OK) {
			/* Creacion Tabla Sensors_table */
			
			sqlx = "CREATE TABLE Sensors_table("  \
			  "ID					INTEGER 	PRIMARY KEY	AUTOINCREMENT," \
			  "Types				CHAR  		NOT NULL," \
			  "Description			CHAR  		NOT NULL);";
			  
			 strcpy(sql,sqlx); // El valor de sqlx se copia en sql
			 rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
		   
		   if( rc != SQLITE_OK ){
			  fprintf(stderr, "SQL error: %s\n", zErrMsg);
			  sqlite3_free(zErrMsg);
			  return 1;
			}
					/*else {
					fprintf(stdout, "Table created successfully\n");
					}
					*/
		}
	return 0;
}
//Creacion de la tabla: Alarms_table
int CreateTable2(sqlite3* db){ 
	int rc;
	char sql[500];
	char sql1[500];
	char *zErrMsg = 0;
	char *sqlx;
			/* Check crear tabla nueva o NO*/
		sprintf(sql1, "SELECT MAX(ID) FROM Alarms_table");
		rc = sqlite3_exec(db, sql1, callback, 0, &zErrMsg);

			if (rc != SQLITE_OK) {
		 
				/* Creacion Tabla Alarms_table */
			sqlx = "CREATE TABLE Alarms_table("  \
			  "Date_time_alarm		DATE    NOT NULL," \
			  "Alarm_description	CHAR    NOT NULL);";
			  
			   strcpy(sql,sqlx); 
			   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
		   
		   if( rc != SQLITE_OK ){
			  fprintf(stderr, "SQL error: %s\n", zErrMsg);
			  sqlite3_free(zErrMsg);
			   return 1; 
		   } 
		   /*
				else {
				fprintf(stdout, "Table created successfully\n");
				}
				*/
		}   
	return 0;
}
//Insertar los datos en tabla Lectures_table
int insertTable(sqlite3* db, char* date, float value){ 
	int rc;
	char sql[500];
	char *zErrMsg = 0;
	
	 /* Insercion de valores Tabla Lectures_table */
	
	sprintf(sql,"INSERT INTO Lectures_table(Date_time_lecture,Value) VALUES ('%s',%f);", date, value);// Date_time y value denominacion en la tabla, denominacion de especificador de formato tipo string y float.
	
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);

		return 1;
		}	
	return 0;
}
//Insertar los datos en tabla Sensors_table
int insertTable1(sqlite3* db, char* date, float value){ 
	int rc;
	char sql[500];
	char *zErrMsg = 0;
	char Description[100];
	char Types[100];
	
		sprintf(Types,"Sensor Tension");
		sprintf(Description,"Sensor que muestra la lectura de tension de la placa fotovoltaica");

			/* Insercion de valores Tabla Sensors_table*/
	sprintf(sql,"INSERT INTO Sensors_table (Types,Description) VALUES ('%s','%s');",Types, Description);
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

		if( rc != SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);

			return 1;
		}

	return 0;
	   
}
//Insertar los datos en tabla Alarms_table
int insertTable2(sqlite3* db, char* date_alarm, char* Alarm_description){ 
	int rc;
	char sql[500];
	char *zErrMsg = 0;
	
	 /* Insercion de valores Tabla Alarms_table*/
	sprintf(sql,"INSERT INTO Alarms_table (Date_time_alarm,Alarm_description) VALUES ('%s','%s');",date_alarm, Alarm_description);
	
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);

		return 1;
		}
	return 0;
	   
}
//Mostrat por pantalla el contenido de la base de datos.
int showTable(sqlite3* db){ 
	
	int rc;
	char sql[500];
	char *zErrMsg = 0;
	
	/* Visulizacion valores Tabla Lectures_table */
	sprintf(sql, "SELECT * FROM Lectures_table");

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);

		return 1;
	}

   return 0;
}
//Funcion encender LED
int parpadeo(void){
	wiringPiSetup();
	pinMode(0, OUTPUT);
	int i=0;
	for (i=0; i<1; i++){
		digitalWrite(0,HIGH);
		delay(100);
		digitalWrite(0,LOW);
		delay(100);
	}
	
	return 0;
	
	}

int main(int argc, char* argv[]) {
	
	char date[100];
	char date_alarm[100];
	char Alarm_description[100];
	int ret=0;
	int value_int;//la variable que se lee por spi
	float value_volts;//Valor escalat a 3.3V
	
	sqlite3 *db;
	
	openDB("captura.db", &db);//abrir base de datos
	
	CreateTable(db);//Creacion tabla Lectures_table
	
	CreateTable1(db);//Creacion tabla Sensors_table
	
	CreateTable2(db);//Creacion tabla Alarms_table

	while(1){
		ret = spiadc_config_transfer( SINGLE_ENDED_CH2, &value_int );
		
		value_volts=3.3*value_int/1023;
		
		/* Funcion de la fecha del sistema */
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);	
		sprintf(date,"%d-%d-%d %d:%d:%d", tm.tm_mday, tm.tm_mon + 1,tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);	

		insertTable(db,date,value_volts);//Insercion datos tabla Lectures_table
		
		parpadeo();//Encender y apagar led
		
		insertTable1(db,date,value_volts);//Insercion datos tabla Sensors_table
		
		/*Alarms*/
		
			if(value_volts > 2.7){
			sprintf(Alarm_description,"Exceso de tension");
			sprintf(date_alarm,"%d-%d-%d %d:%d:%d", tm.tm_mday, tm.tm_mon + 1,tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
			}
			
			else 	{
			sprintf(Alarm_description,NULL);
			sprintf(date_alarm,NULL);
			}
		
		insertTable2(db,date_alarm,Alarm_description);//Insercion datos tabla Alarms_table
	
		//showTable(db); //Funcion que permite imprimir tablas por pantalla.
		
		sleep(5);//Retardo de 5s entre capturas
	}
	
	sqlite3_close(db);
	
	return 0;
}
