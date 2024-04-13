#ifndef SOCKETS_SERVER_H_
#define SOCKETS_SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>

#include "./sockets_common.h"

#define PUERTO "8002"

/*typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;*/

extern t_log* logger;

void* recibir_buffer(int*, int);

int iniciar_servidor(void);
int esperar_cliente(int);
int procesar_conexion(int server_fd);
void iterator(char* value);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);

#endif