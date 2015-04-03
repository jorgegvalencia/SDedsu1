#include "comun.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX 65
#define EVENTO_OK 0
#define EVENTO_ERROR -1

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

typedef struct entrada_tema{
	int id;
	char tema[64];
} EntradaTema;

typedef struct entrada_sub{
	int id;
	uint32_t sub;
} EntradaSub;

int service_port;

int n_subs; // numero de suscriptores totales
int n_topics; // numero de temas totales
int * n_sub_topic; // numero de suscriptores por cada tema
int ** suscriptores_temas;
EntradaTema * temas;
EntradaSub * suscriptores;

void printTemas(){
	int i;
	printf("| TopicId \t| Topic \t\t| \n");
	for(i=0; i<n_topics; i++){
		printf("| %d \t\t| %s \t\t|\n", temas[i].id, temas[i].tema);
	}
}

/* Devuelve el identificador de tema, o -1 si no esta en la lista de temas */
int getTopicId(char *topic){
	bool found = false;
	int topic_id = -1;

	int i;
	for(i=0; i<n_topics && !found; i++){
		// printf("%s\n", temas[i].tema);
		if(strcmp(temas[i].tema,topic) == 0){
			found = true;
			topic_id = temas[i].id;
		}
	}
	return topic_id;
}

/* Añade un tema a la lista de temas y devuelve su identificador o -1 si error */
int addTopic(char *topic){
	if((getTopicId(topic)) != -1){
		printf("Tema ya existe\n");
		return -1;
	}
	int id_topic = n_topics;
	EntradaTema new_entrada;
	new_entrada.id = id_topic;
	strcpy(new_entrada.tema,topic);
	temas[n_topics] = new_entrada;

	n_topics++;
	temas = realloc(temas,(n_topics+1)*sizeof(struct entrada_tema));
	if(temas == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	return id_topic;
}

/* Devuelve el identificador de suscriptor, o -1 si no esta en la lista de suscriptores */
int getSubId(uint32_t subscriber){
	int i;
	bool found;
	int sub_id = -1;
	for(i=0; i<n_subs && !found; i++){
		if(suscriptores[i].sub == subscriber){
			found = true;
			sub_id = i;
		}
	}
	return sub_id;
}

/* Añade un suscriptor a la lista de suscriptores y devuelve su identificador o -1 si error */
int addSub(uint32_t subscriber){
	/* Comprobar si suscriptor esta en la bbdd */
	if((getSubId(subscriber)) != -1){
		fprintf(stderr, "Suscriptor ya en la lista\n");
		return -1;
	}
	int id_sub = n_subs;
	EntradaSub new_entrada;
	new_entrada.id = id_sub;
	new_entrada.sub = subscriber;
	suscriptores[n_subs] = new_entrada;
	/* Si no esta, añadirlo */
	n_subs++;
	suscriptores = realloc(suscriptores,(n_subs+1)*sizeof(uint32_t));
	if(suscriptores == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	
	return id_sub;
}

/* Da de alta a un suscriptor al tema especificado */
int altaSubTopic(uint32_t subscriber, char *topic){
	int id_topic;
	int id_sub;
	/* Existe el topic ? */
	id_topic=getTopicId(topic);
	if(id_topic != -1){
		fprintf(stderr, "No existe el tema especificado\n");
		return -1;
	}
	/* Comprobar subscriber */
	id_sub=getSubId(subscriber);
	if(id_sub != -1){
	// si no esta, se añade
		if((addSub(subscriber)) == -1){
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
	id_topic=getTopicId(topic);
	if(id_topic != -1){
		fprintf(stderr, "No existe el tema especificado\n");
		return -1;
	}
	/* Comprobar subscriber */
	id_sub=getSubId(subscriber);
	if(id_sub != -1){
	// si no esta, se añade
		if((addSub(subscriber)) == -1){
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
	int sub_id;
	struct sockaddr_in tcp_addr_client;
	msg nuevo_evento;
	if(!(topic_id = getTopicId(topic))){
		fprintf(stderr, "Tema no valido\n");
		return -1;
	}
	escribir_msg(EVENTO,topic,value,&nuevo_evento);

	for(i=0;i<n_sub_topic[topic_id];i++){
		sub_id = suscriptores_temas[topic_id][i];
		bzero((char *) &tcp_addr_client, sizeof(tcp_addr_client));  // Inicializar estructura
		/* Establecer parametros de la direccion TCP del intermediario */
		tcp_addr_client.sin_family = AF_INET;
		tcp_addr_client.sin_port = htons(service_port);
		tcp_addr_client.sin_addr.s_addr = suscriptores[sub_id].sub; // tcp_addr_interm.sin_addr.s_addr = intermediario;
		if(connect(tcp_sd,(struct sockaddr*) &tcp_addr_client,sizeof(struct sockaddr_in))<0)
		{
			fprintf(stdout,"ERROR al conectar\n");
			close(tcp_sd);
			return -1;
		}
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

	n_subs = 0;
	n_topics = 0;

	n_sub_topic = (int*) malloc(sizeof(int));
	suscriptores_temas = (int**)malloc(sizeof(int*));
	temas = malloc(sizeof(struct entrada_tema));
	suscriptores = (EntradaSub*) malloc(sizeof(EntradaSub));

	/* Conexion TCP */
	int accept_sd;
	int tcp_sd;
	// int tcp_sr;
	int size;
	struct sockaddr_in tcp_addr_interm, tcp_addr_client;
	msg peticion;
	int respuesta;

	/* Manejo de mensajes */
	// char tema[64];
	// char valor[64];

	/* Parsear argumentos */
	if (argc!=3) {
		fprintf(stderr, "Uso: %s puerto fichero_temas\n", argv[0]);
		return 1;
	}

	service_port = atoi(argv[1]);
	if ((fichero_temas = fopen(argv[2], "r")) == NULL ){
		fprintf(stderr,"Fichero de temas no disponible\n");
		return -1;
	}
	
	/* Leer fichero de temas y crear estructura de temas-subscriptores */
	while (fgets(linea,MAX,fichero_temas)!= NULL){
		char *c = strchr(linea, '\n');
		if (c)
			*c = 0;
		addTopic(linea);
	}
	printTemas();
	fclose(fichero_temas);

	/* ------------- Preparar recepcion de mensajes ---------------- */

	/* Creacion del socket TCP de servicio */
	tcp_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(tcp_sd < 0){
		fprintf(stderr,"INTERMEDIARIO: Creacion del socket TCP: ERROR\n");
		return -1;
	}
	else{
		fprintf(stderr,"INTERMEDIARIO: Creacion del socket TCP: OK\n");
	}

  	/* Asignacion de la direccion local (del servidor) Puerto TCP*/    
	bzero((char *) &tcp_addr_interm, sizeof(tcp_addr_interm));

	tcp_addr_interm.sin_family = AF_INET;
	tcp_addr_interm.sin_addr.s_addr = INADDR_ANY;
	tcp_addr_interm.sin_port = htons(service_port);

	if(bind(tcp_sd, (struct sockaddr *) &tcp_addr_interm, sizeof(tcp_addr_interm)) < 0){
		fprintf(stderr,"INTERMEDIARIO: Asignacion del puerto servidor: ERROR\n");
		close(tcp_sd);
		return -1;
	}

  	/* Aceptamos conexiones por el socket */
	if(listen(tcp_sd,5)<0){
		fprintf(stderr,"INTERMEDIARIO: Aceptacion de peticiones: ERROR\n");
		return -1;
	}
	else{
		fprintf(stderr,"INTERMEDIARIO: Aceptacion de peticiones: OK\n");
	}

	/* Recibir mensajes de alta, baja o evento */
	while(1){
		/* Esperamos la llegada de una conexion */
		bzero((char *) &tcp_addr_client, sizeof(tcp_addr_client));
		size = sizeof(tcp_addr_client);
		if((accept_sd=accept(tcp_sd, (struct sockaddr *) &tcp_addr_client, (socklen_t *)  &size)) < 0){
			fprintf(stderr,"INTERMEDIARIO: Llegada de un mensaje: ERROR\n");
		}
		else{ // conexion correcta
			fprintf(stderr,"INTERMEDIARIO: Llegada de un mensaje: OK\n");
			switch(fork()){ //cambiar por threads, necesita acceso a la misma estructura de datos
				case -1:
				fprintf(stderr,"INTERMEDIARIO: Servicio no disponible\n");
				return -1;
					/* -------------- Servidor dedicado ---------------- */
				case 0:
					/* Recibir peticion */
					recv(accept_sd,(msg *)&peticion,sizeof(struct mensaje),0);
					printf("INTERMEDIARIO: Codigo de operacion: %d\n", ntohs(peticion.cod_op));
					/* Analizar peticion */
					if(ntohs(peticion.cod_op)==ALTA){
						// sprintf(tema, "%s",peticion.tema);
						respuesta=altaSubTopic(tcp_addr_client.sin_addr.s_addr, peticion.tema);
						/* Enviar respuesta */
						send(tcp_sd,&respuesta,1,0);
						close(tcp_sd);
					}
					else if(ntohs(peticion.cod_op)==BAJA){
						// sprintf(tema, "%s",peticion.tema);
						respuesta=bajaSubTopic(tcp_addr_client.sin_addr.s_addr, peticion.tema);
						/* Enviar respuesta */
						send(tcp_sd,&respuesta,1,0);
						close(tcp_sd);
					}
					else if(ntohs(peticion.cod_op)==EVENTO){
						// sprintf(tema, "%s",peticion.tema);
						// sprintf(valor,"%s",peticion.valor);

						/* Comprobar si tema existe */
						if((getTopicId(peticion.tema)) != -1){
							fprintf(stdout,"INTERMEDIARIO: Evento %s para el tema %s: OK\n", peticion.valor, peticion.tema);
							respuesta = EVENTO_OK;
						}
						else{
							fprintf(stdout,"INTERMEDIARIO: Evento %s para el tema %s: ERROR\n", peticion.valor, peticion.tema);
							respuesta = EVENTO_ERROR;
						}
						send(accept_sd,&respuesta,sizeof(int),0);
						close(accept_sd);

						// tcp_sr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						// if(tcp_sr < 0){
						// 	fprintf(stderr,"INTERMEDIARIO: Creacion del socket TCP: ERROR\n");
						// 	return -1;
						// }
						// else{
						// 	fprintf(stderr,"INTERMEDIARIO: Creacion del socket TCP: OK\n");
						// }

						// // Enviar notificacion a suscritos en el tema 
						// respuesta = push_notification(tcp_sr, peticion.tema, peticion.valor);
						// close(tcp_sr);
					}
					else{
						fprintf(stderr, "Codigo de operacion desconocido\n");
						return -1;
				}
				default:
				continue;
			}
		}
		close(tcp_sd);
		free(n_sub_topic);
		free(temas);
		free(suscriptores);
		free(suscriptores_temas);
		return 0;
	}
}
