#ifndef MASTER_H
#define MASTER_H

// Fichier choisi pour l'identification du sémaphore
#define MY_FILE "master.h"

// Identifiant pour le deuxième paramètre de ftok
#define PROJ_ID 42 // ?

static int my_semget();
static int my_mkfifo();
static void my_destroy(int semId);

#endif