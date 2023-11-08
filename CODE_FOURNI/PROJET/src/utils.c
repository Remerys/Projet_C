#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
//TODO d'autres include éventuellement

#include "utils.h"
#include "myassert.h"

/******************************************
 * nombres aléatoires
 ******************************************/
static void ut_initAlea()
{
    static bool first = true;
    if (first)
    {
        srand(getpid());
        first = false;
    }
}

float ut_getAleaFloat(float min, float max, int precision)
{
    myassert(min < max, "min doit être strictement inférieur à max");
    myassert(precision >= 0, "la précision doit être positive");

    ut_initAlea();

    float r;
    int puiss = 1;
    for (int i = 0; i < precision; i++)
        puiss *= 10;

    do
    {
        int rInt = rand();
        r = ((float) rInt)/RAND_MAX * (max - min) + min;
        r = floor(r*puiss)/puiss;
    } while (r >= max);

    return r;
}

float * ut_generateTab(int size, float min, float max, int precision)
{
    float *t = malloc(size * sizeof(float));
    myassert(t != NULL, "allocation mémoire génération tableau float");
    for (int i = 0; i < size; i++)
        t[i] = ut_getAleaFloat(min, max, precision);

    // à mettre à true pour débuguer
    bool toPrint = false;
    if (toPrint)
    {
        printf("[");
        for (int i = 0; i < size; i++)
        {
            if (i != 0)
                printf(" ");
            printf("%g", t[i]);
        }
        printf("]\n");
    }
    return t;
}


//TODO d'autres fonctions utilitaires éventuellement
