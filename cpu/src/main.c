#include "../include/main.h"

int conexion;
char *ip_memoria;
char *puerto_memoria;
char *puerto_escucha_dispatch;
char *puerto_escucha_interrupt;
char *cantidad_entradas_tlb;
char *algoritmo_tlb;

t_log *logger;
t_config *config;

void inicializar_config(){
    logger = iniciar_logger("cpu.log", "CPU");
	config = iniciar_config("./cpu.config", "CPU");

    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    puerto_escucha_dispatch = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
    puerto_escucha_interrupt = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
    cantidad_entradas_tlb = config_get_string_value(config, "CANTIDAD_ENTRADAS_TLB");
    algoritmo_tlb = config_get_string_value(config, "ALGORITMO_TLB");
}


int main(int argc, char* argv[]) 
{
   inicializar_config();

	// Loggeamos el valor de config
	log_info(logger, "Iniciando CPU...");

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip_memoria, puerto_memoria);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje("Mensaje de CPU para memoria", conexion);

	// Armamos y enviamos el paquete
	paquete(conexion, logger);

	terminar_programa(conexion, logger, config);
}


