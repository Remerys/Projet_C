#ifndef BETTER_MY_ASSERT_H
#define BETTER_MY_ASSERT_H

/* ---------------------------- */
/* Dugas Romain & Bruyant Marie */
/*       L3 Informatique        */
/* ---------------------------- */

//Bibliothèque

#include "myassert.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Fichier contenant les asserts de vérifications nécessaire au projet */

/* TUBES, FORK, EXEC */

//création d'un pipe
void assert_mkfifo(int ret);

//destruction d'un pipe
void assert_unlink(int ret);

//ouverture d'un pipe
void assert_openPipe(int fd);

//fermeture d'un pipe
void assert_closePipe(int ret);

//écriture dans un pipe
void assert_writePipe(int ret, int nbrWrite, size_t size);

//lecture dans un pipe
void assert_readPipe(int ret, int nbrRead, size_t size);

//execv
void assert_execv(int ret);

//création d'un sémaphore
void assert_semget(int ret);

//destruction ou initialisation d'un sémaphore
void assert_semctl(int ret);

//opération sur un sémaphore
void assert_semop(int ret);

//génération de clés uniques
void assert_ftok(int cle);

#endif