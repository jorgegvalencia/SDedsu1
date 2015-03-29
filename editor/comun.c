/*
   Incluya en este fichero todas las implementaciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso intermediario), si es que las hubiera.
*/

#include "comun.h"
#include <stdio.h>

/* Método que lee un mensaje de evento y devuelve el tema y el valor */
void leer_msg_evento(msg_evento *evento /* E */, const char *tema, const char *valor /* S */){
	sprintf(evento->tema, "%s",(char *)tema);
	sprintf(evento->valor,"%s",(char *)valor);
}

/* Método que escribe un mensaje de evento dados el tema y el valor */
void escribir_msg_evento(const char *tema, const char *valor /* E */, msg_evento *evento /* S */){
	sprintf((char *)tema, "%s",evento->tema);
	sprintf((char *)valor, "%s",evento->valor);
}

/* Método que lee mensaje de suscripcíon */
void leer_msg_suscriptor(msg_suscriptor *suscripcion, int *cod_op, const char *tema){
	*cod_op = suscripcion->cod_op;
	sprintf(suscripcion->tema,"%s",(char *)tema);
}

/* Metodo que escribe mensaje de suscripcion */
void escribir_msg_suscriptor(int cod_op, const char *tema, msg_suscriptor *suscripcion){
	suscripcion->cod_op = cod_op;
	sprintf((char *)suscripcion, "%s",suscripcion->tema);
}
