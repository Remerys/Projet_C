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
#include "bettermyassert.h"
#include "myassert.h"
#include <assert.h>

#include "master_worker.h"


/************************************************************************
 * Données persistantes d'un worker
 ************************************************************************/
typedef struct
{
    // données internes (valeur de l'élément, cardinalité)
    float value;
    int cardinal;
    // communication avec le père (2 tubes) et avec le master (1 tube en écriture)
    int pipeFathertoMe;
    int pipeMetoFather;
    int pipeToMaster;
    // communication avec le fils gauche s'il existe (2 tubes)
    int pipeLeftSontoMe;
    int pipeMetoLeftSon;
    // communication avec le fils droit s'il existe (2 tubes)
    int pipeRightSontoMe;
    int pipeMetoRightSon;
    //TODO
} Data;


/************************************************************************
 * Usage et analyse des arguments passés en ligne de commande
 ************************************************************************/
static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s <elt> <fdIn> <fdOut> <fdToMaster>\n", exeName);
    fprintf(stderr, "   <elt> : élément géré par le worker\n");
    fprintf(stderr, "   <fdIn> : canal d'entrée (en provenance du père)\n");
    fprintf(stderr, "   <fdOut> : canal de sortie (vers le père)\n");
    fprintf(stderr, "   <fdToMaster> : canal de sortie directement vers le master\n");
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}

static void parseArgs(int argc, char * argv[], Data *data)
{
    myassert(data != NULL, "il faut l'environnement d'exécution");

    if (argc != 5)
        usage(argv[0], "Nombre d'arguments incorrect");

    //TODO initialisation data
    data->value = strtof(argv[1], NULL);
    data->cardinal = 1;
    data->pipeFathertoMe = strtol(argv[2], NULL, 10);
    data->pipeMetoFather = strtol(argv[3], NULL, 10);
    data->pipeToMaster = strtol(argv[4], NULL, 10);
    data->pipeMetoLeftSon = 0;
    data->pipeLeftSontoMe = 0;
    data->pipeMetoRightSon = 0;
    data->pipeRightSontoMe = 0;


    //TODO (à enlever) comment récupérer les arguments de la ligne de commande
    /*float elt = strtof(argv[1], NULL);
    int fdIn = strtol(argv[2], NULL, 10);
    int fdOut = strtol(argv[3], NULL, 10);
    int fdToMaster = strtol(argv[4], NULL, 10);
    printf("%g %d %d %d\n", elt, fdIn, fdOut, fdToMaster);*/
    //END TODO
}

void creaWorker(float val, int pipeFtoNew, int pipeNewtoF, int pipetoM, Data * data) {
  data->cardinal = 1;
  data->value = val;
  data->pipeFathertoMe = pipeFtoNew;
  data->pipeMetoFather = pipeNewtoF;
  data->pipeLeftSontoMe = 0;
  data->pipeMetoLeftSon = 0;
  data->pipeMetoRightSon = 0;
  data->pipeRightSontoMe = 0;
  data->pipeToMaster = pipetoM;
}


/************************************************************************
 * Stop 
 ************************************************************************/
void stopAction(Data *data)
{
    TRACE3("    [worker (%d, %d) {%g}] : ordre stop\n", getpid(), getppid(), data->value /*TODO élément*/);
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int retWrite;
    int order = MW_ORDER_STOP;

    //TODO
    // - traiter les cas où les fils n'existent pas
    if (data->pipeLeftSontoMe != 0) {
    // - envoyer au worker gauche ordre de fin (cf. master_worker.h)
      retWrite = write(data->pipeMetoLeftSon, &order, sizeof(int));
      assert_writePipeAnonymous(retWrite, sizeof(int));
    // - attendre la fin des deux fils
      wait(NULL);
    }
    if (data->pipeRightSontoMe != 0) {
    // - envoyer au worker droit ordre de fin (cf. master_worker.h)
      retWrite = write(data->pipeMetoRightSon, &order, sizeof(int));
      assert_writePipeAnonymous(retWrite, sizeof(int));
    // - attendre la fin des deux fils
      wait(NULL);
    }
    
    //END TODO
}


/************************************************************************
 * Combien d'éléments
 ************************************************************************/
static void howManyAction(Data *data)
{
    TRACE3("    [worker (%d, %d) {%g}] : ordre how many\n", getpid(), getppid(), data->value /*TODO élément*/);
    myassert(data != NULL, "il faut l'environnement d'exécution");

  int ret = 0;
  int filsG = 0;
  int filsG_dist = 0;
  int filsD = 0;
  int filsD_dist = 0;
  int howmany = 0;
  int howmany_dist = 0;

  int order = MW_ORDER_HOW_MANY;
  int answer = MW_ANSWER_HOW_MANY;

    //TODO
    // - traiter les cas où les fils n'existent pas
    // - pour chaque fils
    //       . envoyer ordre howmany (cf. master_worker.h)
    //       . recevoir accusé de réception (cf. master_worker.h)
    //       . recevoir deux résultats (nb elts, nb elts distincts) venant du fils
    if (data->pipeMetoLeftSon == 0)
    {
      filsG = 0;
      filsG_dist = 0;
    }
    else
    {
      ret = write(data->pipeMetoLeftSon, &order, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));

      ret = read(data->pipeLeftSontoMe, &answer, sizeof(int));
      assert_readPipeAnonymous(ret, sizeof(int));
      assert(answer == MW_ANSWER_HOW_MANY);

      ret = read(data->pipeLeftSontoMe, &filsG, sizeof(int));
      assert_readPipeAnonymous(ret, sizeof(int));

      ret = read(data->pipeLeftSontoMe, &filsG_dist, sizeof(int));
      assert_readPipeAnonymous(ret, sizeof(int));
    }
    if (data->pipeMetoRightSon == 0)
    {
      filsD = 0;
      filsD_dist = 0;
    }
    else
    {
      ret = write(data->pipeMetoRightSon, &order, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));

      ret = read(data->pipeRightSontoMe, &answer, sizeof(int));
      assert_readPipeAnonymous(ret, sizeof(int));
      assert(answer == MW_ANSWER_HOW_MANY);

      ret = read(data->pipeRightSontoMe, &filsD, sizeof(int));
      assert_readPipeAnonymous(ret, sizeof(int));

      ret = read(data->pipeRightSontoMe, &filsD_dist, sizeof(int));
      assert_readPipeAnonymous(ret, sizeof(int));
    }
    // - envoyer l'accusé de réception au père (cf. master_worker.h)
    ret = write(data->pipeMetoFather, &answer, sizeof(int));
    assert_writePipeAnonymous(ret, sizeof(int));
    // - envoyer les résultats (les cumuls des deux quantités + la valeur locale) au père
    howmany = data->cardinal + filsD + filsG;
    ret = write(data->pipeMetoFather, &howmany, sizeof(int));
    assert_writePipeAnonymous(ret, sizeof(int));

    howmany_dist = 1 + filsD_dist + filsG_dist;
    ret = write(data->pipeMetoFather, &howmany_dist, sizeof(int));
    assert_writePipeAnonymous(ret, sizeof(int));
    //END TODO
}


/************************************************************************
 * Minimum
 ************************************************************************/
static void minimumAction(Data *data)
{
    TRACE3("    [worker (%d, %d) {%g}] : ordre minimum\n", getpid(), getppid(), data->value /*TODO élément*/);
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int answer = MW_ANSWER_MINIMUM;
    int order = MW_ORDER_MINIMUM;
    int ret;
    float mini = data->value;

    //TODO
    // - si le fils gauche n'existe pas (on est sur le minimum)
    //       . envoyer l'accusé de réception au master (cf. master_worker.h)
    //       . envoyer l'élément du worker courant au master
    if (data->pipeMetoLeftSon == 0)
    {
      ret = write(data->pipeToMaster, &answer, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
      ret = write(data->pipeToMaster, &mini, sizeof(float));
      assert_writePipeAnonymous(ret, sizeof(float));
    }
    // - sinon
    //       . envoyer au worker gauche ordre minimum (cf. master_worker.h)
    //       . note : c'est un des descendants qui enverra le résultat au master
    else
    {
      ret = write(data->pipeMetoLeftSon, &order, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
    }
    //END TODO
}


/************************************************************************
 * Maximum
 ************************************************************************/
static void maximumAction(Data *data)
{
    TRACE3("    [worker (%d, %d) {%g}] : ordre maximum\n", getpid(), getppid(), data->value /*TODO élément*/);
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int answer = MW_ANSWER_MAXIMUM;
    int order = MW_ORDER_MAXIMUM;
    int ret;
    float maxi = data->value;

    //TODO
    // cf. explications pour le minimum
    if (data->pipeMetoRightSon == 0)
    {
      ret = write(data->pipeToMaster, &answer, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
      ret = write(data->pipeToMaster, &maxi, sizeof(float));
      assert_writePipeAnonymous(ret, sizeof(float));
    }
    else
    {
      ret = write(data->pipeMetoRightSon, &order, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
    }
    //END TODO
}


/************************************************************************
 * Existence
 ************************************************************************/
static void existAction(Data *data)
{
    TRACE3("    [worker (%d, %d) {%g}] : ordre exist\n", getpid(), getppid(), data->value /*TODO élément*/);
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int order = MW_ORDER_EXIST;
    int answer_yes = MW_ANSWER_EXIST_YES;
    int answer_no = MW_ANSWER_EXIST_NO;

    int card = data->cardinal;

    //TODO
    // - recevoir l'élément à tester en provenance du père
    float element;
    int ret = read(data->pipeFathertoMe, &element, sizeof(float));
    assert_readPipeAnonymous(ret, sizeof(float));
    // - si élément courant == élément à tester
    //       . envoyer au master l'accusé de réception de réussite (cf. master_worker.h)
    //       . envoyer cardinalité de l'élément courant au master
    if (data->value == element)
    {

      ret = write(data->pipeToMaster, &answer_yes, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
      ret = write(data->pipeToMaster, &card, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
    }
    // - sinon si (elt à tester < elt courant) et (pas de fils gauche)
    //       . envoyer au master l'accusé de réception d'échec (cf. master_worker.h)
    else if (element < data->value && data->pipeMetoLeftSon == 0)
    {
      ret = write(data->pipeToMaster, &answer_no, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
    }
    // - sinon si (elt à tester > elt courant) et (pas de fils droit)
    //       . envoyer au master l'accusé de réception d'échec (cf. master_worker.h)
    else if (element > data->value && data->pipeMetoRightSon == 0)
    {
      ret = write(data->pipeToMaster, &answer_no, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
    }
    // - sinon si (elt à tester < elt courant)
    //       . envoyer au worker gauche ordre exist (cf. master_worker.h)
    //       . envoyer au worker gauche élément à tester
    //       . note : c'est un des descendants qui enverra le résultat au master
    else if (element < data->value)
    {
      ret = write(data->pipeMetoLeftSon, &order, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
      ret = write(data->pipeMetoLeftSon, &element, sizeof(float));
      assert_writePipeAnonymous(ret, sizeof(float));
    }
    // - sinon (donc elt à tester > elt courant)
    //       . envoyer au worker droit ordre exist (cf. master_worker.h)
    //       . envoyer au worker droit élément à tester
    //       . note : c'est un des descendants qui enverra le résultat au master
    else
    {
      ret = write(data->pipeMetoRightSon, &order, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
      ret = write(data->pipeMetoRightSon, &element, sizeof(float));
      assert_writePipeAnonymous(ret, sizeof(float));
    }
    //END TODO
}


/************************************************************************
 * Somme
 ************************************************************************/
static void sumAction(Data *data)
{
    TRACE3("    [worker (%d, %d) {%g}] : ordre sum\n", getpid(), getppid(), data->value /*TODO élément*/);
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int ret;
    float sum = 0;
    float filsG = 0;
    float filsD = 0;

    int order = MW_ORDER_SUM;
    int answer = MW_ANSWER_SUM;

    //TODO
    // - traiter les cas où les fils n'existent pas
    // - pour chaque fils
    //       . envoyer ordre sum (cf. master_worker.h)
    //       . recevoir accusé de réception (cf. master_worker.h)
    //       . recevoir résultat (somme du fils) venant du fils
    if (data->pipeMetoLeftSon == 0)
    {
      filsG = 0;
    }
    else
    {
      ret = write(data->pipeMetoLeftSon, &order, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));

      ret = read(data->pipeLeftSontoMe, &answer, sizeof(int));
      assert_readPipeAnonymous(ret, sizeof(int));
      assert(answer == MW_ANSWER_SUM);

      ret = read(data->pipeLeftSontoMe, &filsG, sizeof(float));
      assert_readPipeAnonymous(ret, sizeof(float));
    }
    if (data->pipeMetoRightSon == 0)
    {
      filsD = 0;
    }
    else
    {
      ret = write(data->pipeMetoRightSon, &order, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));

      ret = read(data->pipeRightSontoMe, &answer, sizeof(int));
      assert_readPipeAnonymous(ret, sizeof(int));
      assert(answer == MW_ANSWER_SUM);

      ret = read(data->pipeRightSontoMe, &filsD, sizeof(float));
      assert_readPipeAnonymous(ret, sizeof(float));
    }
    // - envoyer l'accusé de réception au père (cf. master_worker.h)
    ret = write(data->pipeMetoFather, &answer, sizeof(int));
    assert_writePipeAnonymous(ret, sizeof(int));
    // - envoyer le résultat (le cumul des deux quantités + la valeur locale) au père
    sum = data->value + filsD + filsG;
    ret = write(data->pipeMetoFather, &sum, sizeof(float));
    assert_writePipeAnonymous(ret, sizeof(float));
    //END TODO
}


/************************************************************************
 * Insertion d'un nouvel élément
 ************************************************************************/
static void insertAction(Data *data)
{
    TRACE3("    [worker (%d, %d) {%g}] : ordre insert\n", getpid(), getppid(), data->value /*TODO élément*/);
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int response = MW_ANSWER_INSERT;
    int order = MW_ORDER_INSERT;

    //TODO
    // - recevoir l'élément à insérer en provenance du père
    float element;
    int ret = read(data->pipeFathertoMe, &element, sizeof(float));
    assert_readPipeAnonymous(ret, sizeof(float));
    // - si élément courant == élément à tester
    //       . incrémenter la cardinalité courante
    //       . envoyer au master l'accusé de réception (cf. master_worker.h)
    if (data->value == element)
    {
      data->cardinal += 1;
      int ret = write(data->pipeToMaster, &response, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
    }
    // - sinon si (elt à tester < elt courant) et (pas de fils gauche)
    //       . créer un worker à gauche avec l'élément reçu du client
    //       . note : c'est ce worker qui enverra l'accusé de réception au master
    else if ((element < data->value) && (data->pipeMetoLeftSon == 0))
    {
      int pereToFils[2];
      int filsToPere[2];
      int ret;
      pid_t retFork;

      ret = pipe(pereToFils);
      assert_pipePipe(ret);
      ret = pipe(filsToPere);
      assert_pipePipe(ret);

      retFork = fork();
      assert_ftok(retFork);

      if (retFork == 0)
      {
        ret = close(pereToFils[1]);
        assert_closePipe(ret);
        ret = close(filsToPere[0]);
        assert_closePipe(ret);
        creaWorker(element, pereToFils[0], filsToPere[1], data->pipeToMaster, data);
        
        ret = write(data->pipeToMaster, &response, sizeof(int));
        assert_writePipeAnonymous(ret, sizeof(int));
      }
      else
      {
        ret = close(pereToFils[0]);
        assert_closePipe(ret);
        ret = close(filsToPere[1]);
        assert_closePipe(ret);

        data->pipeLeftSontoMe = filsToPere[0];
        data->pipeMetoLeftSon = pereToFils[1];
      }
    }
    // - sinon si (elt à tester > elt courant) et (pas de fils droit)
    //       . créer un worker à droite avec l'élément reçu du client
    //       . note : c'est ce worker qui enverra l'accusé de réception au master
    else if ((element > data->value) && (data->pipeMetoRightSon == 0))
    {
      int pereToFils[2];
      int filsToPere[2];
      int ret;
      pid_t retFork;

      ret = pipe(pereToFils);
      assert_pipePipe(ret);
      ret = pipe(filsToPere);
      assert_pipePipe(ret);

      retFork = fork();
      assert_ftok(retFork);

      if (retFork == 0)
      {
        ret = close(pereToFils[1]);
        assert_closePipe(ret);
        ret = close(filsToPere[0]);
        assert_closePipe(ret);
        creaWorker(element, pereToFils[0], filsToPere[1], data->pipeToMaster, data);
        
        ret = write(data->pipeToMaster, &response, sizeof(int));
        assert_writePipeAnonymous(ret, sizeof(int));
      }
      else
      {
        ret = close(pereToFils[0]);
        assert_closePipe(ret);
        ret = close(filsToPere[1]);
        assert_closePipe(ret);
        
        data->pipeRightSontoMe = filsToPere[0];
        data->pipeMetoRightSon = pereToFils[1];
      }
    }
    // - sinon si (elt à insérer < elt courant)
    //       . envoyer au worker gauche ordre insert (cf. master_worker.h)
    //       . envoyer au worker gauche élément à insérer
    //       . note : c'est un des descendants qui enverra l'accusé de réception au master
    else if (element < data->value)
    {
      int ret;
      ret = write(data->pipeMetoLeftSon, &order, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));
      
      ret = write(data->pipeMetoLeftSon, &element, sizeof(float));
      assert_writePipeAnonymous(ret, sizeof(float));
    }
    // - sinon (donc elt à insérer > elt courant)
    //       . envoyer au worker droit ordre insert (cf. master_worker.h)
    //       . envoyer au worker droit élément à insérer
    //       . note : c'est un des descendants qui enverra l'accusé de réception au master
    else
    {
      int ret;
      ret = write(data->pipeMetoRightSon, &order, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));

      ret = write(data->pipeMetoRightSon, &element, sizeof(float));
      assert_writePipeAnonymous(ret, sizeof(float));
    }
    //END TODO
}


/************************************************************************
 * Affichage
 ************************************************************************/
static void printAction(Data *data)
{
    TRACE3("    [worker (%d, %d) {%g}] : ordre print\n", getpid(), getppid(), data->value /*TODO élément*/);
    myassert(data != NULL, "il faut l'environnement d'exécution");

    int ret;

    int order = MW_ORDER_PRINT;
    int accuse_reception = MW_ANSWER_PRINT;

    //TODO
    // - si le fils gauche existe
    //       . envoyer ordre print (cf. master_worker.h)
    //       . recevoir accusé de réception (cf. master_worker.h)
    if (data->pipeMetoLeftSon != 0) {
      ret = write(data->pipeMetoLeftSon, &order, sizeof(int));
      assert_writePipeAnonymous(ret, sizeof(int));

      ret = read(data->pipeLeftSontoMe, &accuse_reception, sizeof(float));
      assert_readPipeAnonymous(ret, sizeof(float));
      assert(accuse_reception == MW_ANSWER_PRINT);
    }
    // - afficher l'élément courant avec sa cardinalité
    printf("(%f, %d)\n", data->value, data->cardinal);
    // - si le fils droit existe
    //       . envoyer ordre print (cf. master_worker.h)
    //       . recevoir accusé de réception (cf. master_worker.h)
    if (data->pipeMetoRightSon != 0) {
        ret = write(data->pipeMetoRightSon, &order, sizeof(int));
        assert_writePipeAnonymous(ret, sizeof(int));

        ret = read(data->pipeRightSontoMe, &accuse_reception, sizeof(float));
        assert_readPipeAnonymous(ret, sizeof(float));
        assert(accuse_reception == MW_ANSWER_PRINT);
    }
    // - envoyer l'accusé de réception au père (cf. master_worker.h)
    ret = write(data->pipeMetoFather, &accuse_reception, sizeof(int));
    assert_writePipeAnonymous(ret, sizeof(int));
    //END TODO
}


/************************************************************************
 * Boucle principale de traitement
 ************************************************************************/
void loop(Data *data)
{
    bool end = false;

    while (!end)
    {
        //int order = MW_ORDER_STOP;   //TODO pour que ça ne boucle pas, mais recevoir l'ordre du père
        int order;
        int ret = read(data->pipeFathertoMe, &order, sizeof(int));
        assert_readPipeAnonymous(ret, sizeof(int));
        switch(order)
        {
          case MW_ORDER_STOP:
            stopAction(data);
            end = true;
            break;
          case MW_ORDER_HOW_MANY:
            howManyAction(data);
            break;
          case MW_ORDER_MINIMUM:
            minimumAction(data);
            break;
          case MW_ORDER_MAXIMUM:
            maximumAction(data);
            break;
          case MW_ORDER_EXIST:
            existAction(data);
            break;
          case MW_ORDER_SUM:
            sumAction(data);
            break;
          case MW_ORDER_INSERT:
            insertAction(data);
            break;
          case MW_ORDER_PRINT:
            printAction(data);
            break;
          default:
            myassert(false, "ordre inconnu");
            exit(EXIT_FAILURE);
            break;
        }

        TRACE3("    [worker (%d, %d) {%g}] : fin ordre\n", getpid(), getppid(), data->value /*TODO élément*/);
    }
}


/************************************************************************
 * Fonctions auxiliaires
 ************************************************************************/

/* which permet de savoir quelle info utiliser (si on veut un int ou un float)
    - true : info1 int
    - false : info2 float */

void sendInfoToFather(Data *data, int * info1, float * info2, bool which)
{
  int retWrite;
  if (which)
  {
    retWrite = write(data->pipeMetoFather, info1, sizeof(int));
    assert_writePipeAnonymous(retWrite, sizeof(int));
  }
  else
  {
    retWrite = write(data->pipeMetoFather, info2, sizeof(float));
    assert_writePipeAnonymous(retWrite, sizeof(float));
  }
}

void sendInfoToMaster(Data *data, int * info1, float * info2, bool which) {
  int retWrite;
  if (which)
  {
    retWrite = write(data->pipeToMaster, info1, sizeof(int));
    assert_writePipeAnonymous(retWrite, sizeof(int));
  }
  else if (info2 != NULL)
  {
    retWrite = write(data->pipeToMaster, info2, sizeof(float));
    assert_writePipeAnonymous(retWrite, sizeof(float));
  }
}

void sendInfoToLeftSon(Data *data, int * info1, float * info2, bool which) {
  int retWrite;
  if (which)
  {
    retWrite = write(data->pipeMetoLeftSon, info1, sizeof(int));
    assert_writePipeAnonymous(retWrite, sizeof(int));
  }
  else if (info2 != NULL)
  {
    retWrite = write(data->pipeMetoLeftSon, info2, sizeof(float));
    assert_writePipeAnonymous(retWrite, sizeof(float));
  }
}

void sendInfoToRightSon(Data *data, int * info1, float * info2, bool which) {
  int retWrite;
  if (which)
  {
    retWrite = write(data->pipeMetoRightSon, info1, sizeof(int));
    assert_writePipeAnonymous(retWrite, sizeof(int));
  }
  else if (info2 != NULL)
  {
    retWrite = write(data->pipeMetoRightSon, info2, sizeof(float));
    assert_writePipeAnonymous(retWrite, sizeof(float));
  }
}

void receiveInfoFromFather(Data *data, int * info1, float * info2, bool which) {
  int retRead;
  if (which)
  {
    retRead = read(data->pipeFathertoMe, info1, sizeof(int));
    assert_readPipeAnonymous(retRead, sizeof(int));
  }
  else
  {
    retRead = read(data->pipeFathertoMe, info2, sizeof(float));
    assert_readPipeAnonymous(retRead, sizeof(float));
  }
}

void receiveInfoFromLeftSon(Data *data, int * info1, float * info2, bool which) {
  int retRead;
  if (which)
  {
    retRead = read(data->pipeLeftSontoMe, info1, sizeof(int));
    assert_readPipeAnonymous(retRead, sizeof(int));
  }
  else
  {
    retRead = read(data->pipeLeftSontoMe, info2, sizeof(float));
    assert_readPipeAnonymous(retRead, sizeof(float));
  }
}

void receiveInfoFromRightSon(Data *data, int * info1, float * info2, bool which) {
  int retRead;
  if (which)
  {
    retRead = read(data->pipeRightSontoMe, info1, sizeof(int));
    assert_readPipeAnonymous(retRead, sizeof(int));
  }
  else
  {
    retRead = read(data->pipeRightSontoMe, info2, sizeof(float));
    assert_readPipeAnonymous(retRead, sizeof(float));
  }
}

/************************************************************************
 * Programme principal
 ************************************************************************/

int main(int argc, char * argv[])
{
    Data data;
    parseArgs(argc, argv, &data);
    TRACE3("    [worker (%d, %d) {%g}] : début worker\n", getpid(), getppid(), data.value /*TODO élément*/);

    //TODO envoyer au master l'accusé de réception d'insertion (cf. master_worker.h)
    //TODO note : en effet si je suis créé c'est qu'on vient d'insérer un élément : moi
    int answer = MW_ANSWER_INSERT;
    int ret = write(data.pipeToMaster, &answer, sizeof(int));
    assert_writePipeAnonymous(ret, sizeof(int));

    loop(&data);

    //TODO fermer les tubes
    if (data.pipeMetoLeftSon != 0)
    {
      ret = close(data.pipeMetoLeftSon);
      assert_closePipe(ret);
      ret = close(data.pipeLeftSontoMe);
      assert_closePipe(ret);
    }
    if (data.pipeMetoRightSon != 0)
    {
      ret = close(data.pipeMetoRightSon);
      assert_closePipe(ret);
      ret = close(data.pipeRightSontoMe);
      assert_closePipe(ret);
    }

    TRACE3("    [worker (%d, %d) {%g}] : fin worker\n", getpid(), getppid(), data.value /*TODO élément*/);
    return EXIT_SUCCESS;
}
