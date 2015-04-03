#!/bin/bash

#rsync -v -e ssh ./intermediario/intermediario.c ./intermediario/comun.c ./intermediario/comun.h ./subscriptor/subscriptor.c ./subscriptor/edsu_comun.c ./subscriptor/edsu_comun.h ./editor/editor.c t110118@triqui3.fi.upm.es:~/DATSI/SD/EDSU.2014b/ 
scp ./intermediario/intermediario.c ./intermediario/comun.c ./intermediario/comun.h ./subscriptor/subscriptor.c ./subscriptor/edsu_comun.c ./subscriptor/edsu_comun.h ./editor/editor.c t110118@triqui3.fi.upm.es:~/DATSI/SD/EDSU.2014b/
