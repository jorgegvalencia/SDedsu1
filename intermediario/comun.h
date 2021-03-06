/*
   Incluya en este fichero todas las definiciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso intermediario), si es que las hubiera.
*/

/* Operaciones de suscriptor */
#define ALTA 0
#define BAJA 1
#define EVENTO 2

/* Mensaje de alta, baja o evento */
typedef struct mensaje
{
	int cod_op;
	int port;
	char tema[64];
	char valor[64];
} msg;

void escribir_msg(int cod_op, int port, const char *tema, const char *valor, msg *peticion);
