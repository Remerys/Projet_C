#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "utils.h"
#include "myassert.h"

#include "client_master.h"
#include "master_worker.h"

// Include à nous
#include <sys/ipc.h>
#include <sys/sem.h>
#include "master.h"
#include "bettermyassert.h"
#include <sys/stat.h>
#include <fcntl.h>


/************************************************************************
 * Données persistantes d'un master
 ************************************************************************/
typedef struct
{
    // communication avec le client
    int pipe_to_client;
    int pipe_from_client;
    // données internes

    //TODO


    // communication avec le premier worker (double tubes)
    int pipe_to_worker;
    int pipe_from_worker;
    // communication en provenance de tous les workers (un seul tube en lecture)
    int pipe_allWorkers;
    //TODO
} Data;


/************************************************************************
 * Usage et analyse des arguments passés en ligne de commande
 ************************************************************************/
static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s\n", exeName);
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}


/************************************************************************
 * initialisation complète
 ************************************************************************/
void init(Data *data)
{
    myassert(data != NULL, "il faut l'environnement d'exécution");

    //TODO initialisation data
    data->pipe_from_client = -1;
    data->pipe_to_client = -1;
    data->pipe_to_worker = 0;
    data->pipe_from_worker = 0;
    data->pipe_allWorkers = 0;

}


/************************************************************************
 * fin du master
 ************************************************************************/
void orderStop(Data *data)
{
    TRACE0("[master] ordre stop\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    //TODO
    // - traiter le cas ensemble vide (pas de premier worker)
    // - envoyer au premier worker ordre de fin (cf. master_worker.h)
    // - attendre sa fin
    // - envoyer l'accusé de réception au client (cf. client_master.h)
    //END TODO
}


/************************************************************************
 * quel est la cardinalité de l'ensemble
 ************************************************************************/
void orderHowMany(Data *data)
{
    TRACE0("[master] ordre how many\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    //TODO
    // - traiter le cas ensemble vide (pas de premier worker)
    // - envoyer au premier worker ordre howmany (cf. master_worker.h)
    // - recevoir accusé de réception venant du premier worker (cf. master_worker.h)
    // - recevoir résultats (deux quantités) venant du premier worker
    // - envoyer l'accusé de réception au client (cf. client_master.h)
    // - envoyer les résultats au client
    //END TODO
}


/************************************************************************
 * quel est la minimum de l'ensemble
 ************************************************************************/
void orderMinimum(Data *data)
{
    TRACE0("[master] ordre minimum\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    //TODO
    // - si ensemble vide (pas de premier worker)
    //       . envoyer l'accusé de réception dédié au client (cf. client_master.h)
    // - sinon
    //       . envoyer au premier worker ordre minimum (cf. master_worker.h)
    //       . recevoir accusé de réception venant du worker concerné (cf. master_worker.h)
    //       . recevoir résultat (la valeur) venant du worker concerné
    //       . envoyer l'accusé de réception au client (cf. client_master.h)
    //       . envoyer le résultat au client
    //END TODO
}


/************************************************************************
 * quel est la maximum de l'ensemble
 ************************************************************************/
void orderMaximum(Data *data)
{
    TRACE0("[master] ordre maximum\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    //TODO
    // cf. explications pour le minimum
    //END TODO
}


/************************************************************************
 * test d'existence
 ************************************************************************/
void orderExist(Data *data)
{
    TRACE0("[master] ordre existence\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    //TODO
    // - recevoir l'élément à tester en provenance du client
    // - si ensemble vide (pas de premier worker)
    //       . envoyer l'accusé de réception dédié au client (cf. client_master.h)
    // - sinon
    //       . envoyer au premier worker ordre existence (cf. master_worker.h)
    //       . envoyer au premier worker l'élément à tester
    //       . recevoir accusé de réception venant du worker concerné (cf. master_worker.h)
    //       . si élément non présent
    //             . envoyer l'accusé de réception dédié au client (cf. client_master.h)
    //       . sinon
    //             . recevoir résultat (une quantité) venant du worker concerné
    //             . envoyer l'accusé de réception au client (cf. client_master.h)
    //             . envoyer le résultat au client
    //END TODO
}

/************************************************************************
 * somme
 ************************************************************************/
void orderSum(Data *data)
{
    TRACE0("[master] ordre somme\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    //TODO
    // - traiter le cas ensemble vide (pas de premier worker) : la somme est alors 0
    // - envoyer au premier worker ordre sum (cf. master_worker.h)
    // - recevoir accusé de réception venant du premier worker (cf. master_worker.h)
    // - recevoir résultat (la somme) venant du premier worker
    // - envoyer l'accusé de réception au client (cf. client_master.h)
    // - envoyer le résultat au client
    //END TODO
}

/************************************************************************
 * insertion d'un élément
 ************************************************************************/

//TODO voir si une fonction annexe commune à orderInsert et orderInsertMany est justifiée

void orderInsert(Data *data)
{
    TRACE0("[master] ordre insertion\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    //TODO
    // - recevoir l'élément à insérer en provenance du client
    // - si ensemble vide (pas de premier worker)
    //       . créer le premier worker avec l'élément reçu du client
    // - sinon
    //       . envoyer au premier worker ordre insertion (cf. master_worker.h)
    //       . envoyer au premier worker l'élément à insérer
    // - recevoir accusé de réception venant du worker concerné (cf. master_worker.h)
    // - envoyer l'accusé de réception au client (cf. client_master.h)
    //END TODO
}


/************************************************************************
 * insertion d'un tableau d'éléments
 ************************************************************************/
void orderInsertMany(Data *data)
{
    TRACE0("[master] ordre insertion tableau\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    //TODO
    // - recevoir le tableau d'éléments à insérer en provenance du client
    // - pour chaque élément du tableau
    //       . l'insérer selon l'algo vu dans orderInsert (penser à factoriser le code)
    // - envoyer l'accusé de réception au client (cf. client_master.h)
    //END TODO
}


/************************************************************************
 * affichage ordonné
 ************************************************************************/
void orderPrint(Data *data)
{
    TRACE0("[master] ordre affichage\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    //TODO
    // - traiter le cas ensemble vide (pas de premier worker)
    // - envoyer au premier worker ordre print (cf. master_worker.h)
    // - recevoir accusé de réception venant du premier worker (cf. master_worker.h)
    //   note : ce sont les workers qui font les affichages
    // - envoyer l'accusé de réception au client (cf. client_master.h)
    //END TODO
}


/************************************************************************
 * boucle principale de communication avec le client
 ************************************************************************/
static int myOpen(const char * pipeId, int parameter) {
    int retOpen;

    retOpen = open(pipeId, parameter);
    assert_openPipe(retOpen);

    return retOpen;
}

static void myClose(int pipeId) {
    int retClose;

    retClose = close(pipeId);
    assert_closePipe(retClose);
}

static void exitWaiting(int semaphoreId) {
    struct sembuf operation = {0, -1, 0};
    int retSemop = semop(semaphoreId, &operation, 1);
    assert_semop(retSemop);
}

void loop(Data *data, const char * pipe_client_to_master, const char * pipe_master_to_client, int waitSemaphore)
{
    bool end = false;

    init(data);

    while (! end)
    {
        //TODO ouverture des tubes avec le client (cf. explications dans client.c)
        int pipe_client_to_master_id, pipe_master_to_client_id;
        pipe_client_to_master_id = myOpen(pipe_client_to_master, O_RDONLY);
        pipe_master_to_client_id = myOpen(pipe_master_to_client, O_WRONLY);
        //rajouter des assert !!


        //data->pipe_from_client = pipe_client_to_master;
        //data->pipe_to_client = pipe_master_to_client;

        int order = CM_ORDER_STOP;   //TODO pour que ça ne boucle pas, mais recevoir l'ordre du client




        switch(order)
        {
          case CM_ORDER_STOP:
            orderStop(data);
            end = true;
            break;
          case CM_ORDER_HOW_MANY:
            orderHowMany(data);
            break;
          case CM_ORDER_MINIMUM:
            orderMinimum(data);
            break;
          case CM_ORDER_MAXIMUM:
            orderMaximum(data);
            break;
          case CM_ORDER_EXIST:
            orderExist(data);
            break;
          case CM_ORDER_SUM:
            orderSum(data);
            break;
          case CM_ORDER_INSERT:
            orderInsert(data);
            break;
          case CM_ORDER_INSERT_MANY:
            orderInsertMany(data);
            break;
          case CM_ORDER_PRINT:
            orderPrint(data);
            break;
          default:
            myassert(false, "ordre inconnu");
            exit(EXIT_FAILURE);
            break;
        }

        //TODO fermer les tubes nommés
        //     il est important d'ouvrir et fermer les tubes nommés à chaque itération
        //     voyez-vous pourquoi ?
        myClose(pipe_client_to_master_id);
        myClose(pipe_master_to_client_id);
        //TODO attendre ordre du client avant de continuer (sémaphore pour une précédence)
        exitWaiting(waitSemaphore);

        TRACE0("[master] fin ordre\n");
    }
}


/************************************************************************
 * Fonction principale
 ************************************************************************/

//TODO N'hésitez pas à faire des fonctions annexes ; si les fonctions main
//TODO et loop pouvaient être "courtes", ce serait bien

// Création du sémaphore
static int my_semget() {
    key_t key;
    int semId, retSet;

    key = ftok(MY_FILE, PROJ_ID);
    printf("%d\n", key);
    assert_ftok(key);
  
    semId = semget(key, 1, IPC_CREAT | IPC_EXCL | 0641); // 644 = rw-r----x
    assert_semget(semId);

    retSet = semctl(semId, 0, SETVAL, 1);
    assert_semctl(retSet);

    return semId;
}

static int my_mkfifo(const char * pipeName) {
  int retMkfifo;

  retMkfifo = mkfifo(pipeName, 0644); // 644 = rw-r--r--
  assert_mkfifo(retMkfifo);

  return retMkfifo;
}

// Déstruction du sémaphore
static void my_destroy_semaphore(int semId) {
    // TODO
    int retClose = semctl(semId, -1, IPC_RMID);
    assert_semctl(retClose);
}

// Déstruction du tube nommé
static void my_destroy_pipe(const char * pipeName) {
    // TODO
    int retUnlink = unlink(pipeName);
    assert_unlink(retUnlink);
}

int main(int argc, char * argv[])
{
    if (argc != 1)
        usage(argv[0], NULL);

    TRACE0("[master] début\n");

    Data data;
    int mainSemaphore, waitSemaphore;
    const char * pipe_client_to_master = "pipe_client_to_master";
    const char * pipe_master_to_client = "pipe_master_to_client";

    TRACE0("[master] TEST1\n");
    //TODO
    // - création des sémaphores
    mainSemaphore = my_semget();
    TRACE0("[master] TEST2\n");
    waitSemaphore = my_semget();    //deux fois même semget ? //semget tableau peut-être faire s[2] avec s[0] = main et s[1] = wait
    // - création des tubes nommés
    my_mkfifo(pipe_client_to_master);
    my_mkfifo(pipe_master_to_client);      //my_mkfifo return in int ??
    //END TODO

    loop(&data, pipe_client_to_master, pipe_master_to_client, waitSemaphore);

    //TODO destruction des tubes nommés, des sémaphores, ...
    my_destroy_semaphore(mainSemaphore);
    my_destroy_semaphore(waitSemaphore);
    my_destroy_pipe(pipe_client_to_master);
    my_destroy_pipe(pipe_master_to_client);

    TRACE0("[master] terminaison\n");
    return EXIT_SUCCESS;
}
