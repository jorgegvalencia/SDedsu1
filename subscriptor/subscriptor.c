#include "comun.h"
#include "edsu_comun.h"
#include "subscriptor.h"
#include <pthread.h>
#include <stdbool.h>

int atender_notificaciones();
bool init = false;
bool escuchando = false;
void (*func_notif)(const char*, const char*);
void (*func_notif_alta)(const char*);
void (*func_notif_baja)(const char*);
int puerto_oyente;
msg notification;
int socket_notif;
int s_conec;
int size;
struct sockaddr_in tcp_addr_sub;
struct sockaddr_in tcp_addr_interm;
struct sockaddr_in addr_tcp;

int alta_subscripcion_tema(const char *tema) {
	int socket;
	int respuesta;
	msg suscripcion;

	/* Comprobar que se ha iniciado al suscriptor */
	if(!init){
		return -1;
	}
	/* Crear mensaje de alta */
	escribir_msg(ALTA,puerto_oyente,tema,"0",&suscripcion);

	/* Abrir conexion */
	socket = abrir_conexion_tcp(0);
	if(socket < 0){
		return -1;
	}

	/* Enviar la suscripcion al intermediario */
	send(socket,&suscripcion,sizeof(struct mensaje),0);

	/* Esperar respuesta */
	recv(socket,&respuesta,sizeof(int),0);

	/* Cerrar conexion */
	close(socket);
	return respuesta;
}

int baja_subscripcion_tema(const char *tema) {
	int socket;
	int respuesta;
	msg suscripcion;

	/* Comprobar que se ha iniciado al suscriptor */
	if(!init){
		return -1;
	}
	/* Crear mensaje de baja */
	escribir_msg(BAJA,puerto_oyente,tema,"0",&suscripcion);

	/* Abrir conexion */
	socket = abrir_conexion_tcp(0);
	if(socket < 0){
		return -1;
	}

	/* Enviar la suscripcion al intermediario */
	send(socket,&suscripcion,sizeof(struct mensaje),0);

	/* Esperar respuesta */
	recv(socket,&respuesta,sizeof(int),0);

	/* Cerrar conexion */
	close(socket);
	return respuesta;
}
int inicio_subscriptor(void (*notif_evento)(const char *, const char *),
	void (*alta_tema)(const char *),
	void (*baja_tema)(const char *)) {
	
	int socket_d;
	int respuesta;
	msg suscripcion;
	pthread_t thread_id;
	pthread_attr_t atrib_th;

	func_notif = notif_evento;	// anotar funcion de notificacion de la aplicacion
	func_notif_alta = alta_tema;
	func_notif_baja = baja_tema;

	/* Recibir peticion */
	socket_notif = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(socket_notif < 0){
		return -1;
	}

	/* Asignacion de la direccion local (suscriptor) Puerto TCP*/    
	bzero((char *) &tcp_addr_sub, sizeof(tcp_addr_sub));

	tcp_addr_sub.sin_family = AF_INET;
	tcp_addr_sub.sin_addr.s_addr = INADDR_ANY;
	tcp_addr_sub.sin_port = htons(0); //Puerto para atender notificaciones

	if(bind(socket_notif, (struct sockaddr *) &tcp_addr_sub, sizeof(tcp_addr_sub)) < 0){
		close(socket_notif);
		exit(1);
	}
	int sa_len = sizeof(addr_tcp);
	getsockname(socket_notif, (struct sockaddr *) &addr_tcp, (socklen_t *) &sa_len);
	puerto_oyente = ntohs(addr_tcp.sin_port);

  	/* Aceptamos conexiones por el socket */
	if(listen(socket_notif,5)<0){
		exit(1);
	}

	/* Lanzar thread para escuchar notificaciones del intermediario */
	pthread_attr_init(&atrib_th);
	pthread_attr_setdetachstate(&atrib_th, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread_id, &atrib_th, (void *)atender_notificaciones,NULL);

	/* ------------ Avisar al intermediario ------------- */

	/* Crear mensaje de alta */
	escribir_msg(INIT_SUB,puerto_oyente,"metatema","0",&suscripcion);

	/* Abrir conexion */
	socket_d = abrir_conexion_tcp(0);
	if(socket_d < 0){
		return -1;
	}

	/* Enviar la suscripcion al intermediario */
	send(socket_d,&suscripcion,sizeof(struct mensaje),0);

	/* Esperar respuesta */
	recv(socket_d,&respuesta,sizeof(int),0);

	/* Cerrar conexion */
	close(socket_d);

	if(respuesta < 0){
		return respuesta;
	}
	/* ------------ Avisar al intermediario ------------- */

	init = true; // anotar que la rutina inicio_subscriptor se ha ejecutado
	return respuesta;
}

/* Avanzada */
int fin_subscriptor() {
	int socket_d;
	int respuesta;
	msg suscripcion;
	/* ------------ Avisar al intermediario ------------- */

	/* Crear mensaje de alta */
	escribir_msg(FIN_SUB,puerto_oyente,"metatema","0",&suscripcion);

	/* Abrir conexion */
	socket_d = abrir_conexion_tcp(0);
	if(socket_d < 0){
		return -1;
	}

	/* Enviar la suscripcion al intermediario */
	send(socket_d,&suscripcion,sizeof(struct mensaje),0);

	/* Esperar respuesta */
	recv(socket_d,&respuesta,sizeof(int),0);

	/* Cerrar conexion */
	close(socket_d);

	/* ------------ Avisar al intermediario ------------- */
	return respuesta;
}

int atender_notificaciones(){
	while(1){
		bzero((char *) &tcp_addr_interm, sizeof(tcp_addr_interm));
		size = sizeof(tcp_addr_interm);
		if((s_conec=accept(socket_notif, (struct sockaddr *) &tcp_addr_interm, (socklen_t *)  &size)) < 0){
			close(socket_notif);
			close(s_conec);
			continue;
		}
		recv(s_conec,&notification,sizeof(struct mensaje),0);
		/* Analizar peticion */
		switch(ntohs(notification.cod_op)){
			case EVENTO:
				func_notif((const char*)notification.tema, (const char*)notification.valor);
				break;
			case CREAR_TEMA:
				func_notif_alta((const char*) notification.valor);
				break;
			case ELIMINAR_TEMA:
				func_notif_baja((const char*) notification.valor);
				break;
			default:
				break;
		}
		// if(ntohs(notification.cod_op)==EVENTO){
		// 	func_notif((const char*)notification.tema, (const char*)notification.valor);
		// }
		// else if(ntohs(notification.cod_op)==CREAR_TEMA){
		// 	func_notif_alta((const char*) notification.valor);
		// }
		// else if(ntohs(notification.cod_op)==ELIMINAR_TEMA){
		// 	func_notif_baja((const char*) notification.valor);
		// }
		close(s_conec);
	}
	return 0;
	close(socket_notif);
}