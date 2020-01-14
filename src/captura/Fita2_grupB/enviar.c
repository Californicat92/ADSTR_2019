#include "fitxer.h"
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
#include <string.h>


#define REQUEST_MSG_SIZE	1024
#define REPLY_MSG_SIZE		500
#define SERVER_PORT_NUM		25



 /************************
*
*
* tcpClient
*
*
*/
void funcioenviar(char *cap, char *from, char *to, char *data, char *sub) {
	struct sockaddr_in	serverAddr;
	char	    serverName[] = "172.20.0.21"; //Adreça IP on està el client
	int			sockAddrSize;
	int			sFd;
	int			mlen;
	int 		result;
	char		buffer[10000];

	char cerrar[]= "close";
	
	
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

	/*Rebre*/
	result = read(sFd, buffer, 256);
	buffer[result]=0;
	printf("Missatge rebut del servidor(bytes %d): %s\n",	result, buffer);
	
	/*Enviar*/
	strcpy(buffer,cap); //Copiar missatge a buffer
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n",	result, buffer);

	
	/*Rebre*/
	result = read(sFd, buffer, 256);
	buffer[result]=0;
	printf("Missatge rebut del servidor(bytes %d): %s\n",	result, buffer);
	
	/*Enviar*/
	strcpy(buffer,from); //Copiar missatge a buffer
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n",	result, buffer);

	
	/*Rebre*/
	result = read(sFd, buffer, 256);
	buffer[result]=0;
	printf("Missatge rebut del servidor(bytes %d): %s\n",	result, buffer);
	
	/*Enviar*/
	strcpy(buffer,to); //Copiar missatge a buffer
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n",	result, buffer);

	/*Rebre*/
	result = read(sFd, buffer, 256);
	buffer[result]=0;
	printf("Missatge rebut del servidor(bytes %d): %s\n",	result, buffer);
	
	/*Enviar*/
	strcpy(buffer,data); //Copiar missatge a buffer
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n",	result, buffer);
	
	/*Rebre*/
	result = read(sFd, buffer, 256);
	buffer[result]=0;
	printf("Missatge rebut del servidor(bytes %d): %s\n",	result, buffer);
	
	/*Enviar*/
	strcpy(buffer,sub); //Copiar missatge a buffer
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n",	result, buffer);
	
	/*Rebre*/
	result = read(sFd, buffer, 256);
	buffer[result]=0;
	printf("Missatge rebut del servidor(bytes %d): %s\n",	result, buffer);
	
	/*Enviar*/
	strcpy(buffer,cerrar); //Copiar missatge a buffer
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n",	result, buffer);
	
	
	/*Tancar el socket*/
	close(sFd);

	return 0;
	}
