/*
   Incluya en este fichero todas las implementaciones que pueden
   necesitar compartir los módulos editor y subscriptor,
   si es que las hubiera.
*/

#include "edsu_comun.h"

int tcp_sd; // descriptor de fichero del socket TCP
int port_tcp; // puerto TCP del intermediario
char *direccion; // direccion del intermediario
struct hostent *hp;
struct sockaddr_in tcp_interm_addr;

void get_direccion_intermediario(){
	char * endp; //puntero para la cadena no valida
	if(getenv("PUERTO") != NULL){
		port_tcp=strtol(getenv("PUERTO"),&endp,10);
	}
	if(getenv("SERVIDOR") != NULL){
		direccion=getenv("SERVIDOR");
	}
	hp = gethostbyname(direccion);
}

int abrir_conexion_tcp(){

	/* Creacion del socket TCP de servicio */
	tcp_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(tcp_sd < 0){
		fprintf(stderr,"SERVIDOR: Creacion del socket TCP: ERROR\n");
		exit(1);
	}
	else{
		fprintf(stderr,"SERVIDOR: Creacion del socket TCP: OK\n");
	}
	/* Nos conectamos al intermediario */
	bzero((char *) &tcp_interm_addr, sizeof(tcp_interm_addr));  // Inicializar estructura
	tcp_interm_addr.sin_family = AF_INET;
	memcpy (&(tcp_interm_addr.sin_addr), hp->h_addr, hp->h_length); // tcp_interm_addr.sin_addr.s_addr = intermediario;
	tcp_interm_addr.sin_port = htons(0);

	if(connect(tcp_sd,
		(struct sockaddr*)&tcp_interm_addr,
		sizeof(struct sockaddr_in))<0)
	{
		fprintf(stdout,"ERROR %d\n",errno);
		close(tcp_sd); exit(1);
	}
	fprintf(stdout,"OK\n");

	return(tcp_sd);
}
