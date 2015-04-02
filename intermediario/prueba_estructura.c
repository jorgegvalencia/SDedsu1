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

int main(int argc, char* argv[]){

	FILE * fichero_temas;
	char * topic;

	n_subs = 0;
	n_topics = 0;

	n_sub_topic = (int*) malloc(sizeof(int));
	suscriptores_temas = (int**)malloc(sizeof(int*));
	temas = malloc(sizeof(struct entrada_tema));
	suscriptores = (EntradaSub*) malloc(sizeof(EntradaSub));

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
	while (fgets(topic,64,fichero_temas)!= NULL){
		char *c = strchr(topic, '\n');
		if (c)
			*c = 0;
		addTopic(topic);
		printTemas();
	}

	fclose(fichero_temas);

	free(n_sub_topic);
	free(temas);
	free(suscriptores);
	free(suscriptores_temas);
	/* Bucle de free */
	return 0;

}

// suscriptores = (uint32_t*) realloc (suscriptores, (n_subs+1)*sizeof(uint32_t));
// n_sub_topic = (int*) realloc (n_sub_topic, (n_topics+1)*sizeof(int));

//    suscriptores_temas=(int**)realloc(suscriptores_temas,(n_topics+1)*sizeof(int*));
//    for (i=0; i<nrows; i++)
//       suscriptores_temas[i]=(int*)realloc(suscriptores_temas[i],(n_sub_topic[i]+1)*sizeof(int));

// temas = (char***) realloc(temas, (n_topics+1)*sizeof(char**));
