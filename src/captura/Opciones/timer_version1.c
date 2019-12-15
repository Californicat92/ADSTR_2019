/*
 * (c) EUSS 2013
 *
 * 
 * Exemple d'utilització dels timers de la biblioteca librt
 * Crea dos timers que es disparen cada segon de forma alternada
 * Cada cop que es disparen imprimeixen per pantalla un missatge
 * 
 * Per compilar: gcc exemple_timer.c -lrt -lpthread -o timer
 */
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (timer_callback) (union sigval);

/* Funció set_timer
 * 
 * Crear un timer
 * 
 * Paràmetres:
 * timer_id: punter a una estructura de tipus timer_t
 * delay: retard disparament timer (segons)
 * interval: periode disparament timer  (segons)
 * func: funció que s'executarà al disparar el timer
 * data: informació que es passarà a la funció func
 * 
 * */
int set_timer(timer_t * timer_id, float delay, float interval, timer_callback * func, void * data) 
{
    int status =0;
    struct itimerspec ts;
    struct sigevent se;

    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = data;
    se.sigev_notify_function = func;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_REALTIME, &se, timer_id);

    ts.it_value.tv_sec = abs(delay);
    ts.it_value.tv_nsec = (delay-abs(delay)) * 1e09;
    ts.it_interval.tv_sec = abs(interval);
    ts.it_interval.tv_nsec = (interval-abs(interval)) * 1e09;

    status = timer_settime(*timer_id, 0, &ts, 0);
    return 0;
}

void comprueba_alarma(union sigval si)
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

int main(int argc, char ** argv)
{

    timer_t frequencia_alarma;
    
    http(ip_servidor, nombre_archivo, &min_lectura, &seg_lectura);
	printf("\nHem rebut %i min %i seg\n\n",min_lectura,seg_lectura);
    
    //set_timer(&frequencia_alarma, min_lectura, seg_lectura, comprueba_alarma, (void *) "tick" );
    //faltaria dato a pasar a la rutina de la interrupcion
	set_timer(&frequencia_alarma, min_lectura, seg_lectura, comprueba_alarma);

    getchar();

    return 0;
}
