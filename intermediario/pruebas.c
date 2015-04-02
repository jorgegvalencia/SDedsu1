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

#define MAX 64

int n_subs;
int n_topics;
uint32_t * suscriptores; // suscriptores[1] = sub1 (en formato de red)
char *** temas; // temas[1] = tema1
int ** suscriptores_temas; // suscriptores_temas[tema][1] = sub1
int * n_sub_topic; // n_sub_topic[tema] = numero de subs en el topic

/* --------------- Funciones ----------------- */

void printTemas(){
	int i;
	printf("| TopicId \t| Topic \t\t| \n");
	for(i=0; i<n_topics; i++){
		printf("| %d \t\t| %s \t\t|\n", i, *temas[i]);
	}
}

/* Añade un tema a la lista de temas y devuelve su identificador o -1 si error */
int addTopic(char *topic){
	char buffer[64];
	int topic_id;
	int checkid;
	checkid = getTopicId(topic);
	printf("%d\n", checkid);
	/* Comprobar que el tema no esta en la bbdd */
	// if(checkid>-1){
	// 	fprintf(stderr, "Tema ya en la lista\n");
	// 	return -1;
	// }
	/* Si no esta, añadirlo */
	temas = (char ***) realloc(temas,(n_topics+1)*sizeof(char**));
	if(temas == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	topic_id=n_topics;
	// memcpy (&temas[topic_id], topic, strlen(topic));
	// strcpy(buffer,topic); //nuevo topic
	*temas[n_topics] = topic;
	
	/* Inicializar numero de suscriptores al tema */
	n_sub_topic = (int*) realloc(n_sub_topic,(n_topics+1)*sizeof(int));
	if(n_sub_topic == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	n_sub_topic[topic_id]=0; //0 subs en el topic

	n_topics++;
	return topic_id;
}
/* Devuelve el identificador de tema, o -1 si no esta en la lista de temas */
int getTopicId(char *topic){
	int i;
	bool found = false;
	int id = -1;
	for(i=0; i<n_topics && !found; i++){
		if(strcmp(*temas[i],topic) == 0){
			found = true;
			id = i;
		}
	}
	return id;
}

int main(int argc, char* argv[]){

	/* Temas y suscriptores */
	FILE * fichero_temas;
	char * topic;

	n_subs=0;
	n_topics=0;
	suscriptores = (uint32_t*) malloc(sizeof(uint32_t));
	int i;
	temas = (char***) malloc(sizeof(char**));
	for (i=0; i<n_topics; i++)
		temas[i]=(char**)realloc(temas[i],64*sizeof(char*));

	suscriptores_temas=(int**)realloc(suscriptores_temas,(n_topics+1)*sizeof(int*));
	for (i=0; i<n_topics; i++)
		suscriptores_temas[i]=(int*)realloc(suscriptores_temas[i],(n_sub_topic[i]+1)*sizeof(int));
	n_sub_topic = (int*) malloc(sizeof(int));

	/* Parsear argumentos */
	if (argc!=2) {
		fprintf(stderr, "Uso: %s puerto fichero_temas\n", argv[0]);
		return 1;
	}

	if ((fichero_temas = fopen(argv[1], "r")) == NULL ){
		fprintf(stderr,"Fichero de temas no disponible\n");
		return -1;
	}

	/* Leer fichero de temas y crear estructura de temas-subscriptores */
	while (fgets(topic,MAX,fichero_temas)!= NULL){
		char *c = strchr(topic, '\n');
		if (c)
			*c = 0;
		printf("%s\n", topic);
		addTopic(topic);
		printTemas();
	}

	fclose(fichero_temas);
	free(suscriptores);
	free(temas);
	free(suscriptores_temas);
	free(n_sub_topic);
	return 0;
}