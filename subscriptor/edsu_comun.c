/*
   Incluya en este fichero todas las implementaciones que pueden
   necesitar compartir los módulos editor y subscriptor,
   si es que las hubiera.
*/

#include "edsu_comun.h"

int tcp_sd; // descriptor de fichero del socket TCP
int port_tcp; // puerto TCP del intermediario
char *direccion; // direccion del intermediario
struct hostent *host;
struct sockaddr_in tcp_addr_interm;

int get_direccion_intermediario(){
	char * endp; //puntero para la cadena no valida
	if(getenv("SERVIDOR") != NULL){
		direccion=getenv("SERVIDOR");
		host = gethostbyname(direccion);
	}
	else{
		fprintf(stderr, "Direccion del intermediario no disponible\n");
		return -1;
	}
	if(getenv("PUERTO") != NULL){
		port_tcp=strtol(getenv("PUERTO"),&endp,10);
	}
	else{
		fprintf(stderr, "Puerto de servicio del intermediario no disponible\n");
		return -1;
	}
	return 0;
}

int abrir_conexion_tcp(){

	if(get_direccion_intermediario() < 0){
		fprintf(stderr, "No se ha podido resolver la direccion del intermediario\n");
		exit(1);
	}

	/* Creacion del socket TCP de servicio */
	tcp_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(tcp_sd < 0){
		fprintf(stderr,"Creacion del socket TCP: ERROR\n");
		exit(1);
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
		exit(1);
	}
	fprintf(stdout,"Conexion establecida\n");
	return(tcp_sd);
}
