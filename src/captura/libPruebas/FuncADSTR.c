/*
 * Funciones.c
 * 
 * Copyright 2019  <pi@raspberrypi>
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

//Libreria con las demás librerias, declaraciones de variables y funciones
#include "funcADSTR.h"

int verbose = 0;

static char *cntdevice = "/dev/spidev0.0";// definicio pin select (0.1)/CS1

// -----------------------------GETOPT HELP-------------------------------------
void print_usage() {
    printf(	"\n/---------------------HELP-------------------------------\n"\
			"Uso:	Introducir -p seguido de la IP del servidor \n" \
			"\tIntroducir -r seguido de la ruta y nombre.db de la base de "\
			"datos\n"\
			"/-----------------------------------------------------------\n\n");
}
// -----------------------------------------------------------------------------
// Indicacion de un error
static void pabort(const char *s){
	perror(s); // informa del error
	abort();// cierre del programa
}
// -----------------------------------------------------------------------------

static void spiadc_config_tx(int conf, uint8_t tx[3]){
	int i;

	uint8_t tx_dac[3] = { 0x01, 0x00, 0x00 };//Les dades de que volem trasnmetre
	uint8_t n_tx_dac = 3;// quantitas de dades que volem transmetre
	
	for (i=0; i < n_tx_dac; i++) {
		tx[i] = tx_dac[i];
	}
	
// Estableix el mode de comunicació en la parta alta del 2n byte
	tx[1]=conf<<4;
	
	if(verbose) { //Mostra el que s'esta enviant 
		for (i=0; i < n_tx_dac; i++) {
			printf("spi tx dac byte:(%02d)=0x%02x\n",i,tx[i] );
		}
	}		
}
// -----------------------------------------------------------------------------

static int spiadc_transfer(int fd, uint8_t bits, uint32_t speed, uint16_t delay, uint8_t tx[3], uint8_t *rx, int len ){
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
// -----------------------------------------------------------------------------

static int spiadc_config_transfer(int conf, int *value){
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
// -----------------------------------------------------------------------------

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
// -----------------------------------------------------------------------------

int openDB(char * name, sqlite3** db){ //Crear o obrir la base de dades.
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
// -----------------------------------------------------------------------------

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
			   
					else {
					fprintf(stdout, "Table created successfully\n");
					}
		   
		}
	return 0;
}
// -----------------------------------------------------------------------------

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
					else {
					fprintf(stdout, "Table created successfully\n");
					}
		}
	return 0;
}
// -----------------------------------------------------------------------------

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
		   
				else {
				fprintf(stdout, "Table created successfully\n");
				}
		}   
	return 0;
}
// -----------------------------------------------------------------------------
//Insertar en la tabla lecture
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
			else{
				fprintf(stdout, "Insercio correcta");
			}
			
	return 0;
}
// -----------------------------------------------------------------------------
//Insertar en tabla sensor
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
// -----------------------------------------------------------------------------
//Insertar los valores en Alarms_table
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
	else{
		fprintf(stdout, "Insercio correcta");
	}
	return 0; 
}
// -----------------------------------------------------------------------------

//Mostrat por pantalla el contenido de la base de datos.
int showTable(sqlite3* db){ 
	
	int rc;
	char sql[500];
	char *zErrMsg = 0;
	
	/* Visulizacion valores Tabla 1 */
	
	sprintf(sql, "SELECT * FROM Lectures_table");
	//sprintf(sql, "SELECT * FROM Lectures_table,Sensors_table,Alarms_table");

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);

		return 1;
	}

   return 0;
}
// -----------------------------------------------------------------------------


