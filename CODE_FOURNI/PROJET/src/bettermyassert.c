/* ---------------------------- */
/* Dugas Romain & Bruyant Marie */
/*       L3 Informatique        */
/* ---------------------------- */

//Bibliothèque

#include "bettermyassert.h"

/* TUBES, FORK, EXEC */

//création d'un pipe
void assert_mkfifo(int ret) {
    myassert((ret == 0), "Erreur lors de la création du pipe !");
}

//création d'un pipe anonyme
void assert_pipePipe(int ret) {
    myassert((ret == 0), "Erreur lors de la création du pipe anonyme !");
}

//destruction d'un pipe
void assert_unlink(int ret) {
    myassert((ret == 0), "Erreur lors de la destruction du pipe !");
}

//ouverture d'un pipe
void assert_openPipe(int fd) {
    myassert((fd != -1), "Erreur lors de l'ouverture du pipe !");
}

//fermeture d'un pipe
void assert_closePipe(int ret) {
    myassert((ret != -1), "Erreur lors de la fermeture du pipe !");
}

//écriture dans un pipe nommé
void assert_writePipeNomme(int ret, int nbrWrite, size_t size) {
    myassert(((ret != -1) && (ret == (int)(nbrWrite*size))), "Erreur lors de l'écriture dans le pipe nommé !");
}

//lecture dans un pipe nommé
void assert_readPipeNomme(int ret, int nbrRead, size_t size) {
    myassert(((ret != 0) && (ret == (int)(nbrRead*size))), "Erreur lors de la lecture dans le pipe nommé !");
}

//écriture dans un pipe anonyme
void assert_writePipeAnonymous(int ret, size_t size) {
    myassert(((ret != -1) && (ret == (int)(size))), "Erreur lors de l'écriture dans le pipe anonyme !");
}

//lecture dans un pipe anonyme
void assert_readPipeAnonymous(int ret, size_t size) {
    myassert(((ret != 0) && (ret == (int)(size))), "Erreur lors de la lecture dans le pipe anonyme !");
}

//execv
void assert_execv(int ret) {
    myassert((ret != -1), "Erreur lors du execv !");
}

//création d'un sémaphore
void assert_semget(int ret) {
    myassert((ret != -1), "Erreur lors de la création/récupération du sémaphore !");
}

//destruction ou initialisation d'un sémaphore
void assert_semctl(int ret) {
    myassert((ret != -1), "Erreur lors de la destruction/initialisation du sémaphore !");
}

//opération sur un sémaphore
void assert_semop(int ret) {
    myassert((ret != -1), "Erreur lors de l'opération sur le sémaphore !");
}

//génération de clés uniques
void assert_ftok(int cle) {
    myassert((cle != -1), "Erreur lors de la génération de la clé !");
}

//création d'un nouveau processus avec fork
void assert_fork(int ret) {
    myassert((ret != -1), "Erreur lors de la création d'un nouveau processus avec fork !");
}