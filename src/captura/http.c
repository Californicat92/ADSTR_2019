//Fita 2

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
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "http.h"



#define REQUEST_MSG_SIZE	1024
#define REPLY_MSG_SIZE		500
#define SERVER_PORT_NUM		80



/**Funcion HTTP:
 * Con esta funcion, se consigue acceder al servidor que se nos proporciona y
 * adquirimos los valores necesarios para hacer las capturas y las alarmas.
 * Esta libreria es dinámica.
 **/
int http(char serverName[20], char* nombre_archivo, int *minuts, int *segons){
	*minuts=0,*segons=0;
	
	struct sockaddr_in	serverAddr;
	int			sockAddrSize;
	int			sFd;
	int 		result;
	char		buffer[256];
	int			i=0,val=0;
	char		parametre_web[5];
	char		missatge[200];
	const char 	s[2] = ",";
	char 		*min,*seg;
	
	sprintf(missatge,	"GET /%s HTTP/1.1\r\n"\
						"Host: captura\r\n\r\n", nombre_archivo);
	

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
	printf("\nConnexió establerta amb el servidor: adreça %s, port %d\n",	inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

	/*Enviar*/
	strcpy(buffer,missatge); //Copiar missatge a buffer
	result = write(sFd, buffer, strlen(buffer));
	//~ printf("Missatge enviat a servidor(bytes %d): %s\n",	result, missatge);

	/*Rebre fins que no arribi el caràcter $*/
	do{
		result = read(sFd, buffer, 256);
		buffer[result] = 0;
		//~ printf("Missatge rebut del servidor(bytes %d): %s\n",	result, buffer);
	}
	while (buffer == NULL);
	
	i = strlen(buffer);		//medimos longitud buffer
	val = i-5;				//nos quedamos con los ultimos 5 caracteres
	int c=0;
	for (c=0; val < i; val++, c++){
		parametre_web[c]=buffer[val];
	}
	//~ printf("val = %s \n", parametre_web);

	min = strtok(parametre_web, s);
	seg = strtok(NULL, s);
	*minuts = atoi(min);
	*segons = atoi(seg);
		
	//~ printf("min = %i \n", *minuts);
	//~ printf("seg = %i \n", *segons);
	
	/*Tancar el socket*/
	close(sFd);

	return 0;
}
