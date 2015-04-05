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
	// printf("EDITOR: Enviando evento a INTERMEDIARIO\n");
	/* Enviar el evento al intermediario */
	send(socket,&nuevo_evento,sizeof(struct mensaje),0);

	// printf("EDITOR: Esperando respuesta de INTERMEDIARIO\n");
	/* Esperar respuesta */
	recv(socket,&respuesta,sizeof(int),0);
	// if respuesta = -1 then return -1
	if(respuesta < 0){
	/* Cerrar conexion */
		// printf("EDITOR: Evento RECHAZADO por intermediario\n");
		close(socket);
		return -1;
	}
	// if respuesta = 0 then return 0
	else{
	/* Cerrar conexion */
		// printf("EDITOR: Evento ACEPTADO por intermediario\n");
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

