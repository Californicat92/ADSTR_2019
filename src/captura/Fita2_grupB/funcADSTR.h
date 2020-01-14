//Fita 2

#ifndef funcADSTH_H
#define funcADSTR_H

//Otras librerias
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
#include "http.h"
#include <wiringPi.h>

#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

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

#define	LED	0 // LED Pin - wiringPi pin 0 is BCM_GPIO 17.

//Declaración de funciones
void print_usage(); // Func. opciones ayuda GETOPT
void pabort(const char *s);// Indicacion de un error
void spiadc_config_tx(int conf, uint8_t tx[3]);
int spiadc_transfer(int fd, uint8_t bits, uint32_t speed, uint16_t delay, uint8_t tx[3], uint8_t *rx, int len);
int spiadc_config_transfer( int conf, int *value);
int callback(void *data, int argc, char **argv, char **azColName);
int alarmas_vals(void *data, int argc, char **argv, char **azColName);
int openDB(char * name, sqlite3** db); //Crear o obrir la base de dades
int insert_Lectures_table(sqlite3* db, char* date, float value, int id);
int insert_Sensors_table(sqlite3* db, char* date,int id, char* types, char* description);
int insert_Alarms_table(sqlite3* db, char* date_alarm, char* Alarm_description);
int showTable(sqlite3* db); //Mostrat por pantalla el contenido de la base de datos.
int blink (void);
//int set_timer(timer_t * timer_id, float delay, float interval, timer_callback * func, void * data);
// -----------------------------------------------------------------------------

#endif
