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
	socket = abrir_conexion_tcp(0);
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
	return 0;
}

/* solo para la version avanzada */
int eliminar_tema(const char *tema) {
	return 0;
}

