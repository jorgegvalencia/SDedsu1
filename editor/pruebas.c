#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>   
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Operaciones de suscriptor */
#define ALTA 0
#define BAJA 1
#define EVENTO 2

/* Mensaje de alta, baja o evento */
typedef struct mensaje
{
	int cod_op;
	char tema[64];
	char valor[64];
} msg;

char ** intermediario;

void escribir_msg(int cod_op, const char *tema, const char *valor, msg *peticion){
	
	/*Inicializar estructura */
	bzero((void*)peticion,sizeof(peticion));

	/* Escribir mensaje */
	peticion->cod_op = htons(cod_op);
	sprintf(peticion->tema, "%s",(char *)tema);
	sprintf(peticion->valor, "%s",(char *)valor);
}

int get_info_intermediario(int *port_tcp, char **direccion){
	char * endp; //puntero para la cadena no valida

	if((getenv("SERVIDOR")) != NULL){
		*direccion=getenv("SERVIDOR");
	}
	else{
		fprintf(stderr, "Direccion del intermediario no disponible\n");
		return -1;
	}
	if((getenv("PUERTO")) != NULL){
		*port_tcp=strtol(getenv("PUERTO"),&endp,10);
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
	struct sockaddr_in tcp_addr_interm;
	struct hostent *host;

	printf("Control Point 1\n");

	if((get_info_intermediario(&port_tcp,intermediario)) < 0){
		fprintf(stderr, "Error al obtener la direccion de intermediario\n");
		return -1;
	}
	if(puerto != 0){
		port_tcp = puerto;
	}

	printf("Control Point 2: %s\n", *intermediario);

	host = gethostbyname(*intermediario);
	//inet_ntoa

	printf("Control Point 3\n");

	/* Creacion del socket TCP de servicio */
	tcp_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(tcp_sd < 0){
		fprintf(stderr,"Creacion del socket TCP: ERROR\n");
		return -1;
	}
	else{
		fprintf(stderr,"Creacion del socket TCP: OK\n");
	}

	bzero((char *) &tcp_addr_interm, sizeof(tcp_addr_interm));  // Inicializar estructura

	/* Establecer parametros de la direccion TCP del intermediario */
	tcp_addr_interm.sin_family = AF_INET;
	memcpy (&(tcp_addr_interm.sin_addr), host->h_addr, host->h_length); // tcp_addr_interm.sin_addr.s_addr = intermediario;
	tcp_addr_interm.sin_port = htons(port_tcp);

	/* Nos conectamos al intermediario */
	if(connect(tcp_sd,(struct sockaddr*) &tcp_addr_interm,sizeof(struct sockaddr_in))<0)
	{
		fprintf(stdout,"ERROR %d\n",errno);
		close(tcp_sd);
		return -1;
	}
	fprintf(stdout,"Conexion establecida\n");
	return tcp_sd;
}

int main(int argc, char *argv[]) {

	/* Prueba escribir_msg */
	// msg mensaje;
	// char *tema = "futbol";
	// char *valor = "madrid";

	// escribir_msg(EVENTO, tema, valor, &mensaje);

	// fprintf(stdout, "Contenido del mensaje \n");
	// fprintf(stdout, "mensaje.cod_op: %d\n", ntohs(mensaje.cod_op));
	// fprintf(stdout, "mensaje.tema: %s\n", mensaje.tema);
	// fprintf(stdout, "mensaje.valor: %s\n", mensaje.valor);

	/* Prueba get_info_intermediario */
	char *servidor;
	int port_tcp;
	get_info_intermediario(&port_tcp,&servidor);

	printf("%s\n",servidor);

	/* Prueba abrir_conexion_tcp */
	// int socket ;
	// socket = abrir_conexion_tcp(0);
	// fprintf(stdout, "Valor del socket %d\n", socket);
	// if(socket < 0){
	// 	return -1;
	// }
	// close(socket);

	return 0;
}