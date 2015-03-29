#include "comun.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX 65

int push_notifications();

void parse_args();

int main(int argc, char *argv[]) {

	/* Temas */
	FILE * fichero_temas;
	char linea[64];

	/* Conexion TCP */
	int tcp_sd, port_tcp;
	int size;
	struct sockaddr_in tcp_addr_interm, tcp_addr_client;
	msg peticion;
	int respuesta;

	/* Manejo de mensajes */
	char tema[64];
	char valor[64];

	/* Parsear argumentos */
	if (argc!=3) {
		fprintf(stderr, "Uso: %s puerto fichero_temas\n", argv[0]);
		return 1;
	}

	port_tcp = *argv[1];
	if ((fichero_temas = fopen(argv[2], "r")) == NULL ){
		fprintf(stderr,"Fichero de temas no disponible\n");
		return -1;
	}
	
	/* Leer fichero de temas y crear estructura de temas-subscriptores */
	while (fgets(linea,MAX,fichero_temas)!= NULL){

	}
	fclose(fichero_temas);

	/* ------------- Preparar recepcion de mensajes ---------------- */

	/* Creacion del socket TCP de servicio */
	tcp_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(tcp_sd < 0){
		fprintf(stderr,"SERVIDOR: Creacion del socket TCP: ERROR\n");
		exit(1);
	}
	else{
		fprintf(stderr,"SERVIDOR: Creacion del socket TCP: OK\n");
	}

  	/* Asignacion de la direccion local (del servidor) Puerto TCP*/    
	bzero((char *) &tcp_addr_interm, sizeof(tcp_addr_interm));

	tcp_addr_interm.sin_family = AF_INET;
	tcp_addr_interm.sin_addr.s_addr = INADDR_ANY;
	tcp_addr_interm.sin_port = htons(port_tcp);

	if(bind(tcp_sd, (struct sockaddr *) &tcp_addr_interm, sizeof(tcp_addr_interm)) < 0){
		fprintf(stderr,"SERVIDOR: Asignacion del puerto servidor: ERROR\n");
		close(tcp_sd);
		exit(1);
	}

  	/* Aceptamos conexiones por el socket */
	if(listen(tcp_sd,5)<0){
		fprintf(stderr,"SERVIDOR: Aceptacion de peticiones: ERROR\n");
		exit(1);
	}
	else{
		fprintf(stderr,"SERVIDOR: Aceptacion de peticiones: OK\n");
	}

	/* Recibir mensajes de alta, baja o evento */
	while(1){
		/* Esperamos la llegada de una conexion */
		bzero((char *) &tcp_addr_client, sizeof(tcp_addr_client));
		size = sizeof(tcp_addr_client);
		if((accept(tcp_sd, (struct sockaddr *) &tcp_addr_client, (socklen_t *)  &size)) < 0){
			fprintf(stderr,"SERVIDOR: Llegada de un mensaje: ERROR\n");
		}
		else{
			fprintf(stderr,"SERVIDOR: Llegada de un mensaje: OK\n");
			switch(fork()){
				case -1:

				fprintf(stderr,"SERVIDOR: Servicio no disponible\n");
				exit(1);
				/* -------------- Servidor dedicado ---------------- */
				case 0:

				/* Recibir peticion */
				recv(tcp_sd,&peticion,sizeof(msg),0);

				/* Analizar peticion */
				if(ntohl(peticion.cod_op)==ALTA){
					sprintf(tema, "%s",peticion.tema);
					/* Comprobar si tema valido */

					/* Comprobar si suscriptor dado ya de alta */

					/* Añadir suscriptor a lista de interesados en el tema */
					respuesta=0;
				}
				else if(ntohl(peticion.cod_op)==BAJA){
					sprintf(tema, "%s",peticion.tema);
					/* Comprobar si tema valido */

					/* Comprobar si suscriptor dado ya de alta */

					/* Eliminar suscriptor de la lista de interesados en el tema */
					respuesta=0;
				}
				else if(ntohl(peticion.cod_op)==EVENTO){
					sprintf(tema, "%s",peticion.tema);
					/* Comprobar si tema valido */

					sprintf(valor,"%s",peticion.valor);
					respuesta=0;
					/* Enviar notificacion a suscritos en el tema */
				}
				else{
					fprintf(stderr, "Codigo de operacion desconocido\n");
					return -1;
				}

				/* Enviar respuesta */
				send(tcp_sd,&respuesta,1,0);
				close(tcp_sd);
				return 0;
				default:

				continue;
			}
		}
		return 0;
	}
}
