#include "editor.h"
#include "comun.h"
#include "edsu_comun.h"

int generar_evento(const char *tema, const char *valor) {
	int socket;
	int respuesta;
	msg nuevo_evento;
	/* Crear evento */
	escribir_msg(EVENTO,tema,valor,&nuevo_evento);

	/* Abrir conexion */
	socket = abrir_conexion_tcp(0);

	/* Enviar el evento al intermediario */
	send(socket,&nuevo_evento,sizeof(msg),0);

	/* Esperar respuesta */
	recv(socket,&respuesta,sizeof(msg),0);
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

/* solo para la version avanzada */
int crear_tema(const char *tema) {
	return 0;
}

/* solo para la version avanzada */
int eliminar_tema(const char *tema) {
	return 0;
}

