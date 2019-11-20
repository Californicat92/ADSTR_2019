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

//Libreria con las demás librerias, declaraciones de variables y funciones
#include "funcADSTR.h"

int main(int argc, char* argv[]) {
	//Lectura por comandos de IP del servidor para consultas HTTP y ruta y nombre de la base de datos
	int opt= 0;
	char *ip_servidor=0,*ruta_bbdd=0;
	static struct option long_options[] = {
        {"ip_servidor",	required_argument,	0,	'i'},
        {"ruta_bbdd",	required_argument,	0,	'r'},
        {0,								0,	0,	0}
    };
    int long_index =0;	
    while ((opt = getopt_long(argc, argv,"i:r:", 
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'i' : ip_servidor = optarg;
                 break;
             case 'r' : ruta_bbdd = optarg;
                 break;
             default: print_usage(); 
                 exit(EXIT_FAILURE);
        }
    }
    printf("La IP del servidor seleccionada es: %s\n",ip_servidor);
    printf("La ruta de la base de datos es: %s\n",ruta_bbdd);

	char date[100];
	char date_alarm[100];
	char Alarm_description[100];
	int ret = 0;
	int value_int;
	float value_volts,value_amps;
	int seg_lectura,min_alarma;
	
	sqlite3 *db;
	
	openDB("captura.db", &db);
	
	CreateTable(db);
	
	CreateTable1(db);
	
	CreateTable2(db);

	while(1){
		
		//----Lectura de sensor tensión en bornes de la batería---------
		ret = spiadc_config_transfer(SINGLE_ENDED_CH2, &value_int);

		//printf("voltatge %.3f V\n", value_volts);
		//printf("valor llegit (0-1023) %d\n", value_int);
		//fprintf(stdout, "%lu\n", (unsigned long)t);
		
		value_volts=3.3*value_int/1023; //Conversión a voltaje real
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		sprintf(date,"%d-%d-%d %d:%d:%d", tm.tm_mday, tm.tm_mon + 1,
		tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);	

		insertTable(db,date,value_volts);
		
		insertTable1(db,date,value_volts);
		
			
		/*Alarms*/
		
		if(value_volts > 2.7){
			sprintf(Alarm_description,"Exceso de tension");
			sprintf(date_alarm,"%d-%d-%d %d:%d:%d", tm.tm_mday,
			tm.tm_mon + 1,tm.tm_year + 1900, tm.tm_hour, tm.tm_min,
			tm.tm_sec);
		}

		
		insertTable2(db, date_alarm, Alarm_description);

		//----Lectura de sensor intensidad de carga a la batería--------
		ret = spiadc_config_transfer(DIFERENTIAL_CH0_CH1, &value_int);
		
		value_amps=value_int/(0.5*1023); //Conversión a voltaje real
		sprintf(date,"%d-%d-%d %d:%d:%d", tm.tm_mday, tm.tm_mon + 1,
		tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);	

		insertTable(db,date,value_amps);
		
		insertTable1(db,date,value_amps);
		
			
		/*Alarms*/
		
		if(value_amps > 1){
			sprintf(Alarm_description,"Peligro, hay una fuga de corriente");
			sprintf(date_alarm,"%d-%d-%d %d:%d:%d", tm.tm_mday,
			tm.tm_mon + 1,tm.tm_year + 1900, tm.tm_hour, tm.tm_min,
			tm.tm_sec);
		}

		
		insertTable2(db, date_alarm, Alarm_description);
		//showTable(db);
		
		sleep(seg_lectura);
	}
	
	sqlite3_close(db);
	
	return 0;
}
