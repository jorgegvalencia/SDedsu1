/*
   Incluya en este fichero todas las definiciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso intermediario), si es que las hubiera.
*/

/* Operaciones de suscriptor */
#define ALTA 0
#define BAJA 1

/* Mensaje de evento que envia el editor y que recibe el suscriptor */
typedef struct msg_evento
{
	char tema[64];
	char valor[64];
} msg_evento;

/* Mensaje de alta o baja de un tema por parte del suscriptor*/
typedef struct msg_suscriptor
{
	int cod_op;
	char tema[64];
} msg_suscriptor;

/* Método que lee un mensaje de evento y devuelve el tema y el valor */
void leer_msg_evento(msg_evento *evento /* E */, const char *tema, const char *valor /* S */);

/* Método que escribe un mensaje de evento dados el tema y el valor */
void escribir_msg_evento(const char *tema, const char *valor /* E */, msg_evento *evento /* S */);

/* Método que lee mensaje de suscripcíon */
void leer_msg_suscriptor(msg_suscriptor *suscripcion, int *cod_op, const char *tema);

/* Metodo que escribe mensaje de suscripcion */
void escribir_msg_suscriptor(int cod_op, const char *tema, msg_suscriptor *suscripcion);
