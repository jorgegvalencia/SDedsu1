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

char *intermediario;

int get_info_intermediario(int *port_tcp, char *direccion);
int abrir_conexion_tcp(int puerto);