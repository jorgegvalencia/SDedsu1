/*
   Incluya en este fichero todas las implementaciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso intermediario), si es que las hubiera.
*/

#include "comun.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

void escribir_msg(int cod_op, const char *tema, const char *valor, msg *peticion){
	
	/*Inicializar estructura */
	bzero((void*)peticion,sizeof(peticion));

	/* Escribir mensaje */
	peticion->cod_op = htons(cod_op);
	sprintf(peticion->tema, "%s",(char *)tema);
	sprintf(peticion->valor, "%s",(char *)valor);
}

