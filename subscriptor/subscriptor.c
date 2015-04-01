#include "comun.h"
#include "edsu_comun.h"
#include "subscriptor.h"
#include <pthread.h>
#include <stdbool.h>

bool init = false;
void * func_notif;

int alta_subscripcion_tema(const char *tema) {
	int socket;
	int respuesta;
	msg suscripcion;

	/* Comprobar que se ha iniciado al suscriptor */
	if(!init){
		return -1;
	}
	/* Crear mensaje de alta */
	escribir_msg(ALTA,tema,NULL,&suscripcion);

	/* Abrir conexion */
	socket = abrir_conexion_tcp(0);

	/* Enviar la suscripcion al intermediario */
	send(socket,&suscripcion,sizeof(msg),0);

	/* Esperar respuesta */
	recv(socket,&respuesta,1,0);
	// if respuesta = -1 then return -1
	if(respuesta < 0){
	/* Cerrar conexion */
		close(socket);
		return -1;
	}
	// if respuesta = 0 then return 0
	else{
	/* Cerrar conexion */
		close(socket);
		return 0;
	}
}

int baja_subscripcion_tema(const char *tema) {
	int socket;
	int respuesta;
	msg suscripcion;

	/* Comprobar que se ha iniciado al suscriptor */
	if(!init){
		return -1;
	}
	/* Crear mensaje de alta */
	escribir_msg(BAJA,tema,NULL,&suscripcion);

	/* Abrir conexion */
	socket = abrir_conexion_tcp(0);

	/* Enviar la suscripcion al intermediario */
	send(socket,&suscripcion,sizeof(msg),0);

	/* Esperar respuesta */
	recv(socket,&respuesta,1,0);
	// if respuesta = -1 then return -1
	if(respuesta < 0){
	/* Cerrar conexion */
		close(socket);
		return -1;
	}
	// if respuesta = 0 then return 0
	else{
	/* Cerrar conexion */
		close(socket);
		return 0;
	}
}
int inicio_subscriptor(void (*notif_evento)(const char *, const char *),
                void (*alta_tema)(const char *),
                void (*baja_tema)(const char *)) {
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, (void *)notif_evento, NULL);
	init = true;
	return 0;
}

void atender_notificaciones(){

	abrir_conexion_tcp(66666);
}

/* Avanzada */
int fin_subscriptor() {
	return 0;
}

