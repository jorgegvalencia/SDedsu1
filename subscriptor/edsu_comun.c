/*
   Incluya en este fichero todas las implementaciones que pueden
   necesitar compartir los módulos editor y subscriptor,
   si es que las hubiera.
*/

#include "edsu_comun.h"
#include <stdlib.h>

int get_puerto(int *port_tcp){
	char * endp; //puntero para la cadena no valida
	if(getenv("PUERTO") != NULL){
		*port_tcp=strtol(getenv("PUERTO"),&endp,10);
		// fprintf(stdout, "PUERTO: %d\n", *port_tcp);
	}
	else{
		fprintf(stderr, "Puerto de servicio del intermediario no disponible\n");
		return -1;
	}
	return 0;
}

int get_info_intermediario(int *port_tcp, char **direccion){
	char * endp; //puntero para la cadena no valida
	if(getenv("SERVIDOR") != NULL){
		*direccion=getenv("SERVIDOR");
		// fprintf(stdout, "SERVIDOR: %s\n", *direccion);
	}
	else{
		fprintf(stderr, "Direccion del intermediario no disponible\n");
		return -1;
	}
	if(getenv("PUERTO") != NULL){
		*port_tcp=strtol(getenv("PUERTO"),&endp,10);
		// fprintf(stdout, "PUERTO: %d\n", *port_tcp);
	}
	else{
		fprintf(stderr, "Puerto de servicio del intermediario no disponible\n");
		return -1;
	}
	return 0;
}

int abrir_conexion_tcp(int puerto){
	int port_tcp;
	int tcp_sd; // descriptor de fichero del socket TCP
	char * intermediario;
	struct sockaddr_in tcp_addr_interm;
	struct hostent *host;

	if((get_info_intermediario(&port_tcp,&intermediario)) < 0){
		fprintf(stderr, "Error al obtener la direccion de intermediario\n");
		return -1;
	}
	if(puerto != 0){
		port_tcp = puerto;
	}

	host = gethostbyname(intermediario);
	//inet_ntoa

	/* Creacion del socket TCP de servicio */
	tcp_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(tcp_sd < 0){
		fprintf(stderr,"Creacion del socket TCP: ERROR\n");
		return -1;
	}
	// else{
	// 	fprintf(stderr,"Creacion del socket TCP: OK\n");
	// }
	bzero((char *) &tcp_addr_interm, sizeof(tcp_addr_interm));  // Inicializar estructura

	/* Establecer parametros de la direccion TCP del intermediario */
	tcp_addr_interm.sin_family = AF_INET;
	// tcp_addr_interm.sin_addr.s_addr = inet_addr("127.0.0.1");
	memcpy (&(tcp_addr_interm.sin_addr), host->h_addr, host->h_length); // tcp_addr_interm.sin_addr.s_addr = intermediario;
	tcp_addr_interm.sin_port = htons(port_tcp);

	/* Nos conectamos al intermediario */
	if(connect(tcp_sd,(struct sockaddr*) &tcp_addr_interm,sizeof(struct sockaddr_in))<0)
	{
		fprintf(stdout,"ERROR %d\n",errno);
		close(tcp_sd);
		return -1;
	}
	// fprintf(stdout,"Conexion establecida\n");
	return tcp_sd;
}