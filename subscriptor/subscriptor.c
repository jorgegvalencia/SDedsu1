#include "subscriptor.h"
#include "comun.h"
#include "edsu_comun.h"

int alta_subscripcion_tema(const char *tema) {
	return 0;
}

int baja_subscripcion_tema(const char *tema) {
	return 0;
}

int inicio_subscriptor(void (*notif_evento)(const char *, const char *),
                void (*alta_tema)(const char *),
                void (*baja_tema)(const char *)) {
	return 0;
}

/* Avanzada */
int fin_subscriptor() {
	return 0;
}

