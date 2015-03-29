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
