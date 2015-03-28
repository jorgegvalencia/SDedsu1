/*
   Incluya en este fichero todas las definiciones que pueden
   necesitar compartir los módulos editor y subscriptor,
   si es que las hubiera.
*/
#include "stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>   
#include <errno.h>

int tcp_sd; // descriptor de fichero del socket TCP
int port_tcp; // puerto TCP del intermediario
char *direccion; // direccion del intermediario
struct hostent *hp;
struct sockaddr_in tcp_interm_addr;

void get_direccion_intermediario();
int abrir_conexion_tcp();
