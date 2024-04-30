#include "../include/comunicaciones.h"

pthread_mutex_t mutex_procesos;
t_list procesos_totales;
t_proceso_memoria *proceso_memoria;

typedef struct
{
	t_log *log;
	int fd;
	char *server_name;
} t_procesar_conexion_args;

static void procesar_conexion_memoria(void *void_args)
{
	t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
	t_log *logger = args->log;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	op_cod cop;
	t_list *lista;
	while (cliente_socket != -1)
	{

		if (recv(cliente_socket, &cop, sizeof(op_cod), 0) != sizeof(op_cod))
		{
			log_info(logger, "Se desconecto el cliente!\n");
			return;
		}

		switch (cop)
		{
		case MENSAJE:
			recibir_mensaje(cliente_socket, logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_socket);
			log_info(logger, "Me llegaron los siguientes valores:");
			list_iterate(lista, (void *)iterator);
			break;

		// --------------------------------
		// -- INICIALIZAR_PROCESO ---------
		// --------------------------------

		case INICIALIZAR_PROCESO:
			log_info(logger, "Se recibio un INICIALIZAR_PROCESO");
			proceso_memoria = recibir_proceso_memoria(cliente_socket);
			log_info(logger, "Se recibio el proceso %d con el path %s", proceso_memoria->pid, proceso_memoria->path_proceso);
			
			

		// -------------------
		// -- CPU - MEMORIA --
		// -------------------
		case HANDSHAKE_cpu:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con la CPU");
			break;

		case PEDIDO_INSTRUCCION:
			uint32_t pid, pc;
			pedido_intruccion(&pid, &pc, cliente_socket);
			t_proceso_memoria *proceso = obtener_proceso_pid(pid);
			if(proceso == NULL){
				break;
			}else{
				t_instruccion *instruccion = obtener_instruccion_pid_pc(pid, pc);
				log_info(logger, " Se envia la instruccion a CPU de PC %d para el PID %d y es: %s - %s - %s", pc, pid, obtener_nombre_instruccion(instruccion->codigo), instruccion->parametro1, instruccion->parametro2);
				if(instruccion != NULL) enviar_instruccion(cliente_socket, instruccion);
				break;
			}

		// ----------------------
		// -- KERNEL - MEMORIA --
		// ----------------------
		case HANDSHAKE_kernel:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con el KERNEL");
			break;

		//CASO NO TERMINADO
		/*case INICIALIZAR_PROCESO:
			log_info(logger, " Inicializando estructuras para el proceso");
			proceso_memoria = recibir_proceso_memoria(cliente_socket);
			proceso_memoria = iniciar_proceso_path(proceso_memoria);*/ 


		// -------------------
		// -- I/O - MEMORIA --
		// -------------------
		case HANDSHAKE_in_out:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con el I/O");
			break;

		// ---------------
		// -- ERRORES --
		// ---------------
		case -1:
			log_error(logger, "Cliente desconectado de %s... con cop -1", server_name);
			break;  //hay un return, voy a probar un break
		default:
			log_error(logger, "Algo anduvo mal en el server de %s", server_name);
			log_info(logger, "Cop: %d", cop);
			break;  //hay un return, voy a probar un break
		}
	}

	log_warning(logger, "El cliente se desconecto de %s server", server_name);
	return;
}

int server_escuchar(t_log *logger, char *server_name, int server_socket)
{
	int cliente_socket = esperar_cliente(server_socket, logger);

	if (cliente_socket != -1)
	{
		pthread_t hilo;
		t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
		args->log = logger;
		args->fd = cliente_socket;
		args->server_name = server_name;
		pthread_create(&hilo, NULL, (void *)procesar_conexion_memoria, (void *)args);
		pthread_detach(hilo);
		return 1;
	}
	return 0;
}

void extraer_de_paquete(t_paquete *paquete, void *destino, int size){}
/*{
    if (paquete->buffer->size < size)
    {
        // Manejar error
		log_error(logger, "No se puede extraer %d bytes de un paquete de %d bytes", size, paquete->buffer->size);
		return;
    }

    memcpy(destino, paquete->buffer->stream, size);

    paquete->buffer->size -= size;
    memmove(paquete->buffer->stream, paquete->buffer->stream + size, paquete->buffer->size);
}*/

// ENVIADO DE INSTRUCCIONES DE MEMORIA A CPU
void pedido_instruccion(uint32_t *pid, uint32_t *pc, int socket){
	int size;
	void *buffer = recibir_buffer(&size, socket);
	int offset = 0;
	memcpy(pid, buffer+offset, sizeof(int));
	offset += sizeof(int);
	memcpy(pc, buffer+offset, sizeof(int));
	free(buffer);
}

t_proceso_memoria obtener_proceso_pid(uint32_t pid_){
	bool id_process(void *elemento){
		return ((t_proceso_memoria*)elemento)->pid == pid_;
	}
	t_proceso_memoria *proceso;
	pthread_mutex_lock(&mutex_procesos);
	proceso = list_find(procesos_totales, id_process);
	return proceso;
}

char *obtener_nombre_instruccion(nombre_instruccion instruccion){
	switch(instruccion){
		case SET:
			return "SET";
		case SUM:
			return "SUM";
		case SUB:
			return "SUB";
		case JNZ:
			return "SET";
		case IO_GEN_SLEEP:
			return "IO_GEN_SLEEP";
	}
}

void enviar_instruccion(int socket, t_instruccion *instruccion){
	t_paquete *paquete = crear_paquete_con_codigo_de_operacion(INSTRUCCION);
	serializar_instruccion(paquete, instruccion);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

void serializar_instruccion(t_paquete *paquete, t_instruccion *instruccion){
	paquete->buffer->size = sizeof(nombre_instruccion) + sizeof(uint32_t) * 2 + instruccion->longitud_parametro1 + instruccion->longitud_parametro2;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	int desplazamiento = 0;

	memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->codigo), sizeof(nombre_instruccion));
	desplazamiento += sizeof(nombre_instruccion);
	memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->longitud_parametro1), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->longitud_parametro2), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro1, instruccion->longitud_parametro1);
	desplazamiento += instruccion->longitud_parametro1;
	memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro2, instruccion->longitud_parametro2);
	desplazamiento += instruccion->longitud_parametro2;
}


//ESTA PARTE LA HIZO THOMI (NO LO DIGO DE MALO, SOLO COMENTO PARA SEPARAR LO QUE HICE)
//SE ESTARA USANDO ESTA FUNCION A PRIORI, SI HAY DRAMA, SE REEMPLAZARA
t_proceso_memoria *recibir_proceso_memoria(int socket){

	t_proceso_memoria *proceso = malloc(sizeof(t_proceso_memoria));
	int size;
	void *buffer = recibir_buffer(&size, socket);
	int offset = 0;

	memcpy(&(proceso->pid), buffer+offset, sizeof(int));
	offset += sizeof(int);

	proceso->path_proceso = malloc(size - offset);
	memcpy(proceso->path_proceso, buffer+offset, size - offset);
	free(buffer);
	return proceso;
}

//NO ME CIERRA LA FUNCION, SE REVISARA MAS TARDE (MAS QUE NADA AL DEVOLVER EL PROCESO_NUEVO, YA QUE SE INICIALIZA PREVIAMENTE Y NO SE DESARROLLO LA FUNCION POR EL TEMA DE LA ESTRUCTURA DE LOS PROCESOS EN MEMORIA)
t_proceso_memoria *iniciar_proceso_path(t_proceso_memoria *proceso_nuevo){ 
	proceso_nuevo->instrucciones = parsear_instrucciones(proceso_nuevo->path);
	log_info(LOGGER_MEMORIA, "Instrucciones bien parseadas para el proceso PID [%d]", proceso_nuevo->pid);
	pthread_mutex_lock(&mutex_procesos);
	list_add(procesos_totales, proceso_nuevo);
	pthread_mutex_unlock(&mutex_procesos);
	//inicializar_nuevo_proceso(proceso_nuevo); -> Se usa para inicializar las estructuras de memoria del proceso. Se agregara despues si es que no hay drama con la funcion actual. Si genera problema, se eliminara y buscara otra alternativa
	return proceso_nuevo;
}

//LEE EL ARCHIVO E INGRESA CADA INSTRUCCION EN LA LISTA DE INSTRUCCIONES DEL PROCESO, NO SE TERMINO DE DESARROLLAR
/*t_list *parsear_instrucciones(char *path){
	t_list *instrucciones = list_create();
	char *path_archivo = string_new();
	string_append(&path_archivo, "./cfg/");
	string_append(&path_archivo, path);
	char *codigo_leido = leer_archivo(path_archivo);
	char **split_instrucciones = string_split(codigo_leido, "\n");
	int indice_split = 0;
	while (split_instrucciones[indice_split] != NULL)
	{
		char **palabras = string_split(split_instrucciones[indice_split], " ");
		if(string_equals_ignore_case(palabras[0], "SET")){
			list_add(instrucciones, )
		}
	}
	
}*/