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
    int pipe_master_to_client_id;
    int pipe_client_to_master_id;
    // données internes

    //TODO
    const char * pipe_client_to_master; 
    const char * pipe_master_to_client;
    int waitSemaphore;

    float elt;
    int nb;
    bool many; //true si on insert avec howmany, false sinon

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
    data->pipe_master_to_client_id = -1;
    data->pipe_client_to_master_id = -1;
    data->pipe_to_worker = 0;
    data->pipe_from_worker = 0;
    data->pipe_allWorkers = 0;
    data->many = false;

}


/************************************************************************
 * fin du master
 ************************************************************************/
void orderStop(Data *data)
{
    TRACE0("[master] ordre stop\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int retWrite;
    int order = MW_ORDER_STOP;
    int answerClient = CM_ANSWER_STOP_OK;

    //TODO
    // - traiter le cas ensemble vide (pas de premier worker)
    if (data->pipe_to_worker != 0) {
        // TODO WORKY
        // - envoyer au premier worker ordre de fin (cf. master_worker.h)
        retWrite = write(data->pipe_to_worker, &order, sizeof(int));
        assert_writePipeAnonymous(retWrite, sizeof(int));
        // - attendre sa fin
        wait(NULL);
    }

    // - envoyer l'accusé de réception au client (cf. client_master.h)
    retWrite = write(data->pipe_master_to_client_id, &answerClient, sizeof(int));
    assert_writePipeNomme(retWrite, 1, sizeof(int));
    //END TODO
}


/************************************************************************
 * quel est la cardinalité de l'ensemble
 ************************************************************************/
void orderHowMany(Data *data)
{
    TRACE0("[master] ordre how many\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int retRead, retWrite;
    int order = MW_ORDER_HOW_MANY;
    int answerWorker;
    int answerClient = CM_ANSWER_HOW_MANY_OK;
    int howMany = 0;
    int howManyDist = 0;

    //TODO
    // - traiter le cas ensemble vide (pas de premier worker)
    if (data->pipe_to_worker != 0) {
    // - envoyer au premier worker ordre howmany (cf. master_worker.h)
        retWrite = write(data->pipe_to_worker, &order, sizeof(int));
        assert_writePipeAnonymous(retWrite, sizeof(int));
    // - recevoir accusé de réception venant du premier worker (cf. master_worker.h)
        retRead = read(data->pipe_from_worker, &answerWorker, sizeof(int));
        assert_readPipeAnonymous(retRead, sizeof(int));
    // - recevoir résultats (deux quantités) venant du premier worker
        retRead = read(data->pipe_from_worker, &howMany, sizeof(int));
        assert_readPipeAnonymous(retRead, sizeof(int));
        retRead = read(data->pipe_from_worker, &howManyDist, sizeof(int));
        assert_readPipeAnonymous(retRead, sizeof(int));
    }
    // - envoyer l'accusé de réception au client (cf. client_master.h)
    retWrite = write(data->pipe_master_to_client_id, &answerClient, sizeof(int));
    assert_writePipeNomme(retWrite, 1, sizeof(int));
    // - envoyer les résultats au client
    retWrite = write(data->pipe_master_to_client_id, &howMany, sizeof(int));
    assert_writePipeNomme(retWrite, 1, sizeof(int));
    retWrite = write(data->pipe_master_to_client_id, &howManyDist, sizeof(int));
    assert_writePipeNomme(retWrite, 1, sizeof(int));
    //END TODO
}


/************************************************************************
 * quel est la minimum de l'ensemble
 ************************************************************************/
void orderMinimum(Data *data)
{
    TRACE0("[master] ordre minimum\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int retWrite, retRead;
    int answerC = CM_ANSWER_MINIMUM_EMPTY;
    int answerW;
    int order = MW_ORDER_MINIMUM;
    float res;

    //TODO
    // - si ensemble vide (pas de premier worker)
    if (data->pipe_to_worker == 0) {
        // . envoyer l'accusé de réception dédié au client (cf. client_master.h)
        retWrite = write(data->pipe_master_to_client_id, &answerC, sizeof(int));
        assert_writePipeNomme(retWrite, 1, sizeof(int));
    }
    // - sinon
    else {
        // . envoyer au premier worker ordre minimum (cf. master_worker.h)
        retWrite = write(data->pipe_to_worker, &order, sizeof(int));
        assert_writePipeNomme(retWrite, 1, sizeof(int));
        // . recevoir accusé de réception venant du worker concerné (cf. master_worker.h)
        retRead = read(data->pipe_allWorkers, &answerW, sizeof(int));
        assert_readPipeAnonymous(retRead, sizeof(int));
        myassert(answerW == MW_ANSWER_MINIMUM, "orderMinimum : Problème Worker");
        // . recevoir résultat (la valeur) venant du worker concerné
        retRead = read(data->pipe_allWorkers, &res, sizeof(float));
        assert_readPipeAnonymous(retRead, sizeof(float));
        // . envoyer l'accusé de réception au client (cf. client_master.h)
        answerC = CM_ANSWER_MINIMUM_OK;
        retWrite = write(data->pipe_master_to_client_id, &answerC, sizeof(int));
        assert_writePipeNomme(retWrite, 1, sizeof(int));
        // . envoyer le résultat au client
        retWrite = write(data->pipe_master_to_client_id, &res, sizeof(float));
        assert_writePipeNomme(retWrite, 1, sizeof(float));
    }
    //END TODO
}


/************************************************************************
 * quel est la maximum de l'ensemble
 ************************************************************************/
void orderMaximum(Data *data)
{
    TRACE0("[master] ordre maximum\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int retWrite, retRead;
    int answerC = CM_ANSWER_MAXIMUM_EMPTY;
    int answerW;
    int order = MW_ORDER_MAXIMUM;
    float res;

    //TODO
    // - si ensemble vide (pas de premier worker)
    if (data->pipe_to_worker == 0) {
        // . envoyer l'accusé de réception dédié au client (cf. client_master.h)
        retWrite = write(data->pipe_master_to_client_id, &answerC, sizeof(int));
        assert_writePipeNomme(retWrite, 1, sizeof(int));
    }
    // - sinon
    else {
        // . envoyer au premier worker ordre minimum (cf. master_worker.h)
        retWrite = write(data->pipe_to_worker, &order, sizeof(int));
        assert_writePipeNomme(retWrite, 1, sizeof(int));
        // . recevoir accusé de réception venant du worker concerné (cf. master_worker.h)
        retRead = read(data->pipe_allWorkers, &answerW, sizeof(int));
        assert_readPipeAnonymous(retRead, sizeof(int));
        myassert(answerW == MW_ANSWER_MAXIMUM, "orderMaximum : Problème Worker");
        // . recevoir résultat (la valeur) venant du worker concerné
        retRead = read(data->pipe_allWorkers, &res, sizeof(float));
        assert_readPipeAnonymous(retRead, sizeof(float));
        // . envoyer l'accusé de réception au client (cf. client_master.h)
        answerC = CM_ANSWER_MAXIMUM_OK;
        retWrite = write(data->pipe_master_to_client_id, &answerC, sizeof(int));
        assert_writePipeNomme(retWrite, 1, sizeof(int));
        // . envoyer le résultat au client
        retWrite = write(data->pipe_master_to_client_id, &res, sizeof(float));
        assert_writePipeNomme(retWrite, 1, sizeof(float));
    }
    //END TODO
}


/************************************************************************
 * test d'existence
 ************************************************************************/
void orderExist(Data *data)
{
    TRACE0("[master] ordre existence\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int retRead, retWrite;
    int order = MW_ORDER_EXIST;
    int answerClient = CM_ANSWER_EXIST_NO;
    int answerWorker;
    int answerCard;

    //TODO
    // - recevoir l'élément à tester en provenance du client
    retRead = read(data->pipe_client_to_master_id, &(data->elt), sizeof(float));
    assert_readPipeNomme(retRead, 1, sizeof(float));
    // - si ensemble vide (pas de premier worker)
    //       . envoyer l'accusé de réception dédié au client (cf. client_master.h)
    if (data->pipe_to_worker == 0) {
        retWrite = write(data->pipe_master_to_client_id, &answerClient, sizeof(int));
        assert_writePipeNomme(retWrite, 1, sizeof(int));
    }
    // - sinon
    else
    {
    //       . envoyer au premier worker ordre existence (cf. master_worker.h)
        retWrite = write(data->pipe_to_worker, &order, sizeof(int));
        assert_writePipeAnonymous(retWrite, sizeof(int));
    //       . envoyer au premier worker l'élément à tester
        retWrite = write(data->pipe_to_worker, &(data->elt), sizeof(float));
        assert_writePipeAnonymous(retWrite, sizeof(float));
    //       . recevoir accusé de réception venant du worker concerné (cf. master_worker.h)
        retRead = read(data->pipe_allWorkers, &answerWorker, sizeof(int));
        assert_readPipeAnonymous(retRead, sizeof(int));
    //       . si élément non présent
        if (answerWorker == MW_ANSWER_EXIST_NO) {
    //             . envoyer l'accusé de réception dédié au client (cf. client_master.h)
            retWrite = write(data->pipe_master_to_client_id, &answerClient, sizeof(int));
            assert_writePipeNomme(retWrite, 1, sizeof(int));
        }
    //       . sinon
        else
        {
    //             . recevoir résultat (une quantité) venant du worker concerné
            retRead = read(data->pipe_allWorkers, &answerCard, sizeof(int));
            assert_readPipeAnonymous(retRead, sizeof(int));
    //             . envoyer l'accusé de réception au client (cf. client_master.h)
            answerClient = CM_ANSWER_EXIST_YES;
            retWrite = write(data->pipe_master_to_client_id, &answerClient, sizeof(int));
            assert_writePipeNomme(retWrite, 1, sizeof(int));
    //             . envoyer le résultat au client
            retWrite = write(data->pipe_master_to_client_id, &answerCard, sizeof(int));
            assert_writePipeNomme(retWrite, 1, sizeof(int));
        }
    }
    //END TODO
}

/************************************************************************
 * somme
 ************************************************************************/
void orderSum(Data *data)
{
    TRACE0("[master] ordre somme\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int retWrite, retRead;
    int orderW = MW_ORDER_SUM;
    int answerC = CM_ANSWER_SUM_OK;
    int answerW;
    int sum = 0;

    //TODO
    // - traiter le cas ensemble vide (pas de premier worker) : la somme est alors 0
    if (data->pipe_to_worker != 0) {
    // - envoyer au premier worker ordre sum (cf. master_worker.h)
        retWrite = write(data->pipe_to_worker, &orderW, sizeof(int));
        assert_writePipeAnonymous(retWrite, sizeof(int));
    // - recevoir accusé de réception venant du premier worker (cf. master_worker.h)
        retRead = read(data->pipe_from_worker, &answerW, sizeof(float));
        assert_readPipeAnonymous(retRead, sizeof(float));
    // - recevoir résultat (la somme) venant du premier
        retRead = read(data->pipe_from_worker, &sum, sizeof(int));
        assert_readPipeAnonymous(retRead, sizeof(int));
    }
    // - envoyer l'accusé de réception au client (cf. client_master.h)
    retWrite = write(data->pipe_master_to_client_id, &answerC, sizeof(int));
    assert_writePipeNomme(retWrite, 1, sizeof(int));
    // - envoyer le résultat au client
    retWrite = write(data->pipe_master_to_client_id, &sum, sizeof(int));
    assert_writePipeNomme(retWrite, 1, sizeof(int));
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

    int retRead, retWrite, retPipe;
    pid_t retFork;
    int answer = CM_ANSWER_INSERT_OK;
    int order = MW_ORDER_INSERT;
    int pipe_master_to_worker[2], pipe_worker_to_master[2], pipe_all_workers[2];

    //TODO
    // - recevoir l'élément à insérer en provenance du client
    if(!(data->many)) {
        retRead = read(data->pipe_client_to_master_id, &(data->elt), sizeof(float));
        assert_readPipeNomme(retRead, 1, sizeof(float));
    }
    // - si ensemble vide (pas de premier worker)
    if (data->pipe_to_worker == 0) {
    //       . créer le premier worker avec l'élément reçu du client
        retPipe = pipe(pipe_master_to_worker);
        assert_pipePipe(retPipe);

        retPipe = pipe(pipe_worker_to_master);
        assert_pipePipe(retPipe);

        retPipe = pipe(pipe_all_workers);
        assert_pipePipe(retPipe);

        retFork = fork();
        assert_fork(retFork);

        //0 read ; 1 write
        if (retFork == 0) {
            //worker
            close(pipe_master_to_worker[1]);
            close(pipe_worker_to_master[0]);
            close(pipe_all_workers[0]);

            char *argv[6];
            char intToMaster[10];
            char intFromMaster[10];
            char intAllWorky[10];
            char floatElt[20];

            sprintf(intToMaster, "%d", pipe_worker_to_master[1]);
            sprintf(intFromMaster, "%d", pipe_master_to_worker[0]);
            sprintf(intAllWorky, "%d", pipe_all_workers[1]);
            sprintf(floatElt, "%f", data->elt);

            argv[0] = "worker";
            argv[1] = floatElt;
            argv[2] = intFromMaster;
            argv[3] = intToMaster;
            argv[4] = intAllWorky;
            argv[5] = NULL;

            execv(argv[0], argv);
        } 
        else {
            //master
            close(pipe_master_to_worker[0]);
            close(pipe_worker_to_master[1]);
            close(pipe_all_workers[1]);
            data->pipe_to_worker = pipe_master_to_worker[1];
            data->pipe_from_worker = pipe_worker_to_master[0];
            data->pipe_allWorkers = pipe_all_workers[0];
        }
    }
    // - sinon
    else {
        // . envoyer au premier worker ordre insertion (cf. master_worker.h)
        retWrite = write(data->pipe_to_worker, &order, sizeof(int));
        assert_writePipeAnonymous(retWrite, sizeof(int));
        // . envoyer au premier worker l'élément à insérer
        retWrite = write(data->pipe_to_worker, &(data->elt), sizeof(float));
        assert_writePipeAnonymous(retWrite, sizeof(float));
    }

    // - recevoir accusé de réception venant du worker concerné (cf. master_worker.h)
    retRead = read(data->pipe_allWorkers, &answer, sizeof(int));
    assert_readPipeAnonymous(retRead, sizeof(int));

    if (!(data->many)) {
    // - envoyer l'accusé de réception au client (cf. client_master.h)
        retWrite = write(data->pipe_master_to_client_id, &answer, sizeof(int));
        assert_writePipeNomme(retWrite, 1, sizeof(int));
    }
    //END TODO
}


/************************************************************************
 * insertion d'un tableau d'éléments
 ************************************************************************/
void orderInsertMany(Data *data)
{
    TRACE0("[master] ordre insertion tableau\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int retRead, retWrite;
    int answerClient = CM_ANSWER_INSERT_MANY_OK;

    data->many = true;

    //TODO
    // - recevoir le tableau d'éléments à insérer en provenance du client
    retRead = read(data->pipe_client_to_master_id, &(data->nb), sizeof(int));
    assert_readPipeNomme(retRead, 1, sizeof(int));
    float tabValue[data->nb];
    retRead = read(data->pipe_client_to_master_id, &tabValue, data->nb * sizeof(float));
    assert_readPipeNomme(retRead, data->nb, sizeof(float));
    // - pour chaque élément du tableau
    for (int i = 0; i < data->nb; i++)
    {
    //       . l'insérer selon l'algo vu dans orderInsert (penser à factoriser le code)
        data->elt = tabValue[i];
        orderInsert(data);
    }
    // - envoyer l'accusé de réception au client (cf. client_master.h)
    retWrite = write(data->pipe_master_to_client_id, &answerClient, sizeof(int));
    assert_writePipeNomme(retWrite, 1, sizeof(int));

    data->many = false;

    //END TODO
}


/************************************************************************
 * affichage ordonné
 ************************************************************************/
void orderPrint(Data *data)
{
    TRACE0("[master] ordre affichage\n");
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int retRead, retWrite;
    int order = MW_ORDER_PRINT;
    int answerWorker;
    int answerClient = CM_ANSWER_PRINT_OK;

    //TODO
    // - traiter le cas ensemble vide (pas de premier worker)
    if (data->pipe_to_worker != 0) {
    // - envoyer au premier worker ordre print (cf. master_worker.h)
        retWrite = write(data->pipe_to_worker, &order, sizeof(int));
        assert_writePipeAnonymous(retWrite, sizeof(int));
    // - recevoir accusé de réception venant du premier worker (cf. master_worker.h)
        retRead = read(data->pipe_from_worker, &answerWorker, sizeof(int));
        assert_readPipeAnonymous(retRead, sizeof(int));
    //   note : ce sont les workers qui font les affichages
    }
    // - envoyer l'accusé de réception au client (cf. client_master.h)
    retWrite = write(data->pipe_master_to_client_id, &answerClient, sizeof(int));
    assert_writePipeNomme(retWrite, 1, sizeof(int));
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

static void enterWaiting(int semaphoreId) {
    struct sembuf operation = {0, -1, 0};
    int retSemop = semop(semaphoreId, &operation, 1);
    assert_semop(retSemop);
}

void loop(Data *data)
{
    bool end = false;

    init(data);

    while (!end)
    {
        //TODO ouverture des tubes avec le client (cf. explications dans client.c)
        data->pipe_client_to_master_id = myOpen(data->pipe_client_to_master, O_RDONLY);
        data->pipe_master_to_client_id = myOpen(data->pipe_master_to_client, O_WRONLY);

        int order; // = CM_ORDER_STOP;   //TODO pour que ça ne boucle pas, mais recevoir l'ordre du client

        int retRead = read(data->pipe_client_to_master_id, &order, sizeof(int));
        assert_readPipeNomme(retRead, 1, sizeof(int));

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
        myClose(data->pipe_client_to_master_id);
        myClose(data->pipe_master_to_client_id);
        //TODO attendre ordre du client avant de continuer (sémaphore pour une précédence)
        enterWaiting(data->waitSemaphore);

        TRACE0("[master] fin ordre\n");
    }
}


/************************************************************************
 * Fonction principale
 ************************************************************************/

//TODO N'hésitez pas à faire des fonctions annexes ; si les fonctions main
//TODO et loop pouvaient être "courtes", ce serait bien

// Création du sémaphore
static int my_semget(int keyNumber, int init) {
    key_t key;
    int semId, retSet;

    key = ftok(MY_FILE, keyNumber);
    assert_ftok(key);
  
    semId = semget(key, 1, IPC_CREAT | IPC_EXCL | 0641); // 644 = rw-r----x
    assert_semget(semId);

    retSet = semctl(semId, 0, SETVAL, init);
    assert_semctl(retSet);

    return semId;
}

static void my_mkfifo(const char * pipeName) {
  int retMkfifo;
  retMkfifo = mkfifo(pipeName, 0644); // 644 = rw-r--r--
  assert_mkfifo(retMkfifo);
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
    int mainSemaphore;
    data.pipe_client_to_master = "pipe_client_to_master";
    data.pipe_master_to_client = "pipe_master_to_client";

    //TODO
    // - création des sémaphores
    mainSemaphore = my_semget(PROJ_ID1, 1);
    data.waitSemaphore = my_semget(PROJ_ID2, 0);

    // - création des tubes nommés
    my_mkfifo(data.pipe_client_to_master);
    my_mkfifo(data.pipe_master_to_client);
    //END TODO

    loop(&data);

    //TODO destruction des tubes nommés, des sémaphores, ...
    my_destroy_semaphore(mainSemaphore);
    my_destroy_semaphore(data.waitSemaphore);
    my_destroy_pipe(data.pipe_client_to_master);
    my_destroy_pipe(data.pipe_master_to_client);

    TRACE0("[master] terminaison\n");
    return EXIT_SUCCESS;
}
