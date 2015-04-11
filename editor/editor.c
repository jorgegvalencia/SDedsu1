#include "editor.h"
#include "comun.h"
#include "edsu_comun.h"

int generar_evento(const char *tema, const char *valor) {
	int socket;
	int respuesta;
	msg nuevo_evento;
	/* Crear evento */
	escribir_msg(EVENTO,0,tema,valor,&nuevo_evento);

	/* Abrir conexion */
	socket = abrir_conexion_tcp(0); // 0 = puerto_intermediario
	if(socket < 0){
		return -1;
	}
	/* Enviar el evento al intermediario */
	send(socket,&nuevo_evento,sizeof(struct mensaje),0);

	/* Esperar respuesta */
	recv(socket,&respuesta,sizeof(int),0);
	close(socket);
	return respuesta;
}

/* solo para la version avanzada */
int crear_tema(const char *tema) {
	int socket;
	int respuesta;
	msg nuevo_evento;
	/* Crear evento */
	escribir_msg(CREAR_TEMA,0,tema,"0",&nuevo_evento);

	/* Abrir conexion */
	socket = abrir_conexion_tcp(0); // 0 = puerto_intermediario
	if(socket < 0){
		return -1;
	}
	/* Enviar el nuevo tema al intermediario */
	send(socket,&nuevo_evento,sizeof(struct mensaje),0);

	/* Esperar respuesta */
	recv(socket,&respuesta,sizeof(int),0);
	close(socket);
	return respuesta;
}

/* solo para la version avanzada */
int eliminar_tema(const char *tema) {
	int socket;
	int respuesta;
	msg nuevo_evento;
	/* Crear evento */
	escribir_msg(ELIMINAR_TEMA,0,tema,"0",&nuevo_evento);

	/* Abrir conexion */
	socket = abrir_conexion_tcp(0); // 0 = puerto_intermediario
	if(socket < 0){
		return -1;
	}
	/* Enviar el tema a eliminar al intermediario */
	send(socket,&nuevo_evento,sizeof(struct mensaje),0);

	/* Esperar respuesta */
	recv(socket,&respuesta,sizeof(int),0);
	close(socket);
	return respuesta;
}

