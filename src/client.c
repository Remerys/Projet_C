#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"
#include "myassert.h"

#include "client_master.h"

// Include à nous
#include <assert.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "bettermyassert.h"
#include <fcntl.h>

/************************************************************************
 * chaines possibles pour le premier paramètre de la ligne de commande
 ************************************************************************/
#define TK_STOP "stop"              // arrêter le master
#define TK_HOW_MANY "howmany"       // combien d'éléments dans l'ensemble
#define TK_MINIMUM "min"            // valeur minimale de l'ensemble
#define TK_MAXIMUM "max"            // valeur maximale de l'ensemble
#define TK_EXIST "exist"            // test d'existence d'un élément, et nombre d'exemplaires
#define TK_SUM "sum"                // somme de tous les éléments
#define TK_INSERT "insert"          // insertion d'un élément
#define TK_INSERT_MANY "insertmany" // insertions de plusieurs éléments aléatoires
#define TK_PRINT "print"            // debug : demande aux master/workers d'afficher les éléments
#define TK_LOCAL "local"            // lancer un calcul local (sans master) en multi-thread

/************************************************************************
 * structure stockant les paramètres du client
 * - les infos pour communiquer avec le master
 * - les infos pour effectuer le travail (cf. ligne de commande)
 *   (note : une union permettrait d'optimiser la place mémoire)
 ************************************************************************/
typedef struct
{
    // communication avec le master
    // TODO
    int pipe_client_to_master;
    int pipe_master_to_client;
    // infos pour le travail à faire (récupérées sur la ligne de commande)
    int order;     // ordre de l'utilisateur (cf. CM_ORDER_* dans client_master.h)
    float elt;     // pour CM_ORDER_EXIST, CM_ORDER_INSERT, CM_ORDER_LOCAL
    int nb;        // pour CM_ORDER_INSERT_MANY, CM_ORDER_LOCAL
    float min;     // pour CM_ORDER_INSERT_MANY, CM_ORDER_LOCAL
    float max;     // pour CM_ORDER_INSERT_MANY, CM_ORDER_LOCAL
    int nbThreads; // pour CM_ORDER_LOCAL
} Data;

/************************************************************************
 * Usage
 ************************************************************************/
static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usages : %s <ordre> [[[<param1>] [<param2>] ...]]\n", exeName);
    fprintf(stderr, "   $ %s " TK_STOP "\n", exeName);
    fprintf(stderr, "          arrêt master\n");
    fprintf(stderr, "   $ %s " TK_HOW_MANY "\n", exeName);
    fprintf(stderr, "          combien d'éléments dans l'ensemble\n");
    fprintf(stderr, "   $ %s " TK_MINIMUM "\n", exeName);
    fprintf(stderr, "          plus petite valeur de l'ensemble\n");
    fprintf(stderr, "   $ %s " TK_MAXIMUM "\n", exeName);
    fprintf(stderr, "          plus grande valeur de l'ensemble\n");
    fprintf(stderr, "   $ %s " TK_EXIST " <elt>\n", exeName);
    fprintf(stderr, "          l'élement <elt> est-il présent dans l'ensemble ?\n");
    fprintf(stderr, "   $ %s " TK_SUM "\n", exeName);
    fprintf(stderr, "           somme des éléments de l'ensemble\n");
    fprintf(stderr, "   $ %s " TK_INSERT " <elt>\n", exeName);
    fprintf(stderr, "          ajout de l'élement <elt> dans l'ensemble\n");
    fprintf(stderr, "   $ %s " TK_INSERT_MANY " <nb> <min> <max>\n", exeName);
    fprintf(stderr, "          ajout de <nb> élements (dans [<min>,<max>[) aléatoires dans l'ensemble\n");
    fprintf(stderr, "   $ %s " TK_PRINT "\n", exeName);
    fprintf(stderr, "          affichage trié (dans la console du master)\n");
    fprintf(stderr, "   $ %s " TK_LOCAL " <nbThreads> <elt> <nb> <min> <max>\n", exeName);
    fprintf(stderr, "          combien d'exemplaires de <elt> dans <nb> éléments (dans [<min>,<max>[)\n"
                    "          aléatoires avec <nbThreads> threads\n");

    if (message != NULL)
        fprintf(stderr, "message :\n    %s\n", message);

    exit(EXIT_FAILURE);
}

/************************************************************************
 * Analyse des arguments passés en ligne de commande
 ************************************************************************/
static void parseArgs(int argc, char *argv[], Data *data)
{
    data->order = CM_ORDER_NONE;

    if (argc == 1)
        usage(argv[0], "Il faut préciser une commande");

    // première vérification : la commande est-elle correcte ?
    if (strcmp(argv[1], TK_STOP) == 0)
        data->order = CM_ORDER_STOP;
    else if (strcmp(argv[1], TK_HOW_MANY) == 0)
        data->order = CM_ORDER_HOW_MANY;
    else if (strcmp(argv[1], TK_MINIMUM) == 0)
        data->order = CM_ORDER_MINIMUM;
    else if (strcmp(argv[1], TK_MAXIMUM) == 0)
        data->order = CM_ORDER_MAXIMUM;
    else if (strcmp(argv[1], TK_EXIST) == 0)
        data->order = CM_ORDER_EXIST;
    else if (strcmp(argv[1], TK_SUM) == 0)
        data->order = CM_ORDER_SUM;
    else if (strcmp(argv[1], TK_INSERT) == 0)
        data->order = CM_ORDER_INSERT;
    else if (strcmp(argv[1], TK_INSERT_MANY) == 0)
        data->order = CM_ORDER_INSERT_MANY;
    else if (strcmp(argv[1], TK_PRINT) == 0)
        data->order = CM_ORDER_PRINT;
    else if (strcmp(argv[1], TK_LOCAL) == 0)
        data->order = CM_ORDER_LOCAL;
    else
        usage(argv[0], "commande inconnue");

    // deuxième vérification : nombre de paramètres correct ?
    if ((data->order == CM_ORDER_STOP) && (argc != 2))
        usage(argv[0], TK_STOP " : il ne faut pas d'argument après la commande");
    if ((data->order == CM_ORDER_HOW_MANY) && (argc != 2))
        usage(argv[0], TK_HOW_MANY " : il ne faut pas d'argument après la commande");
    if ((data->order == CM_ORDER_MINIMUM) && (argc != 2))
        usage(argv[0], TK_MINIMUM " : il ne faut pas d'argument après la commande");
    if ((data->order == CM_ORDER_MAXIMUM) && (argc != 2))
        usage(argv[0], TK_MAXIMUM " : il ne faut pas d'argument après la commande");
    if ((data->order == CM_ORDER_EXIST) && (argc != 3))
        usage(argv[0], TK_EXIST " : il faut un et un seul argument après la commande");
    if ((data->order == CM_ORDER_SUM) && (argc != 2))
        usage(argv[0], TK_SUM " : il ne faut pas d'argument après la commande");
    if ((data->order == CM_ORDER_INSERT) && (argc != 3))
        usage(argv[0], TK_INSERT " : il faut un et un seul argument après la commande");
    if ((data->order == CM_ORDER_INSERT_MANY) && (argc != 5))
        usage(argv[0], TK_INSERT_MANY " : il faut 3 arguments après la commande");
    if ((data->order == CM_ORDER_PRINT) && (argc != 2))
        usage(argv[0], TK_PRINT " : il ne faut pas d'argument après la commande");
    if ((data->order == CM_ORDER_LOCAL) && (argc != 7))
        usage(argv[0], TK_LOCAL " : il faut 5 arguments après la commande");

    // extraction des arguments
    if (data->order == CM_ORDER_EXIST)
    {
        data->elt = strtof(argv[2], NULL);
    }
    else if (data->order == CM_ORDER_INSERT)
    {
        data->elt = strtof(argv[2], NULL);
    }
    else if (data->order == CM_ORDER_INSERT_MANY)
    {
        data->nb = strtol(argv[2], NULL, 10);
        data->min = strtof(argv[3], NULL);
        data->max = strtof(argv[4], NULL);
        if (data->nb < 1)
            usage(argv[0], TK_INSERT_MANY " : nb doit être strictement positif");
        if (data->max < data->min)
            usage(argv[0], TK_INSERT_MANY " : max ne doit pas être inférieur à min");
    }
    else if (data->order == CM_ORDER_LOCAL)
    {
        data->nbThreads = strtol(argv[2], NULL, 10);
        data->elt = strtof(argv[3], NULL);
        data->nb = strtol(argv[4], NULL, 10);
        data->min = strtof(argv[5], NULL);
        data->max = strtof(argv[6], NULL);
        if (data->nbThreads < 1)
            usage(argv[0], TK_LOCAL " : nbThreads doit être strictement positif");
        if (data->nb < 1)
            usage(argv[0], TK_LOCAL " : nb doit être strictement positif");
        if (data->max <= data->min)
            usage(argv[0], TK_LOCAL " : max ne doit être strictement supérieur à min");
    }
}

/************************************************************************
 * Partie multi-thread
 ************************************************************************/
// TODO Une structure pour les arguments à passer à un thread (aucune variable globale autorisée)
typedef struct
{
    int * result;
    pthread_mutex_t * mutex;
    float elt;
    int min;
    int max;
    float * tab;
    
} ThreadData;

// TODO
//  Code commun à tous les threads
//  Un thread s'occupe d'une portion du tableau et compte en interne le nombre de fois
//  où l'élément recherché est présent dans cette portion. On ajoute alors,
//  en section critique, ce nombre au compteur partagé par tous les threads.
//  Le compteur partagé est la variable "result" de "lauchThreads".
//  A vous de voir les paramètres nécessaires  (aucune variable globale autorisée)

void * codeThread(void * arg) {
    ThreadData *data = (ThreadData *) arg;

    for (int i = data->min; i < data->max; i++) {
        if (data->tab[i] == data->elt) {
            pthread_mutex_lock(data->mutex);
            (*(data->result))++;
            pthread_mutex_unlock(data->mutex);
        }
    }

    return NULL;
}
// END TODO

void lauchThreads(const Data *data)
{
    // TODO déclarations nécessaires : mutex, ...
    int result = 0;
    float *tab = ut_generateTab(data->nb, data->min, data->max, 0);
    pthread_t threadId[data->nbThreads];
    int createThread, waitThread;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    ThreadData datas[data->nbThreads];

    int step = data->nb / data->nbThreads;
    int stepBis;

    if (data->nb % data->nbThreads != 0) {
        stepBis = data->nb % data->nbThreads;
    }

    for (int i = 0; i < data->nbThreads; i++) {
        datas[i].result = &result;
        datas[i].mutex = &mutex;
        datas[i].tab = tab;
        datas[i].elt = data->elt;
        datas[i].min = step*i;
        datas[i].max = step * i + step;
        if (i == data->nbThreads-1 && stepBis != 0) {
            datas[i].max = step * i + stepBis;
        }
        
    }

    // TODO lancement des threads
    for (int i = 0; i < data->nbThreads; i++) {
        createThread = pthread_create(&(threadId[i]), NULL, codeThread, &(datas[i]));
        assert(createThread == 0); // a refaire
    }    

    // TODO attente de la fin des threads
    for (int i = 0; i < data->nbThreads; i++) {
        waitThread = pthread_join(threadId[i], NULL);
        assert(waitThread == 0); // a refaire
    }

    // résultat (result a été rempli par les threads)
    // affichage du tableau si pas trop gros
    if (data->nb <= 20)
    {
        printf("[");
        for (int i = 0; i < data->nb; i++)
        {
            if (i != 0)
                printf(" ");
            printf("%g", tab[i]);
        }
        printf("]\n");
    }
    // recherche linéaire pour vérifier
    int nbVerif = 0;
    for (int i = 0; i < data->nb; i++)
    {
        if (tab[i] == data->elt)
            nbVerif++;
    }
    printf("Elément %g présent %d fois (%d attendu)\n", data->elt, result, nbVerif);
    if (result == nbVerif)
        printf("=> ok ! le résultat calculé par les threads est correct\n");
    else
        printf("=> PB ! le résultat calculé par les threads est incorrect\n");

    // TODO libération des ressources
    int retDestroy = pthread_mutex_destroy(&mutex);
    assert(retDestroy == 0);

}

/************************************************************************
 * Partie communication avec le master
 ************************************************************************/
// envoi des données au master
void sendData(const Data *data)
{
    myassert(data != NULL, "pb !"); // TODO à enlever (présent pour éviter le warning)
    int retWrite;
    // TODO
    //  - envoi de l'ordre au master (cf. CM_ORDER_* dans client_master.h)
    retWrite = write(data->pipe_client_to_master, &(data->order), sizeof(int));
    assert_writePipeNomme(retWrite, 1, sizeof(int));
    int taille = 1;
    if (data->order == CM_ORDER_INSERT_MANY) {
        taille = data->nb;
    }
    float tabValue[taille];
    float randomValue;
    //  - envoi des paramètres supplémentaires au master (pour CM_ORDER_EXIST, CM_ORDER_INSERT et CM_ORDER_INSERT_MANY)
    switch (data->order) {
        case CM_ORDER_EXIST :
            retWrite = write(data->pipe_client_to_master, &(data->elt), sizeof(float));
            assert_writePipeNomme(retWrite, 1, sizeof(float));
            break;
        case CM_ORDER_INSERT :
            retWrite = write(data->pipe_client_to_master, &(data->elt), sizeof(float));
            assert_writePipeNomme(retWrite, 1, sizeof(float));
            break;
        case CM_ORDER_INSERT_MANY :
            randomValue = 0;
            retWrite = write(data->pipe_client_to_master, &(data->nb), sizeof(int));
            assert_writePipeNomme(retWrite, 1, sizeof(int));
            for (int i = 0 ; i < data->nb ; i++) {
                randomValue = ((float)rand() / (float)(RAND_MAX)) * (data->max - data->min) + data->min;
                tabValue[i] = randomValue;
            }
            retWrite = write(data->pipe_client_to_master, &tabValue, data->nb * sizeof(float));
            assert_writePipeNomme(retWrite, data->nb, sizeof(float));
            break;
        default :
            break;
    }
    
    // END TODO
}

// attente de la réponse du master
void receiveAnswer(const Data *data)
{
    myassert(data != NULL, "pb !"); // TODO à enlever (présent pour éviter le warning)
    int retRead;
    int answer;
    float max;
    float min;
    float sum;
    int howMany;
    int howManyDist;
    int answerExist;

    // TODO
    //  - récupération de l'accusé de réception du master (cf. CM_ANSWER_* dans client_master.h)
    retRead = read(data->pipe_master_to_client, &answer, sizeof(int));
    assert_readPipeNomme(retRead, 1, sizeof(int));

    //  - selon l'ordre et l'accusé de réception :
    //       . récupération de données supplémentaires du master si nécessaire
    switch (answer) {
        case CM_ANSWER_MAXIMUM_OK:
            retRead = read(data->pipe_master_to_client, &max, sizeof(float));
            assert_readPipeNomme(retRead, 1, sizeof(float));
            printf("[client] ordre : maximum\n");
            printf("[client] res : %f\n", max);
            break;
        case CM_ANSWER_MINIMUM_OK:
            retRead = read(data->pipe_master_to_client, &min, sizeof(float));
            assert_readPipeNomme(retRead, 1, sizeof(float));
            printf("[client] ordre : minimum\n");
            printf("[client] res : %f\n", min);
            break;
        case CM_ANSWER_SUM_OK:
            retRead = read(data->pipe_master_to_client, &sum, sizeof(float));
            assert_readPipeNomme(retRead, 1, sizeof(float));
            printf("[client] ordre : sum\n");
            printf("[client] res sum : %f\n", sum);
            break;
        case CM_ANSWER_HOW_MANY_OK:
            retRead = read(data->pipe_master_to_client, &howMany, sizeof(int));
            assert_readPipeNomme(retRead, 1, sizeof(int));
            printf("[client] ordre : howmany\n");
            printf("[client] res : %d\n", howMany);
            retRead = read(data->pipe_master_to_client, &howManyDist, sizeof(int));
            assert_readPipeNomme(retRead, 1, sizeof(int));
            printf("[client] res sumDist : %d\n", howManyDist);
            break;
        case CM_ANSWER_EXIST_YES:
            retRead = read(data->pipe_master_to_client, &answerExist, sizeof(int));
            assert_readPipeNomme(retRead, 1, sizeof(int));
            printf("[client] ordre : exist\n");
            printf("[client] res : %d\n", answerExist);
            break;
        default:
            break;
    }
    //  - affichage du résultat
    // END TODO
}

// Récupération du sémaphore
static int my_semget(int keyNumber)
{
    key_t key;
    int semId;

    key = ftok(MY_FILE, keyNumber);
    assert_ftok(key);

    semId = semget(key, 1, 0);
    assert_semget(semId);

    return semId;
}

static void enterSC(int semId)
{
    struct sembuf operation = {0, -1, 0};
    int retSemop = semop(semId, &operation, 1);
    assert_semop(retSemop);
}

static void exitSC(int semId)
{
    struct sembuf operation = {0, +1, 0};
    int retSemop = semop(semId, &operation, 1);
    assert_semop(retSemop);
}

static int myOpen(const char *pipeId, int parameter)
{
    int retOpen;

    retOpen = open(pipeId, parameter);
    assert_openPipe(retOpen);

    return retOpen;
}

static void myClose(int pipeId)
{
    int retClose;

    retClose = close(pipeId);
    assert_closePipe(retClose);
}

static void exitWaiting(int semaphoreId)
{
    struct sembuf operation = {0, +1, 0};
    int retSemop = semop(semaphoreId, &operation, 1);
    assert_semop(retSemop);
}

/************************************************************************
 * Fonction principale
 ************************************************************************/
int main(int argc, char *argv[])
{
    Data data;
    parseArgs(argc, argv, &data);

    if (data.order == CM_ORDER_LOCAL)
        lauchThreads(&data);
    else
    {
        int mainSemaphore, waitSemaphore;
        // get semaphore
        mainSemaphore = my_semget(PROJ_ID1);
        waitSemaphore = my_semget(PROJ_ID2);
        // TODO
        //  - entrer en section critique :
        //        . pour empêcher que 2 clients communiquent simultanément
        //        . le mutex est déjà créé par le master
        enterSC(mainSemaphore);
        // - ouvrir les tubes nommés (ils sont déjà créés par le master)
        //       . les ouvertures sont bloquantes, il faut s'assurer que
        //         le master ouvre les tubes dans le même ordre
        data.pipe_client_to_master = myOpen("pipe_client_to_master", O_WRONLY);
        data.pipe_master_to_client = myOpen("pipe_master_to_client", O_RDONLY);
        // END TODO

        sendData(&data);
        receiveAnswer(&data);

        // TODO
        //  - sortir de la section critique
        exitSC(mainSemaphore);
        // - libérer les ressources (fermeture des tubes, ...)
        myClose(data.pipe_client_to_master);
        myClose(data.pipe_master_to_client);
        // - débloquer le master grâce à un second sémaphore (cf. ci-dessous)
        // Une fois que le master a envoyé la réponse au client, il se bloque
        // sur un sémaphore ; le dernier point permet donc au master de continuer
        exitWaiting(waitSemaphore);
        // N'hésitez pas à faire des fonctions annexes ; si la fonction main
        // ne dépassait pas une trentaine de lignes, ce serait bien.
    }

    return EXIT_SUCCESS;
}
