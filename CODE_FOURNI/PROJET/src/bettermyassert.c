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
    myassert((ret == 0), "Erreur lors de la fermeture du pipe !");
}

//écriture dans un pipe
void assert_writePipe(int ret, int nbrWrite, size_t size) {
    myassert(((ret != -1) && (ret == (nbrWrite*size))), "Erreur lors de l'écriture dans le pipe !");
}

//lecture dans un pipe
void assert_readPipe(int ret, int nbrRead, size_t size) {
    myassert(((ret != 0) && (ret == (nbrRead*size))), "Erreur lors de la lecture dans le pipe !");
}

//execv
void assert_execv(int ret) {
    myassert((ret != -1), "Erreur lors du execv !");
}

//création d'un sémaphore
void assert_semget(int ret) {
    myassert((ret != -1), "Erreur lors de la création du sémaphore !");
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