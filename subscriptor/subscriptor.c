#include "comun.h"
#include "edsu_comun.h"
#include "subscriptor.h"
#include <pthread.h>
#include <stdbool.h>

bool init = false;
void (*func_notif)(const char*, const char*);

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
	if(socket < 0){
		return -1;
	}

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
	/* Cerrar conexion */
	close(socket);
	return 0;
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
	if(socket < 0){
		return -1;
	}

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
	/* Cerrar conexion */
	close(socket);
	return 0;
}
int inicio_subscriptor(void (*notif_evento)(const char *, const char *),
                void (*alta_tema)(const char *),
                void (*baja_tema)(const char *)) {
	int socket_notif;
	int *s_conec;
	int size;
	int puerto;
	struct sockaddr_in tcp_addr_sub;
	struct sockaddr_in tcp_addr_interm;
	pthread_t thread_id;
	pthread_attr_t atrib_th;

	pthread_attr_init(&atrib_th);
	pthread_attr_setdetachstate(&atrib_th, PTHREAD_CREATE_DETACHED);
	// anotar funcion de notificacion de la aplicacion
	func_notif = notif_evento;	
	get_puerto(&puerto);

	socket_notif = abrir_conexion_tcp(puerto);
	if(socket < 0){
		return -1;
	}

	/* Asignacion de la direccion local (suscriptor) Puerto TCP*/    
	bzero((char *) &tcp_addr_sub, sizeof(tcp_addr_sub));

	tcp_addr_sub.sin_family = AF_INET;
	tcp_addr_sub.sin_addr.s_addr = INADDR_ANY;
	tcp_addr_sub.sin_port = htons(puerto)/* htons(port_tcp) Puerto para atender notificaciones */;

	if(bind(socket_notif, (struct sockaddr *) &tcp_addr_sub, sizeof(tcp_addr_sub)) < 0){
		fprintf(stderr,"SERVIDOR: Asignacion del puerto servidor: ERROR\n");
		close(socket_notif);
		exit(1);
	}
  	/* Aceptamos conexiones por el socket */
	if(listen(socket_notif,1)<0){
		fprintf(stderr,"SERVIDOR: Aceptacion de peticiones: ERROR\n");
		exit(1);
	}
	else{
		fprintf(stderr,"SERVIDOR: Aceptacion de peticiones: OK\n");
	}
	// anotar que la rutina inicio_subscriptor se ha ejecutado
	init = true;
	while(1){
		bzero((char *) &tcp_addr_interm, sizeof(tcp_addr_interm));
		s_conec=malloc(sizeof(int));
		size = sizeof(tcp_addr_interm);
		if((*s_conec=accept(socket_notif, (struct sockaddr *) &tcp_addr_interm, (socklen_t *)  &size)) < 0){
			fprintf(stderr,"SERVIDOR: Llegada de un mensaje: ERROR\n");
			close(socket_notif);
		}
		// conexion correcta
		// lanzar thread para escuchar notificaciones del intermediario
		pthread_create(&thread_id, &atrib_th, (void *)atender_notificaciones, s_conec);
	}

	close(socket_notif);
	return 0;
}

int atender_notificaciones(int *socket_notif){
	msg notification;
	/* Recibir peticion */
	// int socket_notif = (int) s_conec;
	recv(*socket_notif,&notification,sizeof(msg),0);
	/* Analizar peticion */
	if(ntohl(notification.cod_op)==EVENTO){
		func_notif((const char*)notification.tema, (const char*)notification.valor);
	}
	else{
		fprintf(stderr, "Codigo de operacion desconocido\n");
		return -1;
	}
	return 0;
}

/* Avanzada */
int fin_subscriptor() {
	return 0;
}

