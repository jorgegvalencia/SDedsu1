#include "comun.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX 65

/* ---------------- Prototipos de funciones ----------------*/

/* Añade un suscriptor a la lista de suscriptores y devuelve su identificador o -1 si error */
int addSub(uint32_t subscriber);
/* Borra un suscriptor de la lista de suscriptores y devuelve 0 o -1 si error */
int delSub(uint32_t subscriber);
/* Devuelve el identificador de suscriptor, o -1 si no esta en la lista de suscriptores */
int getSubId(uint32_t subscriber);

/* Añade un tema a la lista de temas y devuelve su identificador o -1 si error */
int addTopic(char *topic);
/* Borra un tema de la lista de temas y devuelve 0 o -1 si error */
int delTopic(char *topic);
/* Devuelve el identificador de tema, o -1 si no esta en la lista de temas */
int getTopicId(char *topic);

/* Da de alta a un suscriptor al tema especificado */
int altaSubTopic(uint32_t subscriber, char* topic);
/* Da de alta a un suscriptor al tema especificado */
int bajaSubTopic(uint32_t subscriber, char* topic);
/* Envia un evento a los suscriptores correspondientes */
int push_notification(int tcp_sd, char *topic, char *valor);

/* --------------- Variables ---------------- */

int n_subs;
int n_topics;
uint32_t * suscriptores; // suscriptores[1] = sub1 (en formato de red)
char ** temas; // temas[1] = tema1
int ** suscriptores_temas; // suscriptores_temas[tema][1] = sub1
int * n_sub_topic; // n_sub_topic[tema] = numero de subs en el topic

/* --------------- Funciones ----------------- */

/* Añade un suscriptor a la lista de suscriptores y devuelve su identificador o -1 si error */
int addSub(uint32_t subscriber){
	/* Comprobar si suscriptor esta en la bbdd */
	if(getSubId(subscriber)){
		fprintf(stderr, "Suscriptor ya en la lista\n");
		return -1;
	}
	/* Si no esta, añadirlo */
	suscriptores = realloc(suscriptores,(n_subs+1)*sizeof(uint32_t));
	if(suscriptores == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	suscriptores[n_subs] = subscriber;
	n_subs++;
	return n_subs;
}
/* Devuelve el identificador de suscriptor, o -1 si no esta en la lista de suscriptores */
int getSubId(uint32_t subscriber){
	int i;
	for(i=0; i<=n_subs; i++){
		if(suscriptores[i] == subscriber){
			return i;
		}
	}
	return -1;
}
/* Añade un tema a la lista de temas y devuelve su identificador o -1 si error */
int addTopic(char *topic){
	/* Comprobar que el tema no esta en la bbdd */
	if(getTopicId(topic)){
		fprintf(stderr, "Tema ya en la lista\n");
		return -1;
	}
	/* Si no esta, añadirlo */
	temas = realloc(temas,(n_topics+1)*sizeof(char*));
	if(temas == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	temas[n_topics] = topic; //nuevo topic
	
	/* Inicializar numero de suscriptores al tema */
	n_sub_topic = realloc(n_sub_topic,(n_topics+1)*sizeof(int));
	if(n_sub_topic == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	n_sub_topic[n_topics]=0; //0 subs en el topic
	n_topics++;
	return n_topics;
}
/* Devuelve el identificador de tema, o -1 si no esta en la lista de temas */
int getTopicId(char *topic){
	int i;
	for(i=0; i<=n_topics; i++){
		if(temas[i] == topic){
			return i;
		}
	}
	return -1;
}
/* Da de alta a un suscriptor al tema especificado */
int altaSubTopic(uint32_t subscriber, char *topic){
	int id_topic;
	int id_sub;
	/* Existe el topic ? */
	if(!(id_topic=getTopicId(topic))){
		fprintf(stderr, "No existe el tema especificado\n");
		return -1;
	}
	/* Comprobar subscriber */
	if(!(id_sub=getSubId(subscriber))){
	// si no esta, se añade
		if(!(addSub(subscriber))){
			fprintf(stderr, "No es posible añadir el suscriptor\n");
			return -1;
		}
	}
	/* Comprobar que no esta dado de alta */
	int i;
	for(i=0;i<n_sub_topic[id_topic];i++){
		if((suscriptores_temas[id_topic][i]) == id_sub){
			fprintf(stderr, "Suscriptor dado ya de alta\n");
			return -1;
		}
	}
	// dar de alta
	suscriptores_temas[id_topic] = realloc(suscriptores_temas[id_topic],
		(n_sub_topic[id_topic]+1)*sizeof(int));
	if(suscriptores_temas[id_topic] == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	n_sub_topic[id_topic]++;
	return 0;
}
/* Da de alta a un suscriptor al tema especificado */
int bajaSubTopic(uint32_t subscriber, char *topic){
	int id_topic;
	int id_sub;
	int id_last_sub;
	int last_sub_pos;
	/* Existe el topic ? */
	if(!(id_topic=getTopicId(topic))){
		fprintf(stderr, "No existe el tema especificado\n");
		return -1;
	}
	/* Comprobar subscriber */
	if(!(id_sub=getSubId(subscriber))){
	// si no esta, se añade
		if(!(addSub(subscriber))){
			fprintf(stderr, "Error: No es posible añadir el suscriptor\n");
			return -1;
		}
	}
	/* Comprobar que esta dado de alta */
	int i;
	bool found = false;
	for(i=0;i<n_sub_topic[id_topic] && !found;i++){
		if((suscriptores_temas[id_topic][i]) == id_sub){
			found = true;
		}
	}
	if(!found){
		fprintf(stderr,"El suscriptor no esta dado de alta en este tema\n");
		return -1;
	}

	// dar de baja
	last_sub_pos = n_sub_topic[id_topic];
	id_last_sub = suscriptores_temas[id_topic][last_sub_pos];

	bool deleted;
	for(i=0;i<n_sub_topic[id_topic] && !deleted;i++){
		if((suscriptores_temas[id_topic][i]) == id_sub){
			// Determinar Id del ultimo suscrito al tema
			suscriptores_temas[id_topic][i] = id_last_sub;
			// Reubicar memoria dinamica
			suscriptores_temas[id_topic] = realloc(suscriptores_temas[id_topic],
				(n_sub_topic[id_topic]-1)*sizeof(int));
			if(suscriptores_temas[id_topic] == NULL){
				fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
				return -1;
			}
			// Decrementar el numero de suscriptores
			n_sub_topic[id_topic]--;
			deleted=true;
		}
	}
	if(!deleted){
		fprintf(stderr, "Error al dar de baja al suscriptor\n");
		return -1;
	}
	return 0;
}
/* Envia un evento a los suscriptores correspondientes */
int push_notification(int tcp_sd, char *topic, char *value){
	int i;
	int topic_id;
	msg nuevo_evento;
	if(!(topic_id = getTopicId(topic))){
		fprintf(stderr, "Tema no valido\n");
		return -1;
	}
	for(i=0;i<n_sub_topic[topic_id];i++){
		/* Mandar evento a suscriptor i*/
		escribir_msg(EVENTO,topic,value,&nuevo_evento);
		/* Abrir conexion con nuevo socket tcp y puerto de notificaciones */
		send(tcp_sd,&nuevo_evento,sizeof(msg),0);
	}
	return 0;
}

int main(int argc, char *argv[]) {

	/* Temas y suscriptores */
	FILE * fichero_temas;
	char linea[64];

	n_subs=0;
	n_topics=0;
	suscriptores = malloc(sizeof(uint32_t));
	temas = malloc(sizeof(char*));
	suscriptores_temas = malloc(sizeof(int**));
	n_sub_topic = malloc(sizeof(int));

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
		addTopic(linea);
		fprintf(stdout, "Añadido el tema <%s>\n", linea);
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
		else{ // conexion correcta
			fprintf(stderr,"SERVIDOR: Llegada de un mensaje: OK\n");
			switch(fork()){ //cambiar por threads, necesita acceso a la misma estructura de datos
				case -1:
					fprintf(stderr,"SERVIDOR: Servicio no disponible\n");
					exit(1);
					/* -------------- Servidor dedicado ---------------- */
				case 0:
					/* Recibir peticion */
					recv(tcp_sd,&peticion,sizeof(msg),0);
					/* Analizar peticion */
					if(ntohl(peticion.cod_op)==ALTA){
					// sprintf(tema, "%s",peticion.tema);
						respuesta=altaSubTopic(tcp_addr_client.sin_addr.s_addr, peticion.tema);
					}
					else if(ntohl(peticion.cod_op)==BAJA){
					// sprintf(tema, "%s",peticion.tema);
						respuesta=bajaSubTopic(tcp_addr_client.sin_addr.s_addr, peticion.tema);
					}
					else if(ntohl(peticion.cod_op)==EVENTO){
						sprintf(tema, "%s",peticion.tema);
					/* Comprobar si tema valido */
						sprintf(valor,"%s",peticion.valor);
					/* Enviar notificacion a suscritos en el tema */
						respuesta = push_notification(tcp_sd, peticion.tema, peticion.valor);
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
