#include "../include/utils_memoria.h"

t_proceso_memoria *proceso_memoria;

t_proceso_memoria *recibir_proceso_memoria(int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_proceso_memoria *proceso = deserializar_proceso(paquete->buffer);
    eliminar_paquete(paquete);
    return proceso;
}

t_proceso_memoria *deserializar_proceso(t_buffer *buffer)
{
    t_proceso_memoria *proceso = malloc(sizeof(t_proceso_memoria));
    if (proceso == NULL)
    {
        return NULL;
    }

    uint32_t long_path;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(proceso->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(long_path), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    proceso->path = malloc(long_path);

    memcpy(proceso->path, stream + desplazamiento, long_path);

    return proceso;
}

// ENVIADO DE INSTRUCCIONES DE MEMORIA A CPU
void recibir_pedido_instruccion(uint32_t *pid, uint32_t *pc, int socket)
{
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_pedido_instruccion(pid, pc, paquete->buffer);
    eliminar_paquete(paquete);
}

void deserializar_pedido_instruccion(uint32_t *pid, uint32_t *pc, t_buffer *buffer)
{
    int desplazamiento = 0;
    memcpy(pid, buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(pc, buffer->stream + desplazamiento, sizeof(uint32_t));
}

t_proceso_memoria *obtener_proceso_pid(uint32_t pid_pedido)
{
    t_proceso_memoria *proceso;

    bool id_process(void *elemento)
    {
        return ((t_proceso_memoria *)elemento)->pid == pid_pedido;
    }

    pthread_mutex_lock(&mutex_procesos);
    proceso = list_find(procesos_totales, id_process);
    pthread_mutex_unlock(&mutex_procesos);

    return proceso;
}

t_instruccion *obtener_instruccion_del_proceso_pc(t_proceso_memoria *proceso, uint32_t pc)
{
    usleep(RETARDO_RESPUESTA * 1000); // 1000 * 1000 = 1 segundo
    return list_get(proceso->instrucciones, pc);
}

char *instruccion_to_string(nombre_instruccion instruccion)
{
    switch (instruccion)
    {
    case SET:
        return "SET";
    case SUM:
        return "SUM";
    case SUB:
        return "SUB";
    case JNZ:
        return "JNZ";
    case IO_GEN_SLEEP:
        return "IO_GEN_SLEEP";
    case MOV_IN:
        return "MOV_IN";
    case MOV_OUT:
        return "MOV_OUT";
    case RESIZE:
        return "RESIZE";
    case COPY_STRING:
        return "COPY_STRING";
    case WAIT:
        return "WAIT";
    case SIGNAL:
        return "SIGNAL";
    case IO_STDIN_READ:
        return "IO_STDIN_READ";
    case IO_STDOUT_WRITE:
        return "IO_STDOUT_WRITE";
    case IO_FS_CREATE:
        return "IO_FS_CREATE";
    case IO_FS_DELETE:
        return "IO_FS_DELETE";
    case IO_FS_TRUNCATE:
        return "IO_FS_TRUNCATE";
    case IO_FS_WRITE:
        return "IO_FS_WRITE";
    case IO_FS_READ:
        return "IO_FS_READ";
    case EXIT:
        return "EXIT";
    default:
        return "Instruccion no reconocida";
        break;
    }
}

void enviar_instruccion(int socket, t_instruccion *instruccion)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(INSTRUCCION);
    serializar_instruccion(paquete, instruccion);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}



void serializar_instruccion(t_paquete *paquete, t_instruccion *instruccion)
{
    paquete->buffer->size = sizeof(nombre_instruccion) +
                   sizeof(uint32_t) * 5 +
                   instruccion->longitud_parametro1 +
                   instruccion->longitud_parametro2 +
                   instruccion->longitud_parametro3 +
                   instruccion->longitud_parametro4 +
                   instruccion->longitud_parametro5;

    paquete->buffer->stream = malloc(paquete->buffer->size);

    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->nombre), sizeof(nombre_instruccion));
    desplazamiento += sizeof(nombre_instruccion);

    uint32_t longitud_parametro1 = instruccion->longitud_parametro1;                          // Cambio aquí
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro1, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro2 = instruccion->longitud_parametro2;                          // Cambio aquí
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro2, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro3 = instruccion->longitud_parametro3;                          // Cambio aquí
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro3, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro4 = instruccion->longitud_parametro4;                          // Cambio aquí
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro4, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro5 = instruccion->longitud_parametro5;                          // Cambio aquí
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro5, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream  + desplazamiento, instruccion->parametro1, instruccion->longitud_parametro1);
    desplazamiento += instruccion->longitud_parametro1;

    memcpy(paquete->buffer->stream  + desplazamiento, instruccion->parametro2, instruccion->longitud_parametro2);
    desplazamiento += instruccion->longitud_parametro2;

    memcpy(paquete->buffer->stream  + desplazamiento, instruccion->parametro3, instruccion->longitud_parametro3);
    desplazamiento += instruccion->longitud_parametro3;

    memcpy(paquete->buffer->stream  + desplazamiento, instruccion->parametro4, instruccion->longitud_parametro4);
    desplazamiento += instruccion->longitud_parametro4;

    memcpy(paquete->buffer->stream  + desplazamiento, instruccion->parametro5, instruccion->longitud_parametro5);
}

t_proceso_memoria *iniciar_proceso_path(t_proceso_memoria *proceso_nuevo)
{
    pthread_mutex_lock(&mutex_procesos);
    proceso_nuevo->instrucciones = parsear_instrucciones(proceso_nuevo->path);
    log_debug(LOGGER_MEMORIA, "Instrucciones bien parseadas para el proceso PID [%d]", proceso_nuevo->pid);
    list_add(procesos_totales, proceso_nuevo);
    pthread_mutex_unlock(&mutex_procesos);
    iniciar_estructura_proceso_memoria(proceso_nuevo);
    return proceso_nuevo;
}

// LEE EL ARCHIVO E INGRESA CADA INSTRUCCION EN LA LISTA DE INSTRUCCIONES DEL PROCESO
t_list *parsear_instrucciones(char *path)
{
    t_list *instrucciones = list_create();
    char *path_archivo = string_new();
    string_append(&path_archivo, PATH_INSTRUCCIONES);
    string_append(&path_archivo, path);
    char *codigo_leido = leer_archivo(path_archivo);
    char **split_instrucciones = string_split(codigo_leido, "\n");
    int indice_split = 0;
    while (split_instrucciones[indice_split] != NULL)
    {
        char **palabras = string_split(split_instrucciones[indice_split], " ");
        if (string_equals_ignore_case(palabras[0], "SET"))
        {
            list_add(instrucciones, armar_estructura_instruccion(SET, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "SUM"))
        {
            list_add(instrucciones, armar_estructura_instruccion(SUM, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "SUB"))
        {
            list_add(instrucciones, armar_estructura_instruccion(SUB, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "JNZ"))
        {
            list_add(instrucciones, armar_estructura_instruccion(JNZ, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "WAIT"))
        {
            list_add(instrucciones, armar_estructura_instruccion(WAIT, palabras[1], "", "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "SIGNAL"))
        {
            list_add(instrucciones, armar_estructura_instruccion(SIGNAL, palabras[1], "", "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "RESIZE"))
        {
            list_add(instrucciones, armar_estructura_instruccion(RESIZE, palabras[1], "", "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "MOV_IN"))
        {
            list_add(instrucciones, armar_estructura_instruccion(MOV_IN, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "COPY_STRING"))
        {
            list_add(instrucciones, armar_estructura_instruccion(COPY_STRING, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "IO_GEN_SLEEP"))
        {
            list_add(instrucciones, armar_estructura_instruccion(IO_GEN_SLEEP, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "IO_STDIN_READ"))
        {
            list_add(instrucciones, armar_estructura_instruccion(IO_STDIN_READ, palabras[1], palabras[2], palabras[3], "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "IO_STDOUT_WRITE"))
        {
            list_add(instrucciones, armar_estructura_instruccion(IO_STDOUT_WRITE, palabras[1], palabras[2], palabras[3], "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "EXIT"))
        {
            list_add(instrucciones, armar_estructura_instruccion(EXIT, "", "", "", "", ""));
        }

        indice_split++;
        string_iterate_lines(palabras, (void (*)(char *))free);
        free(palabras);
    }
    free(codigo_leido);
    string_iterate_lines(split_instrucciones, (void (*)(char *))free);
    free(split_instrucciones);
    free(path_archivo);
    return instrucciones;
}

t_instruccion *armar_estructura_instruccion(nombre_instruccion instruccion, char *parametro1, char *parametro2, char *parametro3, char *parametro4, char *parametro5)
{
    t_instruccion *estructura = (t_instruccion *)malloc(sizeof(t_instruccion));

    estructura->nombre = instruccion;
    estructura->parametro1 = (parametro1[0] != '\0') ? strdup(parametro1) : parametro1; //(parametro1 && parametro1[0] != '\0') ? strdup(parametro1) : NULL;
    estructura->parametro2 = (parametro2[0] != '\0') ? strdup(parametro2) : parametro2; //(parametro2 && parametro2[0] != '\0') ? strdup(parametro2) : NULL;

    estructura->longitud_parametro1 = strlen(estructura->parametro1) + 1;
    estructura->longitud_parametro2 = strlen(estructura->parametro2) + 1;

    estructura->parametro3 = (parametro3[0] != '\0') ? strdup(parametro3) : parametro3;
    estructura->longitud_parametro3 = strlen(estructura->parametro3) + 1;

    estructura->parametro4 = (parametro4[0] != '\0') ? strdup(parametro4) : parametro4;
    estructura->longitud_parametro4 = strlen(estructura->parametro4) + 1;

    estructura->parametro5 = (parametro5[0] != '\0') ? strdup(parametro5) : parametro5;
    estructura->longitud_parametro5 = strlen(estructura->parametro5) + 1;

    return estructura;
}

char *leer_archivo(char *path)
{
    char instrucciones[100];
    strcpy(instrucciones, path);
    FILE *archivo = fopen(instrucciones, "r");

    if (archivo == NULL)
    {
        perror("Error al abrir el archivo");
    }

    fseek(archivo, 0, SEEK_END);

    int cant_elementos = ftell(archivo);
    rewind(archivo);

    char *cadena = calloc(cant_elementos + 1, sizeof(char));
    if (cadena == NULL)
    {
        perror("Error en la reserva de memoria \n");
        fclose(archivo);
        return NULL;
    }

    int cant_elementos_leidos = fread(cadena, sizeof(char), cant_elementos, archivo);
    if (cant_elementos != cant_elementos_leidos)
    {
        perror("Error leyendo el archivo \n");
        fclose(archivo);
        free(cadena);
        return NULL;
    }
    fclose(archivo);
    return cadena;
}

void recibir_pedido_resize(uint32_t *pid, uint32_t *nueva_cantidad_paginas, int socket)
{
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_pedido_resize(pid, nueva_cantidad_paginas, paquete->buffer);
    eliminar_paquete(paquete);
}

void deserializar_pedido_resize(uint32_t *pid, uint32_t *tamanio, t_buffer *buffer)
{
    int desplazamiento = 0;
    memcpy(pid, buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(tamanio, buffer->stream + desplazamiento, sizeof(uint32_t));
}

void iniciar_semaforos()
{
    pthread_mutex_init(&mutex_procesos, NULL);
    pthread_mutex_init(&mutex_comunicacion_procesos, NULL);
}

void recibir_mov_out_cpu(uint32_t *direccion_fisica, char **registro, int cliente_socket)
{
    t_paquete *paquete = recibir_paquete(cliente_socket);
    deserializar_datos_mov_out(paquete, direccion_fisica, registro);
    eliminar_paquete(paquete);
}

void deserializar_datos_mov_out(t_paquete *paquete, uint32_t *direccion_fisica, char **registro)
{
    int desplazamiento = 0;
    memcpy(&direccion_fisica, paquete->buffer->stream, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    int char_length = strlen(*registro) + 1;
    memcpy(&char_length, paquete->buffer->stream, sizeof(int));
    desplazamiento += sizeof(int);
    registro = malloc(char_length);
    memcpy(registro, paquete->buffer->stream, char_length);
}

void deserializar_datos_mov_in(t_paquete *paquete, uint32_t *direccion_fisica, uint32_t *tamanio_registro)
{
    int desplazamiento = 0;
    memcpy(direccion_fisica, paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(tamanio_registro, paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
}

void recibir_mov_in_cpu(int socket_cliente, uint32_t *direccion_fisica, uint32_t *tamanio_registro)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    deserializar_datos_mov_in(paquete, direccion_fisica, tamanio_registro);
    eliminar_paquete(paquete);
}

void recibir_pedido_marco(uint32_t *pagina, uint32_t *pid_proceso, int socket)
{
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_pedido_marco(pagina, pid_proceso, paquete->buffer);
    eliminar_paquete(paquete);
}

void deserializar_pedido_marco(uint32_t *pagina, uint32_t *pid_proceso, t_buffer *buffer)
{
    int desplazamiento = 0;
    memcpy(pid_proceso, buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(pagina, buffer->stream + desplazamiento, sizeof(uint32_t));
}

void enviar_marco(int socket, uint32_t marco)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(ENVIAR_MARCO); // ??????
    serializar_marco(paquete, marco);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_marco(t_paquete *paquete, uint32_t marco)
{
    paquete->buffer->size = sizeof(uint32_t);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, &(marco), sizeof(uint32_t));
}

void enviar_valor_mov_in_memoria(char* valor, int socket)
{
    t_paquete *paquete_mov_in = crear_paquete_con_codigo_de_operacion(PEDIDO_MOV_IN);
    serializar_valor_leido_mov_in(paquete_mov_in, valor);
    enviar_paquete(paquete_mov_in, socket);
    eliminar_paquete(paquete_mov_in);
}

void serializar_valor_leido_mov_in(t_paquete *paquete, char* valor)
{
    uint32_t tamanio_valor = strlen(valor) + 1;
    paquete->buffer->size = sizeof(uint32_t);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, &tamanio_valor, sizeof(uint32_t));
}

void escribir_memoria(uint32_t direccion_fisica, char* valor)
{
	// log_info(logger_memoria_info, "voy a escribir un valor en memoria usuario"); // TODO BORRAR
	pthread_mutex_lock(&mutex_memoria_usuario);
	memcpy(memoriaUsuario + direccion_fisica, &valor, sizeof(uint32_t));
	pthread_mutex_unlock(&mutex_memoria_usuario);

	// log_error(logger_memoria_info, "ya guarde en memoria"); // TODO BORRAR
	//t_marco *marco = marco_desde_df(direccion_fisica); //chequear esto

	//marcar_pag_modificada(marco->pid, marco->num_de_marco); //chequear esto
	
	usleep(RETARDO_RESPUESTA * 1000);
}


uint32_t leer_memoria(uint32_t dir_fisica, uint32_t tamanio_registro)
{
    printf("Entro a leer memoria \n");
    uint32_t valor_leido;
    uint32_t numaux = 3;
    int numero_marco = dir_fisica / TAM_PAGINA;
    pthread_mutex_lock(&mutex_memoria_usuario);
    //TESTEO
    memcpy(&dir_fisica, &numaux, sizeof(tamanio_registro));
    //TESTEO
    memcpy(&valor_leido, &dir_fisica, sizeof(tamanio_registro));
    printf("Valor leido: %d \n", valor_leido);
    pthread_mutex_unlock(&mutex_memoria_usuario);
    usleep(RETARDO_RESPUESTA * 1000);

    /*t_marco *marco = marco_desde_df(dir_fisica);
    // TODO -- VALIDAR -- cuando hago un F_READ debo marcar la pagina que tiene el marco como modificada
    t_proceso_memoria *proceso = obtener_proceso_pid((uint32_t)marco->pid);
    t_list *paginas_en_memoria = obtener_entradas_con_bit_presencia_1(proceso);
    t_entrada_tabla_pag *pagina_modificada = obtener_entrada_con_marco(paginas_en_memoria, marco->num_de_marco);
    actualizo_entrada_para_futuro_reemplazo(pagina_modificada);

    if (config_valores_memoria.retardo_respuesta / 1000 > 0)
        sleep(config_valores_memoria.retardo_respuesta / 1000);
    else
        sleep(1);
    usleep(config_valores_memoria.retardo_respuesta * 1000);
    log_info(logger_memoria_info, "***** ACCESO A ESPACIO USUARIO - CPU - PID [%d] - ACCION: [LEER] - DIRECCION FISICA: [%d]", marco->pid, dir_fisica); // LOG OBLIGATORIO
    */
    return valor_leido;
}