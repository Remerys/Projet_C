#ifndef MASTER_WORKER_H
#define MASTER_WORKER_H

#include <sys/types.h>
#include <unistd.h>

// ordres possibles du master pour le premier worker, ou d'un worker pour un de ses fils
#define MW_ORDER_STOP            0
#define MW_ORDER_HOW_MANY       10
#define MW_ORDER_MINIMUM        20
#define MW_ORDER_MAXIMUM        30
#define MW_ORDER_EXIST          40
#define MW_ORDER_SUM            50
#define MW_ORDER_INSERT         60
#define MW_ORDER_PRINT          70

// réponses possibles d'un worker pour le master, ou d'un worker pour son père
// pas de MW_ANSWER_STOP : le master attend la fin du premier worker, ou un worker attend la fin de ses fils
#define MW_ANSWER_HOW_MANY      10
#define MW_ANSWER_MINIMUM       20
#define MW_ANSWER_MAXIMUM       30
#define MW_ANSWER_EXIST_NO      40
#define MW_ANSWER_EXIST_YES     41
#define MW_ANSWER_SUM           50
#define MW_ANSWER_INSERT        60
#define MW_ANSWER_PRINT         70


//TODO
// Vous pouvez mettre ici des informations/fonctions soit communes au master et au
// worker, soit liées aux deux :
// . structures de données
// . libération/initialisation de ressources (sémaphores, tubes, ...)
// . communications
// . lancement d'un worker
//END TODO

#endif
