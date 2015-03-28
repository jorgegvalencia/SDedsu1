/*
   Incluya en este fichero todas las definiciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso intermediario), si es que las hubiera.
*/

/* Operaciones de suscriptor */
#define ALTA 0
#define BAJA 1

typedef struct msg_evento
{
	char tema[64];
	char valor[64];
} msg_evento;

typedef struct msg_suscriptor
{
	int op;
	char tema[64];
} msg_suscriptor;

void leer_msg_evento();
